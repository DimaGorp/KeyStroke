add_executable(authentication_demo "${DEMOS_FOLDER}/Authentication/App.cpp")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# GLFW (submodule in demo/glfw)
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw) # Point to demo/glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(${GLFW_DIR})

target_link_libraries (
	authentication_demo PRIVATE
	SFML
	ImGuiSFML
    OpenGL::GL
    glfw
)
# Include directories
target_include_directories(authentication_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include
)