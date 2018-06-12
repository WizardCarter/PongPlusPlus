cmake_minimum_required(VERSION 2.6)
project(pongplusplus)

add_executable(pongplusplus main.cpp)

target_link_libraries(pongplusplus PUBLIC sfml-graphics)
target_link_libraries(pongplusplus PUBLIC sfml-window)
target_link_libraries(pongplusplus PUBLIC sfml-system)

install(TARGETS pongplusplus RUNTIME DESTINATION bin)
