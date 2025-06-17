# cmake/PreventInSourceBuild.cmake
#
# ──────────────────────────────────────────────────────────
#  Stop users from running `cmake .` inside the source tree.
#  It keeps the repo clean and avoids CMake cache collisions.
# ──────────────────────────────────────────────────────────

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "❌  In-source builds are not allowed.\n"
        "    Create a separate build folder, then run:\n"
        "      mkdir -p build && cd build\n"
        "      cmake ..  -GNinja|Unix Makefiles|…"
    )
endif()
