cmake_minimum_required(VERSION 3.16)

project(NexusUtils
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Version appli (unique)
# -------------------------
set(APP_VERSION "1.0.0")
string(REPLACE "." "," APP_VERSION_COMMA "${APP_VERSION}")
add_definitions(-DAPP_VERSION="${APP_VERSION}")

# ------------------------- 
# Génération du fichier RC 
# ------------------------- 
if (WIN32) 
    configure_file( 
        ${CMAKE_SOURCE_DIR}/app.rc.in 
        ${CMAKE_BINARY_DIR}/app.rc 
        @ONLY 
    ) 
endif()

# -------------------------
# OpenSSL
# -------------------------
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
if (WIN32)
    add_definitions(-DOPENSSL_USE_STATIC_LIBS)
endif()

# -------------------------
# Qt
# -------------------------
find_package(Qt6 REQUIRED COMPONENTS 
    Widgets 
    WebEngineWidgets
)
qt_standard_project_setup()

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
qt_add_resources(${PROJECT_NAME} "resources"
    PREFIX "/"
    FILES
        Asset/Icone.png
)

# -------------------------
# Ressources Windows (.rc)
# -------------------------
if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE app.rc)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
endif()

# -------------------------
# Link
# -------------------------
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt6::Widgets
        Qt6::WebEngineWidgets
        OpenSSL::SSL
        OpenSSL::Crypto
)
