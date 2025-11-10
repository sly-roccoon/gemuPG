
cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(gemupg VERSION 0.1)

set(BUILD_SHARED_LIBS OFF)
if(NOT BUILD_SHARED_LIBS)
    set(SDL_STATIC ON)
    set(SDLTTF_BUILD_SHARED_LIBS OFF)
endif()

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(SDL_SHARED OFF)
    set(SDL_PTHREADS ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

# SDL
if(NOT SDL3_STATIC)
    find_package(SDL3 CONFIG QUIET)
endif()
if(NOT SDL3_FOUND)
    if(NOT SDL3_FOUND)
        message(STATUS "SDL3 not found, fetching from GitHub...")
    endif()
    include(FetchContent)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.8
    )
    FetchContent_MakeAvailable(SDL3)
else()
    message(STATUS "SDL3 found, using system provided version")
endif()

# SDL_ttf
# create the variable
set(GEMUPG_SYSTEM_SDL_TTF AUTO CACHE STRING "Use system provided SDL-ttf")
#  define list of values GUI will offer for the variable
set_property(CACHE GEMUPG_SYSTEM_SDL_TTF PROPERTY STRINGS AUTO ON OFF)
if(GEMUPG_SYSTEM_SDL_TTF STREQUAL "AUTO")
    find_package(SDL3_ttf CONFIG)
elseif(GEMUPG_SYSTEM_SDL_TTF STREQUAL "ON")
    find_package(SDL3_ttf CONFIG REQUIRED)
endif()

if(SDL3_ttf_FOUND)
    message(STATUS "Using system provided SDL3-ttf")
else()
    include(cmake/UpdateSubmodules.cmake)

    # ! needs to run script under include/SDL_ttf/external first!
    set(SDLTTF_VENDORED ON)
    set(SDLTTF_HARFBUZZ ON)
    set(SDLTTF_PLUTOSVG OFF)
    add_subdirectory(include/SDL_ttf EXCLUDE_FROM_ALL)
    message(STATUS "Using vendored SDL3-ttf")
endif()


# IMGUI
file(GLOB_RECURSE IMGUI_SOURCES CONFIGURE_DEPENDS "include/imgui/*.cpp")
# add_library(IMGUI ${IMGUI_SOURCES})
# target_link_libraries(IMGUI PUBLIC SDL3::SDL3)
# target_include_directories(IMGUI PUBLIC "include/imgui")

# GEMUPG
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Using clang")
    add_compile_options(-Wno-int-conversions)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC")
    add_compile_options(-Wno-pmf-conversions)
endif()

file(GLOB_RECURSE gemupg_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE gemupg_HEADERS CONFIGURE_DEPENDS "src/*.h")

set(gemupg_INCLUDE_DIRS "")
foreach(_headerFile ${gemupg_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list(APPEND geumpg_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES gemupg_INCLUDE_DIRS)

add_executable(${PROJECT_NAME} ${gemupg_SOURCES} ${IMGUI_SOURCES} bt.rc)
target_include_directories(${PROJECT_NAME} PRIVATE ${gemupg_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE "include" "include/imgui")

# TODO: reduce linked libraries to actually used ones...
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)
# Mark SDL3 and SDL3_ttf includes as SYSTEM to suppress warnings from their headers
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    $<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:SDL3_ttf::SDL3_ttf,INTERFACE_INCLUDE_DIRECTORIES>
)


if(EMSCRIPTEN)
    target_link_options(${PROJECT_NAME} PRIVATE
        "-s PTHREAD_POOL_SIZE=16"
        "-sPROXY_TO_PTHREAD"
        "-sUSE_PTHREADS=1"
        "-sENVIRONMENT=web"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sMAXIMUM_MEMORY=1gb"
    )
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/fp:fast>
    $<$<CXX_COMPILER_ID:GNU>:-ffast-math>
)

# Configure for dynamic library loading
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32s_HOT_RELOAD=1)

    # Enable PDB generation for better debugging during hot reload
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/Zi>)

    # Set linker flags for incremental linking to support hot reload
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/INCREMENTAL>)

elseif(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    # Linux specific hot reload settings
    target_compile_definitions(${PROJECT_NAME} PRIVATE LINUX_HOT_RELOAD=1)

    # Add necessary linker flags
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
endif()

if(WIN32)
    set_target_properties(gemupg PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
