project(tracery)

include_directories("../vendor/cJSON")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wextra -Wall -Wno-padded -Wno-unused-parameter -Wno-reserved-id-macro -Wno-conversion")

set(SRC_FILES
    "modifiers.cpp"
    "tracery.cpp"
)

set(INC_FILES
    "modifiers.h"
    "tracery.h"
)

source_group("Source Files" FILES ${SRC_FILES})
source_group("Header Files" FILES "${INC_FILES}")

add_library(tracery ${SRC_FILES} ${INC_FILES})
