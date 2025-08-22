cmake_minimum_required(VERSION 3.16) 
project(hello-port-audio)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::PORTAUDIO)

if(PORTAUDIO_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
endif()

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework CoreAudio" "-framework AudioToolbox" "-framework AudioUnit"
        "-framework CoreFoundation" "-framework CoreServices")
endif()
