cmake_minimum_required(VERSION 3.28)

set(CMAKE_COLOR_DIAGNOSTICS OFF)
set(CMAKE_CXX_STANDARD 20)

project(gemupg VERSION 0.9)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	message(STATUS "Using clang")
	add_compile_options(-Wno-int-conversions)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	message(STATUS "Using GCC")
	add_compile_options(-Wno-pmf-conversions)
endif()

# SDL
find_package(SDL3 CONFIG REQUIRED)

# SDL_ttf
# ! needs to run script under include/SDL_ttf/external first!
set(SDLTTF_VENDORED ON)
set(SDLTTF_HARFBUZZ OFF)
add_subdirectory(include/SDL_ttf EXCLUDE_FROM_ALL)

# IMGUI
file (GLOB IMGUI_SOURCES CONFIGURE_DEPENDS "include/imgui/*.cpp")
add_library(IMGUI ${IMGUI_SOURCES})
target_link_libraries(IMGUI PUBLIC SDL3::SDL3)
target_include_directories(IMGUI PUBLIC "include/imgui")

# GEMUPG

file (GLOB_RECURSE gemupg_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file (GLOB_RECURSE gemupg_HEADERS CONFIGURE_DEPENDS "src/*.h")

set (gemupg_INCLUDE_DIRS "")
foreach (_headerFile ${gemupg_HEADERS})
get_filename_component(_dir ${_headerFile} PATH)
list (APPEND geumpg_INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES gemupg_INCLUDE_DIRS)

add_executable(${PROJECT_NAME} ${gemupg_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${gemupg_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE "include")

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 IMGUI SDL3_ttf::SDL3_ttf)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
	COMMAND_EXPAND_LISTS
	)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/assets/*
	COMMAND_EXPAND_LISTS
	)