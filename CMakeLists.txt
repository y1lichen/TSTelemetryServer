cmake_minimum_required(VERSION 3.15)

project(ts_telemetry_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 強制 MinGW 使用靜態鏈接 runtime
if(MINGW)
    message(STATUS "Detected MinGW, enabling static runtime linking")
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

# 禁用 json 的 test
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(json)

add_library(TSTelemetryServer SHARED 
    src/json_telemetry_serializer.cpp  
    src/network_handler.cpp  
    src/ts_telemetry_server.cpp
    src/event_queue.cpp 
    src/config_handler.cpp
    src/scs_variable_saver.cpp
)

# Windows-specific settings
if(WIN32)
    target_link_libraries(TSTelemetryServer PRIVATE wsock32 ws2_32)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(TSTelemetryServer PRIVATE
            -static -static-libgcc -static-libstdc++)
    elseif (MSVC)
        set_property(TARGET TSTelemetryServer PROPERTY
                     MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set(CMAKE_LINK_DEF_FILE_FLAG "/DEF:")
        target_sources(TSTelemetryServer PRIVATE src/TSTelemetryServer.def)
    endif()
endif()

# 警告設置
if(MSVC)
    target_compile_options(TSTelemetryServer PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(TSTelemetryServer PRIVATE
        -Wall -Wextra -Wpedantic 
        -Wcast-align -Wcast-qual -Wctor-dtor-privacy 
        -Wdisabled-optimization -Wformat=2 -Winit-self 
        -Wlogical-op -Wmissing-include-dirs -Wnoexcept 
        -Wold-style-cast -Woverloaded-virtual 
        -Wredundant-decls -Wshadow -Wsign-promo 
        -Wstrict-null-sentinel -Wundef 
        -fdiagnostics-show-option
        $<$<CONFIG:DEBUG>:-ggdb>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(TSTelemetryServer PRIVATE
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++98-c++11-compat-pedantic
        -Wno-documentation
        -Wno-documentation-unknown-command
        -Wno-padded
        -Wno-newline-eof
        -Wno-double-promotion
        -Wno-global-constructors
        -Wno-missing-variable-declarations
        -Wno-exit-time-destructors
        -Wno-unsafe-buffer-usage
        -Wno-missing-prototypes
        -Wno-covered-switch-default
        -Wno-zero-as-null-pointer-constant
        -Wno-weak-vtables
        $<$<CONFIG:DEBUG>:-ggdb>)
endif()

# Include & link JSON library
target_link_libraries(TSTelemetryServer PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(TSTelemetryServer PRIVATE include)
