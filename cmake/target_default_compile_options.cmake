# this seems redundant as the target
# property is set. but for apple clang
# this seems to be needed

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(target_default_compile_options_c THIS)

    set_target_properties(${THIS} PROPERTIES
            LINKER_LANGUAGE C
            C_STANDARD 99
            C_STANDARD_REQUIRED ON
            C_VISIBILITY_PRESET hidden)

endfunction()

function(target_default_compile_options_cxx THIS)

    set_target_properties(${THIS} PROPERTIES
            LINKER_LANGUAGE CXX
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED ON)

endfunction()
