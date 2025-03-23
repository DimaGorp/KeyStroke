cmake_minimum_required(VERSION 3.28)

#Build UI Libs for demonstraction

add_subdirectory(Authentication)

add_executable(authentication_demo WIN32 "${DEMOS_FOLDER}/Authentication/App.cpp")

target_include_directories(authentication_demo PRIVATE ${wxWidgets_include_dirs} ${UICORE_INCLUDE_DIRS})
target_link_libraries(authentication_demo PRIVATE ${wxWidgets_libs} Core)