include(FetchContent)

# --------------------------------------------------------------------

# Offline mode (uncomment next line to enable)
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

# Libraries

# --------------------------------------------------------------------

# fmt with println

project_log("FetchContent: fmt")

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 87c066a35b7cc70bb7d438a543c8b49b577e61f4
)
FetchContent_MakeAvailable(fmt)

# --------------------------------------------------------------------

project_log("FetchContent: wheels")

FetchContent_Declare(
        wheels
        GIT_REPOSITORY https://gitlab.com/Lipovsky/wheels.git
        GIT_TAG 74ae096b83365f193cb8d8f75dc047fecff4df01
)
FetchContent_MakeAvailable(wheels)

# # --------------------------------------------------------------------

# # Unique Function

# project_log("FetchContent: function2")

# FetchContent_Declare(
#         function2
#         GIT_REPOSITORY https://github.com/Naios/function2.git
#         GIT_TAG 4.2.2
# )
# FetchContent_MakeAvailable(function2)

# # --------------------------------------------------------------------

# # tl::expected

# project_log("FetchContent: expected")

# FetchContent_Declare(
#         expected
#         GIT_REPOSITORY https://github.com/TartanLlama/expected.git
#         GIT_TAG cmake_love
# )
# FetchContent_MakeAvailable(expected)

# # --------------------------------------------------------------------

# project_log("FetchContent: weave")

# FetchContent_Declare(
#         weave
#         GIT_REPOSITORY https://github.com/E1pp/weave.git
#         GIT_TAG main
# )
# FetchContent_MakeAvailable(weave)

# # --------------------------------------------------------------------
