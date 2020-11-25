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

add_executable(PlayerDriver ${PROJECT_SOURCE_DIR}/src/player/PlayerDriver.cpp)
target_link_libraries(PlayerDriver WarzoneLib)

add_executable(CardsDriver ${PROJECT_SOURCE_DIR}/src/cards/CardsDriver.cpp)
target_link_libraries(CardsDriver WarzoneLib)

add_executable(GameEngineDriver ${PROJECT_SOURCE_DIR}/src/game_engine/GameEngineDriver.cpp)
target_link_libraries(GameEngineDriver WarzoneLib)

add_executable(AdapterDriver ${PROJECT_SOURCE_DIR}/src/map_loader/AdapterDriver.cpp)
target_link_libraries(AdapterDriver WarzoneLib)

add_executable(PlayerStrategiesDriver ${PROJECT_SOURCE_DIR}/src/strategies/PlayerStrategiesDriver.cpp)
target_link_libraries(PlayerStrategiesDriver WarzoneLib)