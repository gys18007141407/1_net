
add_compile_options(-std=c++11)

add_subdirectory(utils)

include_directories(${PROJECT_SOURCE_DIR}/extra/log/src/include)
include_directories(include)
include_directories(utils/timer)
include_directories(utils/buffer)

aux_source_directory(. CPP)


add_library(net STATIC ${CPP})
target_link_libraries(net timer buffer pthread log)
