#!/usr/bin/env python3
"""Post-processes the Doxygen output of the Flecs documentation website.

Generates two files in the HTML output directory:

  flecs-nav.js       sidebar navigation, resolved from docs/cfg/nav.json and
                     the Doxygen tag file
  flecs-search.json  full-text search index over the manuals and the API
                     reference

Usage: postprocess.py [html_dir] [nav.json]
"""

import html
import json
import os
import re
import sys
import xml.etree.ElementTree as ET
from html.parser import HTMLParser

STOP_WORDS = set("""
a an and are as at be been but by can do for from has have if in into is it its
of on or that the their then there these this to was we were what when which
will with you your not all also any each more such than
""".split())

WORD_RE = re.compile(r"[a-z0-9_]+")
TAG_RE = re.compile(r"<[^>]+>")
SPACE_RE = re.compile(r"\s+")

KIND_CODES = {
    "function": "f", "define": "m", "typedef": "t", "enumeration": "e",
    "enumvalue": "v", "variable": "d", "struct": "s", "class": "c",
    "union": "u", "namespace": "n", "group": "g", "page": "p", "file": "h",
}


def strip_tags(text):
    text = TAG_RE.sub("", text)
    text = html.unescape(text)
    return SPACE_RE.sub(" ", text).strip()


def tokenize(text):
    return [w for w in WORD_RE.findall(text.lower())
            if len(w) > 1 and w not in STOP_WORDS]


class PageExtractor(HTMLParser):
    """Splits the content of a Doxygen page into sections per heading."""

    HEADINGS = {"h1", "h2", "h3", "h4", "h5", "h6"}

    def __init__(self, page_title):
        super().__init__(convert_charrefs=True)
        self.page_title = page_title
        self.sections = []
        self.stack = []
        self.depth = 0
        self.contents_depth = None
        self.skip_depth = None
        self.heading = None
        self.heading_id = None
        self.heading_text = []
        self.code_depths = []
        self.current = self.new_section(page_title, "", 0)

    def new_section(self, title, anchor, level):
        while self.stack and self.stack[-1][0] >= level:
            self.stack.pop()
        section = {
            "title": title,
            "anchor": anchor,
            "level": level,
            "crumbs": [s[1] for s in self.stack],
            "text": [],
            "code": [],
        }
        self.stack.append((level, title))
        self.sections.append(section)
        return section

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        cls = attrs.get("class", "")
        self.depth += 1
        if self.contents_depth is None:
            if tag == "div" and "contents" in cls.split():
                self.contents_depth = self.depth
            return
        if self.skip_depth is not None:
            return
        classes = cls.split()
        if tag in ("script", "style") or (tag == "div" and "ttc" in classes) \
                or (tag == "span" and "lineno" in classes) \
                or "tab-title" in classes or "fl-example-source" in classes:
            self.skip_depth = self.depth
            return
        if tag in self.HEADINGS and self.heading is None:
            self.heading = (tag, self.depth)
            self.heading_id = attrs.get("id")
            self.heading_text = []
            return
        if self.heading is not None and tag == "a" and self.heading_id is None:
            self.heading_id = attrs.get("id") or attrs.get("name")
            return
        if (tag == "div" and "fragment" in classes) or tag in ("code", "pre"):
            self.code_depths.append(self.depth)

    def handle_endtag(self, tag):
        if self.skip_depth is not None and self.depth == self.skip_depth:
            self.skip_depth = None
        elif self.contents_depth is not None and self.depth == self.contents_depth:
            self.contents_depth = -1
        elif self.heading is not None and tag == self.heading[0] and self.depth == self.heading[1]:
            title = SPACE_RE.sub(" ", "".join(self.heading_text)).strip()
            level = int(tag[1])
            self.current = self.new_section(title, self.heading_id or "", level)
            self.heading = None
        elif self.code_depths and self.depth == self.code_depths[-1]:
            self.code_depths.pop()
        self.depth -= 1

    def handle_data(self, data):
        if self.contents_depth is None or self.contents_depth == -1:
            return
        if self.skip_depth is not None:
            return
        if self.heading is not None:
            self.heading_text.append(data)
        elif self.code_depths:
            self.current["code"].append(data + " ")
        else:
            self.current["text"].append(data)


