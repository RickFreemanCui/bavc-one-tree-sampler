cmake_minimum_required(VERSION 3.10)

# Project Name
project(MyCppProject VERSION 1.0 LANGUAGES CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF) # Optional: Disable compiler-specific extensions

# Add executable sources
add_executable(my_app main.cpp tree_utils.cpp sampler.cpp)

# Optional: Add include directories
# target_include_directories(my_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Optional: Link libraries
# target_link_libraries(my_app PRIVATE some_library)

# Enable warnings (optional but recommended)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Installation (optional)
# install(TARGETS my_app DESTINATION bin)
