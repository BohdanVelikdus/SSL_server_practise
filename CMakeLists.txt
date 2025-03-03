cmake_minimum_required(VERSION 3.28)
project(as_server)

if(MSVC)
    add_compile_options(/std::c++latest)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif()
set(OSBitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OSBitness 64)
endif()



set(ProjectPath "${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${OSBitness}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ProjectPath})

# use installed boost
set(BOOST_ROOT $ENV{BOOST_ROOT})
set(Boost_INCLUDE_DIRS $ENV{BOOST_INCLUDE_DIR})
set(Boost_LIBRARIES $ENV{BOOST_LIBRARY_DIR})

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.87 REQUIRED COMPONENTS filesystem system)

if(NOT TARGET nlohmann_json)
    add_subdirectory(external/json)
endif()

# Include directories
include_directories(${Boost_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Link libraries
set(CMAKE_FIND_LIBRARY_SUFFIXES .so)
#link_libraries(MyExecutable PRIVATE ${Boost_LIBRARIES})
link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)