def load_tagfile(path):
    root = ET.parse(path).getroot()
    compounds = []
    for c in root.findall("compound"):
        entry = {
            "kind": c.get("kind"),
            "name": c.findtext("name") or "",
            "title": c.findtext("title") or c.findtext("name") or "",
            "filename": c.findtext("filename") or "",
            "subgroups": [s.text for s in c.findall("subgroup")],
            "members": [],
        }
        for m in c.findall("member"):
            entry["members"].append({
                "kind": m.get("kind"),
                "name": m.findtext("name") or "",
                "anchorfile": m.findtext("anchorfile") or "",
                "anchor": m.findtext("anchor") or "",
            })
        compounds.append(entry)
    return compounds


PAGE_PREFIX = "md_docs_2"

REDIRECT_TEMPLATE = """<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8"/>
<title>Redirecting</title>
<link rel="canonical" href="{target}"/>
<meta http-equiv="refresh" content="0; url={target}"/>
<script>location.replace("{target}" + location.hash);</script>
</head><body><p>This page moved to <a href="{target}">{target}</a>.</p></body></html>
"""


def rename_pages(html_dir, compounds):
    """Renames md_docs_2Foo.html to Foo.html, rewrites links, leaves redirects."""
    mapping = {}
    for c in compounds:
        if c["kind"] != "page" or not c["filename"].startswith(PAGE_PREFIX):
            continue
        new_name = re.sub(r"_(_|2)", lambda m: "_" if m.group(1) == "_" else "-",
                          c["filename"][len(PAGE_PREFIX):])
        mapping[c["filename"]] = new_name
    generated = {c["filename"] for c in compounds}
    for new_name in mapping.values():
        if new_name in generated:
            sys.exit(f"cannot rename to {new_name}: name is used by another generated page")
    if not mapping:
        return
    pattern = re.compile("|".join(re.escape(k) for k in sorted(mapping, key=len, reverse=True)))

    def rewrite(text):
        return pattern.sub(lambda m: mapping[m.group(0)], text)

    for name in os.listdir(html_dir):
        if not (name.endswith(".html") or name.endswith(".js") or name.endswith(".tag")):
            continue
        path = os.path.join(html_dir, name)
        with open(path, encoding="utf-8") as f:
            text = f.read()
        updated = rewrite(text)
        if updated != text:
            with open(path, "w", encoding="utf-8") as f:
                f.write(updated)
    for old, new in mapping.items():
        os.replace(os.path.join(html_dir, old), os.path.join(html_dir, new))
        with open(os.path.join(html_dir, old), "w", encoding="utf-8") as f:
            f.write(REDIRECT_TEMPLATE.format(target=new))
    for c in compounds:
        c["filename"] = rewrite(c["filename"])
        for m in c["members"]:
            m["anchorfile"] = rewrite(m["anchorfile"])
    print(f"renamed {len(mapping)} manual pages (redirects left in place)")


def page_id_for(path, mainpage):
    if os.path.normpath(path) == os.path.normpath(mainpage):
        return "index"
    base = re.sub(r"\.md$", "", path)
    return "md_" + base.replace("_", "__").replace("/", "_2")


def read_mainpage(doxyfile):
    with open(doxyfile, encoding="utf-8") as f:
        for line in f:
            m = re.match(r"\s*USE_MDFILE_AS_MAINPAGE\s*=\s*(\S+)", line)
            if m:
                return m.group(1)
    return "README.md"


def page_headings(html_dir, filename, title):
    path = os.path.join(html_dir, filename)
    if not os.path.exists(path):
        return []
    with open(path, encoding="utf-8") as f:
        parser = PageExtractor(title)
        parser.feed(f.read())
    sections = [s for s in parser.sections if s["level"] and s["anchor"]]
    if not sections:
        return []
    root = {"children": []}
    stack = [(0, root)]
    for s in sections:
        node = {"title": s["title"], "href": filename + "#" + s["anchor"]}
        while stack and stack[-1][0] >= s["level"]:
            stack.pop()
        parent = stack[-1][1]
        parent.setdefault("children", []).append(node)
        stack.append((s["level"], node))
    return root["children"]


def examples_nodes(manifest_path, pages, mainpage, html_dir):
    if not os.path.exists(manifest_path):
        print(f"warning: {manifest_path} not found, run docs/cfg/examples.py (skipping examples)")
        return []
    with open(manifest_path, encoding="utf-8") as f:
        manifest = json.load(f)

    def page_node(item):
        page = pages.get(page_id_for(item["page"], mainpage))
        if not page:
            sys.exit(f"examples: page {item['page']} not found in tag file")
        return {"title": item["title"], "href": page["filename"], "page": True}

    index = pages.get(page_id_for(manifest["index"], mainpage))
    if not index:
        sys.exit(f"examples: index page {manifest['index']} not found in tag file")
    anchors = {h["title"]: h["href"] for h in page_headings(html_dir, index["filename"], index["title"])}
    children = [page_node(item) for item in manifest["uncategorized"]]
    for category in manifest["categories"]:
        children.append({"title": category["title"],
                         "href": anchors.get(category["title"], index["filename"]),
                         "children": [page_node(item) for item in category["examples"]]})
    return [{"title": "Overview", "href": index["filename"], "page": True, "children": children}]


