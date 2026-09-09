// Ace theme for the "Try it out!" page. Adapted from the Flecs Explorer's
// flecs-script theme, mapped onto the documentation theme variables so it
// follows light and dark mode.
ace.define("ace/theme/flecs-docs", ["require", "exports", "module", "ace/lib/dom"],
function(require, exports, module) {
    exports.isDark = false;
    exports.cssClass = "ace-flecs-docs";
    exports.cssText = `
        .ace-flecs-docs {
            background: var(--pg-editor-bg, var(--fl-bg));
            color: var(--fl-code-text);
        }
        .ace-flecs-docs .ace_gutter {
            background: var(--pg-editor-bg, var(--fl-bg));
            color: var(--fl-text-3);
        }
        .ace-flecs-docs .ace_gutter-active-line {
            background: transparent;
            color: var(--fl-text);
        }
        .ace-flecs-docs .ace_print-margin {
            width: 1px;
            background: var(--fl-border);
        }
        .ace-flecs-docs .ace_cursor {
            color: var(--fl-text);
        }
        .ace-flecs-docs .ace_marker-layer .ace_selection {
            background: var(--fl-accent-soft);
        }
        .ace-flecs-docs.ace_multiselect .ace_selection.ace_start {
            box-shadow: none;
            border-radius: 0;
        }
        .ace-flecs-docs .ace_marker-layer .ace_step {
            background: var(--warning-color-bg);
        }
        .ace-flecs-docs .ace_marker-layer .ace_bracket {
            margin: -1px 0 0 -1px;
            border: 1px solid var(--fl-border-strong);
        }
        .ace-flecs-docs .ace_marker-layer .ace_active-line {
            background: var(--fl-bg-soft);
        }
        .ace-flecs-docs .ace_marker-layer .ace_selected-word {
            border: 1px solid var(--fl-border-strong);
        }
        .ace-flecs-docs .ace_invisible {
            color: var(--fl-border-strong);
        }
        .ace-flecs-docs .ace_keyword {
            color: var(--fl-code-keyword);
        }
        .ace-flecs-docs .ace_keyword.ace_operator {
            color: var(--fl-text-3);
        }
        .ace-flecs-docs .ace_storage,
        .ace-flecs-docs .ace_storage.ace_type,
        .ace-flecs-docs .ace_support.ace_type,
        .ace-flecs-docs .ace_support.ace_class {
            color: var(--fl-code-type);
        }
        .ace-flecs-docs .ace_constant.ace_numeric,
        .ace-flecs-docs .ace_constant.ace_language,
        .ace-flecs-docs .ace_constant.ace_character,
        .ace-flecs-docs .ace_support.ace_constant {
            color: var(--fl-accent-strong);
        }
        .ace-flecs-docs .ace_variable {
            color: var(--fl-code-preproc);
        }
        .ace-flecs-docs .ace_comment {
            color: var(--fl-code-comment);
            font-style: italic;
        }
        .ace-flecs-docs .ace_comment.ace_documentation {
            color: var(--fl-code-preproc);
            font-style: normal;
        }
        .ace-flecs-docs .ace_string {
            color: var(--fl-code-string);
        }
        .ace-flecs-docs .ace_invalid {
            color: #ffffff;
            background-color: var(--deprecated-color-hl);
        }
        .ace-flecs-docs .ace_fold {
            background-color: var(--fl-accent);
            border-color: var(--fl-bg);
        }
        .ace-flecs-docs .ace_indent-guide {
            background: none;
            border-right: 1px solid var(--fl-border);
        }
        .ace-flecs-docs .ace_scrollbar::-webkit-scrollbar-thumb {
            background: var(--fl-border-strong);
        }
        .ace-flecs-docs .pg-error-line {
            position: absolute;
            background: color-mix(in srgb, var(--deprecated-color-hl) 12%, transparent);
        }
        .ace-flecs-docs .pg-error-squiggle {
            position: absolute;
            background: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='6' height='3'%3E%3Cpath d='M 0 3 L 1.5 1 L 3 3 L 4.5 1 L 6 3' fill='none' stroke='%23ef4444' stroke-width='1'/%3E%3C/svg%3E") repeat-x left bottom;
        }
    `;
    var dom = require("../lib/dom");
    dom.importCssString(exports.cssText, exports.cssClass);
});
