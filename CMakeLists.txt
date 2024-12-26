cmake_minimum_required(VERSION 3.28)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD 20)

project(gemupg VERSION 0.1)

# SDL
add_subdirectory(include/SDL EXCLUDE_FROM_ALL)

# GEMUPG
add_compile_options(-Wno-pmf-conversions)

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

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
  COMMAND_EXPAND_LISTS
)