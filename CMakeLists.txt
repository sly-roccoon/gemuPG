cmake_minimum_required(VERSION 3.25)

set(CMAKE_COLOR_DIAGNOSTICS OFF)
set(CMAKE_CXX_STANDARD 20)

project(gemupg VERSION 1.0)

if(EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF)
    set(SDL_SHARED OFF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

# SDL
find_package(SDL3 CONFIG QUIET)
if (NOT SDL3_FOUND)
    message(STATUS "SDL3 not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.8
    )
    FetchContent_MakeAvailable(SDL3)
else ()
    message(STATUS "SDL3 found, using system provided version")
endif ()

# SDL_ttf
set(GEMUPG_SYSTEM_SDL_TTF AUTO CACHE STRING "Use system provided SDL-ttf")  # create the variable
set_property(CACHE GEMUPG_SYSTEM_SDL_TTF PROPERTY STRINGS AUTO ON OFF)  #  define list of values GUI will offer for the variable
if (GEMUPG_SYSTEM_SDL_TTF STREQUAL "AUTO")
    find_package(SDL3_ttf CONFIG QUIET)
if (NOT SDL3_ttf_FOUND)
    message(STATUS "SDL3-ttf not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.2
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    add_subdirectory(${SDL3_ttf_SOURCE_DIR} ${SDL3_ttf_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
elseif (GEMUPG_SYSTEM_SDL_TTF STREQUAL "ON")
    find_package(SDL3_ttf CONFIG REQUIRED)
endif ()

if (SDL3_ttf_FOUND)
    message(STATUS "Using system provided SDL3-ttf")
else ()
    # ! needs to run script under include/SDL_ttf/external first!
    set(SDLTTF_VENDORED ON)
    set(SDLTTF_HARFBUZZ OFF)
    set(SDLTTF_PLUTOSVG OFF)
    add_subdirectory(include/SDL_ttf EXCLUDE_FROM_ALL)
    message(STATUS "Using vendored SDL3-ttf")
endif ()

# IMGUI
file(GLOB_RECURSE IMGUI_SOURCES CONFIGURE_DEPENDS "include/imgui/*.cpp")
# add_library(IMGUI ${IMGUI_SOURCES})
# target_link_libraries(IMGUI PUBLIC SDL3::SDL3)
# target_include_directories(IMGUI PUBLIC "include/imgui")

# GEMUPG
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Using clang")
    add_compile_options(-Wno-int-conversions)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC")
    add_compile_options(-Wno-pmf-conversions)
endif ()

file(GLOB_RECURSE gemupg_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE gemupg_HEADERS CONFIGURE_DEPENDS "src/*.h")

set(gemupg_INCLUDE_DIRS "")
foreach (_headerFile ${gemupg_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list(APPEND geumpg_INCLUDE_DIRS ${_dir})
endforeach ()
list(REMOVE_DUPLICATES gemupg_INCLUDE_DIRS)

add_executable(${PROJECT_NAME} ${gemupg_SOURCES} ${IMGUI_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${gemupg_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE "include" "include/imgui")

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

if (EMSCRIPTEN)
target_link_options( ${PROJECT_NAME} PRIVATE
    "-s PTHREAD_POOL_SIZE=16"
    "-pthread"
    "--emrun"
    "-sALLOW_MEMORY_GROWTH"
    "-sPROXY_TO_PTHREAD"
)
endif()

# set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
# target_link_options(${PROJECT_NAME} PUBLIC "-s WASM=1" "-o ${PROJECT_NAME}.html")

target_compile_options(${PROJECT_NAME} PRIVATE 
    $<$<CXX_COMPILER_ID:MSVC>:/fp:fast>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-ffast-math>
)

if(EMSCRIPTEN)
    # on the web, we have to put the files inside of the webassembly
    # somewhat unintuitively, this is done via a linker argument.
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        "--preload-file \"${CMAKE_SOURCE_DIR}/assets/Sono-SemiBold.ttf@/\""
        "--preload-file \"${CMAKE_SOURCE_DIR}/assets/icon.bmp@/\""
)
endif()

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND_EXPAND_LISTS
    )
endif ()
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/assets/icon.bmp $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/assets/Sono-SemiBold.ttf $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
)

if (WIN32)
    set_target_properties(gemupg PROPERTIES WIN32_EXECUTABLE TRUE)
endif()