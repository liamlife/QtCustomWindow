cmake_minimum_required(VERSION 3.5)

project(qt_custom_window LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if (CMAKE_BUILD_TYPE MATCHES Release)
	set(CMAKE_BUILD_TYPE Release)
	set(DLL_SUFFIX "")
else ()
	set(CMAKE_BUILD_TYPE Debug)
	set(DLL_SUFFIX "d")
endif ()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}/${TP_VERSION_MAJOR}.${TP_VERSION_MINOR}.${TP_VERSION_PATCH}/")
set(DLL_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}/${TP_VERSION_MAJOR}.${TP_VERSION_MINOR}.${TP_VERSION_PATCH}/)

set(EXECUTABLE_OUTPUT_PATH ${DLL_OUTPUT_PATH}) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${DLL_OUTPUT_PATH})   
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${DLL_OUTPUT_PATH}) 



find_package(Qt5 COMPONENTS Core Widgets Quick QuickWidgets PrintSupport WebSockets Svg LinguistTools REQUIRED)

find_package(Qt5Svg REQUIRED)

include_directories(${Qt5Svg_INCLUDE_DIRS})

# source files________________________________________________________________.

file(GLOB header_files "src/*.h")
file(GLOB source_files "src/*.cpp")

file(GLOB SOURCE_FILES ${header_files} ${source_files})
 
set(TS_FILES src/lang/zh_CN.ts)

list(APPEND ICO_SOURCES "src/logo.rc")

if(ANDROID)
  add_library(${PROJECT_NAME} SHARED
    ${SOURCE_FILES}
    ${TS_FILES}
  )
else()
  add_executable(${PROJECT_NAME}
   ${SOURCE_FILES}
   ${ICO_SOURCES}
    ${TS_FILES}
	src/application.qrc
  )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${DLL_OUTPUT_PATH}")

# include_____________________________________________________________________.
target_include_directories(${PROJECT_NAME} PUBLIC "src")




# link________________________________________________________________________.

# lib.
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets
	Qt5::Quick
	Qt5::QuickWidgets
	Qt5::PrintSupport
	Qt5::WebSockets
	Qt5::Svg
	Dwmapi.lib
)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})


# file group__________________________________________________________________.
IF (WIN32)
	source_group("header\\" FILES ${tp_header_files})
	source_group("source\\" FILES ${tp_source_files})
ENDIF ()


IF (WIN32)
	set(CMD_COPY_PATH "\"${CMAKE_CURRENT_SOURCE_DIR}/src/assets\" \"${DLL_OUTPUT_PATH}/assets/*\"")
	STRING(REPLACE "/" "\\" file_path ${CMD_COPY_PATH})
	add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND xcopy ARGS ${file_path} /D /E /Y)
ELSEIF (APPLE)

ELSEIF (UNIX)

ENDIF ()
