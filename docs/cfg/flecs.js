/* Flecs documentation theme script. Runs on every page. */
(function () {
  "use strict";

  var CHEVRON = '<svg viewBox="0 0 24 24" width="14" height="14" aria-hidden="true"><path d="M9 6l6 6-6 6" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/></svg>';
  var COPY_ICON = '<svg viewBox="0 0 24 24" width="15" height="15" aria-hidden="true"><rect x="9" y="9" width="11" height="11" rx="2" fill="none" stroke="currentColor" stroke-width="2"/><path d="M5 15V5a1 1 0 0 1 1-1h10" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>';
  var CHECK_ICON = '<svg viewBox="0 0 24 24" width="15" height="15" aria-hidden="true"><path d="M5 12l5 5L20 7" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"/></svg>';

  var KIND_LABELS = {
    f: "func", m: "macro", t: "type", e: "enum", v: "const", d: "var",
    s: "struct", c: "class", u: "union", n: "ns", g: "topic", h: "file", p: "page", x: "example"
  };

  function currentPage() {
    var name = location.pathname.split("/").pop();
    return name || "index.html";
  }

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

  function escapeHtml(s) {
    return String(s).replace(/[&<>"]/g, function (c) {
      return { "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" }[c];
    });
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

  /* Sidebar */

  function renderSidebar(container) {
    if (!container) return;
    var nav = window.FLECS_NAV;
    var page = currentPage();
    var activeLink = null;
    container.innerHTML = "";

    var mobile = el("div", { class: "fl-nav-mobile" });
    mobile.appendChild(el("a", { href: "https://github.com/SanderMertens/flecs", target: "_blank", rel: "noopener", text: "GitHub" }));
    mobile.appendChild(el("a", { href: "https://discord.gg/BEzP5Rgrrp", target: "_blank", rel: "noopener", text: "Discord" }));
    mobile.appendChild(el("a", { href: "https://flecs.dev/explorer", target: "_blank", rel: "noopener", text: "Explorer" }));
    container.appendChild(mobile);

    if (!nav || !nav.sections) {
      container.appendChild(el("div", { class: "fl-nav-empty", text: "Navigation index not generated. Run docs/build.sh." }));
      return;
    }

    function link(item) {
      var attrs = { href: item.href };
      if (item.external) {
        attrs.class = "fl-external";
        attrs.target = "_blank";
        attrs.rel = "noopener";
      }
      var a = el("a", attrs);
      a.textContent = item.title;
      if (!item.external && item.href === page) {
        a.classList.add("fl-active");
        a.setAttribute("aria-current", "page");
        activeLink = a;
      }
      if (item.href.indexOf("#") > 0 && item.href.split("#")[0] === page) {
        a.classList.add("fl-outline-link");
        a.setAttribute("href", "#" + item.href.split("#")[1]);
      }
      return a;
    }

    function containsActive(item) {
      if (!item.children) return item.href.split("#")[0] === page;
      return item.href === page || item.children.some(containsActive);
    }

    function renderItems(items, inOutline) {
      var ul = el("ul");
      items.forEach(function (item) {
        var li = el("li");
        if (item.children && item.children.length) {
          var details = el("details");
          if (item.outline) details.className = "fl-outline";
          if (inOutline) details.open = false;
          else if (item.outline) details.open = false;
          else if (containsActive(item)) details.open = true;
          var summary = el("summary");
          summary.appendChild(link(item));
          summary.appendChild(el("span", { class: "fl-nav-chevron", "aria-hidden": "true", html: CHEVRON }));
          summary.addEventListener("click", function (e) {
            e.preventDefault();
            var target = item.href.split("#");
            if (item.external || target[0] !== page) {
              location.href = item.href;
              return;
            }
            if (target[1]) {
              details.open = !details.open;
              if (location.hash === "#" + target[1]) {
                var anchor = document.getElementById(target[1]);
                if (anchor) anchor.scrollIntoView();
              } else {
                location.hash = target[1];
              }
            } else {
              e.stopPropagation();
              details.open = !details.open;
              if (location.hash) {
                history.pushState(null, "", location.pathname + location.search);
                window.scrollTo(0, 0);
              }
            }
          });
          details.addEventListener("toggle", function () { window.dispatchEvent(new Event("scroll")); });
          details.appendChild(summary);
          details.appendChild(renderItems(item.children, inOutline || !!item.outline));
          li.appendChild(details);
        } else {
          li.appendChild(link(item));
        }
        ul.appendChild(li);
      });
      return ul;
    }

    nav.sections.forEach(function (section) {
      var div = el("div", { class: "fl-nav-section" });
      if (section.title) div.appendChild(el("p", { class: "fl-nav-title", text: section.title }));
      div.appendChild(renderItems(section.items));
      container.appendChild(div);
    });

    if (activeLink) {
      var sidebar = container.closest(".fl-sidebar") || container;
      var top = activeLink.getBoundingClientRect().top - sidebar.getBoundingClientRect().top;
      if (top > sidebar.clientHeight - 80) {
        sidebar.scrollTop = top - sidebar.clientHeight / 2;
      }
    }
  }

  function flatPages() {
    var nav = window.FLECS_NAV;
    var pages = [];
    if (!nav) return pages;
    nav.sections.forEach(function (section) {
      section.items.forEach(function (item) {
        if (item.external || !item.page) return;
        pages.push({ title: item.title, href: item.href, section: section.title });
      });
    });
    return pages;
  }

  function renderPageNav() {
    var container = document.getElementById("fl-pagenav");
    if (!container) return;
    var pages = flatPages();
    var page = currentPage();
    var index = -1;
    pages.forEach(function (p, i) { if (p.href === page) index = i; });
    if (index < 0) return;
    if (index > 0) {
      var prev = pages[index - 1];
      container.appendChild(el("a", { class: "fl-prev", href: prev.href }, [
        el("span", { class: "fl-pagenav-label", text: "Previous" }),
        el("span", { class: "fl-pagenav-title", text: prev.title })
      ]));
    }
    if (index < pages.length - 1) {
      var next = pages[index + 1];
      container.appendChild(el("a", { class: "fl-next", href: next.href }, [
        el("span", { class: "fl-pagenav-label", text: "Next" }),
        el("span", { class: "fl-pagenav-title", text: next.title })
      ]));
    }
  }

  /* Theme */

  function initTheme() {
    var btn = document.querySelector(".fl-theme-btn");
    if (!btn) return;
    btn.addEventListener("click", function () {
      var root = document.documentElement;
      var dark = !root.classList.contains("dark-mode");
      root.classList.toggle("dark-mode", dark);
      root.classList.toggle("light-mode", !dark);
      store("flecs-theme", dark ? "dark" : "light");
    });
  }

  /* Mobile drawer */

  function initDrawer() {
    var btn = document.querySelector(".fl-menu-btn");
    var backdrop = document.querySelector(".fl-backdrop");
    if (!btn) return;
    function setOpen(open) {
      document.body.classList.toggle("fl-sidebar-open", open);
      btn.setAttribute("aria-expanded", open ? "true" : "false");
      if (backdrop) backdrop.hidden = !open;
    }
    btn.addEventListener("click", function () {
      setOpen(!document.body.classList.contains("fl-sidebar-open"));
    });
    if (backdrop) backdrop.addEventListener("click", function () { setOpen(false); });
    document.addEventListener("keydown", function (e) {
      if (e.key === "Escape" && document.body.classList.contains("fl-sidebar-open")) setOpen(false);
    });
    var sidebar = document.getElementById("fl-sidebar");
    if (sidebar) {
      sidebar.addEventListener("click", function (e) {
        var a = e.target.closest("a");
        if (a && a.getAttribute("href") && !a.classList.contains("fl-external")) setOpen(false);
      });
    }
  }

  /* Headings, anchors and table of contents */

  function headingId(h) {
    if (h.id) return h.id;
    var anchor = h.querySelector("a.anchor[id], a[id]");
    if (anchor) return anchor.id;
    var prev = h.previousElementSibling;
    if (prev && prev.tagName === "A" && prev.id) return prev.id;
    return null;
  }

  function headingText(h) {
    var clone = h.cloneNode(true);
    clone.querySelectorAll(".permalink, .fl-anchor, .overload").forEach(function (n) { n.remove(); });
    return clone.textContent.replace(/\s+/g, " ").trim();
  }

  function collectHeadings() {
    var contents = document.querySelector("#doc-content .contents");
    if (!contents) return [];
    var nodes = contents.querySelectorAll("h1.doxsection, h2.doxsection, h3.doxsection, h2.groupheader, h2.memtitle");
    var result = [];
    var lastGroupHeader = null;
    nodes.forEach(function (h) {
      var id = headingId(h);
      if (!id) return;
      var level;
      if (h.classList.contains("groupheader")) {
        level = 1;
        lastGroupHeader = h;
      } else if (h.classList.contains("memtitle")) {
        level = lastGroupHeader ? 2 : 1;
      } else {
        level = parseInt(h.tagName.substring(1), 10);
      }
      var mono = h.classList.contains("memtitle");
      var text = headingText(h);
      if (mono) text = text.replace(/\(\)$/, "");
      result.push({ el: h, id: id, level: level, text: text, mono: mono });
    });
    return result;
  }

  function initHeadings() {
    var contents = document.querySelector("#doc-content .contents");
    if (!contents) return;
    contents.querySelectorAll("h1.doxsection, h2.doxsection, h3.doxsection, h4.doxsection, h2.groupheader, h2.memtitle").forEach(function (h) {
      var id = headingId(h);
      if (!id) return;
      var a = el("a", { class: "fl-anchor", href: "#" + id, "aria-label": "Link to this section", title: "Link to this section" });
      h.appendChild(a);
    });
  }

  function initToc() {
    var toc = document.getElementById("fl-toc");
    if (!toc) return;
    var headings = collectHeadings();
    if (headings.length < 2 || headings.length > 400) return;
    var minLevel = Math.min.apply(null, headings.map(function (h) { return h.level; }));
    toc.appendChild(el("p", { class: "fl-toc-title", text: "On this page" }));
    var ul = el("ul");
    var links = [];
    headings.forEach(function (h) {
      var depth = Math.min(h.level - minLevel + 1, 3);
      var a = el("a", { href: "#" + h.id });
      if (h.mono) a.appendChild(el("code", { text: h.text }));
      else a.textContent = h.text;
      ul.appendChild(el("li", { class: "fl-toc-l" + depth }, [a]));
      links.push({ a: a, target: h.el });
    });
    toc.appendChild(ul);
    var ticking = false;
    function update() {
      ticking = false;
      var offset = 56 + 56;
      var current = null;
      for (var i = 0; i < links.length; i++) {
        var rect = links[i].target.getBoundingClientRect();
        if (rect.top - offset <= 0) current = links[i];
        else break;
      }
      if (!current && links.length) current = links[0];
      links.forEach(function (l) { l.a.classList.toggle("fl-active", l === current); });
      if (current) {
        var r = current.a.getBoundingClientRect();
        var t = toc.getBoundingClientRect();
        if (r.top < t.top + 40 || r.bottom > t.bottom - 40) {
          toc.scrollTop += r.top - t.top - t.height / 2;
        }
      }
    }
    window.addEventListener("scroll", function () {
      if (!ticking) { ticking = true; requestAnimationFrame(update); }
    }, { passive: true });
    update();
  }

  function initOutline() {
    var links = [];
    document.querySelectorAll(".fl-nav a.fl-outline-link").forEach(function (a) {
      var id = decodeURIComponent(a.getAttribute("href").substring(1));
      var target = document.getElementById(id);
      if (!target) return;
      if (target.tagName === "A") target = target.closest("h1,h2,h3,h4,h5,h6") || target.nextElementSibling || target;
      links.push({ a: a, target: target });
    });
    if (!links.length) return;
    var sidebar = document.getElementById("fl-sidebar");
    var ticking = false;
    function update() {
      ticking = false;
      var offset = 56 + 56;
      var current = null;
      for (var i = 0; i < links.length; i++) {
        var rect = links[i].target.getBoundingClientRect();
        if (rect.top - offset <= 0) current = links[i];
        else break;
      }
      if (!current && links.length) current = links[0];
      var visible = null;
      if (current) {
        visible = current.a;
        var node = current.a.parentElement;
        while (node && !node.classList.contains("fl-outline")) {
          if (node.tagName === "DETAILS" && !node.open) {
            visible = node.querySelector(":scope > summary > a");
          }
          node = node.parentElement;
        }
      }
      links.forEach(function (l) { l.a.classList.toggle("fl-current", l.a === visible); });
      if (visible && sidebar) {
        var r = visible.getBoundingClientRect();
        var t = sidebar.getBoundingClientRect();
        if (r.top < t.top + 40 || r.bottom > t.bottom - 40) {
          sidebar.scrollTop += r.top - t.top - t.height / 2;
        }
      }
    }
    window.addEventListener("scroll", function () {
      if (!ticking) { ticking = true; requestAnimationFrame(update); }
    }, { passive: true });
    update();
  }

  /* Language tabs */

  function initTabs() {
    var groups = document.querySelectorAll(".flecs-snippet-tabs");
    if (!groups.length) return;
    var all = [];
    groups.forEach(function (group) {
      var list = group.querySelector(":scope > ul");
      if (!list) return;
      var tabs = [];
      Array.prototype.forEach.call(list.children, function (li) {
        var title = li.querySelector(".tab-title");
        if (!title) return;
        tabs.push({ li: li, name: title.textContent.trim() });
      });
      if (!tabs.length) return;
      var bar = el("div", { class: "fl-tabs-bar", role: "tablist" });
      tabs.forEach(function (tab) {
        tab.button = el("button", { type: "button", role: "tab", text: tab.name });
        tab.button.addEventListener("click", function () {
          store("flecs-tab", tab.name);
          selectAll(tab.name);
        });
        bar.appendChild(tab.button);
      });
      group.insertBefore(bar, list);
      all.push(tabs);
    });

    function selectAll(name) {
      all.forEach(function (tabs) {
        var match = tabs.filter(function (t) { return t.name === name; })[0] || tabs[0];
        tabs.forEach(function (t) {
          var on = t === match;
          t.li.classList.toggle("fl-selected", on);
          t.button.classList.toggle("fl-active", on);
          t.button.setAttribute("aria-selected", on ? "true" : "false");
        });
      });
    }
    selectAll(load("flecs-tab") || "C++");
  }

  /* Code blocks */

  function fragmentText(fragment) {
    var lines = [];
    fragment.querySelectorAll(":scope > div.line").forEach(function (line) {
      var clone = line.cloneNode(true);
      clone.querySelectorAll(".lineno, .fold, a[id]").forEach(function (n) { n.remove(); });
      lines.push(clone.textContent.replace(/ /g, " ").replace(/\s+$/, ""));
    });
    return lines.join("\n");
  }

  function initCopyButtons() {
    if (!navigator.clipboard) return;
    document.querySelectorAll("#doc-content div.fragment").forEach(function (fragment) {
      if (!fragment.querySelector("div.line")) return;
      var btn = el("button", { class: "fl-copy", type: "button", "aria-label": "Copy code", title: "Copy code", html: COPY_ICON });
      btn.addEventListener("click", function () {
        navigator.clipboard.writeText(fragmentText(fragment)).then(function () {
          btn.innerHTML = CHECK_ICON;
          btn.classList.add("fl-copied");
          setTimeout(function () {
            btn.innerHTML = COPY_ICON;
            btn.classList.remove("fl-copied");
          }, 1500);
        });
      });
      fragment.appendChild(btn);
    });
  }

  /* Tables, inherited members, diagrams */

  function initTables() {
    document.querySelectorAll("#doc-content table.markdownTable, #doc-content table.doxtable").forEach(function (table) {
      if (table.parentElement.classList.contains("fl-table-wrap")) return;
      var wrap = el("div", { class: "fl-table-wrap" });
      table.parentNode.insertBefore(wrap, table);
      wrap.appendChild(table);
    });
  }

  function initMemberTables() {
    document.querySelectorAll("#doc-content table.memberdecls").forEach(function (table) {
      var textual = false;
      Array.prototype.forEach.call(table.rows, function (row) {
        if (row.classList.contains("heading")) {
          var anchor = row.querySelector("a[id]");
          var id = anchor ? anchor.id : "";
          textual = id === "groups" || id === "namespaces";
        } else if (textual) {
          row.classList.add("fl-text-row");
        }
      });
    });
  }

  function initInherited() {
    document.querySelectorAll("tr.inherit_header").forEach(function (header) {
      var id = Array.prototype.filter.call(header.classList, function (c) { return c !== "inherit_header"; })[0];
      if (!id) return;
      header.addEventListener("click", function () {
        var open = header.classList.toggle("fl-open");
        header.parentElement.querySelectorAll("tr.inherit." + id).forEach(function (row) {
          row.classList.toggle("fl-open", open);
        });
      });
    });
  }

  function initDiagrams() {
    document.querySelectorAll("#doc-content .contents img[src$='.svg'], #doc-content .contents object[type='image/svg+xml']").forEach(function (img) {
      var src = img.getAttribute("src") || img.getAttribute("data");
      if (!src || /^[a-z]+:/i.test(src)) return;
      fetch(src).then(function (r) { return r.ok ? r.text() : null; }).then(function (text) {
        if (!text) return;
        var doc = new DOMParser().parseFromString(text, "image/svg+xml");
        var svg = doc.documentElement;
        if (!svg || svg.tagName !== "svg") return;
        svg.classList.add("fl-diagram");
        svg.setAttribute("role", "img");
        var viewBox = (svg.getAttribute("viewBox") || "").split(/[\s,]+/);
        var natural = parseFloat(viewBox[2]) || parseFloat(svg.getAttribute("width")) || 0;
        if (natural) svg.style.minWidth = Math.min(Math.round(natural * 0.6), 640) + "px";
        svg.removeAttribute("width");
        svg.removeAttribute("height");
        if (img.alt) svg.setAttribute("aria-label", img.alt);
        var wrap = el("div", { class: "fl-diagram-wrap" });
        wrap.appendChild(document.importNode(svg, true));
        var image = img.closest("div.image") || img;
        image.parentNode.replaceChild(wrap, image);
      }).catch(function () {});
    });
  }

  function initReadme() {
    if (currentPage() !== "index.html") return;
    var contents = document.querySelector("#doc-content .contents");
    if (!contents) return;
    var first = contents.querySelector(".textblock > p, p");
    if (first && first.querySelectorAll("a > img").length >= 3 && !first.textContent.trim()) {
      first.classList.add("fl-badges");
    }
  }

  /* Search */

  var search = {
    index: null,
    loading: null,
    open: false,
    selected: 0,
    results: [],
    modal: null,
    input: null,
    body: null
  };

  function tokenize(text) {
    return (String(text).toLowerCase().match(/[a-z0-9_]+/g) || []);
  }

  function nameTokens(name) {
    var lower = name.toLowerCase();
    var parts = lower.split(/[^a-z0-9]+/).filter(Boolean);
    var camel = name.replace(/([a-z0-9])([A-Z])/g, "$1 $2").toLowerCase().split(/[^a-z0-9]+/).filter(Boolean);
    return parts.concat(camel);
  }

  function coreTokens(name) {
    var camel = name.replace(/([a-z0-9])([A-Z])/g, "$1 $2").toLowerCase().split(/[^a-z0-9]+/).filter(Boolean);
    var i = 0;
    while (i < camel.length - 1 && (camel[i] === "ecs" || camel[i] === "flecs")) i++;
    return camel.slice(i);
  }

  function buildIndex(data) {
    var words = data.words;
    var docs = data.docs.map(function (d) {
      var ids = d.w ? d.w.split(" ") : [];
      var set = {};
      ids.forEach(function (h) { set[words[parseInt(h, 16)]] = true; });
      tokenize(d.t).forEach(function (w) { set[w] = true; });
      return { t: d.t, c: d.c, u: d.u, s: d.s, k: d.k, words: set, title: d.t.toLowerCase(), titleTokens: tokenize(d.t) };
    });
    var compounds = data.compounds.map(function (c) {
      return { t: c.t, k: c.k, c: c.c, u: c.u, s: c.s, name: c.t.toLowerCase(), tokens: nameTokens(c.t), core: coreTokens(c.t), brief: tokenize(c.s) };
    });
    var members = data.members.map(function (m) {
      var parent = compounds[m.p];
      var url = m.u || (parent.u + "#" + m.a);
      var crumb = parent.c ? parent.c + " › " + parent.t : parent.t;
      return { t: m.t, k: m.k, c: crumb, u: url, s: m.s, name: m.t.toLowerCase(), tokens: nameTokens(m.t).concat(nameTokens(parent.t)), core: coreTokens(m.t), brief: tokenize(m.s), parentName: parent.name };
    });
    var wordList = Object.keys(docs.reduce(function (acc, d) {
      Object.keys(d.words).forEach(function (w) { acc[w] = true; });
      return acc;
    }, {})).sort();
    return { docs: docs, api: compounds.concat(members), wordList: wordList };
  }

  function prefixRange(list, prefix) {
    var lo = 0, hi = list.length;
    while (lo < hi) {
      var mid = (lo + hi) >> 1;
      if (list[mid] < prefix) lo = mid + 1; else hi = mid;
    }
    var out = [];
    for (var i = lo; i < list.length && list[i].indexOf(prefix) === 0; i++) out.push(list[i]);
    return out;
  }

  function editDistance(a, b, max) {
    if (Math.abs(a.length - b.length) > max) return max + 1;
    var prev2 = null, prev = [], cur, i, j;
    for (j = 0; j <= b.length; j++) prev[j] = j;
    for (i = 1; i <= a.length; i++) {
      cur = [i];
      var rowMin = i;
      for (j = 1; j <= b.length; j++) {
        var cost = a.charCodeAt(i - 1) === b.charCodeAt(j - 1) ? 0 : 1;
        var v = Math.min(prev[j] + 1, cur[j - 1] + 1, prev[j - 1] + cost);
        if (i > 1 && j > 1 && a.charCodeAt(i - 1) === b.charCodeAt(j - 2) && a.charCodeAt(i - 2) === b.charCodeAt(j - 1)) {
          v = Math.min(v, prev2[j - 2] + 1);
        }
        cur[j] = v;
        if (v < rowMin) rowMin = v;
      }
      if (rowMin > max) return max + 1;
      prev2 = prev;
      prev = cur;
    }
    return prev[b.length];
  }

  function fuzzyBudget(token) {
    return token.length < 4 ? 0 : token.length < 8 ? 1 : 2;
  }

  function fuzzyMatch(token, word) {
    var max = fuzzyBudget(token);
    if (!max) return false;
    if (editDistance(token, word, max) <= max) return true;
    if (word.length > token.length + 1) {
      return editDistance(token, word.slice(0, token.length + 1), max) <= max;
    }
    return false;
  }

  function expandToken(index, token) {
    var prefix = prefixRange(index.wordList, token);
    var seen = {};
    prefix.forEach(function (w) { seen[w] = true; });
    var fuzzy = [];
    if (fuzzyBudget(token)) {
      index.wordList.forEach(function (w) {
        if (!seen[w] && fuzzyMatch(token, w)) fuzzy.push(w);
      });
    }
    return { prefix: prefix, fuzzy: fuzzy };
  }

  function searchDocs(index, tokens, query) {
    var expansions = tokens.map(function (t) { return expandToken(index, t); });
    var results = [];
    index.docs.forEach(function (d) {
      var score = 0;
      var hits = [];
      for (var i = 0; i < tokens.length; i++) {
        var t = tokens[i];
        var exp = expansions[i];
        var best = 0;
        var j;
        if (d.words[t]) {
          best = 3;
          hits.push(t);
        } else {
          for (j = 0; j < exp.prefix.length; j++) {
            if (d.words[exp.prefix[j]]) { best = 1.5; hits.push(exp.prefix[j]); break; }
          }
          if (!best) {
            for (j = 0; j < exp.fuzzy.length; j++) {
              if (d.words[exp.fuzzy[j]]) { best = 1; hits.push(exp.fuzzy[j]); break; }
            }
          }
        }
        if (!best) return;
        if (d.titleTokens.indexOf(t) >= 0) best += 6;
        else if (d.titleTokens.some(function (w) { return w.indexOf(t) === 0; })) best += 3;
        else if (d.titleTokens.some(function (w) { return fuzzyMatch(t, w); })) best += 1.5;
        score += best;
      }
      if (d.title === query) score += 20;
      else if (d.title.indexOf(query) >= 0) score += 8;
      score -= Math.min(d.c.length, 60) / 60;
      results.push({ score: score, item: d, hits: hits });
    });
    results.sort(function (a, b) { return b.score - a.score || a.item.t.length - b.item.t.length; });
    return results;
  }

  function searchApi(index, tokens, query) {
    var compact = query.replace(/[\s:]+/g, "_");
    var results = [];
    index.api.forEach(function (a) {
      var score = 0;
      var hits = [];
      for (var i = 0; i < tokens.length; i++) {
        var t = tokens[i];
        var best = 0;
        var hit = null;
        if (a.name === t) best = 40;
        else if (a.core.length === 1 && a.core[0] === t) best = 30;
        else if (a.core[0] === t) best = 22;
        else if (a.core.indexOf(t) >= 0) best = 14;
        else if (a.tokens.indexOf(t) >= 0) best = 13;
        else if (a.name.indexOf(t) === 0) best = 12;
        else if (a.core[0].indexOf(t) === 0) best = 10;
        else if (a.tokens.some(function (w) { return w.indexOf(t) === 0; })) best = 8;
        else if (a.name.indexOf(t) >= 0) best = 5;
        else if (a.brief.indexOf(t) >= 0) best = 2;
        else if (a.brief.some(function (w) { return w.indexOf(t) === 0; })) best = 1;
        else if (fuzzyBudget(t)) {
          for (var j = 0; j < a.tokens.length; j++) {
            if (fuzzyMatch(t, a.tokens[j])) { best = 6; hit = a.tokens[j]; break; }
          }
          if (!best && editDistance(t, a.name, 2) <= 2) { best = 5; hit = a.name; }
          if (!best) {
            for (j = 0; j < a.brief.length; j++) {
              if (fuzzyMatch(t, a.brief[j])) { best = 0.5; hit = a.brief[j]; break; }
            }
          }
        }
        if (!best) return;
        if (hit) hits.push(hit);
        score += best;
      }
      if (a.name === compact || a.name === query) score += 40;
      else if (a.name.indexOf(compact) >= 0) score += 10;
      else if (a.name.indexOf(query) >= 0) score += 8;
      else if (compact.length >= 6 && editDistance(compact, a.name, 2) <= 2) { score += 6; hits.push(a.name); }
      if (a.k === "g" || a.k === "s" || a.k === "c") score += 2;
      else if (a.k === "d") score -= 1;
      score -= Math.min(a.name.length, 40) / 20;
      results.push({ score: score, item: a, hits: hits });
    });
    results.sort(function (a, b) { return b.score - a.score || a.item.name.length - b.item.name.length; });
    var seen = {};
    return results.filter(function (r) {
      var key = r.item.k + "\u0000" + r.item.t + "\u0000" + r.item.c;
      if (seen[key]) return false;
      seen[key] = true;
      return true;
    });
  }

  function highlight(text, tokens) {
    var safe = escapeHtml(text);
    if (!tokens.length) return safe;
    var pattern = tokens.map(function (t) { return t.replace(/[.*+?^${}()|[\]\\]/g, "\\$&"); })
      .sort(function (a, b) { return b.length - a.length; }).join("|");
    return safe.replace(new RegExp("(" + pattern + ")", "gi"), "<mark>$1</mark>");
  }

  function renderResults(query) {
    var body = search.body;
    body.innerHTML = "";
    search.results = [];
    search.selected = 0;
    var tokens = tokenize(query);
    if (!tokens.length) {
      body.appendChild(el("div", { class: "fl-search-empty", text: "Type to search the manuals and the API reference." }));
      return;
    }
    var q = query.trim().toLowerCase();
    var docs = searchDocs(search.index, tokens, q).slice(0, 6);
    var api = searchApi(search.index, tokens, q).slice(0, 8);
    if (!docs.length && !api.length) {
      body.appendChild(el("div", { class: "fl-search-empty", text: "No results for “" + query + "”." }));
      return;
    }
    var identifierLike = /[_:.()]/.test(q) || /^ecs[a-z]*$/.test(q) || /^flecs/.test(q);
    var apiFirst = api.length && (identifierLike || api[0].score >= 36 || !docs.length);
    function addGroup(title, items, mono) {
      if (!items.length) return;
      body.appendChild(el("div", { class: "fl-search-group", text: title }));
      items.forEach(function (r) {
        var item = r.item;
        var terms = tokens.concat(r.hits || []).filter(function (w, i, arr) { return w.length > 1 && arr.indexOf(w) === i; });
        var a = el("a", { class: "fl-search-item", href: item.u, role: "option" });
        a.appendChild(el("span", { class: "fl-search-item-kind", text: KIND_LABELS[item.k] || (mono ? "api" : "doc") }));
        var b = el("div", { class: "fl-search-item-body" });
        b.appendChild(el("div", { class: "fl-search-item-title" + (mono ? " fl-mono" : ""), html: highlight(item.t, terms) }));
        if (item.c) b.appendChild(el("div", { class: "fl-search-item-crumb", text: item.c }));
        if (item.s) b.appendChild(el("div", { class: "fl-search-item-snippet", html: highlight(item.s, terms) }));
        a.appendChild(b);
        a.addEventListener("mousemove", function () {
          var idx = search.results.indexOf(a);
          if (idx !== search.selected) { search.selected = idx; updateSelection(false); }
        });
        body.appendChild(a);
        search.results.push(a);
      });
    }
    if (apiFirst) {
      addGroup("API reference", api, true);
      addGroup("Manuals", docs, false);
    } else {
      addGroup("Manuals", docs, false);
      addGroup("API reference", api, true);
    }
    updateSelection(false);
  }

  function updateSelection(scroll) {
    search.results.forEach(function (a, i) {
      var on = i === search.selected;
      a.classList.toggle("fl-active", on);
      if (on && scroll) a.scrollIntoView({ block: "nearest" });
    });
  }

  function loadIndex() {
    if (search.index) return Promise.resolve(search.index);
    if (!search.loading) {
      search.loading = fetch("flecs-search.json").then(function (r) {
        if (!r.ok) throw new Error("index not found");
        return r.json();
      }).then(function (data) {
        search.index = buildIndex(data);
        return search.index;
      });
    }
    return search.loading;
  }

  function openSearch() {
    if (!search.modal) return;
    search.open = true;
    search.modal.hidden = false;
    document.body.style.overflow = "hidden";
    search.input.focus();
    search.input.select();
    if (!search.index) {
      search.body.innerHTML = "";
      search.body.appendChild(el("div", { class: "fl-search-empty", text: "Loading search index…" }));
      loadIndex().then(function () {
        renderResults(search.input.value);
      }).catch(function () {
        search.body.innerHTML = "";
        search.body.appendChild(el("div", { class: "fl-search-empty", text: "Search index unavailable. Run docs/build.sh to generate it." }));
      });
    } else {
      renderResults(search.input.value);
    }
  }

  function closeSearch() {
    if (!search.modal) return;
    search.open = false;
    search.modal.hidden = true;
    document.body.style.overflow = "";
  }

  function initSearch() {
    search.modal = document.getElementById("fl-search");
    search.input = document.getElementById("fl-search-input");
    search.body = document.getElementById("fl-search-results");
    if (!search.modal || !search.input || !search.body) return;

    var isMac = /Mac|iPhone|iPad/.test(navigator.platform);
    document.querySelectorAll(".fl-search-kbd").forEach(function (k) { k.textContent = isMac ? "⌘K" : "Ctrl K"; });

    document.querySelectorAll(".fl-search-btn").forEach(function (b) { b.addEventListener("click", openSearch); });
    search.modal.querySelector(".fl-search-backdrop").addEventListener("click", closeSearch);
    search.modal.querySelector(".fl-search-close").addEventListener("click", closeSearch);

    var timer = null;
    search.input.addEventListener("input", function () {
      if (!search.index) return;
      clearTimeout(timer);
      timer = setTimeout(function () { renderResults(search.input.value); }, 40);
    });

    search.input.addEventListener("keydown", function (e) {
      if (e.key === "ArrowDown" || e.key === "ArrowUp") {
        e.preventDefault();
        if (!search.results.length) return;
        var n = search.results.length;
        search.selected = (search.selected + (e.key === "ArrowDown" ? 1 : n - 1)) % n;
        updateSelection(true);
      } else if (e.key === "Enter") {
        var a = search.results[search.selected];
        if (a) { e.preventDefault(); location.href = a.getAttribute("href"); closeSearch(); }
      }
    });

    document.addEventListener("keydown", function (e) {
      if ((e.key === "k" || e.key === "K") && (e.metaKey || e.ctrlKey)) {
        e.preventDefault();
        if (search.open) closeSearch(); else openSearch();
      } else if (e.key === "Escape" && search.open) {
        closeSearch();
      } else if (e.key === "/" && !search.open) {
        var t = e.target;
        if (t && (t.tagName === "INPUT" || t.tagName === "TEXTAREA" || t.isContentEditable)) return;
        e.preventDefault();
        openSearch();
      }
    });
  }

  function init() {
    initTheme();
    initDrawer();
    initHeadings();
    initTabs();
    initCopyButtons();
    initTables();
    initMemberTables();
    initInherited();
    initDiagrams();
    initReadme();
    initOutline();
    initToc();
    renderPageNav();
    initSearch();
  }

  window.Flecs = { renderSidebar: renderSidebar, init: init };
})();
