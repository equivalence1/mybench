find_package(benchmark REQUIRED)

add_executable(example main.cpp)

target_link_libraries(example benchmark::benchmark)
