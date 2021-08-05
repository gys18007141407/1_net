add_compile_options(-std=c++11)
include_directories(./include)
aux_source_directory(. CPP)

add_library(log STATIC ${CPP})
link_libraries(pthread)