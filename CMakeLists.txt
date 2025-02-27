cmake_minimum_required(VERSION 3.16)

project(SettingsManager LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# HINT: Use this if path is not in environment
#set(CMAKE_PREFIX_PATH ~/Qt/5.15.18/gcc_64/lib/cmake/Qt5)
set(CMAKE_PREFIX_PATH ~/Qt/6.8.1/gcc_64/lib/cmake/Qt6)

set(EXEC_NAME ${PROJECT_NAME})


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(${EXEC_NAME}
  main.cpp
  SettingsManager.h
  SettingsManager.cpp
)
target_link_libraries(${EXEC_NAME} Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS ${EXEC_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
