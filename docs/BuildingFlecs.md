# Building Flecs
This manual describes how to compile the Flecs source code.

## Dependencies
The following libraries need to be linked when building Flecs:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">Windows</b>

| Library       | Used by    | Used for             |
|---------------|------------|----------------------|
| dbghelp       | Core       | Logging stack traces |
| ws2_32        | HTTP addon | Allows [explorer](https://www.flecs.dev/explorer/) to connect |

</li>
<li><b class="tab-title">Linux</b>

| Library       | Used by      | Used for             |
|---------------|--------------|----------------------|
| rt            | Core         | Time functions       |
| pthread       | OS_API addon | Threading functions  |
| m             | Core         | Math functions       |

</li>
<li><b class="tab-title">MacOS</b>

No dependencies required on MacOS.

</li>
</ul>
</div>

## Build with distr files
The easiest way to add Flecs to a project is to add the [distr/flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/distr/flecs.c) and [distr/flecs.h](https://raw.githubusercontent.com/SanderMertens/flecs/master/distr/flecs.h) files to the project source. When building, make sure your build system is setup to do the following:

- If it is a C++ project, make sure to compile [distr/flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/distr/flecs.c) as C code, for example by using `gcc`/`clang` instead of `g++`/`clang++`.

- If you are building on Windows and you're not using the Microsoft Visual Studio compiler, make sure to add `-lws2_32 -ldbghelp` to the end of the linker command.

- When compiling Flecs with `gcc`/`clang`, add `-std=gnu99` to the compiler command. This ensures that addons that rely on time & socket functions are compiled correctly.

- C++ files that use Flecs must be compiled with `-std=c++17` or higher.

### Command line examples
These examples are just for illustration. For most projects it is recommended to use a build system like cmake, bazel or bake.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">Windows</b>

**Clang, C**
```bash
clang my_project.c -c -o my_project.o
clang flecs.c -c -o flecs.o
clang my_project.o flecs.o -ldbghelp -lws2_32 -o my_project
```

**Clang, C++**
```bash
clang++ my_project.cpp -std=c++17 -c -o my_project.o
clang flecs.c -c -o flecs.o
clang++ my_project.o flecs.o -ldbghelp -lws2_32 -o my_project
```

**MSVC, C**
```bash
cl /nologo /c my_project.c /Fo:my_project.obj
cl /nologo /c flecs.c /Fo:flecs.obj
cl /nologo my_project.obj flecs.obj dbghelp.lib ws2_32.lib /Fe:my_project.exe
```

**MSVC, C++**
```bash
cl /nologo /EHsc /c my_project.cpp /Fo:my_project.obj
cl /nologo /TC /c flecs.c /Fo:flecs.obj
cl /nologo my_project.obj flecs.obj dbghelp.lib ws2_32.lib /Fe:my_project.exe
```

</li>
<li><b class="tab-title">Linux</b>

**GCC, C**
```bash
gcc my_project.c -std=gnu99 -c -o my_project.o
gcc flecs.c -std=gnu99 -c -o flecs.o
gcc my_project.o flecs.o -lrt -lpthread -lm -o my_project
```

**GCC, C++**
```bash
g++ my_project.cpp -std=c++17 -c -o my_project.o
gcc flecs.c -std=gnu99 -c -o flecs.o
g++ my_project.o flecs.o -lrt -lpthread -lm -o my_project
```

**Clang, C**
```bash
clang my_project.c -std=gnu99 -c -o my_project.o
clang flecs.c -std=gnu99 -c -o flecs.o
clang my_project.o flecs.o -lrt -lpthread -lm -o my_project
```

**Clang, C++**
```bash
clang++ my_project.cpp -std=c++17 -c -o my_project.o
clang flecs.c -std=gnu99 -c -o flecs.o
clang++ my_project.o flecs.o -lrt -lpthread -lm -o my_project
```

</li>
<li><b class="tab-title">Macos</b>

**Clang, C**
```bash
clang my_project.c -std=gnu99 -c -o my_project.o
clang flecs.c -std=gnu99 -c -o flecs.o
clang my_project.o flecs.o -o my_project
```

**Clang, C++**
```bash
clang++ my_project.cpp -std=c++17 -c -o my_project.o
clang flecs.c -std=gnu99 -c -o flecs.o
clang++ my_project.o flecs.o -o my_project
```

</li>
</ul>
</div>

### Build as shared library
To build the [distr/flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/distr/flecs.c) and [distr/flecs.h](https://raw.githubusercontent.com/SanderMertens/flecs/master/distr/flecs.h) files as a shared library, remove this line from the top of `flecs.h`:

```c
#define flecs_STATIC
```

When compiling [distr/flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/distr/flecs.c), make sure to define `flecs_EXPORTS`, for example by adding `-Dflecs_EXPORTS` to the compiler command. This ensures that Flecs symbols are exported.

The following example shows how to create a shared library using flecs.c and flecs.h with clang on Linux:

```bash
clang flecs.c -c -Dflecs_EXPORTS -o flecs.o
clang flecs.o -lrt -lpthread -lm -shared -o libflecs.so
```

## Updating the distr files
If you've made a modification to the Flecs source code (in the `src` or `include` folders) and you want to update the flecs.c and flecs.h files in the `distr` folder, run bake once in the root of the repository:

```
bake
```

Make sure that the files in `distr` now have your modifications. If they have not been updated, it's possible that the date of the files is newer than your modifications. If this happens, remove the `distr` folder and run `bake` again. This will regenerate files with the latest modifications.

It is not recommended to modify the flecs.c and flecs.h directly.

## Build from source
This will build the files in `src` and `include` with one of the following build systems:

### Cmake
Run this command in the repository root to create flecs library binaries:

```bash
cmake -S . -B cmake_build && cmake --build cmake_build
```

After the command finishes, the `cmake_build` folder will contain one static and one dynamic library binary.

You can also add the Flecs repository as dependency for another cmake project by adding the following line to the `CMakeLists.txt` of the project:

```cmake
add_subdirectory(external/flecs)
```

Alternatively, you can use `FetchContent` to automatically download Flecs:

```cmake
include(FetchContent)

FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG master
)

FetchContent_MakeAvailable(flecs)
```

Then to link with flecs, add the following to `CMakeLists.txt`:

```cmake
target_link_libraries(${PROJECT_NAME} flecs::flecs_static)
```

### Meson
To build Flecs with meson, run the following commands in the repository root:

```bash
meson meson_build
cd meson_build
meson compile
```

### Bazel
To build Flecs with bazel, run the following command in the repository root:

```bash
bazel build //:flecs
```

### Bake
To build Flecs with bake, run the following command in the repository root:

```bash
bake
```

After this Flecs should show up in the bake environment when doing:

```bash
bake list
```

To create a new Flecs C project, do:

```bash
bake new my_project -t flecs
```

To create a new Flecs C++ project, do:
```bash
bake new my_project -t flecs --language c++
```

To run the project, do:

```bash
cd my_project
bake run
```

## Building the documentation
To generate the HTML version of the documentation, run the following command:

```
doxygen docs/cfg/Doxyfile
```

The generated HTML files will be written to docs/html.

## Running the test suite
Flecs uses the test framework that ships with bake. While it is possible to run tests with other build systems, it is recommended to use bake.

### Bake
Make sure to at least build flecs once, by running this in the repository root:

```bash
bake
```

Make sure that Flecs is listed in the bake environment by doing:

```bash
bake list
```

Then run the command for the test project you want to run:

```bash
bake run test/core
bake run test/addons
bake run test/query
bake run test/meta
bake run test/script
bake run test/cpp
bake run test/collections
```

To run a single test suite, do:

```bash
bake run test/core -- Sparse
```

To run a single test case, do:

```bash
bake run test/core -- Sparse.get
```

Some test suites are parameterized. To provide a value for a parameter, do:

```bash
bake run test/core - Sparse --param fragment=no
```

You can specify the number of threads with `-j` (default is 4):

```bash
bake run test/core -- -j 10
```

To add a new test case, first add it to the `project.json` of the project you want to add it to, for example:

```json
"testcases": [
    // existing test cases...
    "my_new_testcase"
]
```

Then build the test project once:

```bash
bake test/core
```

The suite to which you added the test case will now have an empty test function:

```c
void Sparse_my_test_case(void) {
    // Implement testcase
}
```

When running the test suite now you will see:

```
EMPTY Sparse.my_test_case (add test statements)
To run/debug your test, do:
export $(bake env)
test/core/bin/arm64-Darwin-debug/core Sparse.my_test_case --param fragment=yes
```

This indicates that the test case isn't testing anything yet. Add one or more `test_assert` (or equivalent) calls to the code to make this message go away.

### Cmake (experimental)
First make sure to clone [bake](https://github.com/SanderMertens/bake).

Run the following commands to run all the tests:

```bash
# Generate make files for Flecs and tests
cmake -DFLECS_TESTS=ON -DBAKE_DIRECTORY="path to cloned bake repository"

# Build flecs and test suites
cmake --build . -j 4

# Run the tests
ctest -C Debug --verbose
```

### Bazel (experimental)
Run the following commands to run all the tests:

```bash
bazel test --test_output=all //test/...
```

## Building on Emscripten
When building for emscripten, add the following command line options to the `emcc` link command:
```bash 
-s ALLOW_MEMORY_GROWTH=1 
-s STACK_SIZE=1mb
-s EXPORTED_RUNTIME_METHODS=cwrap 
-s MODULARIZE=1 
-s EXPORT_NAME="my_app"
```

## Building on Android
When building for Android, you should:

- download NDK
- check the cmake preset file and update the `cacheVariables` as you like
- run a command like below:

```cmd
cmake -S. -Bbuild_android -GNinja --preset=Android
```

Then run command `ninja` in `build_android` directory to start build.

## Addons
Flecs has a modular architecture that makes it easy to only build the features you really need. By default all addons are built. To customize a build, first define `FLECS_CUSTOM_BUILD`, then add defines for the addons you need. For example:

```c
#define FLECS_CUSTOM_BUILD  // Don't build all addons
#define FLECS_SYSTEM        // Build FLECS_SYSTEM
```

Additionally, you can also specify addons to exclude from a build by adding `NO` to the define:

```c
#define FLECS_NO_LOG
```

The following addons can be configured:

Addon         | Description                                      | Define              |
--------------|--------------------------------------------------|---------------------|
[Cpp](/flecs/group__cpp.html)                              | C++17 API                                        | FLECS_CPP           |
[Module](/flecs/group__c__addons__module.html)             | Organize game logic into reusable modules        | FLECS_MODULE        |
[System](/flecs/group__c__addons__system.html)             | Create & run systems                             | FLECS_SYSTEM        |
[Pipeline](/flecs/group__c__addons__pipeline.html)         | Automatically schedule & multithread systems     | FLECS_PIPELINE      |
[Timer](/flecs/group__c__addons__timer.html)               | Run systems at time intervals or at a rate       | FLECS_TIMER         |
[Meta](/flecs/group__c__addons__meta.html)                 | Flecs reflection system                          | FLECS_META          |
[Units](/flecs/group__c__addons__units.html)               | Builtin unit types                               | FLECS_UNITS         |
[JSON](/flecs/group__c__addons__json.html)                 | JSON format                                      | FLECS_JSON          |
[Doc](/flecs/group__c__addons__doc.html)                   | Add documentation to components, systems & more  | FLECS_DOC           |
[Http](/flecs/group__c__addons__http.html)                 | Tiny HTTP server for processing simple requests  | FLECS_HTTP          |
[Rest](/flecs/group__c__addons__rest.html)                 | REST API for showing entities in the browser     | FLECS_REST          |
Parser                                                     | Parser utilities used by script & query DSL      | FLECS_PARSER        |
[Query DSL](https://www.flecs.dev/flecs/md_docs_2FlecsQueryLanguage.html) | Query DSL parser                  | FLECS_QUERY_DSL     |
[Script](/flecs/group__c__addons__script.html)             | DSL for scenes, assets and configuration         | FLECS_SCRIPT        |
[Stats](/flecs/group__c__addons__stats.html)               | Functions for collecting statistics              | FLECS_STATS         |
[Metrics](/flecs/group__c__addons__metrics.html)           | Create metrics from user-defined components      | FLECS_METRICS       |
[Alerts](/flecs/group__c__addons__alerts.html)             | Create alerts from user-defined queries          | FLECS_ALERTS        |
[Log](/flecs/group__c__addons__log.html)                   | Extended tracing and error logging               | FLECS_LOG           |
[Journal](/flecs/group__c__addons__journal.html)           | Journaling of API functions                      | FLECS_JOURNAL       |
[App](/flecs/group__c__addons__app.html)                   | Flecs application framework                      | FLECS_APP           |
[OS API Impl](/flecs/group__c__addons__os__api__impl.html) | Default OS API implementation for Posix/Win32    | FLECS_OS_API_IMPL   |

The following example shows how to build Flecs without any addons using bake. Run the command from the repository root:

```
bake -D FLECS_CUSTOM_BUILD
```

To only build Flecs with the system addon, do:

```
bake -D FLECS_CUSTOM_BUILD -D FLECS_SYSTEM
```

The following example shows how to build Flecs without any addons using clang:

```
clang my_project.c -std=gnu99 -c -o my_project.o
clang flecs.c -DFLECS_CUSTOM_BUILD -std=gnu99 -c -o flecs.o
clang my_project.o flecs.o -lrt -lpthread -lm -o my_project
```

To only build Flecs with clang with the system addon, do:

```
clang my_project.c -std=gnu99 -c -o my_project.o
clang flecs.c -DFLECS_CUSTOM_BUILD -DFLECS_SYSTEM -std=gnu99 -c -o flecs.o
clang my_project.o flecs.o -lrt -lpthread -lm -o my_project
```
