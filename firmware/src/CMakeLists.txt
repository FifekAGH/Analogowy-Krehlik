add_executable(precise_ammeter main.cpp display.cpp)

target_include_directories(precise_ammeter PRIVATE ./include)

target_link_libraries(precise_ammeter PRIVATE bsp drivers)
