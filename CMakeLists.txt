cmake_minimum_required(VERSION 3.10)
project(TetrisClone)

set(CMAKE_CXX_STANDARD 17)

# Set SFML path
set(SFML_DIR "/opt/sfml2/lib/cmake/SFML")
find_package(SFML 2 COMPONENTS graphics window system audio REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(/opt/sfml2/include)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Piece.cpp
    src/Button.cpp
)

# Header files
set(HEADERS
    include/Game.h
    include/Piece.h
    include/Utils.h
)

# Create executable
add_executable(tetris ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(tetris PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)
target_include_directories(tetris PRIVATE /opt/sfml2/include)