def build_nav(nav, compounds, mainpage, html_dir):
    pages = {c["name"]: c for c in compounds if c["kind"] == "page"}
    groups = {c["name"]: c for c in compounds if c["kind"] == "group"}
    root_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

    def group_node(name):
        g = groups.get(name)
        if not g:
            sys.exit(f"nav.json: unknown group '{name}'")
        node = {"title": g["title"].rstrip("."), "href": g["filename"]}
        if g["subgroups"]:
            node["children"] = [group_node(s) for s in g["subgroups"]]
        return node

    sections = []
    for section in nav["sections"]:
        items = []
        for item in section["items"]:
            if "page" in item:
                pid = page_id_for(item["page"], mainpage)
                page = pages.get(pid)
                if not page:
                    known = ", ".join(sorted(pages))
                    sys.exit(f"nav.json: page '{item['page']}' (id {pid}) not "
                             f"found in tag file. Known pages: {known}")
                node = {"title": item.get("title") or page["title"],
                        "href": page["filename"], "page": True}
                headings = page_headings(html_dir, page["filename"], node["title"])
                if headings:
                    node["children"] = headings
                    node["outline"] = True
                items.append(node)
            elif "examples" in item:
                items.extend(examples_nodes(os.path.join(root_dir, item["examples"]), pages, mainpage, html_dir))
            elif "group" in item:
                node = group_node(item["group"])
                if "title" in item:
                    node["title"] = item["title"]
                items.append(node)
            elif "link" in item:
                items.append({"title": item["title"], "href": item["link"]})
            elif "url" in item:
                items.append({"title": item["title"], "href": item["url"],
                              "external": True})
            else:
                sys.exit(f"nav.json: invalid item {item}")
        sections.append({"title": section["title"], "items": items})
    return {"sections": sections}


def extract_briefs(html_text):
    briefs = {}
    for m in re.finditer(r'<tr class="memdesc:([^"]+)">(.*?)</tr>', html_text, re.S):
        text = re.sub(r'<a [^>]*>More\.\.\.</a>', "", m.group(2))
        text = strip_tags(re.sub(r'<td class="mdescLeft">.*?</td>', "", text, flags=re.S))
        briefs[m.group(1)] = text
    return briefs


def extract_compound_brief(html_text):
    m = re.search(r'<div class="contents">\s*(?:<div class="textblock">)?\s*<p>(.*?)</p>', html_text, re.S)
    if not m:
        return ""
    text = re.sub(r'<a [^>]*>More\.\.\.</a>', "", m.group(1))
    return strip_tags(text)


