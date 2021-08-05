add_compile_options(-std=c++11)

include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/src/utils/buffer)
include_directories(${PROJECT_SOURCE_DIR}/src/utils/timer)
include_directories(${PROJECT_SOURCE_DIR}/extra/log/src/include)

add_executable(server EchoServer.cpp)
add_executable(client EchoClient.cpp)

target_link_libraries(server net)
target_link_libraries(client net)
