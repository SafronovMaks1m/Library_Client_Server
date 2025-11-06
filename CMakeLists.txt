cmake_minimum_required(VERSION 3.10)
project(Project)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS serialization system)

add_executable(my_app src/cpp/main.cpp)

target_link_libraries(my_app 
    ${Boost_LIBRARIES}
    pthread
)

target_include_directories(my_app PRIVATE
    ${Boost_INCLUDE_DIRS}
    src/headers
    examples/headers
)

enable_testing()

add_executable(run_tests
    tests/HandlerServerTests.cpp
    tests/HandlerClientTests.cpp
    src/cpp/Server.cpp
    src/cpp/Connection.cpp
    src/cpp/HandlerMessageServer.cpp
    src/cpp/Client.cpp
    src/cpp/HandlerMessageClient.cpp
    src/cpp/Serealize.cpp 
    examples/cpp/server_create_handlers.cpp
    examples/cpp/client_create_handlers.cpp
    examples/cpp/UserClient.cpp
    examples/cpp/UserServer.cpp
)

target_include_directories(run_tests PRIVATE
    ${Boost_INCLUDE_DIRS}
    include
    src/headers
    examples/headers
)

target_link_libraries(run_tests 
    ${Boost_LIBRARIES}
    gtest 
    gtest_main 
    pthread
)

add_test(NAME AllTests COMMAND run_tests)