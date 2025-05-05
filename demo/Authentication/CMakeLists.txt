
project(${PROJECT_NAME})
set(UICORE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/Core/public" CACHE STRING "UI CORE INCLUDE DIRECTORIES" FORCE)
set(UICORE_SOURCE_DIRS "${PROJECT_SOURCE_DIR}/Core/private" CACHE STRING "UI CORE SOURCE DIRECTORIES" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_library(UICore STATIC 
            "${UICORE_SOURCE_DIRS}/HomePage/HomePage.cpp"
            "${UICORE_SOURCE_DIRS}/WelcomePage/WelcomePage.cpp"
            "${UICORE_SOURCE_DIRS}/LoginPage/LoginPage.cpp"
            "${UICORE_SOURCE_DIRS}/SignupPage/SignupPage.cpp"
)

target_include_directories(UICore PRIVATE 
            ${wxWidgets_include_dirs}
            "${UICORE_INCLUDE_DIRS}/WelcomePage/"
            "${UICORE_INCLUDE_DIRS}/HomePage/"
            "${UICORE_INCLUDE_DIRS}/LoginPage/"
            "${UICORE_INCLUDE_DIRS}/SignupPage/"
            "${UICORE_INCLUDE_DIRS}"
)
message("${EVENT_INCLUDE_DIRS}")
target_link_libraries(UICore PRIVATE ${wxWidgets_libs} GMM EventSystem wxMathPlot)
target_include_directories(UICore PRIVATE ${wxWidgets_include_dirs} ${wxmathplot_SOURCE_DIR} "${CMAKE_BINARY_DIR}/Config/")