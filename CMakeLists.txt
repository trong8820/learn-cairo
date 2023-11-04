cmake_minimum_required(VERSION 3.22)

project(learn-cairo VERSION 1.0.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_EXTENSIONS OFF)

# Detect platform
#WINDOWS   	=  Windows
#ANDROID    =  Android
#LINUX      =  Linux
#MACOS      =  macOS
#IOS        =  iOS
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(WINDOWS TRUE)
	set(PLATFORM_FOLDER windows)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
	set(ANDROID TRUE)
	set(PLATFORM_FOLDER android)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(ANDROID)
		set(ANDROID TRUE)
		set(PLATFORM_FOLDER android)
	else()
		set(LINUX TRUE)
		set(PLATFORM_FOLDER linux)
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(APPLE TRUE)
	set(MACOS TRUE)
	set(PLATFORM_FOLDER macos)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
	set(APPLE TRUE)
	set(IOS TRUE)
	set(PLATFORM_FOLDER ios)
else()
	message(FATAL_ERROR "Unsupported platform, CMake will exit")
	return()
endif()

if(APPLE)
	enable_language(C OBJC)
	set(CMAKE_UNITY_BUILD OFF)
else()
	enable_language(C)
	set(CMAKE_UNITY_BUILD ON)
endif()

# 3rdparty include
include_directories (
	${CMAKE_SOURCE_DIR}/3rdparty/cairo-1.18.0/${PLATFORM_FOLDER}/include
	${CMAKE_SOURCE_DIR}/3rdparty/png-1.6.40/${PLATFORM_FOLDER}/include
	${CMAKE_SOURCE_DIR}/3rdparty/zlib-ng-2.1.4/${PLATFORM_FOLDER}/include
)

# 3rdparty libs
link_directories(
	${CMAKE_SOURCE_DIR}/3rdparty/cairo-1.18.0/${PLATFORM_FOLDER}/lib
	${CMAKE_SOURCE_DIR}/3rdparty/png-1.6.40/${PLATFORM_FOLDER}/lib
	${CMAKE_SOURCE_DIR}/3rdparty/zlib-ng-2.1.4/${PLATFORM_FOLDER}/lib
)

add_executable(${PROJECT_NAME} WIN32 src/main.c)
if(MSVC)
	target_compile_options(${PROJECT_NAME} PUBLIC /arch:AVX2)
else()
	target_compile_options(${PROJECT_NAME} INTERFACE "-mavx2")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE libcairo libpixman-1 libpixman-sse2 libpixman-ssse3)
