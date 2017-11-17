SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#LINK_DIRECTORIES(
#    /usr/lib/x86_64-linux-gnu
#)

SET(LIBCOMMON_SRC
    cdf.c
    cdf.h
    common.c
    common.h
    conn.c
    conn.h
    )

ADD_LIBRARY(common STATIC ${LIBCOMMON_SRC})

TARGET_LINK_LIBRARIES(common libm.so libpthread.so librt.so)
MESSAGE(STATUS ${common})
