cmake_minimum_required(VERSION 3.16)

project(NexusUtils
    VERSION 1.2.3
    LANGUAGES CXX
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Version appli (unique)
# -------------------------
set(APP_VERSION "1.2.3")
string(REPLACE "." "," APP_VERSION_COMMA "${APP_VERSION}")
add_definitions(-DAPP_VERSION="${APP_VERSION}")

# ------------------------- 
# Génération du fichier RC 
# ------------------------- 
if (WIN32) 
    string(REPLACE "\\" "/" CMAKE_SOURCE_DIR_FIXED "${CMAKE_SOURCE_DIR}")
    configure_file( 
        ${CMAKE_SOURCE_DIR}/app.rc.in 
        ${CMAKE_BINARY_DIR}/app.rc 
        @ONLY 
    ) 
    # Copie l'icône dans le build dir pour que le .rc la trouve
    configure_file(
        ${CMAKE_SOURCE_DIR}/Asset/Icone.ico
        ${CMAKE_BINARY_DIR}/Asset/Icone.ico
        COPYONLY
    )
endif()

# -------------------------
# OpenSSL
# -------------------------
if(WIN32)
    set(OPENSSL_USE_STATIC_LIBS TRUE)
endif()
find_package(OpenSSL REQUIRED)
if (WIN32)
    add_definitions(-DOPENSSL_USE_STATIC_LIBS)
endif()

# -------------------------
# Qt Widgets (statique)
# -------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

# -------------------------
# Sources / Headers
# -------------------------
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

# -------------------------
# Exécutable
# -------------------------
qt_add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# -------------------------
# Ressources Qt
# -------------------------
if(WIN32)
    qt_add_resources(${PROJECT_NAME} "resources"
        PREFIX "/"
        FILES
            Asset/Icone.png
    )
endif()

# -------------------------
# Ressources Windows (.rc)
# -------------------------
if (WIN32)
    # Force le compilateur de ressources
    set_source_files_properties(${CMAKE_BINARY_DIR}/app.rc
        PROPERTIES
            LANGUAGE RC
    )
    target_sources(${PROJECT_NAME} PRIVATE app.rc)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
endif()

# -------------------------
# Copie Asset
# -------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Asset
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Asset
)

# -------------------------
# Link
# -------------------------
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
        Qt6::WebEngineWidgets
        OpenSSL::SSL
        OpenSSL::Crypto
)

# -------------------------
# Link Linux
# -------------------------
if (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE 
            X11 
            Xtst
    )
endif()