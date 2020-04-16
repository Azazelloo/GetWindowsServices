cmake_minimum_required(VERSION 3.15)

project(ServicesDllProj)

ADD_DEFINITIONS(-D_UNICODE)

set(SOURCES
		#dllmain.cpp
		main.cpp
		#pch.cpp
)

set(HEADERS
		#framework.h
		Header.h
		#pch.h
)

set ( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

add_library(ServicesDll SHARED ${SOURCES} ${HEADERS})

set_target_properties(ServicesDll PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/library)

set_target_properties(
		ServicesDll PROPERTIES
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED ON
)
