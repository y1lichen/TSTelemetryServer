#[[
This file is part of TSTelemetryServer.

Copyright (C) 2024 OrkenWhite.

TSTelemetryServer is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TSTelemetryServer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TSTelemetryServer. If not, see <https://www.gnu.org/licenses/>. 
]]#

cmake_minimum_required(VERSION 3.5)

project(ts_telemetry_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(reflect-cpp)

add_library(TSTelemetryServer SHARED 
    src/json_telemetry_serializer.cpp  
    src/network_handler.cpp  
    src/trucksim_datagram.cpp  
    src/yyjson/yyjson.cpp 
    src/event_queue.cpp 
    src/config_handler.cpp
    src/scs_variable_saver.cpp
)

if(WIN32)
    target_link_libraries(TSTelemetryServer PRIVATE wsock32 ws2_32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(TSTelemetryServer PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
target_link_options(TSTelemetryServer PRIVATE /constexpr:depth1024)
else()
    target_compile_options(TSTelemetryServer PRIVATE -fconstexpr-depth=1024)
endif()

target_link_libraries(TSTelemetryServer PRIVATE reflectcpp)
target_include_directories(TSTelemetryServer PRIVATE include)
target_include_directories(TSTelemetryServer PRIVATE include/reflect-cpp)

add_custom_command(TARGET TSTelemetryServer POST_BUILD
    COMMAND $<$<CONFIG:release>:${CMAKE_STRIP} ${PROJECT_NAME}>)