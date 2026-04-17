cmake_minimum_required(VERSION 3.6)

project(chess)

set(CMAKE_CXX_STANDARD 20)

add_executable(application
	src/application.hpp
	src/application.cpp
	src/audio.hpp
	src/audio.cpp
	src/input.hpp
	src/input.cpp
	src/ui.hpp
	src/ui.cpp
	src/asset.hpp
	src/asset.cpp
	src/main.cpp

	src/vendor.cpp

	# sources from vendor libraries
	vendor/nanosvg/nanosvg.h
)

add_library(chess
	src/chess.hpp
	src/chess.cpp
	src/log.hpp
	src/log.cpp
	src/common.hpp
	src/common.cpp
	src/math_util.hpp
	src/math_util.cpp
	src/template.hpp
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT application)

add_subdirectory(vendor/SDL-3.4.4 EXCLUDE_FROM_ALL)

# turn unused image formats off
set(SDLIMAGE_ANI  OFF)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP  OFF)
set(SDLIMAGE_GIF  OFF)
set(SDLIMAGE_JPG  OFF)
set(SDLIMAGE_JXL  OFF)
set(SDLIMAGE_LBM  OFF)
set(SDLIMAGE_PCX  OFF)
set(SDLIMAGE_PNM  OFF)
set(SDLIMAGE_QOI  OFF)
set(SDLIMAGE_SVG  OFF)
set(SDLIMAGE_TGA  OFF)
set(SDLIMAGE_TIF  OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_XCF  OFF)
set(SDLIMAGE_XPM  OFF)
set(SDLIMAGE_XV   OFF)
# png
set(SDLIMAGE_PNG ON)
add_subdirectory(vendor/SDL_image-3.4.2 EXCLUDE_FROM_ALL)

add_subdirectory(vendor/SDL_mixer-3.2.0 EXCLUDE_FROM_ALL)
add_subdirectory(vendor/SDL_ttf-3.2.2 EXCLUDE_FROM_ALL)

target_include_directories(application PRIVATE vendor/SDL-3.4.4/include/)
target_include_directories(application PRIVATE vendor/SDL_image-3.4.2/include/)
target_include_directories(application PRIVATE vendor/SDL_mixer-3.2.0/include)
target_include_directories(application PRIVATE vendor/SDL_ttf-3.2.2/include/)

target_include_directories(application PRIVATE vendor/nanosvg)

target_link_libraries(
	application
	PRIVATE
	chess
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
	SDL3_ttf::SDL3_ttf
)
