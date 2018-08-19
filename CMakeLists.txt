CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set(TOPDIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PLATFORMDIR ${TOPDIR}/platforms)
set(OUTDIR ${TOPDIR}/out)


PROJECT(gltest)


if (${CMAKE_SYSTEM_NAME} MATCHES Windows)
    include(${PLATFORMDIR}/windows.cmake)
elseif (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    include(${PLATFORMDIR}/linux.cmake)
elseif (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    include(${PLATFORMDIR}/macos.cmake)
endif()


SET(CMAKE_BUILD_TYPE "RelWithDebInfo")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


add_subdirectory(src)

