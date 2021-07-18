cmake_minimum_required(VERSION 3.0.0)

get_filename_component(PROJECT ${CMAKE_SOURCE_DIR} NAME)
project(${PROJECT})

file(GLOB CPP_SOURCES src/*.cpp)
set(SOURCES ${CPP_SOURCES})
include_directories("include")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(${PROJECT} ${SOURCES})
target_link_libraries( ${PROJECT} ${OpenCV_LIBS} )
add_custom_command(TARGET ${PROJECT} 
                   POST_BUILD 
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT}> ${CMAKE_SOURCE_DIR}
)
