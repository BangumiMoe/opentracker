cmake_minimum_required(VERSION 2.8.4)
project(opentracker)

SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pipe -Wextra #-ansi -pedantic")
SET(GCC_COVERAGE_LINK_FLAGS    "-L/Users/MartianZ/Documents/ot/libowfat -lowfat -pthread -lpthread -lz")


SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

ADD_DEFINITIONS(-DWANT_NOTIFY -DWANT_COMPRESSION_GZIP -DWANT_IP_FROM_QUERY_STRING -DWANT_IP_FROM_PROXY -D_DEBUG_BANGUMI)


set(SOURCE_FILES
    opentracker.c
    ot_accesslist.c
    ot_accesslist.h
    ot_clean.c
    ot_clean.h
    ot_fullscrape.c
    ot_fullscrape.h
    ot_http.c
    ot_http.h
    ot_iovec.c
    ot_iovec.h
    ot_livesync.c
    ot_livesync.h
    ot_mutex.c
    ot_mutex.h
    ot_rijndael.c
    ot_rijndael.h
    ot_stats.c
    ot_stats.h
    ot_sync.c
    ot_sync.h
    ot_udp.c
    ot_udp.h
    ot_vector.c
    ot_vector.h
    scan_urlencoded_query.c
    scan_urlencoded_query.h
    trackerlogic.c
    trackerlogic.h
    bgm_sds.c
    bgm_sds.h
    bgm_notify.c
    bgm_notify.h)

include_directories(.)
include_directories(../libowfat)
add_executable(opentracker ${SOURCE_FILES})
