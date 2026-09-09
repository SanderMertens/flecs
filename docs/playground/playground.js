/* "Try it out!" page: a Flecs Script editor backed by a Flecs wasm image, with
 * an entity tree in the sidebar and an inspector in the right pane that update
 * as you type. The editor is the Ace-based editor from the Flecs Explorer; the
 * wasm image is accessed through the explorer's flecs.js client. */
(function () {
  "use strict";

  var WASM_IMAGE = "flecs_playground.wasm";
  var SCRIPT_ENTITY = "playground";
  var STORAGE_KEY = "flecs-playground-code";
  var EXAMPLE_KEY = "flecs-playground-example";
  var EDITOR_LATENCY_BUDGET_MS = 250;
  var WIDE_QUERY = "(min-width: 1024px)";

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

  function store(key, value) {
    try {
      if (value === null) localStorage.removeItem(key);
      else localStorage.setItem(key, value);
    } catch (e) {}
  }

  function load(key) {
    try { return localStorage.getItem(key); } catch (e) { return null; }
  }

  /* Editor (Ace, as used by the Flecs Explorer) */

  function createEditor(container, onChange) {
    var host = el("div", { class: "pg-ace" });
    container.appendChild(host);
    var editor = ace.edit(host);
    editor.setOption("highlightActiveLine", false);
    editor.setOption("tabSize", 2);
    editor.setOption("cursorStyle", "slim");
    editor.setOption("showPrintMargin", false);
    editor.setOption("fontSize", 15);
    editor.setBehavioursEnabled(true);
    editor.setTheme("ace/theme/flecs-docs");
    editor.session.setMode("ace/mode/flecs-script");
    editor.session.setUseSoftTabs(true);
    editor.renderer.setScrollMargin(8, 8, 0, 0);

    var Range = ace.require("ace/range").Range;
    var errorMarker, squiggleMarker;
    var loading = false;

    editor.session.on("change", function () {
      if (!loading) onChange(editor.getValue());
    });

    function clearMarkers() {
      if (errorMarker !== undefined) { editor.session.removeMarker(errorMarker); errorMarker = undefined; }
      if (squiggleMarker !== undefined) { editor.session.removeMarker(squiggleMarker); squiggleMarker = undefined; }
    }

    function setError(error) {
      clearMarkers();
      var match = error ? error.match(/^(\d+): /) : null;
      if (!match) return;
      var row = parseInt(match[1], 10) - 1;
      errorMarker = editor.session.addMarker(new Range(row, 0, row, Infinity), "pg-error-line", "fullLine");

      var lines = error.split("\n");
      var codeLine = lines[1];
      var caretCol = lines.length >= 3 ? lines[2].indexOf("^") : -1;
      if (caretCol === -1 || codeLine === undefined) return;

      var col = caretCol;
      while (col < codeLine.length && /\s/.test(codeLine[col])) col++;
      if (col === codeLine.length) col = caretCol;
      var start = col;
      while (start > 0 && /[A-Za-z0-9_.$]/.test(codeLine[start - 1])) start--;
      var end = col;
      while (end < codeLine.length && /[A-Za-z0-9_.$]/.test(codeLine[end])) end++;
      if (end === start) end = col + 1;
      squiggleMarker = editor.session.addMarker(new Range(row, start, row, end), "pg-error-squiggle", "text");
    }

    return {
      get: function () { return editor.getValue(); },
      set: function (value) {
        loading = true;
        editor.setValue(value, -1);
        editor.session.getUndoManager().reset();
        loading = false;
        clearMarkers();
      },
      setError: setError,
      resize: function (keepCursor) {
        editor.resize();
        if (keepCursor) editor.renderer.scrollCursorIntoView();
      },
      focus: function () { editor.focus(); },
      addCommand: function (name, keys, exec) {
        editor.commands.addCommand({ name: name, bindKey: keys, exec: exec });
      }
    };
  }

  /* Entity tree */

  function kindOf(e, usedAsTag) {
    var tags = e.tags || [];
    var comps = e.components || {};
    if (tags.indexOf("flecs.core.Disabled") !== -1) return { kind: "disabled", hint: "disabled" };
    if (tags.indexOf("flecs.core.Module") !== -1) return { kind: "module", hint: "module" };
    if (tags.indexOf("flecs.core.Prefab") !== -1) return { kind: "prefab", hint: "prefab" };
    if (comps["flecs.core.Component"]) {
      var type = comps["flecs.meta.type"];
      var kind = type && type.kind ? type.kind.replace(/Type$/, "").toLowerCase() : "component";
      return { kind: "component", hint: kind };
    }
    if (usedAsTag) return { kind: "tag", hint: "tag" };
    return { kind: "entity", hint: "" };
  }

  function createTree(container, onSelect) {
    var list = el("div", { class: "pg-tree" });
    container.appendChild(list);
    var expanded = {};
    var selected = null;
    var nodes = {};

    function build(results) {
      var used = {};
      results.forEach(function (e) {
        (e.tags || []).forEach(function (t) { used[t] = true; });
        Object.keys(e.pairs || {}).forEach(function (r) { used[r] = true; });
      });
      nodes = {};
      var roots = [];
      results.forEach(function (e) {
        if (!e.parent && e.name === SCRIPT_ENTITY) return;
        if (e.parent && (e.parent === SCRIPT_ENTITY || e.parent.indexOf(SCRIPT_ENTITY + ".") === 0)) return;
        var path = e.parent ? e.parent + "." + e.name : e.name;
        var info = kindOf(e, used[path]);
        nodes[path] = { name: e.name, path: path, parent: e.parent || null, kind: info.kind,
          hint: info.hint, children: [], id: e.id || 0 };
      });
      Object.keys(nodes).forEach(function (path) {
        var n = nodes[path];
        if (n.parent && nodes[n.parent]) nodes[n.parent].children.push(n);
        else roots.push(n);
      });
      var byId = function (a, b) { return a.id - b.id; };
      roots.sort(byId);
      Object.keys(nodes).forEach(function (path) { nodes[path].children.sort(byId); });
      return roots;
    }

    function isOpen(n) {
      if (expanded.hasOwnProperty(n.path)) return expanded[n.path];
      return false;
    }

    function renderNode(n) {
      var li = el("li");
      var row = el("div", { class: "pg-node", "data-path": n.path, role: "treeitem" });
      var toggle = el("button", { class: "pg-toggle" + (n.children.length ? "" : " pg-leaf"), type: "button", html: CHEVRON, "aria-label": "Toggle" });
      row.appendChild(toggle);
      row.appendChild(el("span", { class: "pg-icon pg-kind-" + n.kind }));
      row.appendChild(el("span", { class: "pg-node-name", text: n.name }));
      if (n.hint) row.appendChild(el("span", { class: "pg-node-hint", text: n.hint }));
      if (n.path === selected) row.classList.add("pg-selected");
      li.appendChild(row);
      var open = isOpen(n);
      if (n.children.length) {
        row.classList.toggle("pg-open", open);
        var ul = el("ul", { role: "group" });
        n.children.forEach(function (c) { ul.appendChild(renderNode(c)); });
        ul.hidden = !open;
        li.appendChild(ul);
        toggle.addEventListener("click", function (e) {
          e.stopPropagation();
          var now = !isOpen(n);
          expanded[n.path] = now;
          ul.hidden = !now;
          row.classList.toggle("pg-open", now);
        });
      }
      row.addEventListener("click", function () { select(n.path); });
      return li;
    }

    function select(path) {
      selected = path;
      list.querySelectorAll(".pg-node").forEach(function (r) {
        r.classList.toggle("pg-selected", r.getAttribute("data-path") === path);
      });
      onSelect(path);
    }

    return {
      update: function (results) {
        var roots = build(results);
        list.innerHTML = "";
        if (!roots.length) {
          list.appendChild(el("div", { class: "pg-empty", text: "No entities yet. Create some in the editor!" }));
        } else {
          var ul = el("ul", { role: "tree" });
          roots.forEach(function (n) { ul.appendChild(renderNode(n)); });
          list.appendChild(ul);
        }
        if (selected && !nodes[selected]) {
          selected = null;
          onSelect(null);
        } else if (selected) {
          onSelect(selected);
        }
        return Object.keys(nodes).length;
      },
      select: select,
      has: function (path) { return !!nodes[path]; }
    };
  }

  /* Inspector */

  function createInspector(container, onNavigate) {
    var body = el("div", { class: "pg-inspector" });
    container.appendChild(body);
    var collapsed = {};

    function empty(text) {
      body.innerHTML = "";
      body.appendChild(el("div", { class: "pg-empty" }, [el("span", { text: text })]));
    }

    function isPrimitiveArray(value) {
      return Array.isArray(value) && value.every(function (v) { return v === null || typeof v !== "object"; });
    }

    function flatten(prefix, value, rows) {
      if (value !== null && typeof value === "object" && !isPrimitiveArray(value)) {
        if (Array.isArray(value)) {
          value.forEach(function (v, i) { flatten(prefix + "[" + i + "]", v, rows); });
        } else {
          Object.keys(value).forEach(function (k) {
            flatten(prefix ? prefix + "." + k : k, value[k], rows);
          });
        }
      } else {
        rows.push({ member: prefix, value: value });
      }
    }

    function typeInfoFor(info, member) {
      var cur = info;
      var parts = member.split(".");
      for (var i = 0; i < parts.length && cur; i++) {
        if (Array.isArray(cur)) cur = cur[1];
        cur = cur ? cur[parts[i]] : null;
      }
      return Array.isArray(cur) ? cur : null;
    }

    function link(text, target) {
      var a = el("a", { href: "#", text: text });
      a.addEventListener("click", function (e) { e.preventDefault(); onNavigate(target); });
      return a;
    }

    function formatNumber(v) {
      if (Number.isInteger(v)) return String(v);
      var r = Math.round(v * 1000) / 1000;
      return Math.abs(r - v) < 1e-9 ? String(r) : v.toPrecision(6).replace(/\.?0+$/, "");
    }

    function scalar(value, kind) {
      if (value === null || value === undefined) return el("span", { class: "pg-v pg-v-null", text: "null" });
      if (typeof value === "boolean") return el("span", { class: "pg-v pg-v-bool", text: String(value) });
      if (typeof value === "number") return el("span", { class: "pg-v pg-v-num", text: formatNumber(value) });
      if (kind === "entity" && value && onNavigate) return el("span", { class: "pg-v pg-v-ent" }, [link(value, value)]);
      if (kind === "text" || kind === "string") return el("span", { class: "pg-v pg-v-str", text: JSON.stringify(value) });
      return el("span", { class: "pg-v", text: String(value) });
    }

    function valueCell(value, tinfo) {
      var td = el("td", { class: "pg-value" });
      var kind = tinfo ? tinfo[0] : null;
      if (isPrimitiveArray(value) && value.length) {
        var wrap = el("span", { class: "pg-v-list" });
        wrap.appendChild(document.createTextNode("["));
        value.forEach(function (v, i) {
          if (i) wrap.appendChild(document.createTextNode(", "));
          wrap.appendChild(scalar(v, null));
        });
        wrap.appendChild(document.createTextNode("]"));
        td.appendChild(wrap);
      } else if (Array.isArray(value)) {
        td.appendChild(el("span", { class: "pg-v pg-v-null", text: "[]" }));
      } else {
        td.appendChild(scalar(value, kind));
      }
      if (tinfo && tinfo[1] && typeof tinfo[1] === "object" && tinfo[1].symbol) {
        td.appendChild(el("span", { class: "pg-unit", text: tinfo[1].symbol }));
      }
      return td;
    }

    function memberCell(member) {
      var td = el("td", { class: "pg-member" });
      if (!member) {
        td.appendChild(el("span", { class: "pg-member-leaf", text: "value" }));
        return td;
      }
      var m = member.match(/^(.*[.\]])([^.\]]*)$/);
      if (m && m[2]) {
        td.appendChild(el("span", { class: "pg-member-prefix", text: m[1] }));
        td.appendChild(el("span", { class: "pg-member-leaf", text: m[2] }));
      } else {
        td.appendChild(el("span", { class: "pg-member-leaf", text: member }));
      }
      return td;
    }

    function shortName(name) {
      var i = name.lastIndexOf(".");
      return i === -1 ? { ns: "", name: name } : { ns: name.slice(0, i + 1), name: name.slice(i + 1) };
    }

    function renderComponent(name, value, info, inheritedFrom) {
      var card = el("div", { class: "pg-comp" });
      var sn = shortName(name);
      var head = el("div", { class: "pg-comp-name", role: "button", tabindex: "0" }, [
        el("span", { class: "pg-comp-chevron", html: CHEVRON }),
        el("span", { class: "pg-comp-label", title: name }, [
          sn.ns ? el("span", { class: "pg-comp-ns", text: sn.ns }) : null,
          el("span", { text: sn.name })
        ])
      ]);
      if (inheritedFrom) {
        head.appendChild(el("span", { class: "pg-inherited" }, [
          el("span", { text: "from " }),
          onNavigate ? link(shortName(inheritedFrom).name, inheritedFrom) : el("span", { text: inheritedFrom })
        ]));
      }
      card.appendChild(head);
      var content;
      if (value === null || value === undefined) {
        content = el("div", { class: "pg-comp-empty", text: "No value" });
      } else {
        var rows = [];
        flatten("", value, rows);
        content = el("table");
        rows.forEach(function (r) {
          var tr = el("tr");
          tr.appendChild(memberCell(r.member));
          tr.appendChild(valueCell(r.value, r.member ? typeInfoFor(info, r.member) : (Array.isArray(info) ? info : null)));
          content.appendChild(tr);
        });
        if (!rows.length) content = el("div", { class: "pg-comp-empty", text: "Empty" });
      }
      card.appendChild(content);
      var key = (inheritedFrom || "") + "|" + name;
      var setCollapsed = function (c) {
        collapsed[key] = c;
        card.classList.toggle("pg-collapsed", c);
        content.hidden = c;
      };
      setCollapsed(!!collapsed[key]);
      head.addEventListener("click", function (e) {
        if (e.target.closest("a")) return;
        setCollapsed(!collapsed[key]);
      });
      head.addEventListener("keydown", function (e) {
        if (e.key === "Enter" || e.key === " ") { e.preventDefault(); setCollapsed(!collapsed[key]); }
      });
      return card;
    }

    function chip(text, rel) {
      var b = el("span", { class: "pg-badge" + (rel ? " pg-badge-pair" : " pg-badge-tag") });
      if (rel) {
        b.appendChild(onNavigate ? link(rel, rel) : el("span", { text: rel }));
        b.appendChild(el("span", { class: "pg-badge-arrow", html: CHEVRON }));
        b.appendChild(onNavigate ? link(text, text) : el("span", { text: text }));
      } else {
        b.appendChild(el("span", { class: "pg-badge-dot" }));
        b.appendChild(onNavigate ? link(text, text) : el("span", { text: text }));
      }
      return b;
    }

    function section(title, children) {
      if (!children.length) return null;
      var s = el("div", { class: "pg-section" });
      s.appendChild(el("p", { class: "pg-section-title" }, [
        el("span", { text: title }),
        el("span", { class: "pg-section-count", text: String(children.length) })
      ]));
      var wrap = el("div", { class: "pg-section-body" });
      children.forEach(function (c) { wrap.appendChild(c); });
      s.appendChild(wrap);
      return s;
    }

    function isHidden(name) {
      return name.indexOf("(flecs.doc.") === 0 || name === "flecs.script.Script";
    }

    function header(path, e) {
      var parts = path.split(".");
      var name = parts.pop();
      var head = el("div", { class: "pg-entity-head" });
      if (parts.length) {
        var crumbs = el("div", { class: "pg-entity-crumbs" });
        parts.forEach(function (p, i) {
          var target = parts.slice(0, i + 1).join(".");
          if (i) crumbs.appendChild(el("span", { class: "pg-crumb-sep", text: "." }));
          crumbs.appendChild(onNavigate ? link(p, target) : el("span", { text: p }));
        });
        head.appendChild(crumbs);
      }
      var info = kindOf(e, false);
      var title = el("div", { class: "pg-entity-title" }, [
        el("span", { class: "pg-icon pg-kind-" + info.kind }),
        el("span", { class: "pg-entity-name", text: name })
      ]);
      if (info.hint) title.appendChild(el("span", { class: "pg-entity-kind", text: info.hint }));
      if (e.id !== undefined && e.id !== null) title.appendChild(el("span", { class: "pg-entity-id", text: "#" + e.id }));
      head.appendChild(title);
      if (e.doc && e.doc.brief) head.appendChild(el("p", { class: "pg-entity-brief", text: e.doc.brief }));
      return head;
    }

    return {
      clear: function () { empty("Select an entity in the tree to inspect it."); },
      error: function (text) { empty(text); },
      show: function (path, e) {
        body.innerHTML = "";
        body.appendChild(header(path, e));

        var tags = (e.tags || []).map(function (t) { return chip(t); });
        var pairs = [];
        Object.keys(e.pairs || {}).forEach(function (rel) {
          if (isHidden(rel)) return;
          var targets = e.pairs[rel];
          (Array.isArray(targets) ? targets : [targets]).forEach(function (t) { pairs.push(chip(t, rel)); });
        });
        var comps = [];
        var info = e.type_info || {};
        Object.keys(e.components || {}).forEach(function (c) {
          if (isHidden(c)) return;
          comps.push(renderComponent(c, e.components[c], info[c]));
        });
        var inherited = [];
        Object.keys(e.inherited || {}).forEach(function (base) {
          var b = e.inherited[base] || {};
          (b.tags || []).forEach(function (t) { tags.push(chip(t)); });
          Object.keys(b.components || {}).forEach(function (c) {
            if (isHidden(c)) return;
            inherited.push(renderComponent(c, b.components[c], (b.type_info || info)[c], base));
          });
        });

        var sections = [
          section("Tags", tags),
          section("Pairs", pairs),
          section("Components", comps.concat(inherited))
        ].filter(Boolean);
        if (!sections.length) {
          body.appendChild(el("div", { class: "pg-empty pg-empty-inline" }, [
            el("span", { text: "This entity has no tags, pairs or components." })
          ]));
        }
        sections.forEach(function (s) { body.appendChild(s); });
      }
    };
  }

  /* Page */

  function unavailable(root, message) {
    root.appendChild(el("div", { class: "pg-unavailable" }, [
      el("p", { text: message })
    ]));
  }

  function init() {
    var root = document.getElementById("fl-playground");
    if (!root) return;

    if (typeof ace === "undefined" || typeof flecs === "undefined") {
      unavailable(root, "The playground could not be loaded because a script dependency (ace.js or flecs.js) is unavailable.");
      return;
    }

    var status = el("span", { class: "pg-status pg-st-busy", text: "Loading" });
    var version = el("span", { class: "pg-version", text: "" });
    version.hidden = true;
    var exampleList = window.FLECS_PLAYGROUND_EXAMPLES || [];
    var defaultExample = exampleList[0] || { title: "", code: "" };
    var examples = el("select", { class: "pg-select", "aria-label": "Load example" });
    examples.appendChild(el("option", { value: "", text: "", hidden: "" }));
    exampleList.forEach(function (ex) {
      examples.appendChild(el("option", { value: ex.title, text: ex.title }));
    });
    var create = el("button", { class: "pg-btn", type: "button", text: "New" });
    var reset = el("button", { class: "pg-btn", type: "button", text: "Reset" });

    var editorPanel = el("div", { class: "pg-editor-panel" }, [
      el("div", { class: "pg-head" }, [
        version, status, el("span", { class: "pg-spacer" }), examples, create, reset
      ])
    ]);
    var errorBox = el("pre", { class: "pg-error" });
    errorBox.hidden = true;

    var count = el("span", { class: "pg-count", text: "" });
    var treePane = el("div", { class: "pg-pane pg-tree-pane" }, [
      el("div", { class: "pg-head" }, [
        el("span", { class: "pg-title", text: "Entities" }), el("span", { class: "pg-spacer" }), count
      ])
    ]);
    var inspectorPane = el("div", { class: "pg-pane pg-inspector-pane" }, [
      el("div", { class: "pg-head" }, [el("span", { class: "pg-title", text: "Inspector" })])
    ]);
    var stack = el("div", { class: "pg-stack" });

    root.appendChild(editorPanel);
    root.appendChild(stack);

    var conn = null;
    var connected = false;
    var runPending = false;
    var pendingRequest = null;

    var inspector = createInspector(inspectorPane, function (path) {
      if (tree.has(path)) tree.select(path);
    });
    inspector.clear();

    var tree = createTree(treePane, function (path) {
      if (!path) { inspector.clear(); return; }
      conn.entity(path, { values: true, type_info: true, inherited: true, doc: true, entity_id: true },
        function (e) { inspector.show(path, e); },
        function (err) { inspector.error(err && err.error ? err.error : "Failed to load entity"); });
    });

    var editor = createEditor(editorPanel, function (value) {
      store(STORAGE_KEY, value);
      run(false);
    });
    editorPanel.appendChild(errorBox);
    editor.addCommand("run", { win: "Ctrl-Enter", mac: "Command-Enter" }, function () { run(true); });

    function setStatus(cls, text) {
      status.className = "pg-status " + cls;
      status.textContent = text;
    }

    function showError(message) {
      editor.setError(message);
      errorBox.innerHTML = "";
      errorBox.appendChild(el("span", { class: "pg-error-label", text: "error" }));
      errorBox.appendChild(document.createTextNode(": " + message));
      var wasHidden = errorBox.hidden;
      errorBox.hidden = false;
      setStatus("pg-st-error", "Error");
      if (wasHidden) editor.resize(true);
    }

    function clearError() {
      editor.setError(null);
      var wasShown = !errorBox.hidden;
      errorBox.hidden = true;
      setStatus("pg-st-ok", "Ready");
      if (wasShown) editor.resize(true);
    }

    function refreshTree() {
      conn.world(function (msg) {
        var data = JSON.parse(msg);
        var n = tree.update(data.results || []);
        count.textContent = n ? n + (n === 1 ? " entity" : " entities") : "";
      }, function () {});
    }

    function run(immediate) {
      if (!connected) { runPending = true; return; }
      runPending = false;
      if (pendingRequest) { pendingRequest.abort(); pendingRequest = null; }
      pendingRequest = conn.scriptUpdate(SCRIPT_ENTITY, editor.get(), {
        try: true,
        latency_budget_ms: immediate ? undefined : EDITOR_LATENCY_BUDGET_MS
      }, function (reply) {
        pendingRequest = null;
        if (reply && reply.error) showError(reply.error);
        else clearError();
        refreshTree();
      }, function (reply) {
        pendingRequest = null;
        showError(reply && reply.error ? reply.error : "Failed to run script");
        refreshTree();
      });
    }

    function loadExample(title, code) {
      editor.set(code);
      examples.value = title;
      store(STORAGE_KEY, code);
      store(EXAMPLE_KEY, title);
      run(true);
      editor.focus();
    }

    examples.addEventListener("change", function () {
      var ex = exampleList.filter(function (e) { return e.title === examples.value; })[0];
      if (ex) loadExample(ex.title, ex.code);
    });

    create.addEventListener("click", function () {
      loadExample("", "");
    });

    reset.addEventListener("click", function () {
      loadExample(defaultExample.title, defaultExample.code);
    });

    var savedCode = load(STORAGE_KEY);
    var savedExample = load(EXAMPLE_KEY);
    editor.set(savedCode !== null ? savedCode : defaultExample.code);
    examples.value = savedCode !== null ? (savedExample || "") : defaultExample.title;

    /* On wide screens the tree lives in the sidebar and the inspector in the
     * right pane; on narrow screens both stack below the editor and the
     * sidebar drawer keeps the documentation navigation. */
    var sidebar = document.getElementById("fl-sidebar");
    var rightPane = document.getElementById("fl-toc");
    var wide = window.matchMedia(WIDE_QUERY);
    function layout() {
      if (wide.matches && sidebar && rightPane) {
        sidebar.appendChild(treePane);
        rightPane.appendChild(inspectorPane);
      } else {
        stack.appendChild(treePane);
        stack.appendChild(inspectorPane);
      }
      editor.resize();
    }
    if (wide.addEventListener) wide.addEventListener("change", layout);
    else wide.addListener(layout);
    layout();

    conn = flecs.connect({
      host: WASM_IMAGE,
      on_status: function (s) {
        if (s === flecs.ConnectionStatus.Connected) {
          connected = true;
          setStatus("pg-st-ok", "Ready");
          run(true);
        } else if (s === flecs.ConnectionStatus.RetryConnecting) {
          connected = false;
          setStatus("pg-st-busy", "Reconnecting");
        } else if (s === flecs.ConnectionStatus.Disconnected) {
          connected = false;
          setStatus("pg-st-error", "Disconnected");
        }
      },
      on_heartbeat: function (info) {
        var build = info && info.components && info.components["flecs.core.BuildInfo"];
        if (build && build.version && version.hidden) {
          version.textContent = "v" + build.version;
          version.hidden = false;
        }
      }
    });

    editor.focus();
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }
})();
