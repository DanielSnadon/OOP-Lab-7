cmake_minimum_required(VERSION 3.10)

project(task6 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
    npc.cpp
    bear.cpp
    vip.cpp
    vihuhol.cpp
    battleManager.cpp
)

add_executable(main 
    main.cpp 
    ${PROJECT_SOURCES}
)

enable_testing()

find_package(GTest REQUIRED)

add_executable(tests 
    tests.cpp 
    ${PROJECT_SOURCES}
)

target_link_libraries(tests 
    PRIVATE 
    GTest::gtest_main
)