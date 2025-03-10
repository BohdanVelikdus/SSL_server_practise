cmake_minimum_required(VERSION 3.28)
project(as_server)

if(MSVC)
    add_compile_options(/std::c++latest)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

set(ProjectPath "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ProjectPath})

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.87 REQUIRED CONFIG)

if(Boost_FOUND)
    message(STATUS "Boost found!!!")
    message(STATUS ${Boost_INCLUDE_DIRS})
    message(STATUS ${Boost_LIBRARIES})
else()
    message(STATUS "THe boost does NOT found")
endif()

find_package(OpenSSL REQUIRED CONFIG)

if(OpenSSL_FOUND)
    message(STATUS "OpenSSL found!!!")
    message(STATUS ${OpenSSL_INCLUDE_DIRS})
    message(STATUS ${OpenSSL_LIBRARIES})
else()
    message(STATUS "OpenSSL does NOT found")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})


target_link_libraries(${PROJECT_NAME} PRIVATE 
    nlohmann_json::nlohmann_json
    Boost::boost 
    Boost::system 
    Boost::thread
    ${OpenSSL_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${OpenSSL_INCLUDE_DIR}
)