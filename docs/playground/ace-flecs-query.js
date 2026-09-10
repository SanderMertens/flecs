ace.define("ace/mode/flecs-query", ["require", "exports", "module", "ace/lib/oop", "ace/mode/text", "ace/mode/flecs-query-highlight_rules"], 
    function(require, exports, module) {
        var oop = require("ace/lib/oop");
        var TextMode = require("ace/mode/text").Mode;
        var HighlightRules = require("ace/mode/flecs-query-highlight_rules").HighlightRules;

        const Mode = function() {
            this.HighlightRules = HighlightRules;
        };
        oop.inherits(Mode, TextMode);

        (function() {
            this.$id = "ace/mode/flecs_query";
        }).call(Mode.prototype);

        exports.Mode = Mode;
    });

ace.define("ace/mode/flecs-query-highlight_rules", ["require", "exports", "module", "ace/lib/oop", "ace/mode/text_highlight_rules"], 
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
                        token: "string",           // single line
                        regex: '".*?"'
                    },
                    {
                        token: "keyword",
                        regex: "\\b(self|up|cascade|desc)\\b"
                    },
                    {
                        token: "keyword.operator",
                        regex: "(\\.|{|}|\\[|\\]|:|\\(|\\)|=|[?]|[!]|[,]|[|]|[\\\\])"
                    },
                    {
                        token: "constant.numeric",
                        regex: "\\b([0-9e]+)\\b"
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
