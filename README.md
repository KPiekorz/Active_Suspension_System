# Active Suspension System

## System description 

A system that allows for adaptive control of the suspension of a modern BMW or AUDI electric car (mock different sensor and components of system, which allow to simulate it).

## Setup python enviroment

- Run following commands in terminal (in main project folder) (https://www.youtube.com/watch?v=ClOlSlDpCm4):
  - sudo apt upgrade 
  - sudo apt install python3-venv python3-pip python3-tk
  - sudo apt install python3.7 python3.7-dev python3.7-venv
  - python3.7 -m venv gui3.7
  - source gui3.7/bin/activate
  - pip install numpy scipy matplotlib pyqt5

## Run project

- To build projekt insert follownig command in root projekt folder (automatic_suspension_system):
  - mkdir build
  - cd build
  - cmake ..
  - make
- To run projekt insert follownig command in root projekt folder (automatic_suspension_system):
  - ./build/automatic_suspension_system

## System requirements

- installed Python 3.6.9 version (https://github.com/vim/vim/issues/2984, for cmake https://stackoverflow.com/questions/11041299/python-h-no-such-file-or-directory, for python dev instalation https://github.com/stevenlovegrove/Pangolin/issues/494)
- run python script from c (https://medium.datadriveninvestor.com/how-to-quickly-embed-python-in-your-c-application-23c19694813)

## System operation assumption

- System handles multiple processes at once
- System reacts to external events with a deternimist delay, which is very short;

## System functionalities

- Change hardness/stiffness of the suspension;
- Preventing the car from tilting when turning;
- Change of the suspension hardness depending on the quality of the surface, on which the car is moving (detected with accelerometer, sonar);
- Simulation environment (ROS and Gazebo), in which the operation of the system will be presented in practice

## System structure and architecture

- to do

## Used sensor

- Prescriptive indication of the sensors used is not possible at this time. You will probably need a fusion of a dozen or so sensors that will allow you to accurately recreate the condition of the suspension and the body, steered, etc.