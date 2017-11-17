SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

ADD_EXECUTABLE(client client.c)
TARGET_LINK_LIBRARIES(client common)

ADD_EXECUTABLE(simple-client simple-client.c)
TARGET_LINK_LIBRARIES(simple-client common)

ADD_EXECUTABLE(incast-client incast-client.c)
TARGET_LINK_LIBRARIES(incast-client common)
