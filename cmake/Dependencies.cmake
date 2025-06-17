include(FetchContent)

FetchContent_Declare(
    boost-headers-only
    GIT_REPOSITORY https://github.com/gerwaric/boost-headers-only.git
    GIT_TAG        boost-1.88.0
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.3
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG        v5.4.1
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(boost-headers-only glaze spdlog)
