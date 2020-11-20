# Copyright: (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Ugo Pattacini
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 3.5)
project(design-holder-realsense)

find_package(YCM)

find_package(ICUBcontrib REQUIRED)
find_package(YARP REQUIRED COMPONENTS os sig dev math eigen)
find_package(GAZEBO REQUIRED)
find_package(SuperquadricLib REQUIRED)

set(PCL_MODULE_PATH "$ENV{ROBOT_CODE}/pcl/cmake/Modules")
list(APPEND CMAKE_MODULE_PATH ${PCL_MODULE_PATH})
find_package(PCL REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})
find_package(IPOPT REQUIRED)

include(ICUBcontribOptions)
include(ICUBcontribHelpers)

icubcontrib_set_default_prefix()

if (GAZEBO_FOUND)
    add_library(${PROJECT_NAME}-world SHARED src/world.cpp)
    target_compile_definitions(${PROJECT_NAME}-world PRIVATE _USE_MATH_DEFINES)
    target_include_directories(${PROJECT_NAME}-world PRIVATE ${GAZEBO_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}-world PRIVATE ${GAZEBO_LIBRARIES} ${YARP_LIBRARIES})
    install(TARGETS ${PROJECT_NAME}-world LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib)
endif()

yarp_add_idl(IDL_GEN_FILES src/rpc.thrift)
add_executable(${PROJECT_NAME} src/main.cpp ${IDL_GEN_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    ${IPOPT_INCLUDE_DIRS} ${PCL_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${IPOPT_DEFINITIONS} _USE_MATH_DEFINES)
set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " ${IPOPT_LINK_FLAGS}")
target_link_libraries(${PROJECT_NAME} PUBLIC ${YARP_LIBRARIES} ${IPOPT_LIBRARIES}
    SuperquadricLib::SuperquadricLibModel SuperquadricLib::SuperquadricLibVis
    SuperquadricLib::SuperquadricLibGrasp) #${PCL_LIBRARIES}
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

file(GLOB scripts ${CMAKE_SOURCE_DIR}/app/scripts/*.xml)
yarp_install(FILES ${scripts} DESTINATION ${ICUBCONTRIB_APPLICATIONS_INSTALL_DIR}/${PROJECT_NAME})
install(PROGRAMS ${CMAKE_SOURCE_DIR}/app/scripts/run-analysis.sh DESTINATION bin)

add_subdirectory(gazebo)
