# cmake/CppWarningFlags.cmake
#
# Creates an INTERFACE target `myapp::warnings`
# that you can link to any target to enable a sane,
# cross-compiler set of warning flags.  You *never* add
# raw compiler options anywhere else in the tree.

add_library(acquisition_WARNINGS INTERFACE)

# -------------------------------------------
#   CMake options for callers / CI
# -------------------------------------------
option(ACQUISITION_WARNINGS_AS_ERRORS      "Treat warnings as errors (CI)" OFF)
option(ACQUISITION_ENABLE_EXTRA_WARNINGS   "Enable very pedantic flags"    ON)

# -------------------------------------------
#   Compiler-specific flags
# -------------------------------------------
set(_common   "-Wall" "-Wextra" "-Wpedantic" "-Wshadow" "-Wconversion")
set(_errors   "-Werror")
set(_extra_gcc  "-Wduplicated-cond" "-Wduplicated-branches" "-Wlogical-op")
set(_extra_cl  "/permissive-" "/w44265" "/w44062")        # MSVC level-4 extras

if(MSVC)
    target_compile_options(acquisition_WARNINGS INTERFACE
        /W4
        $<$<BOOL:${ACQUISITION_WARNINGS_AS_ERRORS}>:${_extra_cl}>
        $<$<BOOL:${ACQUISITION_WARNINGS_AS_ERRORS}>:/WX>
    )
else()  # GCC / Clang / AppleClang
    target_compile_options(acquisition_WARNINGS INTERFACE
        ${_common}
        $<$<BOOL:${ACQUISITION_WARNINGS_AS_ERRORS}>:${_extra_gcc}>
        $<$<BOOL:${ACQUISITION_WARNINGS_AS_ERRORS}>:${_errors}>
    )
endif()

# -------------------------------------------
#   Public helper macro for convenience
# -------------------------------------------
function(acquisition_enable_warnings TARGET)
    target_link_libraries(${TARGET} PRIVATE acquisition::warnings)
endfunction()
