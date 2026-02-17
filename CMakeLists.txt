cmake_minimum_required(VERSION 3.16)
project(Tuto LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Widgets
)
qt_standard_project_setup()

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/Classe/*.cpp"
    "${CMAKE_SOURCE_DIR}/Popup/*.cpp"
    "${CMAKE_SOURCE_DIR}/UI/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/Classe/*.h"
    "${CMAKE_SOURCE_DIR}/Popup/*.h"
    "${CMAKE_SOURCE_DIR}/UI/*.h"
)

qt_add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${HEADERS}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Widgets
)