def build_search(html_dir, nav_data, compounds):
    dictionary = {}

    def word_ids(words):
        ids = []
        for w in words:
            if w not in dictionary:
                dictionary[w] = len(dictionary)
            ids.append(dictionary[w])
        return " ".join(format(i, "x") for i in sorted(set(ids)))

    nav_pages = {}
    for section in nav_data["sections"]:
        for item in section["items"]:
            if "children" not in item and not item.get("external"):
                nav_pages[item["href"]] = (section["title"], item["title"])

    docs = []
    for c in compounds:
        if c["kind"] != "page":
            continue
        path = os.path.join(html_dir, c["filename"])
        if not os.path.exists(path):
            continue
        with open(path, encoding="utf-8") as f:
            text = f.read()
        page_title = nav_pages.get(c["filename"], (None, c["title"]))[1]
        parser = PageExtractor(page_title)
        parser.feed(text)
        for s in parser.sections:
            body = SPACE_RE.sub(" ", "".join(s["text"])).strip()
            code = " ".join(s["code"])
            if not body and not code and s is parser.sections[0]:
                continue
            words = tokenize(s["title"] + " " + body + " " + code)
            url = c["filename"] + ("#" + s["anchor"] if s["anchor"] else "")
            crumbs = s["crumbs"]
            snippet = body or SPACE_RE.sub(" ", code).strip()
            doc = {
                "t": s["title"],
                "c": " › ".join(crumbs),
                "u": url,
                "s": snippet[:160],
                "w": word_ids(words),
            }
            if c["filename"].startswith("examples-"):
                doc["k"] = "x"
                doc["c"] = "Examples" + (" › " + doc["c"] if doc["c"] else "")
            docs.append(doc)

    group_parent = {}
    by_name = {c["name"]: c for c in compounds}
    for c in compounds:
        if c["kind"] == "group":
            for s in c["subgroups"]:
                group_parent[s] = c["name"]

    def group_crumbs(name):
        chain = []
        while name in group_parent:
            name = group_parent[name]
            chain.append(by_name[name]["title"].rstrip("."))
        return list(reversed(chain))

    api_compounds = []
    api_members = []
    seen = {}
    compound_index = {}
    brief_cache = {}

    def briefs_for(filename):
        if filename not in brief_cache:
            path = os.path.join(html_dir, filename)
            if os.path.exists(path):
                with open(path, encoding="utf-8") as f:
                    text = f.read()
                brief_cache[filename] = (extract_briefs(text), extract_compound_brief(text))
            else:
                brief_cache[filename] = ({}, "")
        return brief_cache[filename]

    for c in compounds:
        if c["kind"] in ("page", "dir"):
            continue
        member_briefs, own_brief = briefs_for(c["filename"])
        if c["kind"] == "group":
            crumbs = group_crumbs(c["name"])
            title = c["title"].rstrip(".")
        elif c["kind"] == "file":
            crumbs = ["Header files"]
            title = c["name"]
        else:
            crumbs = ["C++ API"] if "::" in c["name"] else ["C API"]
            title = c["name"]
        idx = len(api_compounds)
        compound_index[c["name"]] = idx
        api_compounds.append({
            "t": title,
            "k": KIND_CODES.get(c["kind"], "?"),
            "c": " › ".join(crumbs),
            "u": c["filename"],
            "s": own_brief[:160],
        })
        own_name = c["name"].split("::")[-1]
        for m in c["members"]:
            if not m["anchor"]:
                continue
            anchor_briefs = member_briefs
            if m["anchorfile"] != c["filename"]:
                anchor_briefs = briefs_for(m["anchorfile"])[0]
            entry = {
                "t": m["name"],
                "k": KIND_CODES.get(m["kind"], "?"),
                "p": idx,
                "s": anchor_briefs.get(m["anchor"], "")[:160],
            }
            if m["anchorfile"] == c["filename"]:
                entry["a"] = m["anchor"]
            else:
                entry["u"] = m["anchorfile"] + "#" + m["anchor"]
            existing = seen.get(m["anchor"])
            if existing is not None:
                existing_kind = api_compounds[api_members[existing]["p"]]["k"]
                if m["name"] == own_name or (existing_kind == "h" and c["kind"] != "file"):
                    api_members[existing] = entry
                continue
            seen[m["anchor"]] = len(api_members)
            api_members.append(entry)

    words = [None] * len(dictionary)
    for w, i in dictionary.items():
        words[i] = w
    return {"words": words, "docs": docs, "compounds": api_compounds, "members": api_members}


def main():
    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    html_dir = sys.argv[1] if len(sys.argv) > 1 else os.path.join(root, "docs", "html")
    nav_path = sys.argv[2] if len(sys.argv) > 2 else os.path.join(root, "docs", "cfg", "nav.json")
    doxyfile = os.path.join(root, "docs", "cfg", "Doxyfile")
    tag_path = os.path.join(html_dir, "flecs.tag")
    if not os.path.exists(tag_path):
        sys.exit(f"tag file not found: {tag_path} (run doxygen first)")

    with open(nav_path, encoding="utf-8") as f:
        nav = json.load(f)
    compounds = load_tagfile(tag_path)
    mainpage = read_mainpage(doxyfile)
    rename_pages(html_dir, compounds)

    nav_data = build_nav(nav, compounds, mainpage, html_dir)
    with open(os.path.join(html_dir, "flecs-nav.js"), "w", encoding="utf-8") as f:
        f.write("window.FLECS_NAV = " + json.dumps(nav_data, ensure_ascii=False, separators=(",", ":")) + ";\n")

    search = build_search(html_dir, nav_data, compounds)
    with open(os.path.join(html_dir, "flecs-search.json"), "w", encoding="utf-8") as f:
        json.dump(search, f, ensure_ascii=False, separators=(",", ":"))

    print(f"flecs-nav.js: {sum(len(s['items']) for s in nav_data['sections'])} top-level items")
    print(f"flecs-search.json: {len(search['docs'])} manual sections, "
          f"{len(search['compounds'])} compounds, {len(search['members'])} members, "
          f"{len(search['words'])} words")


if __name__ == "__main__":
    main()
