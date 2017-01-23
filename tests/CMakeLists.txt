project(tests)

include_directories("../tracery")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wextra -Wall -Wno-padded -Wno-unused-parameter -Wno-reserved-id-macro")

set(SRC_FILES
    "tests.cpp"
    "test_modifiers.cpp"
    "test_tracery.cpp"
)

set(INC_FILES
    "test_tracery.h"
)

source_group("Source Files" FILES ${SRC_FILES})
source_group("Header Files" FILES "${INC_FILES}")

add_executable(tests ${SRC_FILES} ${INC_FILES})

target_link_libraries(tests tracery cJSON)
