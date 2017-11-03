#!/bin/bash
/opt/ros/indigo/setup.bash
catkin_make
devel/setup.bash
roslaunch main_window view_show.launch &
rosrun orb_slam2 Mono   /home/liuzhenkun/work/catkin_ws/src/orb_slam2/Vocabulary/ORBvoc.txt  /home/liuzhenkun/work/catkin_ws/src/orb_slam2/Asus.yaml
