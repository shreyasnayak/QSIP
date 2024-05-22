cmake_minimum_required(VERSION 3.14)

project(QSIP LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(QSIP STATIC
  src/qsip.cpp
  src/qsip.h
  src/qsipaccount.h
  src/qsipaccount.cpp
  src/qsipcall.h
  src/qsipcall.cpp
  src/qsipbuddy.h
  src/qsipbuddy.cpp
)

target_link_libraries(QSIP PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(QSIP PRIVATE QSIP_LIBRARY)
