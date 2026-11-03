
cmake_minimum_required(VERSION 3.19)

# Debe ir ANTES de cualquier llamada a find_package
# -----------------------------------------------------------------------------
# 1. Configuración de ruta de Qt 6 para Linux
# -----------------------------------------------------------------------------
if(UNIX AND NOT APPLE)
    set(QT_INSTALL_DIR "$ENV{HOME}/Qt/6.12.0/gcc_64")
    list(PREPEND CMAKE_PREFIX_PATH "${QT_INSTALL_DIR}")
endif()

project(AplicacionClienteQt LANGUAGES CXX)

include(GNUInstallDirs)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Mqtt)

qt_standard_project_setup()

# -----------------------------------------------------------------------------
# 1. Búsqueda de cabeceras de Qwt (Fundamental en Linux)
# -----------------------------------------------------------------------------
find_path(QWT_INCLUDE_DIR
    NAMES qwt.h
    HINTS
        "${QT_INSTALL_DIR}/include"
        "/usr/include"
        "/usr/local/include"
        "/usr/local/qwt-6.3.0/include"
    PATH_SUFFIXES qwt qwt-qt6 qwt6-qt6 qwt6
)

# -----------------------------------------------------------------------------
# 2. Búsqueda de librerías de Qwt
# -----------------------------------------------------------------------------
# En Linux la librería suele llamarse qwt-qt6 o libqwt-qt6.so
find_library(QWT_LIBRARY_RELEASE
    NAMES qwt-qt6 qwt6-qt6 qwt6 qwt libqwt libqwt-qt6 qwt.dll libqwt.dll.a
    HINTS
        "${QT_INSTALL_DIR}/lib"
        "/usr/lib"
        "/usr/local/qwt-6.3.0/lib"
        "/usr/lib/x86_64-linux-gnu"
)

find_library(QWT_LIBRARY_DEBUG
    NAMES qwtd qwt-qt6d qwt6-qt6d qwtd.dll libqwtd.dll.a
    HINTS
        "${QT_INSTALL_DIR}/lib"
        "/usr/lib"
        "/usr/lib/x86_64-linux-gnu"
)

# En Linux no se suele instalar una versión 'Debug' separada (qwtd); usamos Release como fallback
if(NOT QWT_LIBRARY_DEBUG)
    set(QWT_LIBRARY_DEBUG ${QWT_LIBRARY_RELEASE})
endif()


qt_add_executable(AplicacionClienteQt
    WIN32 MACOSX_BUNDLE
    main.cpp
    GUIPanel.cpp
    GUIPanel.h
    GUIPanel.ui
)

# Incluir las rutas de cabeceras de Qwt encontradas
if(QWT_INCLUDE_DIR)
    target_include_directories(AplicacionClienteQt PRIVATE ${QWT_INCLUDE_DIR})
endif()

target_link_libraries(AplicacionClienteQt
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt::Mqtt
        $<$<CONFIG:Debug>:${QWT_LIBRARY_DEBUG}>
        $<$<NOT:$<CONFIG:Debug>>:${QWT_LIBRARY_RELEASE}>
)


# QWT_DLL solo es necesario para DLLs en Windows (evita conflictos de macros en Linux)
if(WIN32)
    target_compile_definitions(AplicacionClienteQt PRIVATE QWT_DLL)
endif()

install(TARGETS AplicacionClienteQt
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET AplicacionClienteQt
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
