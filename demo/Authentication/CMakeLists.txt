
project(${PROJECT_NAME})
set(UICORE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/Core/public" CACHE STRING "UI CORE INCLUDE DIRECTORIES" FORCE)
set(UICORE_SOURCE_DIRS "${PROJECT_SOURCE_DIR}/Core/private" CACHE STRING "UI CORE SOURCE DIRECTORIES" FORCE)
message("${UICORE_SOURCE_DIRS}")
#set(SRC_INCLUDE_DIRS "${SOURCE_FOLDER}/KDModule/public/" CACHE STRING "SOURCE INCLUDE DIRECTORIES")
#set(SRC_SOURCE_DIRS "${SOURCE_FOLDER}/KDModule/private/" CACHE STRING "SOURCE DIRECTORIES")



add_library(Core STATIC 
            "${UICORE_SOURCE_DIRS}/HomePage/HomePage.cpp"
            "${UICORE_SOURCE_DIRS}/WelcomePage/WelcomePage.cpp"
            "${UICORE_SOURCE_DIRS}/LoginPage/LoginPage.cpp"
            "${UICORE_SOURCE_DIRS}/SignupPage/SignupPage.cpp"
)

target_include_directories(Core PRIVATE 
            ${wxWidgets_include_dirs}
            "${UICORE_INCLUDE_DIRS}/WelcomePage/"
            "${UICORE_INCLUDE_DIRS}/HomePage/"
            "${UICORE_INCLUDE_DIRS}/LoginPage/"
            "${UICORE_INCLUDE_DIRS}/SignupPage/"
            "${UICORE_INCLUDE_DIRS}"
)
target_link_libraries(Core PRIVATE ${wxWidgets_libs})
target_include_directories(Core PRIVATE ${wxWidgets_include_dirs})