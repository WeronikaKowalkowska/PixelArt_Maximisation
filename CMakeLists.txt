cmake_minimum_required(VERSION 3.30)
project(mmpx)

set(CMAKE_CXX_STANDARD 23)

# Output directories (optional)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Add SDL source as a subproject (this builds SDL3::SDL3)
add_subdirectory(thirdparty/SDL EXCLUDE_FROM_ALL)

# Your executable – list ONLY your own source files
add_executable(mmpx
        main.cpp
        Enlarger.cpp
        # add any other .cpp files you wrote (not thirdparty)
)

# Link to the SDL3 library built from the subdirectory
target_link_libraries(mmpx PRIVATE SDL3::SDL3)