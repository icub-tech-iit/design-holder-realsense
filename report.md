Design of Experiment
====================

The following report describes the procedure adopted to determine the optimal poses of the RealSense holder for the iCub.
The procedure consisted of the following steps:
1. [**Identifying the nominal pose**](#1-identifying-the-nominal-pose)
    - We identified a _nominal pose_ starting from the pose conceived for the first holder prototype.
2. [**Sensitivity analysis**](#2-sensitivity-analysis)
    - We performed a sensitivity analysis by _perturbing the nominal pose in position and orientation along the optical axis_.
3. [**Identifying the best poses**](#3-identifying-the-best-poses)
    - We identified the best poses with respect to the robot, optimizing for the difference between the optimal and fitted superquadric.

#### ⚠ **Important note**
Currently, the black iCub neck pitch [minimum value](https://github.com/robotology/robots-configuration/blob/032c70ab33fff1c9da73bf19c13a11ebe58fea1d/iCubGenova01/hardware/motorControl/icub_head.xml#L54) is set at `-37 degrees`, with the [official limit](http://wiki.icub.org/wiki/ICub_joints_limits) set at `-30 degrees`, also shared by the iCub models in Gazebo. We thus adopted this value, in order to rely on the official limits while being consistent with different iCubs.

### 1. Identifying the nominal pose

In order to identify a _nominal pose_, we started from the one conceived for the first holder prototype reported [here](https://github.com/icub-tech-iit/code/issues/463).
However, while the object is visible at high distances from the robot (~ `34 cm`), at lower distances (~ `22 cm`) the object does not fall into the field of view.
Therefore we decided to modify the orientation of the camera by tilting it of additional `30 degrees` and qualitatively evaluated it:



Finally the nominal pose identified is `(-0.084112 0.0 1.092464 2.09492 0 -1.57)`.

### 2. Sensitivity analysis

Given the nominal pose we chose `(-0.084112 0.0 1.092464 2.09492 0 -1.57)`, we perturbed its position and orientation around the optical axis within the following ranges:

- x direction: `[-3 1] cm`;
- y direction: `[-6 6] cm`;
- z direction: `[-10 0] cm`;
- orientation: `[0 25] degrees`;

Such ranges were chosen considering that:
- along the x direction:
    - beyond `-3 cm` the object is outside the Realsense FOV;
    - below `1 cm` the RealSense cannot be physically placed;
- along the y direction:
    - below `-6 cm` the object is outside the Realsense FOV;
    - beyond `6 cm` the object is outside the Realsense FOV;
- along the z direction:
    - beyond `-10 cm` the RealSense enters the iCub cameras' field of view for a tilt of the eyes of `2.0 degrees`;
    - below `0 cm` the RealSense does not see the object.
- for the orientation:
    - below `0 degrees` the object is outside the Realsense FOV;
    - beyond `25 degrees` the robot itself enters the RealSense FOV.

We further identified a suitable range for the object, considering the iCub grasping workspace:

| `23 cm`  | `34 cm` |
| ------------- | ------------- |
| <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99817744-9bb8fc80-2b4d-11eb-8a68-f3c2e11f9525.png width="350"> </p> |   <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/99817755-9e1b5680-2b4d-11eb-9878-8db13937fc2c.png width="350"> </p> |

### 3. Identifying the best poses

| Best pose  | Set of suitable poses |
| ------------- | ------------- |
| <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99816990-a2933f80-2b4c-11eb-808d-dea6ca2d4f2a.jpg width="350"> </p> |   <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/99816999-a626c680-2b4c-11eb-89bd-b16f0dbc65e2.jpg width="550"> </p> |
