/* Query panel for the "Flecs playground" page. The header holds a single-line
 * query editor (Ace, flecs-query mode from the Flecs Explorer); the body shows
 * the matched entities as a table, in a 2D canvas or in a 3D scene (three.js,
 * loaded on demand). The 2D/3D views draw all entities that have a Position
 * and a Rect, Box, Circle or Sphere component (colored by Rgb, Rgba and
 * Emissive) and grey out the ones the query does not match. */
(function () {
  "use strict";

  var THREE_BASE = "https://cdn.jsdelivr.net/npm/three@0.128.0/";
  var THREE_SCRIPTS = [
    "build/three.min.js",
    "examples/js/shaders/CopyShader.js",
    "examples/js/shaders/LuminosityHighPassShader.js",
    "examples/js/postprocessing/EffectComposer.js",
    "examples/js/postprocessing/RenderPass.js",
    "examples/js/postprocessing/ShaderPass.js",
    "examples/js/postprocessing/UnrealBloomPass.js"
  ];
  var LABEL_LIMIT = 100;
  var CHEVRON = '<svg viewBox="0 0 24 24" width="14" height="14" aria-hidden="true"><path d="M9 6l6 6-6 6" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/></svg>';

  function el(tag, attrs, children) {
    var node = document.createElement(tag);
    if (attrs) {
      Object.keys(attrs).forEach(function (k) {
        if (k === "class") node.className = attrs[k];
        else if (k === "html") node.innerHTML = attrs[k];
        else if (k === "text") node.textContent = attrs[k];
        else node.setAttribute(k, attrs[k]);
      });
    }
    (children || []).forEach(function (c) { if (c) node.appendChild(c); });
    return node;
  }

  function clamp(v, min, max) {
    return v < min ? min : v > max ? max : v;
  }

  function cssVar(name, fallback) {
    var v = getComputedStyle(document.documentElement).getPropertyValue(name).trim();
    return v || fallback;
  }

  function theme() {
    return {
      bg: cssVar("--fl-bg", "#ffffff"),
      grid: cssVar("--fl-border", "#e3e6ea"),
      axis: cssVar("--fl-border-strong", "#cfd4da"),
      text: cssVar("--fl-text-2", "#4b5563"),
      muted: cssVar("--fl-text-3", "#6b7280"),
      accent: cssVar("--fl-accent", "#42b983"),
      accentStrong: cssVar("--fl-accent-strong", "#1f8a5a"),
      mono: cssVar("--fl-mono", "monospace"),
      dark: document.documentElement.classList.contains("dark-mode")
    };
  }

  /* Shapes: Position (x, y, z), Rect (width, height), Box (width, height,
   * depth), Circle/Sphere (radius), Rgb/Rgba (u8) and Emissive (r, g, b,
   * intensity) of the entities in the world (GET /world). Entities without
   * geometry are not drawn. Positions of children are relative to their
   * parents. */

  function component(comps, name) {
    if (Object.prototype.hasOwnProperty.call(comps, name)) return comps[name];
    var keys = Object.keys(comps);
    for (var i = 0; i < keys.length; i++) {
      if (keys[i].slice(-(name.length + 1)) === "." + name) return comps[keys[i]];
    }
    return undefined;
  }

  function struct(comps, name) {
    var v = component(comps, name);
    return v && typeof v === "object" ? v : null;
  }

  function member(obj, names, index, fallback) {
    if (!obj || typeof obj !== "object") return fallback;
    for (var i = 0; i < names.length; i++) {
      var v = obj[names[i]];
      if (typeof v === "number" && isFinite(v)) return v;
    }
    var nums = Object.keys(obj).map(function (k) { return obj[k]; }).filter(function (v) {
      return typeof v === "number" && isFinite(v);
    });
    return index >= 0 && index < nums.length ? nums[index] : fallback;
  }

  function channel(v) {
    return clamp(Math.round(v), 0, 255);
  }

  function resultPath(r) {
    if (r.name === undefined) return null;
    return r.parent ? r.parent + "." + r.name : r.name;
  }

  function shapeOf(r) {
    var comps = r.components;
    if (!comps) return null;
    var pos = struct(comps, "Position");
    if (!pos) return null;
    var path = resultPath(r);
    var shape = {
      path: path, name: r.name, key: r.id !== undefined ? "#" + r.id : path,
      x: member(pos, ["x"], 0, 0), y: member(pos, ["y"], 1, 0), z: member(pos, ["z"], 2, 0),
      r: 66, g: 185, b: 131, a: 1, kind: null, w: 0, h: 0, d: 0, radius: 0, emissive: null, dim: false
    };
    var rgba = struct(comps, "Rgba");
    var rgb = rgba || struct(comps, "Rgb");
    if (rgb) {
      shape.r = channel(member(rgb, ["r"], 0, 0));
      shape.g = channel(member(rgb, ["g"], 1, 0));
      shape.b = channel(member(rgb, ["b"], 2, 0));
      if (rgba) shape.a = channel(member(rgb, ["a"], 3, 255)) / 255;
    }
    var box = struct(comps, "Box"), rect = struct(comps, "Rect");
    var sphere = struct(comps, "Sphere"), circle = struct(comps, "Circle");
    if (box) {
      shape.kind = "box";
      shape.w = member(box, ["width", "w"], 0, 0);
      shape.h = member(box, ["height", "h"], 1, 0);
      shape.d = member(box, ["depth", "d"], 2, 0);
    } else if (rect) {
      shape.kind = "rect";
      shape.w = member(rect, ["width", "w"], 0, 0);
      shape.h = member(rect, ["height", "h"], 1, 0);
    } else if (sphere) {
      shape.kind = "sphere";
      shape.radius = member(sphere, ["radius", "r"], 0, 0);
    } else if (circle) {
      shape.kind = "circle";
      shape.radius = member(circle, ["radius", "r"], 0, 0);
    }
    if (!shape.kind) return null;
    if ((shape.kind === "box" || shape.kind === "rect") && !(shape.w > 0 && shape.h > 0)) return null;
    if ((shape.kind === "sphere" || shape.kind === "circle") && !(shape.radius > 0)) return null;
    if (shape.kind === "box" && !(shape.d > 0)) shape.d = (shape.w + shape.h) / 2;
    var em = struct(comps, "Emissive");
    if (em) {
      var hasColor = typeof em.r === "number" || typeof em.g === "number" || typeof em.b === "number";
      shape.emissive = {
        r: hasColor ? channel(member(em, ["r"], 0, 0)) : shape.r,
        g: hasColor ? channel(member(em, ["g"], 1, 0)) : shape.g,
        b: hasColor ? channel(member(em, ["b"], 2, 0)) : shape.b,
        intensity: Math.max(member(em, ["intensity", "strength"], hasColor ? 3 : 0, 1), 0)
      };
    }
    return shape;
  }

  function worldShapes(results) {
    var byPath = {};
    results.forEach(function (r) {
      var p = resultPath(r);
      if (p) byPath[p] = r;
      if (r.id !== undefined) byPath["#" + r.id] = r;
    });
    var memo = {};
    function worldPosition(path) {
      if (memo[path]) return memo[path];
      var r = byPath[path];
      var base = r && r.parent ? worldPosition(r.parent) : { x: 0, y: 0, z: 0 };
      var pos = r ? struct(r.components || {}, "Position") : null;
      var wp = pos ? {
        x: base.x + member(pos, ["x"], 0, 0),
        y: base.y + member(pos, ["y"], 1, 0),
        z: base.z + member(pos, ["z"], 2, 0)
      } : base;
      memo[path] = wp;
      return wp;
    }
    var shapes = [];
    results.forEach(function (r) {
      if ((r.tags || []).indexOf("flecs.core.Prefab") !== -1) return;
      var s = shapeOf(r);
      if (!s) return;
      if (s.path) {
        var wp = worldPosition(s.path);
        s.x = wp.x; s.y = wp.y; s.z = wp.z;
      }
      shapes.push(s);
    });
    return shapes;
  }

  function isRound(s) {
    return s.kind === "circle" || s.kind === "sphere";
  }

  function halfExtent(s) {
    if (s.kind === "sphere") return { x: s.radius, y: s.radius, z: s.radius };
    if (s.kind === "circle") return { x: s.radius, y: s.radius, z: 0 };
    return { x: s.w / 2, y: s.h / 2, z: s.kind === "box" ? s.d / 2 : 0 };
  }

  function bounds(shapes) {
    var b = { minX: Infinity, minY: Infinity, minZ: Infinity, maxX: -Infinity, maxY: -Infinity, maxZ: -Infinity };
    shapes.forEach(function (s) {
      var e = halfExtent(s);
      b.minX = Math.min(b.minX, s.x - e.x); b.maxX = Math.max(b.maxX, s.x + e.x);
      b.minY = Math.min(b.minY, s.y - e.y); b.maxY = Math.max(b.maxY, s.y + e.y);
      b.minZ = Math.min(b.minZ, s.z - e.z); b.maxZ = Math.max(b.maxZ, s.z + e.z);
    });
    return b;
  }

  function niceStep(min) {
    var p = Math.pow(10, Math.floor(Math.log10(min)));
    var m = [1, 2, 5, 10];
    for (var i = 0; i < m.length; i++) if (p * m[i] >= min) return p * m[i];
    return p * 10;
  }

  var DIM_COLOR = { r: 140, g: 140, b: 140, a: 1 };

  function rgba(c, alpha) {
    return "rgba(" + c.r + "," + c.g + "," + c.b + "," + (alpha !== undefined ? alpha : c.a) + ")";
  }

  function canvasChrome(host) {
    var tip = el("div", { class: "pg-canvas-tip" });
    tip.hidden = true;
    var empty = el("div", { class: "pg-canvas-empty", text: "No entities with a Position and a Rect, Box, Circle or Sphere component." });
    empty.hidden = true;
    host.appendChild(tip);
    host.appendChild(empty);
    return {
      tip: tip,
      empty: empty,
      showTip: function (text, x, y) {
        tip.textContent = text;
        tip.hidden = false;
        var w = host.clientWidth, h = host.clientHeight;
        tip.style.left = Math.min(x + 14, Math.max(0, w - tip.offsetWidth - 8)) + "px";
        tip.style.top = Math.min(y + 14, Math.max(0, h - tip.offsetHeight - 8)) + "px";
      },
      hideTip: function () { tip.hidden = true; }
    };
  }

  function localPoint(host, e) {
    var r = host.getBoundingClientRect();
    return { x: e.clientX - r.left, y: e.clientY - r.top };
  }

  /* Table view */

  function createTableView(container, onSelect) {
    var host = el("div", { class: "pg-view pg-view-table" });
    container.appendChild(host);
    var selected = null;

    host.addEventListener("click", function (e) {
      if (!e.target.closest("tr[data-path]")) onSelect(null);
    });

    function format(v) {
      if (v === null || v === undefined) return "null";
      if (typeof v === "string") return JSON.stringify(v);
      if (typeof v !== "object") return String(v);
      if (Array.isArray(v)) return "[" + v.map(format).join(", ") + "]";
      return "{" + Object.keys(v).map(function (k) { return k + ": " + format(v[k]); }).join(", ") + "}";
    }

    function valueCell(r, i, info) {
      var td = el("td", { class: "pg-qvalue" });
      var f = r.fields || {};
      if (f.is_set && f.is_set[i] === false) {
        td.appendChild(el("span", { class: "pg-v pg-v-null", text: "—" }));
        return td;
      }
      var hasValue = !info || !!info.type;
      var v = f.values ? f.values[i] : undefined;
      if (f.ids && f.ids[i]) {
        td.appendChild(el("span", { class: "pg-v pg-v-ent", text: f.ids[i] }));
        if (hasValue && v !== undefined && v !== 0) td.appendChild(document.createTextNode(" "));
      }
      if (hasValue && v !== undefined && !(v === 0 && !info)) {
        var cls = typeof v === "number" ? "pg-v-num" : typeof v === "string" ? "pg-v-str" : typeof v === "boolean" ? "pg-v-bool" : "";
        td.appendChild(el("span", { class: "pg-v " + cls, text: format(v) }));
      } else if (!td.childNodes.length) {
        td.appendChild(el("span", { class: "pg-v pg-v-tag", html: "&#10003;" }));
      }
      if (f.sources && f.sources[i]) {
        td.appendChild(el("span", { class: "pg-qsource", text: "from " + f.sources[i] }));
      }
      return td;
    }

    function fieldLabel(info, i) {
      if (!info) return "Field " + (i + 1);
      var label = info.id || info.type || "Field " + (i + 1);
      return info.optional ? "?" + label : label;
    }

    function markSelected() {
      host.querySelectorAll("tr[data-path]").forEach(function (tr) {
        tr.classList.toggle("pg-selected", tr.getAttribute("data-path") === selected);
      });
    }

    return {
      host: host,
      update: function (reply) {
        host.innerHTML = "";
        var results = reply.results || [];
        if (!results.length) {
          host.appendChild(el("div", { class: "pg-empty", text: "No entities match this query." }));
          return;
        }
        var info = reply.field_info || [];
        var nfields = info.length;
        results.forEach(function (r) {
          if (r.fields && r.fields.values) nfields = Math.max(nfields, r.fields.values.length);
        });
        var vars = [];
        results.forEach(function (r) {
          Object.keys(r.vars || {}).forEach(function (v) { if (vars.indexOf(v) === -1) vars.push(v); });
        });
        var table = el("table", { class: "pg-qtable" });
        var head = el("tr");
        head.appendChild(el("th", { text: "Entity" }));
        for (var i = 0; i < nfields; i++) head.appendChild(el("th", { text: fieldLabel(info[i], i) }));
        vars.forEach(function (v) { head.appendChild(el("th", { text: "$" + v })); });
        table.appendChild(el("thead", null, [head]));
        var body = el("tbody");
        results.forEach(function (r) {
          var path = resultPath(r);
          var tr = el("tr");
          var td = el("td", { class: "pg-qentity" });
          if (path) {
            tr.setAttribute("data-path", path);
            var a = el("a", { href: "#", text: path });
            a.addEventListener("click", function (e) { e.preventDefault(); onSelect(path); });
            td.appendChild(a);
          } else {
            td.appendChild(el("span", { class: "pg-v pg-v-null", text: "—" }));
          }
          tr.appendChild(td);
          for (var j = 0; j < nfields; j++) tr.appendChild(valueCell(r, j, info[j]));
          vars.forEach(function (v) {
            var val = r.vars ? r.vars[v] : undefined;
            tr.appendChild(el("td", { class: "pg-qvalue" }, [
              el("span", { class: "pg-v " + (val === undefined ? "pg-v-null" : "pg-v-ent"), text: val === undefined ? "—" : String(val) })
            ]));
          });
          if (path) tr.addEventListener("click", function (e) { if (!e.target.closest("a")) onSelect(path); });
          body.appendChild(tr);
        });
        table.appendChild(body);
        host.appendChild(table);
        markSelected();
      },
      setSelected: function (path) { selected = path; markSelected(); },
      resize: function () {}
    };
  }

  /* 2D view */

  function createView2D(container, onSelect) {
    var host = el("div", { class: "pg-view pg-view-canvas pg-view-2d" });
    var canvas = el("canvas", { class: "pg-canvas" });
    host.appendChild(canvas);
    var chrome = canvasChrome(host);
    container.appendChild(host);
    var ctx = canvas.getContext("2d");
    var cam = { x: 0, y: 0, zoom: 20 };
    var shapes = [], selected = null, hover = null, fitted = false;
    var width = 0, height = 0, dpr = 1, drag = null;

    function toScreen(x, y) {
      return { x: width / 2 + (x - cam.x) * cam.zoom, y: height / 2 - (y - cam.y) * cam.zoom };
    }

    function toWorld(sx, sy) {
      return { x: cam.x + (sx - width / 2) / cam.zoom, y: cam.y - (sy - height / 2) / cam.zoom };
    }

    function radiusPx(s) {
      return s.radius * cam.zoom;
    }

    function fit() {
      if (!shapes.length || !width || !height) return;
      var b = bounds(shapes);
      var bw = Math.max(b.maxX - b.minX, 1e-6), bh = Math.max(b.maxY - b.minY, 1e-6);
      cam.zoom = clamp(Math.min(width / bw, height / bh) * 0.8, 1e-4, 1e6);
      cam.x = (b.minX + b.maxX) / 2;
      cam.y = (b.minY + b.maxY) / 2;
      fitted = true;
      render();
    }

    function tracePath(s, p) {
      ctx.beginPath();
      if (isRound(s)) ctx.arc(p.x, p.y, radiusPx(s), 0, Math.PI * 2);
      else ctx.rect(p.x - s.w / 2 * cam.zoom, p.y - s.h / 2 * cam.zoom, s.w * cam.zoom, s.h * cam.zoom);
    }

    function render() {
      if (!width || !height) return;
      var c = theme();
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
      ctx.fillStyle = c.bg;
      ctx.fillRect(0, 0, width, height);

      var step = niceStep(48 / cam.zoom);
      var tl = toWorld(0, 0), br = toWorld(width, height);
      ctx.lineWidth = 1;
      ctx.strokeStyle = c.grid;
      ctx.beginPath();
      for (var gx = Math.floor(tl.x / step) * step; gx <= br.x; gx += step) {
        var sx = Math.round(toScreen(gx, 0).x) + 0.5;
        ctx.moveTo(sx, 0); ctx.lineTo(sx, height);
      }
      for (var gy = Math.floor(br.y / step) * step; gy <= tl.y; gy += step) {
        var sy = Math.round(toScreen(0, gy).y) + 0.5;
        ctx.moveTo(0, sy); ctx.lineTo(width, sy);
      }
      ctx.stroke();
      ctx.strokeStyle = c.axis;
      ctx.beginPath();
      var o = toScreen(0, 0);
      ctx.moveTo(Math.round(o.x) + 0.5, 0); ctx.lineTo(Math.round(o.x) + 0.5, height);
      ctx.moveTo(0, Math.round(o.y) + 0.5); ctx.lineTo(width, Math.round(o.y) + 0.5);
      ctx.stroke();

      shapes.forEach(function (s) {
        if (!s.emissive || s.dim) return;
        var p = toScreen(s.x, s.y);
        ctx.save();
        ctx.shadowColor = rgba(s.emissive, Math.min(0.5 + 0.4 * s.emissive.intensity, 1));
        ctx.shadowBlur = Math.min(12 + 24 * s.emissive.intensity, 80);
        ctx.fillStyle = rgba(s.emissive, 0.9);
        tracePath(s, p);
        ctx.fill();
        ctx.fill();
        ctx.restore();
      });

      shapes.forEach(function (s) {
        var p = toScreen(s.x, s.y);
        var active = s.path === selected || s === hover;
        ctx.fillStyle = s.dim ? rgba(DIM_COLOR, s.a * 0.25) : rgba(s);
        ctx.strokeStyle = active ? c.accentStrong : s.dim ? "rgba(0,0,0,0.12)" : "rgba(0,0,0,0.3)";
        ctx.lineWidth = active ? 2 : 1;
        tracePath(s, p);
        ctx.fill();
        ctx.stroke();
      });

      if (shapes.length <= LABEL_LIMIT) {
        ctx.font = "12px " + c.mono;
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        shapes.forEach(function (s) {
          if (!s.name) return;
          var tw = ctx.measureText(s.name).width + 10;
          var fitsW = isRound(s) ? radiusPx(s) * 2 : s.w * cam.zoom;
          var fitsH = isRound(s) ? radiusPx(s) * 2 : s.h * cam.zoom;
          if (fitsW < tw || fitsH < 18) return;
          var p = toScreen(s.x, s.y);
          var bright = s.dim || (s.r * 299 + s.g * 587 + s.b * 114) / 1000 > 140 || s.a < 0.5;
          ctx.fillStyle = s.dim ? "rgba(0,0,0,0.35)" : bright ? "rgba(0,0,0,0.75)" : "rgba(255,255,255,0.9)";
          ctx.fillText(s.name, p.x, p.y);
        });
      }

      ctx.font = "11px " + c.mono;
      ctx.fillStyle = c.muted;
      ctx.textAlign = "right";
      ctx.textBaseline = "bottom";
      ctx.fillText("grid " + step, width - 10, height - 8);
    }

    function hitTest(lx, ly) {
      var w = toWorld(lx, ly);
      for (var i = shapes.length - 1; i >= 0; i--) {
        var s = shapes[i];
        if (isRound(s)) {
          var dx = w.x - s.x, dy = w.y - s.y;
          var r = radiusPx(s) / cam.zoom;
          if (dx * dx + dy * dy <= r * r) return s;
        } else if (Math.abs(w.x - s.x) <= s.w / 2 && Math.abs(w.y - s.y) <= s.h / 2) {
          return s;
        }
      }
      return null;
    }

    function setCursor() {
      host.style.cursor = drag ? "grabbing" : hover ? "pointer" : "grab";
    }

    host.addEventListener("pointerdown", function (e) {
      if (e.button !== 0) return;
      drag = { sx: e.clientX, sy: e.clientY, cx: cam.x, cy: cam.y, moved: false };
      host.setPointerCapture(e.pointerId);
      setCursor();
    });

    host.addEventListener("pointermove", function (e) {
      var p = localPoint(host, e);
      if (drag) {
        var dx = e.clientX - drag.sx, dy = e.clientY - drag.sy;
        if (Math.abs(dx) + Math.abs(dy) > 3) drag.moved = true;
        cam.x = drag.cx - dx / cam.zoom;
        cam.y = drag.cy + dy / cam.zoom;
        chrome.hideTip();
        render();
        return;
      }
      var s = hitTest(p.x, p.y);
      if (s !== hover) { hover = s; render(); setCursor(); }
      if (s) chrome.showTip(s.path || s.name || "", p.x, p.y);
      else chrome.hideTip();
    });

    function endDrag(e) {
      if (!drag) return;
      var moved = drag.moved;
      drag = null;
      try { host.releasePointerCapture(e.pointerId); } catch (err) {}
      setCursor();
      if (!moved) {
        var p = localPoint(host, e);
        var s = hitTest(p.x, p.y);
        onSelect(s && s.path ? s.path : null);
      }
    }

    host.addEventListener("pointerup", endDrag);
    host.addEventListener("pointercancel", endDrag);
    host.addEventListener("pointerleave", function () {
      if (hover) { hover = null; render(); }
      chrome.hideTip();
      setCursor();
    });

    host.addEventListener("wheel", function (e) {
      e.preventDefault();
      var p = localPoint(host, e);
      var w = toWorld(p.x, p.y);
      cam.zoom = clamp(cam.zoom * Math.exp(-e.deltaY * 0.0015), 1e-4, 1e6);
      cam.x = w.x - (p.x - width / 2) / cam.zoom;
      cam.y = w.y + (p.y - height / 2) / cam.zoom;
      render();
    }, { passive: false });

    setCursor();

    return {
      host: host,
      update: function (list) {
        shapes = list;
        chrome.empty.hidden = !!shapes.length;
        if (hover && shapes.indexOf(hover) === -1) hover = null;
        if (!shapes.length) fitted = false;
        if (!fitted) fit();
        render();
      },
      setSelected: function (path) { selected = path; render(); },
      fit: fit,
      reset: function () { fitted = false; },
      resize: function () {
        var r = host.getBoundingClientRect();
        if (!r.width || !r.height) return;
        dpr = window.devicePixelRatio || 1;
        width = r.width;
        height = r.height;
        canvas.width = Math.round(width * dpr);
        canvas.height = Math.round(height * dpr);
        if (!fitted) fit();
        render();
      },
      render: render
    };
  }

  /* 3D view */

  var threeLoading = null;

  function loadScripts(urls, cb) {
    if (!urls.length) { cb(true); return; }
    var script = el("script", { src: urls[0] });
    script.onload = function () { loadScripts(urls.slice(1), cb); };
    script.onerror = function () { cb(false); };
    document.head.appendChild(script);
  }

  function loadThree(cb) {
    if (window.THREE && window.THREE.UnrealBloomPass) { cb(true); return; }
    if (threeLoading) { threeLoading.push(cb); return; }
    threeLoading = [cb];
    loadScripts(THREE_SCRIPTS.map(function (f) { return THREE_BASE + f; }), function (ok) {
      var cbs = threeLoading;
      threeLoading = null;
      ok = ok && !!(window.THREE && window.THREE.UnrealBloomPass);
      cbs.forEach(function (f) { f(ok); });
    });
  }

  var COMPOSITE_VERTEX = [
    "varying vec2 vUv;",
    "void main() {",
    "  vUv = uv;",
    "  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);",
    "}"
  ].join("\n");

  var COMPOSITE_FRAGMENT = [
    "uniform sampler2D baseTexture;",
    "uniform sampler2D bloomTexture;",
    "varying vec2 vUv;",
    "void main() {",
    "  gl_FragColor = texture2D(baseTexture, vUv) + vec4(texture2D(bloomTexture, vUv).rgb, 0.0);",
    "  #include <encodings_fragment>",
    "}"
  ].join("\n");

  function createView3D(container, onSelect) {
    var host = el("div", { class: "pg-view pg-view-canvas pg-view-3d" });
    var chrome = canvasChrome(host);
    var status = el("div", { class: "pg-canvas-status", text: "Loading three.js…" });
    host.appendChild(status);
    container.appendChild(host);

    var T = null, renderer, scene, camera, group, grid, shadowPlane, sun, raycaster, mouse;
    var composer, bloomComposer, darkMaterial, blackBg;
    var meshes = {};
    var shapes = [], selected = null, hover = null, fitted = false, hasEmissive = false;
    var orbit = { target: null, radius: 40, theta: 0.7, phi: 1.05 };
    var drag = null, pendingFrame = false, failed = false, visible = false;

    function ensure(cb) {
      if (T) { cb(); return; }
      if (failed) return;
      loadThree(function (ok) {
        if (!ok) {
          failed = true;
          status.textContent = "The 3D view could not be loaded because three.js is unavailable.";
          return;
        }
        T = window.THREE;
        setup();
        cb();
      });
    }

    function setup() {
      renderer = new T.WebGLRenderer({ antialias: true });
      renderer.setPixelRatio(window.devicePixelRatio || 1);
      renderer.outputEncoding = T.sRGBEncoding;
      renderer.shadowMap.enabled = true;
      renderer.shadowMap.type = T.PCFSoftShadowMap;
      renderer.domElement.className = "pg-canvas";
      host.insertBefore(renderer.domElement, host.firstChild);
      scene = new T.Scene();
      camera = new T.PerspectiveCamera(50, 1, 0.01, 1e6);
      orbit.target = new T.Vector3(0, 0, 0);
      scene.add(new T.HemisphereLight(0xffffff, 0x666666, 0.55));
      sun = new T.DirectionalLight(0xffffff, 0.85);
      sun.castShadow = true;
      sun.shadow.mapSize.set(2048, 2048);
      sun.shadow.bias = -0.0004;
      sun.shadow.normalBias = 0.02;
      scene.add(sun);
      scene.add(sun.target);
      shadowPlane = new T.Mesh(new T.PlaneGeometry(1, 1), new T.ShadowMaterial({ opacity: 0.3 }));
      shadowPlane.rotation.x = -Math.PI / 2;
      shadowPlane.receiveShadow = true;
      scene.add(shadowPlane);
      group = new T.Group();
      scene.add(group);
      raycaster = new T.Raycaster();
      mouse = new T.Vector2();
      setupBloom();
      status.hidden = true;
      applyTheme();
      resize();
      applyShapes();
    }

    function setupBloom() {
      var renderPass = new T.RenderPass(scene, camera);
      bloomComposer = new T.EffectComposer(renderer);
      bloomComposer.renderToScreen = false;
      bloomComposer.addPass(renderPass);
      bloomComposer.addPass(new T.UnrealBloomPass(new T.Vector2(1, 1), 1.2, 0.5, 0));
      var composite = new T.ShaderPass(new T.ShaderMaterial({
        uniforms: {
          baseTexture: { value: null },
          bloomTexture: { value: bloomComposer.renderTarget2.texture }
        },
        vertexShader: COMPOSITE_VERTEX,
        fragmentShader: COMPOSITE_FRAGMENT
      }), "baseTexture");
      composite.needsSwap = true;
      composer = new T.EffectComposer(renderer);
      composer.addPass(renderPass);
      composer.addPass(composite);
      darkMaterial = new T.MeshBasicMaterial({ color: 0x000000 });
      blackBg = new T.Color(0x000000);
    }

    function applyTheme() {
      if (!T) return;
      var c = theme();
      scene.background = new T.Color(c.bg);
      rebuildGrid();
      requestRender();
    }

    function rebuildGrid() {
      if (grid) {
        scene.remove(grid);
        grid.geometry.dispose();
        grid.material.dispose();
        grid = null;
      }
      var c = theme();
      var b = shapes.length ? bounds(shapes) : { minX: -10, maxX: 10, minY: 0, minZ: -10, maxZ: 10 };
      var extent = Math.max(Math.abs(b.minX), Math.abs(b.maxX), Math.abs(b.minZ), Math.abs(b.maxZ), 1) * 2.4;
      var step = niceStep(extent / 24);
      var gsize = step * Math.ceil(extent / step);
      grid = new T.GridHelper(gsize, Math.round(gsize / step), new T.Color(c.axis), new T.Color(c.grid));
      grid.position.y = shapes.length ? b.minY : 0;
      scene.add(grid);
      shadowPlane.scale.set(gsize, gsize, 1);
      shadowPlane.position.y = grid.position.y - step * 0.01;
    }

    function updateLight() {
      var b = shapes.length ? bounds(shapes) : { minX: -10, maxX: 10, minY: 0, maxY: 10, minZ: -10, maxZ: 10 };
      var cx = (b.minX + b.maxX) / 2, cy = (b.minY + b.maxY) / 2, cz = (b.minZ + b.maxZ) / 2;
      var dx = b.maxX - b.minX, dy = b.maxY - b.minY, dz = b.maxZ - b.minZ;
      var r = Math.max(Math.sqrt(dx * dx + dy * dy + dz * dz) / 2, 1e-3);
      var dir = new T.Vector3(0.6, 1, 0.8).normalize();
      sun.position.set(cx + dir.x * r * 3, cy + dir.y * r * 3, cz + dir.z * r * 3);
      sun.target.position.set(cx, cy, cz);
      sun.target.updateMatrixWorld();
      var cam = sun.shadow.camera;
      cam.left = -r * 1.2; cam.right = r * 1.2; cam.top = r * 1.2; cam.bottom = -r * 1.2;
      cam.near = r * 0.5; cam.far = r * 6;
      cam.updateProjectionMatrix();
    }

    function makeGeometry(s) {
      if (s.kind === "box") return new T.BoxGeometry(s.w, s.h, s.d);
      if (s.kind === "rect") return new T.PlaneGeometry(s.w, s.h);
      if (s.kind === "circle") return new T.CircleGeometry(s.radius, 48);
      return new T.SphereGeometry(s.radius, 32, 24);
    }

    function geometryKey(s) {
      return s.kind + ":" + s.w + ":" + s.h + ":" + s.d + ":" + s.radius;
    }

    function applyMaterial(m) {
      var s = m.userData.shape;
      var mat = m.material;
      var c = theme();
      mat.side = s.kind === "rect" || s.kind === "circle" ? T.DoubleSide : T.FrontSide;
      if (s.dim) mat.color.setRGB(DIM_COLOR.r / 255, DIM_COLOR.g / 255, DIM_COLOR.b / 255).convertSRGBToLinear();
      else mat.color.setRGB(s.r / 255, s.g / 255, s.b / 255).convertSRGBToLinear();
      var opacity = s.dim ? s.a * 0.25 : s.a;
      var transparent = opacity < 1;
      if (mat.transparent !== transparent) {
        mat.transparent = transparent;
        mat.needsUpdate = true;
      }
      mat.opacity = opacity;
      m.castShadow = !s.dim;
      if (s.emissive && !s.dim) {
        mat.emissive.setRGB(s.emissive.r / 255, s.emissive.g / 255, s.emissive.b / 255).convertSRGBToLinear();
        mat.emissiveIntensity = s.emissive.intensity;
      } else {
        mat.emissive.setRGB(0, 0, 0);
        mat.emissiveIntensity = 1;
      }
      if (s.path && s.path === selected) mat.emissive.add(new T.Color(c.accentStrong).multiplyScalar(0.6));
      else if (s === hover) mat.emissive.add(new T.Color(c.accent).multiplyScalar(0.35));
    }

    function applyShapes() {
      if (!T) return;
      var seen = {};
      hasEmissive = false;
      shapes.forEach(function (s) {
        seen[s.key] = true;
        if (s.emissive && !s.dim) hasEmissive = true;
        var m = meshes[s.key];
        var gk = geometryKey(s);
        if (!m) {
          m = new T.Mesh(makeGeometry(s), new T.MeshLambertMaterial());
          m.castShadow = true;
          m.receiveShadow = true;
          m.userData.gk = gk;
          group.add(m);
          meshes[s.key] = m;
        } else if (m.userData.gk !== gk) {
          m.geometry.dispose();
          m.geometry = makeGeometry(s);
          m.userData.gk = gk;
        }
        m.userData.shape = s;
        m.position.set(s.x, s.y, s.z);
        applyMaterial(m);
      });
      Object.keys(meshes).forEach(function (k) {
        if (seen[k]) return;
        var m = meshes[k];
        group.remove(m);
        m.geometry.dispose();
        m.material.dispose();
        delete meshes[k];
      });
      if (hover && shapes.indexOf(hover) === -1) hover = null;
      rebuildGrid();
      updateLight();
      if (!shapes.length) fitted = false;
      if (!fitted && shapes.length) fit();
      requestRender();
    }

    function updateCamera() {
      var sp = Math.sin(orbit.phi);
      camera.position.set(
        orbit.target.x + orbit.radius * sp * Math.sin(orbit.theta),
        orbit.target.y + orbit.radius * Math.cos(orbit.phi),
        orbit.target.z + orbit.radius * sp * Math.cos(orbit.theta));
      camera.near = Math.max(orbit.radius / 1000, 0.001);
      camera.far = orbit.radius * 1000;
      camera.updateProjectionMatrix();
      camera.lookAt(orbit.target);
    }

    function fit() {
      if (!T || !shapes.length) return;
      var b = bounds(shapes);
      orbit.target.set((b.minX + b.maxX) / 2, (b.minY + b.maxY) / 2, (b.minZ + b.maxZ) / 2);
      var dx = b.maxX - b.minX, dy = b.maxY - b.minY, dz = b.maxZ - b.minZ;
      var r = Math.max(Math.sqrt(dx * dx + dy * dy + dz * dz) / 2, 1e-3);
      var vfov = camera.fov * Math.PI / 180;
      var hfov = 2 * Math.atan(Math.tan(vfov / 2) * camera.aspect);
      orbit.radius = r / Math.sin(Math.min(vfov, hfov) / 2) * 1.1;
      fitted = true;
      requestRender();
    }

    function draw() {
      if (!T) return;
      updateCamera();
      if (!hasEmissive) {
        renderer.render(scene, camera);
        return;
      }
      var bg = scene.background;
      scene.background = blackBg;
      grid.visible = false;
      shadowPlane.visible = false;
      var swapped = [];
      group.children.forEach(function (m) {
        if (m.userData.shape.emissive && !m.userData.shape.dim) return;
        swapped.push([m, m.material]);
        m.material = darkMaterial;
      });
      bloomComposer.render();
      swapped.forEach(function (p) { p[0].material = p[1]; });
      grid.visible = true;
      shadowPlane.visible = true;
      scene.background = bg;
      composer.render();
    }

    function requestRender() {
      if (!T || pendingFrame || !visible) return;
      pendingFrame = true;
      requestAnimationFrame(function () {
        pendingFrame = false;
        draw();
      });
    }

    function resize() {
      if (!T) return;
      var r = host.getBoundingClientRect();
      if (!r.width || !r.height) return;
      var dpr = window.devicePixelRatio || 1;
      renderer.setPixelRatio(dpr);
      renderer.setSize(r.width, r.height, false);
      composer.setPixelRatio(dpr);
      composer.setSize(r.width, r.height);
      bloomComposer.setPixelRatio(dpr);
      bloomComposer.setSize(r.width, r.height);
      camera.aspect = r.width / r.height;
      camera.updateProjectionMatrix();
      if (!fitted && shapes.length) fit();
      requestRender();
    }

    function pick(lx, ly) {
      if (!T) return null;
      var w = host.clientWidth, h = host.clientHeight;
      if (!w || !h) return null;
      mouse.set((lx / w) * 2 - 1, -(ly / h) * 2 + 1);
      raycaster.setFromCamera(mouse, camera);
      var hits = raycaster.intersectObjects(group.children, false);
      return hits.length ? hits[0].object.userData.shape : null;
    }

    function refreshHighlights() {
      Object.keys(meshes).forEach(function (k) { applyMaterial(meshes[k]); });
      requestRender();
    }

    function setCursor() {
      host.style.cursor = drag ? (drag.pan ? "move" : "grabbing") : hover ? "pointer" : "grab";
    }

    host.addEventListener("contextmenu", function (e) { e.preventDefault(); });

    host.addEventListener("pointerdown", function (e) {
      if (!T || (e.button !== 0 && e.button !== 2)) return;
      drag = { sx: e.clientX, sy: e.clientY, moved: false, pan: e.button === 2 || e.shiftKey };
      host.setPointerCapture(e.pointerId);
      setCursor();
    });

    host.addEventListener("pointermove", function (e) {
      if (!T) return;
      var p = localPoint(host, e);
      if (drag) {
        var dx = e.clientX - drag.sx, dy = e.clientY - drag.sy;
        drag.sx = e.clientX;
        drag.sy = e.clientY;
        if (Math.abs(dx) + Math.abs(dy) > 0) drag.moved = true;
        if (drag.pan) {
          var scale = 2 * orbit.radius * Math.tan(camera.fov * Math.PI / 360) / Math.max(host.clientHeight, 1);
          var right = new T.Vector3(), up = new T.Vector3();
          camera.matrixWorld.extractBasis(right, up, new T.Vector3());
          orbit.target.addScaledVector(right, -dx * scale).addScaledVector(up, dy * scale);
        } else {
          orbit.theta -= dx * 0.006;
          orbit.phi = clamp(orbit.phi - dy * 0.006, 0.05, Math.PI - 0.05);
        }
        chrome.hideTip();
        requestRender();
        return;
      }
      var s = pick(p.x, p.y);
      if (s !== hover) { hover = s; refreshHighlights(); setCursor(); }
      if (s) chrome.showTip(s.path || s.name || "", p.x, p.y);
      else chrome.hideTip();
    });

    function endDrag(e) {
      if (!drag) return;
      var moved = drag.moved;
      drag = null;
      try { host.releasePointerCapture(e.pointerId); } catch (err) {}
      setCursor();
      if (!moved && e.button === 0) {
        var p = localPoint(host, e);
        var s = pick(p.x, p.y);
        onSelect(s && s.path ? s.path : null);
      }
    }

    host.addEventListener("pointerup", endDrag);
    host.addEventListener("pointercancel", endDrag);
    host.addEventListener("pointerleave", function () {
      if (hover) { hover = null; refreshHighlights(); }
      chrome.hideTip();
      setCursor();
    });

    host.addEventListener("wheel", function (e) {
      if (!T) return;
      e.preventDefault();
      orbit.radius = clamp(orbit.radius * Math.exp(e.deltaY * 0.0015), 1e-3, 1e6);
      requestRender();
    }, { passive: false });

    setCursor();

    return {
      host: host,
      update: function (list) {
        shapes = list;
        chrome.empty.hidden = !!shapes.length;
        if (T) applyShapes();
      },
      setSelected: function (path) { selected = path; if (T) refreshHighlights(); },
      fit: function () { if (T) fit(); },
      reset: function () { fitted = false; },
      resize: function () { if (T) resize(); },
      show: function () {
        visible = true;
        ensure(function () { resize(); requestRender(); });
      },
      hide: function () { visible = false; },
      render: applyTheme
    };
  }

  /* Panel */

  var TABS = [
    { id: "table", label: "Table" },
    { id: "2d", label: "2D" },
    { id: "3d", label: "3D" }
  ];

  function createPanel(opts) {
    var root = el("div", { class: "pg-query-panel" });
    var toggle = el("button", { class: "pg-toggle pg-query-toggle", type: "button", html: CHEVRON, "aria-label": "Toggle query panel" });
    var editorHost = el("div", { class: "pg-query-ace" });
    var count = el("span", { class: "pg-count" });
    var head = el("div", { class: "pg-head pg-query-head" }, [
      toggle,
      el("span", { class: "pg-title", text: "Query" }),
      el("div", { class: "pg-query-field" }, [editorHost]),
      count
    ]);
    var tabButtons = {};
    var tabbar = el("div", { class: "pg-tabs", role: "tablist" });
    TABS.forEach(function (t) {
      var b = el("button", { class: "pg-tab", type: "button", role: "tab", text: t.label });
      b.addEventListener("click", function () { setTab(t.id); });
      tabButtons[t.id] = b;
      tabbar.appendChild(b);
    });
    tabbar.appendChild(el("span", { class: "pg-spacer" }));
    var fitBtn = el("button", { class: "pg-btn pg-btn-small", type: "button", text: "Fit" });
    tabbar.appendChild(fitBtn);
    var views = el("div", { class: "pg-query-views" });
    var errorBox = el("pre", { class: "pg-error" });
    errorBox.hidden = true;
    var body = el("div", { class: "pg-query-body" }, [tabbar, views, errorBox]);
    root.appendChild(head);
    root.appendChild(body);

    var onSelect = function (path) { if (opts.onSelect) opts.onSelect(path); };
    var view = {
      table: createTableView(views, onSelect),
      "2d": createView2D(views, onSelect),
      "3d": createView3D(views, onSelect)
    };

    var activeTab = null;
    var expanded = true;
    var pending = null;
    var loading = false;
    var world = [];
    var shapes = [];
    var matched = null;
    var refitPending = false;

    var editor = ace.edit(editorHost);
    editor.setOptions({
      maxLines: 1,
      minLines: 1,
      showGutter: false,
      highlightActiveLine: false,
      showPrintMargin: false,
      fontSize: 14,
      tabSize: 2,
      cursorStyle: "slim",
      wrap: false,
      placeholder: "Position, Velocity"
    });
    editor.setTheme("ace/theme/flecs-docs");
    editor.session.setMode("ace/mode/flecs-query");
    editor.renderer.setScrollMargin(0, 0, 0, 0);
    editor.commands.addCommand({
      name: "runQuery",
      bindKey: { win: "Enter|Shift-Enter|Ctrl-Enter", mac: "Enter|Shift-Enter|Command-Enter" },
      exec: function () { setExpanded(true); refresh(true); }
    });
    editor.on("paste", function (e) { e.text = e.text.replace(/[\r\n]+/g, " "); });
    editor.session.on("change", function () {
      if (loading) return;
      if (opts.onChange) opts.onChange(editor.getValue());
      setExpanded(true);
      refresh(false);
    });

    function query() {
      return editor.getValue().trim();
    }

    function setCount(text) {
      count.textContent = text;
    }

    function showError(message) {
      errorBox.innerHTML = "";
      errorBox.appendChild(el("span", { class: "pg-error-label", text: "error" }));
      errorBox.appendChild(document.createTextNode(": " + message));
      errorBox.hidden = false;
      count.classList.add("pg-count-error");
      setCount("Error");
      resizeViews();
    }

    function clearError() {
      errorBox.hidden = true;
      count.classList.remove("pg-count-error");
      resizeViews();
    }

    function updateViews() {
      shapes.forEach(function (s) {
        s.dim = !!matched && !(matched.keys[s.key] || (s.path && matched.paths[s.path]));
      });
      view["2d"].update(shapes);
      view["3d"].update(shapes);
    }

    function setMatched(results) {
      matched = { keys: {}, paths: {} };
      results.forEach(function (r) {
        if (r.id !== undefined) matched.keys["#" + r.id] = true;
        var path = resultPath(r);
        if (path) matched.paths[path] = true;
        Object.keys(r.vars || {}).forEach(function (v) {
          if (typeof r.vars[v] === "string") matched.paths[r.vars[v]] = true;
        });
      });
    }

    function showResults(reply, tab) {
      clearError();
      var results = reply.results || [];
      setCount(results.length + (results.length === 1 ? " result" : " results"));
      if (tab === "table") {
        view.table.update(reply);
      } else {
        setMatched(results);
        updateViews();
      }
    }

    function showEmpty() {
      clearError();
      setCount("");
      view.table.update({ results: [] });
      view.table.host.innerHTML = "";
      view.table.host.appendChild(el("div", { class: "pg-empty" }, [
        el("span", { text: "No query entered" })
      ]));
      matched = null;
      updateViews();
    }

    function refresh(immediate) {
      var conn = opts.conn();
      if (!conn || !expanded) return;
      if (pending) { pending.abort(); pending = null; }
      var q = query();
      if (!q) { showEmpty(); return; }
      var tab = activeTab;
      var params = { try: true, entity_ids: true };
      if (tab === "table") { params.values = true; params.field_info = true; }
      else params.fields = false;
      if (!immediate && opts.latencyBudget) params.latency_budget_ms = opts.latencyBudget;
      pending = conn.query(q, params, function (reply) {
        pending = null;
        if (reply && reply.error) showError(reply.error);
        else showResults(reply || {}, tab);
      }, function (reply) {
        pending = null;
        showError(reply && reply.error ? reply.error : "The query could not be evaluated");
      });
    }

    function resizeViews() {
      if (!expanded || !activeTab) return;
      view[activeTab].resize();
    }

    function setTab(id) {
      if (activeTab === id) return;
      var previous = activeTab;
      activeTab = id;
      TABS.forEach(function (t) {
        var active = t.id === id;
        tabButtons[t.id].classList.toggle("pg-active", active);
        tabButtons[t.id].setAttribute("aria-selected", active ? "true" : "false");
        view[t.id].host.hidden = !active;
      });
      fitBtn.hidden = id === "table";
      if (previous === "3d") view["3d"].hide();
      if (id === "3d") view["3d"].show();
      if (opts.onTabChange) opts.onTabChange(id);
      resizeViews();
      if (!previous || (previous === "table") !== (id === "table")) refresh(true);
      else if (id !== "table") view[id].render();
    }

    function setExpanded(v) {
      if (expanded === v) return;
      expanded = v;
      root.classList.toggle("pg-collapsed", !v);
      toggle.setAttribute("aria-expanded", v ? "true" : "false");
      if (opts.onExpandChange) opts.onExpandChange(v);
      if (v) {
        if (activeTab === "3d") view["3d"].show();
        resizeViews();
        refresh(true);
      } else {
        if (pending) { pending.abort(); pending = null; }
        if (activeTab === "3d") view["3d"].hide();
      }
    }

    toggle.addEventListener("click", function () { setExpanded(!expanded); });
    fitBtn.addEventListener("click", function () { if (activeTab !== "table") view[activeTab].fit(); });

    if (window.ResizeObserver) {
      new ResizeObserver(function () { resizeViews(); }).observe(views);
    } else {
      window.addEventListener("resize", resizeViews);
    }

    new MutationObserver(function () {
      view["2d"].render();
      view["3d"].render();
    }).observe(document.documentElement, { attributes: true, attributeFilter: ["class"] });

    root.classList.toggle("pg-collapsed", !expanded);
    toggle.setAttribute("aria-expanded", "true");
    setTab(opts.tab && view[opts.tab] ? opts.tab : "table");
    if (opts.expanded === false) setExpanded(false);

    return {
      root: root,
      head: head,
      body: body,
      refresh: function () { refresh(true); },
      setWorld: function (results) {
        world = results || [];
        shapes = worldShapes(world);
        if (refitPending) {
          refitPending = false;
          view["2d"].reset();
          view["3d"].reset();
        }
        updateViews();
      },
      refit: function () { refitPending = true; },
      setQuery: function (text) {
        loading = true;
        editor.setValue(text || "", -1);
        editor.session.getUndoManager().reset();
        loading = false;
      },
      getQuery: query,
      setSelected: function (path) {
        TABS.forEach(function (t) { view[t.id].setSelected(path); });
      },
      isExpanded: function () { return expanded; },
      setExpanded: setExpanded,
      resize: function () {
        editor.resize();
        resizeViews();
      },
      focus: function () { editor.focus(); }
    };
  }

  window.flecsPlaygroundQuery = { createPanel: createPanel };
})();
