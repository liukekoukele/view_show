cmake_minimum_required(VERSION 2.8.0)

project(orb_slam2)

#ADD_SUBDIRECTORY(Thirdparty)

find_package(catkin REQUIRED COMPONENTS roscpp
     cv_bridge sensor_msgs image_transport tf)

find_package(Pangolin REQUIRED)

 set(LIBS
 ${OpenCV_LIBS}
 ${EIGEN3_LIBS}
 ${Pangolin_LIBRARIES}
 ${catkin_LIBRARIES}
 ${PROJECT_SOURCE_DIR}/lib/libDBoW2.so
 ${PROJECT_SOURCE_DIR}/lib/libg2o.so
 ${PROJECT_SOURCE_DIR}/lib/libORB_SLAM2.so
 )

include_directories(
${catkin_INCLUDE_DIRS}
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include/orb_slam2
${Pangolin_INCLUDE_DIRS}
)


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


catkin_package()


add_executable(Mono src/ros_mono.cc )
target_link_libraries(Mono ${LIBS} )


add_executable(MonoAR src/AR/ros_mono_ar.cc src/AR/ViewerAR.h src/AR/ViewerAR.cc )
target_link_libraries(MonoAR  ${LIBS} )

# Node for stereo camera
add_executable(Stereo src/ros_stereo.cc )

target_link_libraries(Stereo ${LIBS} )

# Node for RGB-D camera

add_executable(RGBD src/ros_rgbd.cc )
target_link_libraries(RGBD ${LIBS} )

install(TARGETS Mono RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
install(TARGETS MonoAR RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
install(TARGETS Stereo RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
install(TARGETS RGBD RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})



