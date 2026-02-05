cmake_minimum_required(VERSION 3.20)
project(Assignment_Data_Structure)

set(CMAKE_CXX_STANDARD 17)

# Array Version executable
add_executable(main_array Array_Version/main_array.cpp)

# LinkedList Version executable
add_executable(main_list LinkedList_Version/main_list.cpp)

# Main executable (if needed)
add_executable(main main.cpp Shared_Resources/passengerRepo.cpp)
