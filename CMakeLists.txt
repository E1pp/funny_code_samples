cmake_minimum_required(VERSION 3.14)
project(utility)

include(cmake/Logging.cmake)

include(cmake/CheckCompiler.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/Logging.cmake)
include(cmake/Sanitize.cmake)
include(cmake/Platform.cmake)

add_subdirectory(third_party)

include(cmake/Test.cmake)

add_subdirectory(utility)

add_subdirectory(bin)
