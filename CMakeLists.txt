cmake_minimum_required(VERSION 3.0)

add_subdirectory(tracery)
add_subdirectory(vendor)
add_subdirectory(tests)

project(app)

include_directories("tracery")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wextra -Wall -Wno-padded -Wno-unused-parameter -Wno-reserved-id-macro")

set(SRC_FILES
    "app.cpp"
)

set(INC_FILES
)

source_group("Source Files" FILES ${SRC_FILES})
source_group("Header Files" FILES "${INC_FILES}")

add_executable(app ${SRC_FILES} ${INC_FILES})

target_link_libraries(app tracery cJSON)
