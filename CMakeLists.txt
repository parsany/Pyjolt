cmake_minimum_required(VERSION 3.15)
project(pyjolt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find Python explicitly to ensure headers are available
find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)

# Fetch nanobind
include(FetchContent)
FetchContent_Declare(
  nanobind
  GIT_REPOSITORY https://github.com/wjakob/nanobind.git
  GIT_TAG        v2.4.0
)
FetchContent_MakeAvailable(nanobind)

# Fetch JoltPhysics into local joltengine folder
FetchContent_Declare(
  JoltPhysics
  GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
  GIT_TAG        master
  SOURCE_DIR     "${CMAKE_CURRENT_LIST_DIR}/joltengine"
)
FetchContent_MakeAvailable(JoltPhysics)

# Configure Jolt options
option(USE_SSE4_2 "Enable SSE4.2" ON)
option(TARGET_UNIT_TESTS "Build Unit Tests" OFF)
option(TARGET_HELLO_WORLD "Build Hello World" OFF)
option(TARGET_PERFORMANCE_TEST "Build Performance Test" OFF)
option(TARGET_SAMPLES "Build Samples" OFF)
option(TARGET_VIEWER "Build JoltViewer" OFF)

set(PHYSICS_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/joltengine")

# Include Jolt
include("${PHYSICS_REPO_ROOT}/Jolt/Jolt.cmake")

# Definte PyJolt module
nanobind_add_module(pyjolt 
  src/main.cpp
  src/bindings/math_bindings.cpp
  src/bindings/physics_bindings.cpp
  src/bindings/shape_bindings.cpp
)

# Link Jolt
target_link_libraries(pyjolt PRIVATE Jolt)

# Include directories
target_include_directories(pyjolt PRIVATE "${PHYSICS_REPO_ROOT}")
