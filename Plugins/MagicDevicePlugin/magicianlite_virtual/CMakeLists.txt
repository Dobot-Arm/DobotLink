cmake_minimum_required(VERSION 3.21)
project(simulation_magicianlite)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/msvc2019_64/lib/cmake")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        SerialPort
        REQUIRED)

add_executable(simulation_magicianlite
        src/main.cpp
        src/algorithm/rtKinematicCalc.cpp
        src/algorithm/variable.cpp
        src/algorithm/moveCircle.cpp
        src/algorithm/playBack.cpp
        src/algorithm/alarms.cpp
        src/algorithm/teach.cpp

        src/ui/main_menu.cpp
        src/ui/main_menu.h
        src/runtime/dcps/dcps.h
        src/runtime/dcps/dcps.cpp

        src/runtime/log/log.cpp
        src/runtime/dcps/cmd.cpp
        src/mm/profile.cpp
        src/mm/profile.h
        src/runtime/dcps/semaphore.cpp

        src/function_ctrl/collision_check/CollisionCheckFunc.cpp
        src/runtime/dcps/cmd_id.h)

target_link_libraries(simulation_magicianlite
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::SerialPort
        )

if (MSVC)
    target_compile_options(simulation_magicianlite PRIVATE /wd4819 /wd4305)
endif ()

target_include_directories(simulation_magicianlite PRIVATE include src/runtime src)
target_compile_options(simulation_magicianlite PRIVATE /W3 /WX /wd4200)
target_compile_definitions(simulation_magicianlite PRIVATE _CRT_SECURE_NO_WARNINGS)

if (WIN32)
    set(DEBUG_SUFFIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets SerialPort)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()

