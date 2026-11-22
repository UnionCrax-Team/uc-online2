cmake_minimum_required(VERSION 3.16)

project(uc_online2 LANGUAGES CXX)

# ============================================================
# Linux build of the Steam API emulator.
#
# The Windows build is still the .sln / .vcxproj next to this file; this
# CMakeLists.txt exists for the Linux side, where the game expects to load
# "libsteam_api.so" (the name the Valve Linux SDK uses for the 64-bit
# library -- note there is NO "64" suffix, unlike the Windows
# steam_api64.dll).
#
#   cmake -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build -j
#
# Result: build/x64/libsteam_api.so
#
# Put that file (or a symlink to it) next to the game's executable, where
# the game will find it instead of the real one. The emulator reads
# "union-crax.ini" from the same directory.
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(UCO_ARCH_DIR "x64")
else()
    set(UCO_ARCH_DIR "x86")
endif()

if(APPLE)
    message(FATAL_ERROR "The Linux port targets Linux; macOS is not supported yet.")
endif()

if(NOT UNIX)
    message(FATAL_ERROR "This CMakeLists.txt builds the Linux library. "
                        "Use uc_online2.vcxproj for the Windows DLL.")
endif()

add_library(steam_api SHARED dllmain.cpp)

target_include_directories(steam_api PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_options(steam_api PRIVATE
    -O2
    -fPIC
    -fvisibility=hidden
    -fno-strict-aliasing
    -Wall
    -Wno-unused-variable
    -Wno-unused-but-set-variable
    -Wno-unused-function
)

# dlopen/dlsym for steamclient.so and the plugin loader, pthread for the
# lock shims.
find_package(Threads REQUIRED)
target_link_libraries(steam_api PRIVATE ${CMAKE_DL_LIBS} Threads::Threads)

set_target_properties(steam_api PROPERTIES
    OUTPUT_NAME "steam_api"
    PREFIX "lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${UCO_ARCH_DIR}"
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
)

# The SONAME has to be exactly what the game asks the loader for. A game
# linked against the Steamworks SDK records "libsteam_api.so" as its
# DT_NEEDED, so the SONAME must match -- otherwise the loader looks for a
# second copy under a different name.
set_target_properties(steam_api PROPERTIES
    LINK_FLAGS "-Wl,-soname,libsteam_api.so"
)

# Strip the debug symbols; the dynamic symbol table (the part that
# matters -- it is what the game resolves SteamAPI_* through) is kept.
find_program(UCO_STRIP strip)
if(UCO_STRIP)
    add_custom_command(TARGET steam_api POST_BUILD
        COMMAND "${UCO_STRIP}" --strip-unneeded "$<TARGET_FILE:steam_api>"
        COMMENT "Stripping $<TARGET_FILE:steam_api>")
endif()

install(TARGETS steam_api LIBRARY DESTINATION .)
