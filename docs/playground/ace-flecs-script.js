// Ace mode for Flecs Script, taken from the Flecs Explorer (js/ace-flecs-script.js).
ace.define("ace/mode/flecs-script", ["require", "exports", "module", "ace/lib/oop", "ace/mode/text", "ace/mode/flecs-script-highlight_rules"], 
    function(require, exports, module) {
        var oop = require("ace/lib/oop");
        var TextMode = require("ace/mode/text").Mode;
        var HighlightRules = require("ace/mode/flecs-script-highlight_rules").HighlightRules;

        const Mode = function() {
            this.HighlightRules = HighlightRules;
        };
        oop.inherits(Mode, TextMode);

        (function() {
            this.$id = "ace/mode/flecs_script";
        }).call(Mode.prototype);

        exports.Mode = Mode;
    });

ace.define("ace/mode/flecs-script-highlight_rules", ["require", "exports", "module", "ace/lib/oop", "ace/mode/text_highlight_rules"], 
    function(require, exports, module) {
        var oop = require("ace/lib/oop");
        var TextHighlightRules = require("ace/mode/text_highlight_rules").TextHighlightRules;

        const HighlightRules = function() {
            this.$rules = {
                "start": [
                    {
                        token: "comment",
                        regex: "//.*$"
                    },
                    {
                        token: "comment.documentation",
                        regex: "@.*$"
                    },
                    {
                        token: "string",           // single line
                        regex: '".*?"'
                    },
                    {
                        token: "keyword",
                        regex: "\\b(?:if|else|module|using|include|const|prop|mut|with|template|prefab|slot|for|in|match|new|export|fn|await|try|catch|continue|script)\\b"
                    },
                    {
                        token: "keyword.operator",
                        regex: "(\\.|{|}|\\[|\\]|:|\\(|\\)|=|[?]|[!]|[,]|[|]|->)"
                    },
                    {
                        token: "constant.numeric",
                        regex: "\\b([0-9e]+)\\b"
                    },
                    {
                        token: "storage.type",
                        regex: "\\b(struct|enum|bitmask|primitive|array|vector|opaque|bool|byte|char|i8|i16|i32|i64|iptr|u8|u16|u32|u64|uptr|f32|f64|string|entity)\\b"
                    },
                    {
                        token: "variable",
                        regex: "\\$[a-zA-Z_0-9]+"
                    }
                    // Add more rules here
                ]
            };

            this.normalizeRules();
        };

        oop.inherits(HighlightRules, TextHighlightRules);

        exports.HighlightRules = HighlightRules;
    });
