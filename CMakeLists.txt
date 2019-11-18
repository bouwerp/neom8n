cmake_minimum_required(VERSION 3.14)
project(neom8n)

set(CMAKE_CXX_STANDARD 17)

add_library(neom8n neom8n.cc neom8n.h)

enable_testing()

add_executable(neom8n_test neom8n_test.cc)

target_link_libraries(neom8n_test neom8n)

add_test(MyAwesomeTest neom8n_test)

