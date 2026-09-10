#!/usr/bin/env python3
"""Generates documentation pages for the code examples in the examples folder.

Walks examples/c, examples/cpp and examples/script and writes one markdown page
per example into docs/examples (a build product, ignored by git). Examples that
exist in multiple languages share a page with a language tab per version. The
source code is pulled in by Doxygen with \\include, which highlights it and
links identifiers to the API reference.

Also writes docs/examples/manifest.json, which postprocess.py uses to build the
Examples section of the sidebar, and docs/examples/playground-examples.js, which
lists the script examples for the "Flecs playground" page.

Usage: examples.py [repo_root]
"""

import json
import os
import re
import shutil
import sys

LANGUAGES = [
    ("c", "C"),
    ("cpp", "C++"),
    ("script", "Flecs Script"),
]

SKIP_FILES = {"bake_config.h", "BUILD", "BUILD.bazel", "project.json", ".gitignore", "CMakeLists.txt"}
SKIP_CATEGORIES = {"explorer"}
GITHUB = "https://github.com/SanderMertens/flecs/tree/master/"


def display_name(name):
    return name.replace("_", " ").capitalize()


BOILERPLATE = {"extern \"C\" {", "}"}


def meaningful_header(path):
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith(("#", "//", "/*", "*")) or line in BOILERPLATE:
                continue
            return True
    return False


def source_files(example_dir):
    files = []
    for sub in ("include", "src"):
        base = os.path.join(example_dir, sub)
        if not os.path.isdir(base):
            continue
        for root, _, names in os.walk(base):
            for name in sorted(names):
                if name in SKIP_FILES:
                    continue
                path = os.path.join(root, name)
                if sub == "include" and not meaningful_header(path):
                    continue
                files.append(path)
    return files


def collect(root):
    examples = {}

    def add(category, name, lang, files, single=None):
        key = (category, name)
        entry = examples.setdefault(key, {"category": category, "name": name, "languages": {}})
        entry["languages"][lang] = {"files": files, "single": single}

    for lang in ("c", "cpp"):
        base = os.path.join(root, "examples", lang)
        if not os.path.isdir(base):
            continue
        for category in sorted(os.listdir(base)):
            cat_dir = os.path.join(base, category)
            if not os.path.isdir(cat_dir) or category in SKIP_CATEGORIES:
                continue
            if os.path.isdir(os.path.join(cat_dir, "src")):
                add("", category, lang, source_files(cat_dir))
                continue
            for name in sorted(os.listdir(cat_dir)):
                ex_dir = os.path.join(cat_dir, name)
                if not os.path.isdir(os.path.join(ex_dir, "src")):
                    continue
                files = source_files(ex_dir)
                if files:
                    add(category, name, lang, files)

    script_dir = os.path.join(root, "examples", "script")
    if os.path.isdir(script_dir):
        for name in sorted(os.listdir(script_dir)):
            if name.endswith(".flecs"):
                path = os.path.join(script_dir, name)
                add("script", name[:-6], "script", [path], single=True)

    return examples


def rel(root, path):
    return os.path.relpath(path, root).replace(os.sep, "/")


def render_file(root, path):
    relpath = rel(root, path)
    example_rel = relpath[len("examples/"):]
    out = []
    if path.endswith(".flecs"):
        with open(path, encoding="utf-8", errors="replace") as f:
            code = f.read().rstrip("\n")
        out.append("```")
        out.append(code)
        out.append("```")
    else:
        out.append("\\include{lineno} " + example_rel)
    return "\n".join(out)


