cmake_minimum_required(VERSION 3.0)

project(cJSON)

set(SRC_FILES
    "cJSON/cJSON.c"
)

set(INC_FILES
    "cJSON/cJSON.h"
)

source_group("Source Files" FILES "${SRC_FILES}")
source_group("Header Files" FILES "${INC_FILES}")

add_library(cJSON ${SRC_FILES} ${INC_FILES})
