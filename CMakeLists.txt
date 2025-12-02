cmake_minimum_required(VERSION 3.25) # Mandatory for C++26
project(proxy-master VERSION 5.0)
set(CMAKE_CXX_STANDARD 26)
set(BUILD_SHARED_LIBS OFF)      # Determines whether TGUI build a static or dynamic/shared library
set(TGUI_BACKEND GLFW_OPENGL3)  # Sets which backend TGUI will use
set(TGUI_BUILD_GUI_BUILDER OFF)
include(FetchContent)
FetchContent_Declare(
    TGUI
    GIT_REPOSITORY https://github.com/texus/TGUI
    GIT_TAG v1.11.0
)
FetchContent_MakeAvailable(TGUI)
file(GLOB_RECURSE PROXY_MASTER_SRC src/*.cpp)
include_directories(src/libproxy* ui)
include_directories(src/switch_widget)
configure_file(ui/dark.txt ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY)
configure_file(src/cmake_version.hpp.cfg cmake_version.hpp)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${PROXY_MASTER_SRC})
target_compile_options(${PROJECT_NAME} PRIVATE -fhardened -Wall)
target_link_libraries(${PROJECT_NAME} TGUI::TGUI -static-libstdc++)