def render_example(root, entry):
    category, name = entry["category"], entry["name"]
    title = f"{category}/{name}" if category else name
    lines = [f"# {title}", ""]
    tabs = [(lang, label) for lang, label in LANGUAGES if lang in entry["languages"]]
    lines.append('<div class="flecs-snippet-tabs">')
    lines.append("<ul>")
    for lang, label in tabs:
        version = entry["languages"][lang]
        lines.append(f'<li><b class="tab-title">{label}</b>')
        lines.append("")
        if version["single"]:
            folder = rel(root, os.path.dirname(version["files"][0]))
            link = GITHUB + rel(root, version["files"][0])
        else:
            folder = re.sub(r"/(include|src)(/.*)?$", "", os.path.dirname(rel(root, version["files"][0])))
            link = GITHUB + folder
        lines.append(f'<a class="fl-example-source" href="{link}" target="_blank" rel="noopener">View on GitHub</a>')
        lines.append("")
        for path in version["files"]:
            if len(version["files"]) > 1:
                sub = rel(root, path)[len(folder) + 1:]
                lines.append(f"**{sub}**")
                lines.append("")
            lines.append(render_file(root, path))
            lines.append("")
        lines.append("</li>")
    lines.append("</ul>")
    lines.append("</div>")
    lines.append("")
    return "\n".join(lines)


def render_index(examples):
    lines = ["# Examples", "",
             "Code examples for the C and C++ APIs and for Flecs Script. Each page "
             "shows the example in every language it is available in. The sources "
             "live in the [examples](https://github.com/SanderMertens/flecs/tree/master/examples) "
             "folder of the repository.", ""]
    categories = {}
    for (category, name), entry in sorted(examples.items()):
        categories.setdefault(category, []).append(entry)
    if "" in categories:
        for entry in categories.pop(""):
            lines.append(f"- [{display_name(entry['name'])}]({entry['name']}.md)")
        lines.append("")
    for category, entries in categories.items():
        lines.append(f"## {display_name(category)}")
        for entry in entries:
            langs = ", ".join(label for lang, label in LANGUAGES if lang in entry["languages"])
            lines.append(f"- [{display_name(entry['name'])}]({category}/{entry['name']}.md) ({langs})")
        lines.append("")
    return "\n".join(lines)


HEADER_RE = re.compile(r"\A(?://[^\n]*\n)+\n*")


def playground_code(path):
    with open(path, encoding="utf-8", errors="replace") as f:
        code = f.read()
    m = HEADER_RE.match(code)
    if m and "flecs.dev/explorer" in m.group(0):
        code = code[m.end():]
    return code.rstrip("\n") + "\n"


def write_playground_examples(root, examples, out_dir):
    items = []
    for (category, name), entry in sorted(examples.items()):
        script = entry["languages"].get("script")
        if not script:
            continue
        items.append({"title": display_name(name), "code": playground_code(script["files"][0])})
    with open(os.path.join(out_dir, "playground-examples.js"), "w", encoding="utf-8") as f:
        f.write("window.FLECS_PLAYGROUND_EXAMPLES = " + json.dumps(items, ensure_ascii=False) + ";\n")
    return len(items)


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    out_dir = os.path.join(root, "docs", "examples")
    examples = collect(root)
    if not examples:
        sys.exit("no examples found")

    shutil.rmtree(out_dir, ignore_errors=True)
    os.makedirs(out_dir)
    manifest = {"index": "docs/examples/index.md", "uncategorized": [], "categories": []}
    categories = {}
    for (category, name), entry in sorted(examples.items()):
        if category:
            page_dir = os.path.join(out_dir, category)
            os.makedirs(page_dir, exist_ok=True)
            page = os.path.join(page_dir, name + ".md")
        else:
            page = os.path.join(out_dir, name + ".md")
        with open(page, "w", encoding="utf-8") as f:
            f.write(render_example(root, entry))
        item = {"title": display_name(name), "page": rel(root, page),
                "languages": [lang for lang, _ in LANGUAGES if lang in entry["languages"]]}
        if category:
            categories.setdefault(category, []).append(item)
        else:
            manifest["uncategorized"].append(item)
    for category, items in categories.items():
        manifest["categories"].append({"name": category, "title": display_name(category),
                                       "anchor": re.sub(r"[^a-z0-9]+", "-", display_name(category).lower()),
                                       "examples": items})

    with open(os.path.join(out_dir, "index.md"), "w", encoding="utf-8") as f:
        f.write(render_index(examples))
    with open(os.path.join(out_dir, "manifest.json"), "w", encoding="utf-8") as f:
        json.dump(manifest, f, indent=2)
    scripts = write_playground_examples(root, examples, out_dir)
    print(f"examples: {len(examples)} pages in {len(categories)} categories, "
          f"{scripts} playground scripts")


if __name__ == "__main__":
    main()
