cmake_minimum_required(VERSION 3.6)
project(Warzone)

file(GLOB RESOURCES "resources/*.map")
file(GLOB HEADERS "src/**/*.h")
file(GLOB SOURCES "src/**/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".+Driver.cpp")

add_library(WarzoneLib STATIC ${SOURCES} ${HEADERS} ${RESOURCES})

add_executable(MapDriver ${PROJECT_SOURCE_DIR}/src/map/MapDriver.cpp)
target_link_libraries(MapDriver WarzoneLib)

add_executable(MapLoaderDriver ${PROJECT_SOURCE_DIR}/src/map_loader/MapLoaderDriver.cpp)
target_link_libraries(MapLoaderDriver WarzoneLib)

add_executable(OrdersDriver ${PROJECT_SOURCE_DIR}/src/orders/OrdersDriver.cpp)
target_link_libraries(OrdersDriver WarzoneLib)
