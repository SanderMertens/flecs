/* Query panel for the "Flecs playground" page. The header holds a single-line
 * query editor (Ace, flecs-query mode from the Flecs Explorer); the body shows
 * the matched entities as a table, in a 2D canvas or in a 3D scene (three.js,
 * loaded on demand). The 2D/3D views draw all entities that have a Position
 * and a Rect, Box, Circle, Sphere or Text component (colored by Color, Rgb,
 * Rgba and Emissive) and grey out the ones the query does not match. */
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
      accent: cssVar("--fl-accent", "#42b983"),
      accentStrong: cssVar("--fl-accent-strong", "#1f8a5a"),
      font: "Inter, Helvetica, sans-serif",
      dark: document.documentElement.classList.contains("dark-mode")
    };
  }

  /* Shapes: Position (x, y, z), Rect (width, height), Box (width, height,
   * depth), Circle/Sphere (radius), Text (value, size, width, align), Color,
   * Rgb/Rgba (u8) and Emissive (r, g, b, intensity) of the entities in the
   * world (GET /world). Entities without geometry or text are not drawn.
   * Positions of children are relative to their parents. Text wraps at width
   * (world units) and align (Left, Center, Right) both anchors the text box at
   * the position and aligns the lines inside it. Color is either a struct with
   * r, g, b (a) members, or a color name (an enum constant or a string, case
   * insensitive: any CSS color name or a #rgb, #rgba, #rrggbb or #rrggbbaa
   * hex value). */

  var TEXT_ADVANCE = 0.6;
  var TEXT_LINE = 1.2;
  var TEXT_ALIGN = { left: -1, center: 0, right: 1 };

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

  function stringMember(obj, names, index, fallback, skip) {
    if (typeof obj === "string") return obj;
    if (!obj || typeof obj !== "object") return fallback;
    for (var i = 0; i < names.length; i++) {
      if (typeof obj[names[i]] === "string") return obj[names[i]];
    }
    var strs = Object.keys(obj).filter(function (k) { return (skip || []).indexOf(k) === -1; }).map(function (k) {
      return obj[k];
    }).filter(function (v) { return typeof v === "string"; });
    return index >= 0 && index < strs.length ? strs[index] : fallback;
  }

  function textWidth(text, size) {
    return text.length * size * TEXT_ADVANCE;
  }

  function textAlign(v) {
    if (typeof v === "number") return v < 0 ? -1 : v > 0 ? 1 : 0;
    var a = typeof v === "string" ? TEXT_ALIGN[v.toLowerCase()] : undefined;
    return a === undefined ? 0 : a;
  }

  function wrapText(text, size, width) {
    var lines = [];
    var max = width > 0 ? Math.max(Math.floor(width / (size * TEXT_ADVANCE)), 1) : 0;
    text.split("\n").forEach(function (para) {
      if (!max) { lines.push(para); return; }
      var start = lines.length, line = "";
      para.split(" ").forEach(function (word) {
        while (word.length > max) {
          if (line) { lines.push(line); line = ""; }
          lines.push(word.slice(0, max));
          word = word.slice(max);
        }
        var cand = line ? line + " " + word : word;
        if (cand.length <= max) line = cand;
        else { lines.push(line); line = word; }
      });
      if (line || lines.length === start) lines.push(line);
    });
    return lines;
  }

  function textLineY(s, i) {
    return s.h / 2 - s.size / 2 - i * s.size * TEXT_LINE;
  }

  function textLineX(s) {
    return s.align * s.w / 2;
  }

  function channel(v) {
    return clamp(Math.round(v), 0, 255);
  }

  var colorCache = {};
  var colorCtx = null;

  function cssColor(name) {
    if (!colorCtx) {
      var canvas = document.createElement("canvas");
      canvas.width = canvas.height = 1;
      colorCtx = canvas.getContext("2d");
    }
    colorCtx.fillStyle = "#000000";
    colorCtx.fillStyle = name;
    var dark = colorCtx.fillStyle;
    colorCtx.fillStyle = "#ffffff";
    colorCtx.fillStyle = name;
    if (dark !== colorCtx.fillStyle) return null;
    var m = /^#([0-9a-f]{6})$/.exec(dark);
    if (m) return { r: parseInt(m[1].slice(0, 2), 16), g: parseInt(m[1].slice(2, 4), 16), b: parseInt(m[1].slice(4, 6), 16), a: 1 };
    m = /^rgba?\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*(?:,\s*([\d.]+)\s*)?\)$/.exec(dark);
    if (m) return { r: +m[1], g: +m[2], b: +m[3], a: m[4] !== undefined ? +m[4] : 1 };
    return null;
  }

  function parseColor(v) {
    if (typeof v !== "string") return null;
    var name = v.trim().toLowerCase();
    if (!name) return null;
    if (Object.prototype.hasOwnProperty.call(colorCache, name)) return colorCache[name];
    var c = null;
    var hex = /^#?([0-9a-f]{3,8})$/.exec(name);
    if (hex && (hex[1].length === 3 || hex[1].length === 4 || hex[1].length === 6 || hex[1].length === 8)) {
      var h = hex[1];
      if (h.length <= 4) h = h.split("").map(function (ch) { return ch + ch; }).join("");
      c = {
        r: parseInt(h.slice(0, 2), 16), g: parseInt(h.slice(2, 4), 16), b: parseInt(h.slice(4, 6), 16),
        a: h.length === 8 ? parseInt(h.slice(6, 8), 16) / 255 : 1
      };
    } else {
      c = cssColor(name) || cssColor(name.replace(/[\s_-]+/g, ""));
    }
    colorCache[name] = c;
    return c;
  }

  function colorOf(v) {
    if (typeof v === "string") return parseColor(v);
    if (!v || typeof v !== "object") return null;
    var hasRgb = typeof v.r === "number" || typeof v.g === "number" || typeof v.b === "number";
    var nums = Object.keys(v).filter(function (k) { return typeof v[k] === "number" && isFinite(v[k]); });
    if (hasRgb || nums.length >= 3) {
      return {
        r: channel(member(v, ["r", "red"], 0, 0)), g: channel(member(v, ["g", "green"], 1, 0)),
        b: channel(member(v, ["b", "blue"], 2, 0)), a: channel(member(v, ["a", "alpha"], 3, 255)) / 255
      };
    }
    return parseColor(stringMember(v, ["value", "name", "color"], 0, null));
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
      r: 66, g: 185, b: 131, a: 1, kind: null, w: 0, h: 0, d: 0, radius: 0, text: "", size: 0,
      lines: null, align: 0, ox: 0, emissive: null, dim: false
    };
    var color = colorOf(component(comps, "Color"));
    if (color) {
      shape.r = color.r;
      shape.g = color.g;
      shape.b = color.b;
      shape.a = clamp(color.a, 0, 1);
    }
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
    var cylinder = struct(comps, "Cylinder"), cone = struct(comps, "Cone");
    var torus = struct(comps, "Torus"), prism = struct(comps, "TrianglePrism");
    var text = component(comps, "Text");
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
    } else if (cylinder || cone) {
      var round = cylinder || cone;
      shape.kind = cylinder ? "cylinder" : "cone";
      shape.radius = member(round, ["radius", "r"], -1, 0.5);
      shape.h = member(round, ["length", "height"], -1, 1);
      shape.w = shape.d = shape.radius * 2;
      shape.segments = clamp(Math.round(member(round, ["segments"], -1, 24)), 3, 128);
    } else if (torus) {
      shape.kind = "torus";
      shape.radius = member(torus, ["radius"], -1, 1);
      shape.tube = member(torus, ["tube"], -1, 0.1);
      shape.w = shape.d = (shape.radius + shape.tube) * 2;
      shape.h = shape.tube * 2;
      shape.segments = clamp(Math.round(member(torus, ["segments"], -1, 32)), 3, 128);
      shape.rings = clamp(Math.round(member(torus, ["rings"], -1, 12)), 3, 64);
    } else if (prism) {
      shape.kind = "prism";
      shape.w = member(prism, ["x", "width"], 0, 1);
      shape.h = member(prism, ["y", "height"], 1, 1);
      shape.d = member(prism, ["z", "depth"], 2, 1);
    } else if (text !== undefined) {
      shape.kind = "text";
      shape.text = stringMember(text, ["value", "text"], 0, "", ["align"]);
      shape.size = member(text, ["size", "height"], -1, 1);
      if (!(shape.size > 0)) shape.size = 1;
      var width = member(text, ["width"], -1, 0);
      shape.align = textAlign(typeof text === "object" ? text.align : undefined);
      shape.lines = wrapText(shape.text, shape.size, width);
      shape.w = width > 0 ? width : Math.max.apply(null, shape.lines.map(function (l) { return textWidth(l, shape.size); }));
      shape.h = shape.size * (1 + (shape.lines.length - 1) * TEXT_LINE);
      shape.ox = -shape.align * shape.w / 2;
    }
    if (!shape.kind) return null;
    if ((shape.kind === "box" || shape.kind === "rect") && !(shape.w > 0 && shape.h > 0)) return null;
    if ((shape.kind === "sphere" || shape.kind === "circle") && !(shape.radius > 0)) return null;
    if (shape.kind === "text" && !shape.text.length) return null;
    if (shape.kind === "box" && !(shape.d > 0)) shape.d = (shape.w + shape.h) / 2;
    if (["cylinder", "cone", "torus", "prism"].indexOf(shape.kind) !== -1 &&
        !(shape.w > 0 && shape.h > 0 && shape.d > 0)) return null;
    if (shape.kind === "torus" && !(shape.radius > 0 && shape.tube > 0)) return null;
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
    function worldTransform(path) {
      if (memo[path]) return memo[path];
      var r = byPath[path];
      var base = r && r.parent ? worldTransform(r.parent) : {
        x: 0, y: 0, z: 0, rotation: [1, 0, 0, 0, 1, 0, 0, 0, 1]
      };
      var comps = r ? r.components || {} : {};
      var pos = struct(comps, "Position"), rot = struct(comps, "Rotation");
      var offset = rotatePoint(base.rotation, {
        x: member(pos, ["x"], 0, 0), y: member(pos, ["y"], 1, 0), z: member(pos, ["z"], 2, 0)
      });
      var rx = member(rot, ["x"], -1, 0), ry = member(rot, ["y"], -1, 0), rz = member(rot, ["z"], -1, 0);
      var a = Math.cos(rx), b = Math.sin(rx), c = Math.cos(ry), d = Math.sin(ry), e = Math.cos(rz), f = Math.sin(rz);
      var local = [c * e, -c * f, d, a * f + b * e * d, a * e - b * f * d, -b * c,
        b * f - a * e * d, b * e + a * f * d, a * c];
      var rotation = [];
      for (var i = 0; i < 3; i++) {
        for (var j = 0; j < 3; j++) {
          rotation[i * 3 + j] = 0;
          for (var k = 0; k < 3; k++) rotation[i * 3 + j] += base.rotation[i * 3 + k] * local[k * 3 + j];
        }
      }
      return memo[path] = { x: base.x + offset.x, y: base.y + offset.y, z: base.z + offset.z, rotation: rotation };
    }
    var shapes = [];
    results.forEach(function (r) {
      if ((r.tags || []).indexOf("flecs.core.Prefab") !== -1) return;
      var s = shapeOf(r);
      if (!s) return;
      var wp = worldTransform(s.path || s.key);
      s.rotation = wp.rotation;
      var offset = rotatePoint(s.rotation, { x: s.ox, y: 0, z: 0 });
      s.x = wp.x + offset.x; s.y = wp.y + offset.y; s.z = wp.z + offset.z;
      shapes.push(s);
    });

    /* Bounds of every entity that has shapes in its subtree, and the parent
     * of every entity, for propagating pointer events up the tree with
     * coordinates relative to each parent. */
    var entities = {};
    function entity(key) {
      if (!entities[key]) {
        var r = byPath[key];
        entities[key] = { parent: r && r.parent ? r.parent : null, minX: Infinity, minY: Infinity, maxX: -Infinity, maxY: -Infinity };
      }
      return entities[key];
    }
    shapes.forEach(function (s) {
      var e = halfExtent(s);
      for (var key = s.path || s.key; key; key = entity(key).parent) {
        var b = entity(key);
        b.minX = Math.min(b.minX, s.x - e.x); b.maxX = Math.max(b.maxX, s.x + e.x);
        b.minY = Math.min(b.minY, s.y - e.y); b.maxY = Math.max(b.maxY, s.y + e.y);
      }
    });
    shapes.entities = entities;
    return shapes;
  }

  function isRound(s) {
    return s.kind === "circle" || s.kind === "sphere";
  }

  function rotatePoint(r, p) {
    return {
      x: r[0] * p.x + r[1] * p.y + r[2] * p.z,
      y: r[3] * p.x + r[4] * p.y + r[5] * p.z,
      z: r[6] * p.x + r[7] * p.y + r[8] * p.z
    };
  }

  function halfExtent(s) {
    if (s.kind === "sphere") return { x: s.radius, y: s.radius, z: s.radius };
    var e = s.kind === "circle" ? { x: s.radius, y: s.radius, z: 0 } : {
      x: s.w / 2, y: s.h / 2, z: s.d / 2
    };
    if (!s.rotation || s.kind === "text") return e;
    return rotatePoint(s.rotation.map(Math.abs), e);
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

  function isEmissive(s) {
    return !s.dim && s.emissive && s.emissive.intensity > 0 &&
      (s.emissive.r > 0 || s.emissive.g > 0 || s.emissive.b > 0);
  }

  function linearChannel(v) {
    v /= 255;
    return v <= 0.04045 ? v / 12.92 : Math.pow((v + 0.055) / 1.055, 2.4);
  }

  function linearShapeColor(s) {
    var em = isEmissive(s) ? s.emissive : { r: 0, g: 0, b: 0, intensity: 0 };
    return {
      r: linearChannel(s.r) + linearChannel(em.r) * em.intensity,
      g: linearChannel(s.g) + linearChannel(em.g) * em.intensity,
      b: linearChannel(s.b) + linearChannel(em.b) * em.intensity,
      a: s.a
    };
  }

  function displayColor(c, scale) {
    function encode(v) {
      v /= scale;
      return channel(255 * (v <= 0.0031308 ? v * 12.92 : 1.055 * Math.pow(v, 1 / 2.4) - 0.055));
    }
    return { r: encode(c.r), g: encode(c.g), b: encode(c.b), a: c.a };
  }

  function emissiveColor(s) {
    return isEmissive(s) ? displayColor(linearShapeColor(s), 1) : s;
  }

  function canvasChrome(host) {
    var tip = el("div", { class: "pg-canvas-tip" });
    tip.hidden = true;
    var empty = el("div", { class: "pg-canvas-empty", text: "No drawable entities" });
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

  /* Pointer events are sent to the script event module with the shape under
   * the pointer as target and the pointer position relative to the lower left
   * corner of that shape; the module derives press, drag, release and click
   * from the button state. A gesture that starts on a shape captures the
   * pointer: the shape stays the target until the button is released. A
   * gesture that starts on empty space pans the camera. */
  function shapeKey(s) {
    return s ? (s.path || s.key) : null;
  }

  /* Emit a pointer event for the shape under the pointer and propagate it to
   * the parents of the shape's entity for as long as the module asks for it,
   * with the pointer position relative to the bounds of each entity. */
  function emitPointer(onMouse, entities, s, e, w) {
    if (!s) {
      onMouse(e, null, w ? w.x : 0, w ? w.y : 0);
      return;
    }
    for (var key = s.path || s.key; key; ) {
      var b = entities[key];
      var x = w.x, y = w.y;
      if (b && b.minX !== Infinity) { x -= b.minX; y -= b.minY; }
      if (!onMouse(e, key, x, y)) return;
      key = b ? b.parent : null;
    }
  }

  function listenKeys(host, input) {
    host.tabIndex = 0;
    host.addEventListener("keydown", function (e) { input.key(e, true); });
    host.addEventListener("keyup", function (e) { input.key(e, false); });
  }

  function createView2D(container, onSelect, input) {
    var host = el("div", { class: "pg-view pg-view-canvas pg-view-2d" });
    var canvas = el("canvas", { class: "pg-canvas" });
    host.appendChild(canvas);
    var chrome = canvasChrome(host);
    container.appendChild(host);
    var ctx = canvas.getContext("2d");
    var cam = { x: 0, y: 0, zoom: 20 };
    var shapes = [], selected = null, hover = null, fitted = false;
    var width = 0, height = 0, dpr = 1, drag = null;
    var bloom = null, bloomLoading = false, bloomFailed = false;

    function ensureBloom() {
      if (bloom || bloomLoading || bloomFailed) return;
      bloomLoading = true;
      loadThree(function (ok) {
        bloomLoading = false;
        if (!ok) { bloomFailed = true; return; }
        try {
          bloom = createCanvasBloom(canvas, host);
        } catch (err) {
          bloomFailed = true;
          return;
        }
        render();
      });
    }

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

    function fillShape(s, p, font) {
      if (s.kind !== "text") {
        tracePath(s, p);
        ctx.fill();
        return;
      }
      ctx.font = s.size * cam.zoom + "px " + font;
      ctx.textAlign = s.align < 0 ? "left" : s.align > 0 ? "right" : "center";
      ctx.textBaseline = "middle";
      var lx = p.x + textLineX(s) * cam.zoom;
      s.lines.forEach(function (line, i) {
        ctx.fillText(line, lx, p.y - textLineY(s, i) * cam.zoom);
      });
    }

    function render() {
      if (!width || !height || host.hidden) return;
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
        if (!s.dim) return;
        var p = toScreen(s.x, s.y);
        if (s.kind === "text") {
          ctx.fillStyle = rgba(DIM_COLOR, s.a * 0.35);
          fillShape(s, p, c.font);
        } else {
          ctx.strokeStyle = rgba(DIM_COLOR, s.a * 0.35);
          tracePath(s, p);
          ctx.stroke();
        }
      });
      shapes.forEach(function (s) {
        if (s.dim) return;
        ctx.fillStyle = rgba(emissiveColor(s));
        fillShape(s, toScreen(s.x, s.y), c.font);
      });

      var active = shapes.some(function (s) { return isEmissive(s) && s.a > 0; });
      if (active) ensureBloom();
      if (!bloom) return;
      bloom.setVisible(active);
      if (!active) return;
      bloom.resize();
      var baseCtx = ctx, scale = 1;
      shapes.forEach(function (s) {
        if (!isEmissive(s)) return;
        var color = linearShapeColor(s);
        scale = Math.max(scale, color.r, color.g, color.b);
      });
      ctx = bloom.context;
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
      ctx.fillStyle = "#000000";
      ctx.fillRect(0, 0, width, height);
      shapes.forEach(function (s) {
        if (s.dim) return;
        ctx.fillStyle = isEmissive(s) ? rgba(displayColor(linearShapeColor(s), scale)) : rgba({ r: 0, g: 0, b: 0, a: s.a });
        fillShape(s, toScreen(s.x, s.y), c.font);
      });
      ctx = baseCtx;
      bloom.render(scale);
    }

    function hitTest(lx, ly) {
      var w = toWorld(lx, ly);
      var dimmed = null;
      for (var i = shapes.length - 1; i >= 0; i--) {
        var s = shapes[i];
        var hit;
        if (isRound(s)) {
          var dx = w.x - s.x, dy = w.y - s.y;
          var r = radiusPx(s) / cam.zoom;
          hit = dx * dx + dy * dy <= r * r;
        } else {
          hit = Math.abs(w.x - s.x) <= s.w / 2 && Math.abs(w.y - s.y) <= s.h / 2;
        }
        if (!hit) continue;
        if (!s.dim) {
          return s;
        }
        if (!dimmed) dimmed = s;
      }
      return dimmed;
    }

    function setCursor() {
      host.style.cursor = drag ? "grabbing" : hover ? "pointer" : "grab";
    }

    host.addEventListener("pointerdown", function (e) {
      if (e.button !== 0) return;
      var p = localPoint(host, e);
      drag = { sx: e.clientX, sy: e.clientY, cx: cam.x, cy: cam.y, moved: false, gesture: null };
      var s = hitTest(p.x, p.y);
      drag.gesture = s;
      input.mouse(e, s, toWorld(p.x, p.y));
      host.setPointerCapture(e.pointerId);
      host.focus({ preventScroll: true });
      setCursor();
    });

    host.addEventListener("pointermove", function (e) {
      var p = localPoint(host, e);
      if (drag) {
        var dx = e.clientX - drag.sx, dy = e.clientY - drag.sy;
        if (Math.abs(dx) + Math.abs(dy) > 3) drag.moved = true;
        if (drag.gesture) {
          input.mouse(e, drag.gesture, toWorld(p.x, p.y));
          chrome.hideTip();
          return;
        }
        cam.x = drag.cx - dx / cam.zoom;
        cam.y = drag.cy + dy / cam.zoom;
        chrome.hideTip();
        render();
        return;
      }
      var s = hitTest(p.x, p.y);
      input.mouse(e, s, toWorld(p.x, p.y));
      if (s !== hover) { hover = s; render(); setCursor(); }
      if (s) chrome.showTip(s.path || s.name || "", p.x, p.y);
      else chrome.hideTip();
    });

    function endDrag(e) {
      if (!drag) return;
      var moved = drag.moved, gesture = drag.gesture;
      drag = null;
      try { host.releasePointerCapture(e.pointerId); } catch (err) {}
      setCursor();
      var p = localPoint(host, e);
      var s = hitTest(p.x, p.y);
      if (gesture) input.mouse(e, gesture, toWorld(p.x, p.y));
      if (!moved) {
        onSelect(s && s.path ? s.path : null);
      }
    }

    host.addEventListener("pointerup", endDrag);
    host.addEventListener("pointercancel", endDrag);
    host.addEventListener("pointerleave", function (e) {
      if (!drag) input.mouse(e, null, null);
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

    listenKeys(host, input);
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
        var changed = width !== r.width || height !== r.height;
        dpr = window.devicePixelRatio || 1;
        width = r.width;
        height = r.height;
        canvas.width = Math.round(width * dpr);
        canvas.height = Math.round(height * dpr);
        if (!fitted || changed) fit();
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
    "#include <common>",
    "#include <dithering_pars_fragment>",
    "uniform sampler2D baseTexture;",
    "uniform sampler2D bloomTexture;",
    "varying vec2 vUv;",
    "void main() {",
    "  gl_FragColor = sRGBToLinear(texture2D(baseTexture, vUv)) + vec4(texture2D(bloomTexture, vUv).rgb, 0.0);",
    "  #include <encodings_fragment>",
    "  #include <dithering_fragment>",
    "}"
  ].join("\n");

  function createBloomPipeline(renderer, scene, camera) {
    var T = window.THREE;
    var renderPass = new T.RenderPass(scene, camera);
    var bloomComposer = new T.EffectComposer(renderer);
    bloomComposer.renderToScreen = false;
    bloomComposer.addPass(renderPass);
    var bloom = new T.UnrealBloomPass(new T.Vector2(1, 1), 1.2, 0.5, 0);
    bloomComposer.addPass(bloom);
    var composite = new T.ShaderPass(new T.ShaderMaterial({
      uniforms: {
        baseTexture: { value: null },
        bloomTexture: { value: bloomComposer.renderTarget2.texture }
      },
      vertexShader: COMPOSITE_VERTEX,
      fragmentShader: COMPOSITE_FRAGMENT,
      dithering: true
    }), "baseTexture");
    composite.needsSwap = true;
    var composer = new T.EffectComposer(renderer);
    composer.renderTarget1.texture.encoding = T.sRGBEncoding;
    composer.renderTarget2.texture.encoding = T.sRGBEncoding;
    composer.addPass(renderPass);
    composer.addPass(composite);
    var halfFloat = renderer.capabilities.isWebGL2
      ? renderer.extensions.has("EXT_color_buffer_float")
      : renderer.extensions.has("OES_texture_half_float") &&
        renderer.extensions.has("OES_texture_half_float_linear") &&
        renderer.extensions.has("EXT_color_buffer_half_float");
    if (halfFloat) {
      [composer.renderTarget1, composer.renderTarget2,
        bloomComposer.renderTarget1, bloomComposer.renderTarget2,
        bloom.renderTargetBright].concat(bloom.renderTargetsHorizontal, bloom.renderTargetsVertical).forEach(function (target) {
        target.texture.type = T.HalfFloatType;
      });
    }
    return { composer: composer, bloomComposer: bloomComposer };
  }

  function createCanvasBloom(canvas, host) {
    var T = window.THREE;
    var renderer = new T.WebGLRenderer();
    renderer.outputEncoding = T.sRGBEncoding;
    renderer.domElement.className = "pg-canvas";
    renderer.domElement.style.position = "absolute";
    renderer.domElement.style.inset = "0";
    renderer.domElement.style.pointerEvents = "none";
    renderer.domElement.style.display = "none";
    var emission = document.createElement("canvas");
    var context = emission.getContext("2d");
    var baseTexture = new T.CanvasTexture(canvas);
    var emissionTexture = new T.CanvasTexture(emission);
    [baseTexture, emissionTexture].forEach(function (texture) {
      texture.encoding = T.sRGBEncoding;
      texture.minFilter = T.LinearFilter;
      texture.generateMipmaps = false;
    });
    var scene = new T.Scene();
    var camera = new T.OrthographicCamera(-1, 1, 1, -1, 0, 1);
    var material = new T.MeshBasicMaterial({ map: baseTexture, depthTest: false, depthWrite: false });
    scene.add(new T.Mesh(new T.PlaneGeometry(2, 2), material));
    var pipeline = createBloomPipeline(renderer, scene, camera);
    host.insertBefore(renderer.domElement, canvas);
    return {
      context: context,
      setVisible: function (visible) {
        canvas.style.visibility = visible ? "hidden" : "";
        renderer.domElement.style.display = visible ? "block" : "none";
      },
      resize: function () {
        if (emission.width === canvas.width && emission.height === canvas.height) return;
        emission.width = canvas.width;
        emission.height = canvas.height;
        renderer.setSize(canvas.width, canvas.height, false);
        pipeline.composer.setSize(canvas.width, canvas.height);
        pipeline.bloomComposer.setSize(canvas.width, canvas.height);
      },
      render: function (scale) {
        emissionTexture.needsUpdate = true;
        material.map = emissionTexture;
        material.color.setScalar(scale);
        pipeline.bloomComposer.render();
        baseTexture.needsUpdate = true;
        material.map = baseTexture;
        material.color.setScalar(1);
        pipeline.composer.render();
      }
    };
  }

  function createView3D(container, onSelect, input) {
    var host = el("div", { class: "pg-view pg-view-canvas pg-view-3d" });
    var chrome = canvasChrome(host);
    var status = el("div", { class: "pg-canvas-status", text: "Loading three.js…" });
    host.appendChild(status);
    container.appendChild(host);

    var T = null, renderer, scene, camera, group, grid, shadowPlane, sun, raycaster, mouse;
    var composer, bloomComposer, darkMaterial, blackBg;
    var meshes = {};
    var shapes = [], selected = null, hover = null, fitted = false, hasEmissive = false;
    var viewW = 0, viewH = 0;
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
      applyShapes();
      resize();
    }

    function setupBloom() {
      var pipeline = createBloomPipeline(renderer, scene, camera);
      composer = pipeline.composer;
      bloomComposer = pipeline.bloomComposer;
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
      if (s.kind === "cylinder" || s.kind === "cone") {
        return new T.CylinderGeometry(s.kind === "cone" ? 0 : s.radius, s.radius, s.h, s.segments);
      }
      if (s.kind === "torus") {
        return new T.TorusGeometry(s.radius, s.tube, s.rings, s.segments).rotateX(Math.PI / 2);
      }
      if (s.kind === "prism") {
        var triangle = new T.Shape();
        triangle.moveTo(-s.w / 2, -s.h / 2);
        triangle.lineTo(s.w / 2, -s.h / 2);
        triangle.lineTo(0, s.h / 2);
        triangle.closePath();
        return new T.ExtrudeGeometry(triangle, { depth: s.d, bevelEnabled: false, steps: 1 }).translate(0, 0, -s.d / 2);
      }
      return new T.SphereGeometry(s.radius, 32, 24);
    }

    function makeTextTexture(s) {
      var px = 64, scale = px / s.size;
      var canvas = document.createElement("canvas");
      canvas.width = Math.max(Math.ceil(s.w * scale), 1);
      canvas.height = Math.max(Math.ceil(s.h * scale), 1);
      var tctx = canvas.getContext("2d");
      tctx.font = px + "px " + theme().font;
      tctx.textAlign = s.align < 0 ? "left" : s.align > 0 ? "right" : "center";
      tctx.textBaseline = "middle";
      tctx.fillStyle = "#ffffff";
      var lx = canvas.width / 2 + textLineX(s) * scale;
      s.lines.forEach(function (line, i) {
        tctx.fillText(line, lx, canvas.height / 2 - textLineY(s, i) * scale);
      });
      var tex = new T.CanvasTexture(canvas);
      tex.minFilter = T.LinearFilter;
      tex.encoding = T.sRGBEncoding;
      return tex;
    }

    function makeObject(s) {
      if (s.kind !== "text") {
        var m = new T.Mesh(makeGeometry(s), new T.MeshLambertMaterial());
        m.castShadow = true;
        m.receiveShadow = true;
        return m;
      }
      var sprite = new T.Sprite(new T.SpriteMaterial({ map: makeTextTexture(s), transparent: true }));
      sprite.scale.set(s.w, s.h, 1);
      return sprite;
    }

    function disposeObject(m) {
      if (m.geometry) m.geometry.dispose();
      if (m.material.map) m.material.map.dispose();
      m.material.dispose();
    }

    function geometryKey(s) {
      return s.kind + ":" + s.w + ":" + s.h + ":" + s.d + ":" + s.radius + ":" + s.tube + ":" + s.segments + ":" + s.rings + ":" + s.align + ":" + s.text;
    }

    function applyMaterial(m, c) {
      var s = m.userData.shape;
      var mat = m.material;
      mat.depthWrite = !s.dim;
      if (s.kind === "text") {
        if (s.dim) mat.color.setRGB(DIM_COLOR.r / 255, DIM_COLOR.g / 255, DIM_COLOR.b / 255);
        else mat.color.setRGB(s.r / 255, s.g / 255, s.b / 255);
        if (s.path && s.path === selected) mat.color.lerp(new T.Color(c.accentStrong), 0.6);
        else if (s === hover) mat.color.lerp(new T.Color(c.accent), 0.35);
        mat.color.convertSRGBToLinear();
        mat.opacity = s.dim ? s.a * 0.25 : s.a;
        return;
      }
      mat.side = s.kind === "rect" || s.kind === "circle" ? T.DoubleSide : T.FrontSide;
      mat.wireframe = s.dim;
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
      var c = theme();
      hasEmissive = false;
      shapes.forEach(function (s) {
        seen[s.key] = true;
        if (isEmissive(s)) hasEmissive = true;
        var m = meshes[s.key];
        var gk = geometryKey(s);
        if (m && m.userData.gk !== gk && (s.kind === "text") !== (m.userData.shape.kind === "text")) {
          group.remove(m);
          disposeObject(m);
          m = null;
        }
        if (!m) {
          m = makeObject(s);
          m.userData.gk = gk;
          group.add(m);
          meshes[s.key] = m;
        } else if (m.userData.gk !== gk) {
          if (s.kind === "text") {
            m.material.map.dispose();
            m.material.map = makeTextTexture(s);
            m.scale.set(s.w, s.h, 1);
          } else {
            m.geometry.dispose();
            m.geometry = makeGeometry(s);
          }
          m.userData.gk = gk;
        }
        m.userData.shape = s;
        m.position.set(s.x, s.y, s.z);
        if (s.kind !== "text") {
          var r = s.rotation;
          m.quaternion.setFromRotationMatrix(new T.Matrix4().set(
            r[0], r[1], r[2], 0, r[3], r[4], r[5], 0, r[6], r[7], r[8], 0, 0, 0, 0, 1));
        }
        applyMaterial(m, c);
      });
      Object.keys(meshes).forEach(function (k) {
        if (seen[k]) return;
        var m = meshes[k];
        group.remove(m);
        disposeObject(m);
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
      var st = Math.sin(orbit.theta), ct = Math.cos(orbit.theta);
      var sp = Math.sin(orbit.phi), cp = Math.cos(orbit.phi);
      var right = new T.Vector3(ct, 0, -st);
      var up = new T.Vector3(-cp * st, sp, -cp * ct);
      var back = new T.Vector3(sp * st, cp, sp * ct);
      var tanY = Math.tan(camera.fov * Math.PI / 360) * 0.9;
      var tanX = tanY * camera.aspect;
      var minX = Infinity, maxX = -Infinity, minY = Infinity, maxY = -Infinity, maxZ = -Infinity;
      var point = new T.Vector3();
      shapes.forEach(function (s) {
        var m = meshes[s.key];
        var box;
        if (s.kind !== "text") {
          if (!m.geometry.boundingBox) m.geometry.computeBoundingBox();
          box = m.geometry.boundingBox;
        }
        for (var i = 0; i < 8; i++) {
          if (s.kind === "text") {
            point.copy(right).multiplyScalar((i & 1 ? 1 : -1) * s.w / 2);
            point.addScaledVector(up, (i & 2 ? 1 : -1) * s.h / 2);
          } else {
            point.set(i & 1 ? box.max.x : box.min.x,
              i & 2 ? box.max.y : box.min.y, i & 4 ? box.max.z : box.min.z);
            point.applyQuaternion(m.quaternion);
          }
          point.add(m.position).sub(orbit.target);
          var x = point.dot(right), y = point.dot(up), z = point.dot(back);
          minX = Math.min(minX, x - z * tanX);
          maxX = Math.max(maxX, x + z * tanX);
          minY = Math.min(minY, y - z * tanY);
          maxY = Math.max(maxY, y + z * tanY);
          maxZ = Math.max(maxZ, z);
        }
      });
      orbit.radius = Math.max((maxX - minX) / (2 * tanX),
        (maxY - minY) / (2 * tanY), (maxZ + 0.001) / 0.999, 0.001);
      orbit.target.addScaledVector(right, (minX + maxX) / 2);
      orbit.target.addScaledVector(up, (minY + maxY) / 2);
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
      var swapped = [], hidden = [];
      group.children.forEach(function (m) {
        if (isEmissive(m.userData.shape)) return;
        if (m.userData.shape.dim || m.userData.shape.kind === "text") { hidden.push(m); m.visible = false; return; }
        swapped.push([m, m.material]);
        m.material = darkMaterial;
      });
      bloomComposer.render();
      swapped.forEach(function (p) { p[0].material = p[1]; });
      hidden.forEach(function (m) { m.visible = true; });
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
      var changed = viewW !== r.width || viewH !== r.height;
      viewW = r.width;
      viewH = r.height;
      var dpr = window.devicePixelRatio || 1;
      renderer.setPixelRatio(dpr);
      renderer.setSize(r.width, r.height, false);
      composer.setPixelRatio(dpr);
      composer.setSize(r.width, r.height);
      bloomComposer.setPixelRatio(dpr);
      bloomComposer.setSize(r.width, r.height);
      camera.aspect = r.width / r.height;
      camera.updateProjectionMatrix();
      if ((!fitted || changed) && shapes.length) fit();
      requestRender();
    }

    function pickHit(lx, ly) {
      if (!T) return null;
      var w = host.clientWidth, h = host.clientHeight;
      if (!w || !h) return null;
      mouse.set((lx / w) * 2 - 1, -(ly / h) * 2 + 1);
      raycaster.setFromCamera(mouse, camera);
      var hits = raycaster.intersectObjects(group.children, false);
      var hit = hits.find(function (h) { return !h.object.userData.shape.dim; }) || hits[0];
      return hit ? { shape: hit.object.userData.shape, point: hit.point } : null;
    }

    function pick(lx, ly) {
      var hit = pickHit(lx, ly);
      return hit ? hit.shape : null;
    }

    /* World point of the pointer for drag events: the ray's intersection
     * with the plane through the pressed shape that faces the camera. */
    function pointOnShapePlane(lx, ly, shape) {
      var w = host.clientWidth, h = host.clientHeight;
      if (!w || !h) return null;
      mouse.set((lx / w) * 2 - 1, -(ly / h) * 2 + 1);
      raycaster.setFromCamera(mouse, camera);
      var normal = new T.Vector3();
      camera.getWorldDirection(normal);
      var plane = new T.Plane().setFromNormalAndCoplanarPoint(normal, new T.Vector3(shape.x, shape.y, shape.z));
      var out = new T.Vector3();
      return raycaster.ray.intersectPlane(plane, out) ? out : null;
    }

    function refreshHighlights() {
      var c = theme();
      Object.keys(meshes).forEach(function (k) { applyMaterial(meshes[k], c); });
      requestRender();
    }

    function setHover(s) {
      if (s === hover) return;
      var previous = hover;
      hover = s;
      var c = theme();
      if (previous && meshes[previous.key]) applyMaterial(meshes[previous.key], c);
      if (hover && meshes[hover.key]) applyMaterial(meshes[hover.key], c);
      requestRender();
      setCursor();
    }

    function setCursor() {
      host.style.cursor = drag ? (drag.pan ? "move" : "grabbing") : hover ? "pointer" : "grab";
    }

    host.addEventListener("contextmenu", function (e) { e.preventDefault(); });
    listenKeys(host, input);

    host.addEventListener("pointerdown", function (e) {
      if (!T || (e.button !== 0 && e.button !== 2)) return;
      var p = localPoint(host, e);
      drag = { sx: e.clientX, sy: e.clientY, moved: false, pan: e.button === 2 || e.shiftKey, gesture: null };
      if (e.button === 0) {
        var hit = pickHit(p.x, p.y);
        if (hit) {
          drag.gesture = hit.shape;
          input.mouse(e, hit.shape, hit.point);
        }
      }
      host.setPointerCapture(e.pointerId);
      host.focus({ preventScroll: true });
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
        if (drag.gesture) {
          var wp = pointOnShapePlane(p.x, p.y, drag.gesture);
          if (wp) input.mouse(e, drag.gesture, wp);
          chrome.hideTip();
          return;
        }
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
      var hit = pickHit(p.x, p.y);
      var s = hit ? hit.shape : null;
      input.mouse(e, s, hit ? hit.point : null);
      setHover(s);
      if (s) chrome.showTip(s.path || s.name || "", p.x, p.y);
      else chrome.hideTip();
    });

    function endDrag(e) {
      if (!drag) return;
      var moved = drag.moved, gesture = drag.gesture;
      drag = null;
      try { host.releasePointerCapture(e.pointerId); } catch (err) {}
      setCursor();
      var p = localPoint(host, e);
      if (gesture) {
        var wp = pointOnShapePlane(p.x, p.y, gesture);
        if (wp) input.mouse(e, gesture, wp);
      }
      if (!moved && e.button === 0) {
        var s = pick(p.x, p.y);
        onSelect(s && s.path ? s.path : null);
      }
    }

    host.addEventListener("pointerup", endDrag);
    host.addEventListener("pointercancel", endDrag);
    host.addEventListener("pointerleave", function (e) {
      if (!drag) input.mouse(e, null, null);
      setHover(null);
      chrome.hideTip();
      setCursor();
    });

    host.addEventListener("wheel", function (e) {
      if (!T) return;
      e.preventDefault();
      orbit.radius = clamp(orbit.radius * Math.exp(e.deltaY * 0.0015), 1e-3, 1e6);
      requestRender();
    }, { passive: false });

    listenKeys(host, input);
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
      reset: function () {
        fitted = false;
        orbit.theta = 0.7;
        orbit.phi = 1.05;
      },
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
    { id: "2d", label: "2D" },
    { id: "3d", label: "3D" },
    { id: "table", label: "Table" }
  ];
  var DEFAULT_TAB = "2d";

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
    var views = el("div", { class: "pg-query-views" });
    var errorBox = el("pre", { class: "pg-error" });
    errorBox.hidden = true;
    var body = el("div", { class: "pg-query-body" }, [tabbar, views, errorBox]);
    root.appendChild(head);
    root.appendChild(body);

    var onSelect = function (path) { if (opts.onSelect) opts.onSelect(path); };
    /* Pointer and keyboard events on the views are forwarded to the scripts
     * through the playground's input bridge. The mouse callback takes the
     * pointer position in world coordinates and returns true when a script
     * was waiting for the event. */
    var input = {
      mouse: function (e, shape, w) {
        if (!opts.onMouse) return;
        emitPointer(opts.onMouse, shapes.entities || {}, shape, e, w);
      },
      key: function (e, down) { if (opts.onKey) opts.onKey(e, down); }
    };
    var view = {
      table: createTableView(views, onSelect),
      "2d": createView2D(views, onSelect, input),
      "3d": createView3D(views, onSelect, input)
    };

    var activeTab = null;
    var expanded = true;
    var pending = null;
    var loading = false;
    var world = [];
    var shapes = [];
    var matched = null;
    var refitPending = true;

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
      updateTabs();
      setExpanded(true);
      refresh(false);
    });

    function query() {
      return editor.getValue().trim();
    }

    function updateTabs() {
      var hasQuery = !!query();
      tabButtons.table.hidden = !hasQuery;
      if (!hasQuery && activeTab === "table") setTab(DEFAULT_TAB);
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
      if (id === "table" && !query()) id = DEFAULT_TAB;
      if (activeTab === id) return;
      var previous = activeTab;
      activeTab = id;
      TABS.forEach(function (t) {
        var active = t.id === id;
        tabButtons[t.id].classList.toggle("pg-active", active);
        tabButtons[t.id].setAttribute("aria-selected", active ? "true" : "false");
        view[t.id].host.hidden = !active;
      });
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
    setTab(opts.tab && view[opts.tab] ? opts.tab : DEFAULT_TAB);
    updateTabs();
    if (opts.expanded === false) setExpanded(false);

    return {
      root: root,
      head: head,
      body: body,
      refresh: function () { refresh(true); },
      setWorld: function (results) {
        world = results || [];
        shapes = worldShapes(world);
        if (refitPending && shapes.length) {
          refitPending = false;
          if (!query()) {
            setTab(shapes.some(function (s) { return s.d > 0 || s.kind === "sphere"; }) ? "3d" : "2d");
          }
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
        updateTabs();
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
