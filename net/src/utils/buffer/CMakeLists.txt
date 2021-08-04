add_compile_options(-std=c++11)

include_directories(.)
aux_source_directory(. CPP)

add_library(buffer ${CPP})