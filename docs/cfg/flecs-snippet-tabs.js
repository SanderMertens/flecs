// Heavily modified version of DoxygenAwesomeTabs
// https://github.com/jothepro/doxygen-awesome-css
class FlecsSnippetTabs {
    static init() {
        window.addEventListener("load", () => {
            let changeTab = (tabName) => {
                // Save active tab to local storage
                let activeTabHistory = JSON.parse(localStorage.getItem("activeTabHistory"));
                if (activeTabHistory) {
                    // Remove if already exists and wasn't the last one
                    let index = activeTabHistory.indexOf(tabName);
                    if (index < activeTabHistory.length - 1) {
                        if (index > -1) {
                            activeTabHistory.splice(index, 1);
                        }
                        activeTabHistory.push(tabName);
                        localStorage.setItem("activeTabHistory", JSON.stringify(activeTabHistory));
                    }
                } else {
                    // Create if doesn't exist
                    localStorage.setItem("activeTabHistory", JSON.stringify([tabName]));
                }

                // TODO: Don't switch tabs if this tabbed doesn't have the tab
                document.querySelectorAll(".flecs-snippet-tabs:not(:empty)").forEach((tabbed, tabbedIndex) => {
                    // Find the last active tab that exists in this group of tabs
                    let lastActiveIndex = -1;
                    let tabButtonId = "tab_button_" + tabbedIndex + "_0"; // Default to first tab
                    tabbed.querySelectorAll(".tab-button").forEach((tabLink) => {
                        let index = activeTabHistory.indexOf(tabLink.textContent);
                        if (index > -1 && index > lastActiveIndex) {
                            lastActiveIndex = index;
                            tabButtonId = tabLink.id;
                        }
                    });

                    // Update active tab link
                    tabbed.querySelectorAll(".tab-button").forEach((tabLink) => {
                        if (tabLink.id == tabButtonId) {
                            tabLink.classList.add("active");
                        } else {
                            tabLink.classList.remove("active");
                        }
                    });

                    let tabId = tabButtonId.replace("tab_button_", "tab_");

                    // Update active tab
                    tabbed.querySelectorAll(".flecs-snippet-tabs:not(:empty) > ul > li").forEach((tab, tabIndex) => {
                        if (tab.id == tabId) {
                            tab.classList.add("selected");
                        } else {
                            tab.classList.remove("selected");
                        }
                    });
                });
            };

            document.querySelectorAll(".flecs-snippet-tabs:not(:empty)").forEach((tabbed, tabbedIndex) => {
                let tabsOverview = document.createElement("div");
                tabsOverview.classList.add("flecs-snippet-tabs-overview");

                tabbed.querySelectorAll(".flecs-snippet-tabs:not(:empty) > ul > li").forEach((tab, tabIndex) => {
                    tab.classList.add("flecs-snippet-tabs-tab");
                    tab.id = "tab_" + tabbedIndex + "_" + tabIndex;

                    let header = tab.querySelector(".tab-title");
                    if (!header) {
                        console.error("Tabbed element doesn't have a header", tab);
                        return;
                    }
                    header.title = header.textContent;

                    let tabLink = document.createElement("button");
                    tabLink.id = "tab_button_" + tabbedIndex + "_" + tabIndex;
                    tabLink.classList.add("tab-button");
                    tabLink.appendChild(header);
                    tabLink.addEventListener("click", () => changeTab(header.textContent));

                    tabsOverview.appendChild(tabLink);

                    if (tabIndex == 0) {
                        tab.classList.add("selected");
                        tabLink.classList.add("active");
                    }
                });

                let tabsOverviewContainer = document.createElement("div");
                tabsOverviewContainer.classList.add("flecs-snippet-tabs-overview-container");
                tabsOverviewContainer.appendChild(tabsOverview);

                tabbed.prepend(tabsOverviewContainer);
            });

            // Restore active tab from local storage
            let activeTabHistory = JSON.parse(localStorage.getItem("activeTabHistory"));
            if (activeTabHistory) {
                changeTab(activeTabHistory[activeTabHistory.length - 1]);
            }
        });
    }
}
