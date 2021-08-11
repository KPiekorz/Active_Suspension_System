# Active Suspension System

## System description

A system that allows for adaptive control of the suspension of a modern BMW or AUDI electric car (mock different sensor and components of system, which allow to simulate it).

## Setup python enviroment

- Run following commands in terminal (in main project folder) (https://www.youtube.com/watch?v=ClOlSlDpCm4):
  - sudo apt upgrade
  - sudo apt install python3-venv python3-pip python3-tk
  - sudo apt install python3.7 python3.7-dev python3.7-venv
  - python3.7 -m venv gui-env
  - source gui-env/bin/activate
  - pip install numpy scipy matplotlib pyqt5 sockets

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

## RTOS process and thread

- Waiting for all child precesses:
```c
int status = 0;
pid_t wpid;
while ((wpid = wait(&status)) > 0); // this way, the father waits for all the child processes
```

## PyQt5

- PyQt5 multithreading - https://realpython.com/python-pyqt-qthread/

## Problems

- Cmake fix: https://cgold.readthedocs.io/en/latest/tutorials/libraries/symbols/link-order.html
- Fork VS Code (vs code linux process after fork): https://github.com/microsoft/vscode-cpptools/issues/511, https://stackoverflow.com/questions/40896631/debug-fork-with-visual-studio-c, https://www.youtube.com/watch?v=123IV3xZmQQ

## Model simulation

- https://ctms.engin.umich.edu/CTMS/index.php?example=Suspension&section=ControlStateSpace
- https://aleksandarhaber.com/control-systems-lecture-intro-to-state-space-models-and-python-simulation-of-a-mass-spring-system/
- matrix in c: https://www.codeproject.com/Articles/5283245/Matrix-Library-in-C
- Most important website for simulation:
  - discretization: https://aleksandarhaber.com/simulating-a-state-space-model-in-eigen-c-matrix-library-object-oriented-program/#google_vignette
  - model simulation in python: https://aleksandarhaber.com/control-systems-lecture-intro-to-state-space-models-and-python-simulation-of-a-mass-spring-system/
  - suspension state model: https://ctms.engin.umich.edu/CTMS/index.php?example=Suspension&section=ControlStateSpace
  - matrix operation: https://www.codeproject.com/Articles/5283245/Matrix-Library-in-C
  - matlab discretization: https://www.mathworks.com/help/control/ref/ss.html
  - discretizion in a loop: https://www.mathworks.com/matlabcentral/answers/315209-discrete-state-space-find-and-plot
  - c2d (ZOH): https://www.mathworks.com/help/ident/ref/lti.c2d.html

## Control

- Discretize PID controller: https://github.com/pms67/PID
- Control active suspension system by PID regulator: https://www.researchgate.net/publication/299579915_PID_CONTROLLER_OF_ACTIVE_SUSPENSION_SYSTEM_FOR_A_QUARTER_CAR_MODEL
- https://www.scilab.org/discrete-time-pid-controller-implementation