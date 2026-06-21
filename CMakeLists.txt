cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(remc2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)


################################################################################
# Dependencies
################################################################################

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

if (GITHUB_CI_COMPATIBILITY_PATH)
    set(SDL2_DIR "${GITHUB_CI_COMPATIBILITY_PATH}/cmake/SDL2")
    set(CMAKE_LIBRARY_PATH "${GITHUB_CI_COMPATIBILITY_PATH}")
endif ()

find_package(SDL2 REQUIRED)

if (GITHUB_CI_COMPATIBILITY_PATH)
    # CI cross-compile mód — přímé importované targety bez find_package
    foreach(_entry
        "SDL2_mixer|SDL_mixer.h|SDL2_mixer"
        "SDL2_ttf|SDL_ttf.h|SDL2_ttf"
        "SDL2_image|SDL_image.h|SDL2_image")

        string(REPLACE "|" ";" _pkg "${_entry}")
        list(GET _pkg 0 _name)
        list(GET _pkg 1 _header)
        list(GET _pkg 2 _lib)

        find_path(_inc ${_header}
            PATH_SUFFIXES SDL2
            PATHS
                "${GITHUB_CI_COMPATIBILITY_PATH}/../include"
                "/usr/include"
            NO_DEFAULT_PATH
        )
        find_library(_libpath
            NAMES ${_lib}
            PATHS "${GITHUB_CI_COMPATIBILITY_PATH}"
            NO_DEFAULT_PATH
        )
        if(NOT _inc OR NOT _libpath)
            message(FATAL_ERROR "Nelze najít ${_name} v ${GITHUB_CI_COMPATIBILITY_PATH}")
        endif()
        add_library(${_name}::${_name} UNKNOWN IMPORTED)
        set_target_properties(${_name}::${_name} PROPERTIES
            IMPORTED_LOCATION "${_libpath}"
            INTERFACE_INCLUDE_DIRECTORIES "${_inc}"
        )
        message(STATUS "Found ${_name} (CI): ${_libpath}")
        unset(_inc CACHE)
        unset(_libpath CACHE)
    endforeach()

else()
    # Lokální / nativní build — standardní find_package s fallbackem
    foreach(_entry
        "SDL2_mixer|SDL_mixer.h|SDL2_mixer"
        "SDL2_ttf|SDL_ttf.h|SDL2_ttf"
        "SDL2_image|SDL_image.h|SDL2_image")

        string(REPLACE "|" ";" _pkg "${_entry}")
        list(GET _pkg 0 _name)
        list(GET _pkg 1 _header)
        list(GET _pkg 2 _lib)

        find_package(${_name} QUIET CONFIG)
        if(NOT ${_name}_FOUND)
            find_path(_inc ${_header} PATH_SUFFIXES SDL2)
            find_library(_libpath NAMES ${_lib})
            if(NOT _inc OR NOT _libpath)
                message(FATAL_ERROR "Nelze najít ${_name}. Nainstaluj lib${_lib}-dev.")
            endif()
            if(NOT TARGET ${_name}::${_name})
                add_library(${_name}::${_name} UNKNOWN IMPORTED)
                set_target_properties(${_name}::${_name} PROPERTIES
                    IMPORTED_LOCATION "${_libpath}"
                    INTERFACE_INCLUDE_DIRECTORIES "${_inc}"
                )
            endif()
            message(STATUS "Found ${_name} (fallback): ${_libpath}")
            unset(_inc CACHE)
            unset(_libpath CACHE)
        endif()
    endforeach()
endif()

find_package(PNG REQUIRED)
find_package(fmt REQUIRED)

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_C_FLAGS "")
    set(CMAKE_C_FLAGS_DEBUG "")
    set(CMAKE_C_FLAGS_RELEASE "")
    set(CMAKE_C_FLAGS_XBOX_DEBUG "")
    set(CMAKE_C_FLAGS_XBOX_RELEASE "")
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS_XBOX_DEBUG "")
    set(CMAKE_CXX_FLAGS_XBOX_RELEASE "")
else()
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    add_compile_options(-fpermissive)
    add_compile_options(-Wno-narrowing)
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-pedantic)
    add_compile_options(-Wno-unknown-pragmas)
    add_compile_options(${SPDLOG_CFLAGS_OTHER})

    # Linux/ARM
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|aarch64)")
        add_compile_options(-fsigned-char)
    endif ()

    if (USE_SANITIZERS)
        add_compile_options(-fsanitize=address)
        add_compile_options(-fsanitize=pointer-compare)
        add_compile_options(-fsanitize=pointer-subtract)
        add_compile_options(-fsanitize=leak)
        add_compile_options(-fsanitize=undefined)
    endif ()
endif()


################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
else()
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})
endif()


################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################

# dependencies
set(libADLMIDI_STATIC TRUE)
add_subdirectory(libADLMIDI-master)
add_compile_definitions(COMPILE_FOR_64BIT)
#add_definitions(-w)

# preprocessor flags
if (FLATPAK)
    add_compile_definitions(FLATPAK)
endif ()

# sanitizer flags
if (USE_SANITIZERS)
    set(SANITIZER_OPTIONS
        -fsanitize=address
        -fsanitize=pointer-compare
        -fsanitize=pointer-subtract
        -fsanitize=leak
        -fsanitize=undefined
    )
endif ()

# remc2 and tests
add_subdirectory(remc2)
add_subdirectory(remc2-editor)
add_subdirectory(remc2-configurator)
if (UNIT_TESTS)
    find_package(GTest REQUIRED)
    add_subdirectory(remc2-unit-test)
    add_subdirectory(remc2-regression-test)
endif ()
