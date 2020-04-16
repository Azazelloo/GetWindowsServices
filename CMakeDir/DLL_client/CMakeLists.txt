cmake_minimum_required(VERSION 3.5)

project(DllClientProj)

add_executable(DLL_client main.cpp Header.h)

set_target_properties(
		DLL_client PROPERTIES
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED ON
)

target_include_directories(
		DLL_client
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/../DLL
)

target_link_libraries(DLL_client ServicesDll)