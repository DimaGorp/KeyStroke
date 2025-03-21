cmake_minimum_required(VERSION 3.28)
set(FETCHCONTENT_BASE_DIR "${DEPENDENCIES_BUILD_FOLDER}/libs/" CACHE STRING "FETCH CONTENT Dependency folder ")
# Enable verbose fetch output from the start
set(FETCHCONTENT_QUIET OFF)  # Verbose output for debugging

include(FetchContent)

message(STATUS "Fetching wxWidgets...")

set(wxBUILD_SHARED OFF CACHE BOOL "Build wx libraries as shared libs")
set(wxBUILD_PRECOMP OFF CACHE BOOL "Use precompiled headers")
set(wxBUILD_MONOLITHIC OFF CACHE BOOL "Build a single library")

FetchContent_Declare(
    wxWidgets 
    GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
    GIT_TAG        v3.1.4
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(wxWidgets)

list(APPEND wxWidgets_libs wxcore wxbase)
list(APPEND wxWidgets_include_dirs 
    "${wxWidgets_SOURCE_DIR}/include"
    "${wxWidgets_BINARY_DIR}/lib/vc_x64_lib/mswud"
)

set(wxWidgets_libs ${wxWidgets_libs} PARENT_SCOPE)
set(wxWidgets_include_dirs ${wxWidgets_include_dirs} PARENT_SCOPE)