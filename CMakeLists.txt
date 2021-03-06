cmake_minimum_required(VERSION 2.8.12)

project(DynamicWeb)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
if(NOT WIN OR MINGW)
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# Base library

set(SRCS
	DynamicWebWindow.h
	DynamicWebWindow.cpp
	DynamicWebBackendInterface_p.h
	DynamicWebBackendInterface.cpp
)
add_library(DynamicWeb SHARED ${SRCS})
target_link_libraries(DynamicWeb Qt5::Core Qt5::Gui Qt5::Widgets)
install(TARGETS DynamicWeb DESTINATION lib)
install(FILES DynamicWebWindow.h DESTINATION include)

# Demo

add_executable(DynamicWebDemo demo.cpp)
target_link_libraries(DynamicWebDemo Qt5::Core Qt5::Gui Qt5::Widgets DynamicWeb)

# Backends

include(CMakeParseArguments)
function(add_backend)
	set(options )
	set(oneValueArgs NAME)
	set(multiValueArgs SRCS LIBS)
	cmake_parse_arguments(ADD_BACKEND "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(_target DynamicWeb_${ADD_BACKEND_NAME})
	if("${ADD_BACKEND_SRCS}" STREQUAL "")
		set(ADD_BACKEND_SRCS backends/DynamicWeb${ADD_BACKEND_NAME}Backend_p.h backends/DynamicWeb${ADD_BACKEND_NAME}Backend.cpp)
	endif()
	list(APPEND ADD_BACKEND_LIBS Qt5::Core Qt5::Gui Qt5::Widgets DynamicWeb)

	add_library(${_target} MODULE ${ADD_BACKEND_SRCS})
	target_link_libraries(${_target} ${ADD_BACKEND_LIBS})
	set_target_properties(${_target} PROPERTIES PREFIX "")
	install(TARGETS ${_target} DESTINATION lib)
endfunction()

find_package(Qt5WebKitWidgets QUIET)
if(Qt5WebKitWidgets_FOUND)
	add_backend(NAME QtWebKit LIBS Qt5::WebKitWidgets)
endif()

find_package(Qt5WebEngineWidgets QUIET)
if(Qt5WebEngine_FOUND)
	add_backend(NAME QtWebEngine LIBS Qt5::WebEngineWidgets)
endif()

find_package(KF5WebKit QUIET)
if(KF5WebKit_FOUND)
	add_backend(NAME KDEWebKit LIBS KF5::WebKit)
endif()

if(APPLE)
	find_library(COCOA_LIBRARY Cocoa)
	find_library(WEBKIT_LIBRARY WebKit)
	if(COCOA_LIBRARY AND WEBKIT_LIBRARY)
		add_backend(NAME OSX
			SRCS backends/DynamicWebOSXBackend_p.h backends/DynamicWebOSXBackend.mm
			LIBS ${COCOA_LIBRARY} ${WEBKIT_LIBRARY}
		)
	endif()
endif()

if(WIN32)
	add_backend(NAME Windows)
endif()
