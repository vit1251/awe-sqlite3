cmake_minimum_required(VERSION 3.21)

project (awe-sqlite3)

include_directories(${CMAKE_JS_INC})

file(GLOB SOURCE_FILES
    "src/awe-sqlite3.c"
    "src/awe-sqlite3-open.c"
    "src/awe-sqlite3-close.c"
    "src/awe-sqlite3-exec.c"
)

find_package (SQLite3)
include_directories(${SQLITE3_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
target_link_libraries(${PROJECT_NAME} ${SQLITE3_LIBRARIES})
target_link_libraries(${PROJECT_NAME} sqlite3)