cmake_minimum_required(VERSION 3.19)
project(CalTrain C)

set(CMAKE_C_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(CalTrain caltrain-runner.c caltrain.c)
target_link_libraries(CalTrain PRIVATE Threads::Threads)