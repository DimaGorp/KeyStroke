cmake_minimum_required(VERSION 3.28)

set(FETCHCONTENT_BASE_DIR "${DEPENDENCIES_BUILD_FOLDER}/libs/" CACHE STRING "FETCH CONTENT Dependency folder")
set(FETCHCONTENT_QUIET OFF)

include(FetchContent)

# Fetch and build wxWidgets
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
message(STATUS "wxWidgets source: ${wxWidgets_SOURCE_DIR}")
message(STATUS "wxWidgets binary: ${wxWidgets_BINARY_DIR}")

# Fetch wxMathPlot
message(STATUS "Fetching wxMathPlot...")
FetchContent_Declare(
    wxMathPlot
    GIT_REPOSITORY https://github.com/mkarmona/wxMathPlot.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  nonexistent # Skip its CMakeLists.txt
)
FetchContent_MakeAvailable(wxMathPlot)
message(STATUS "wxMathPlot source: ${wxmathplot_SOURCE_DIR}")
message(STATUS "wxMathPlot binary: ${wxmathplot_BINARY_DIR}")

# Define wxMathPlot manually
add_library(wxMathPlot STATIC
    ${wxmathplot_SOURCE_DIR}/mathplot.cpp
)
target_include_directories(wxMathPlot PUBLIC ${wxmathplot_SOURCE_DIR})
target_link_libraries(wxMathPlot PUBLIC wx::core wx::base)

# Export variables
set(wxWidgets_libs wx::core wx::base wx::richtext PARENT_SCOPE)
set(wxWidgets_include_dirs 
    "${wxWidgets_SOURCE_DIR}/include"
    "${wxWidgets_BINARY_DIR}/lib/vc_x64_lib/mswud"
    PARENT_SCOPE
)