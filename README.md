Sandbox to design the holder for the realsense on the iCub robot
====================================================

## 🚶🏻‍♂️ Walkthrough

RealSense cameras have been recently proved to be a valid technology for 3D perception. Therefore, it would be beneficial to plug it onto the iCub robot, in order to facilitate perception and grasping, while allowing a comparison with the current cameras mounted on the robot.

A possible solution is to design a holder for the camera to be placed on the robot's head:

<p align="center">
<img src="https://user-images.githubusercontent.com/9716288/99807903-6c4fc300-2b40-11eb-9856-4725f4e541b7.png" width="400">
</p>

This sandbox contains the software to run the experiments needed to identify a set of suitable camera poses.
To define a _suitable_ pose, we considered the following points:

- a typical grasping task consists of having the robot to grasp an object placed on a table. To maximize the visibility of the object from the cameras, the robot's neck is usually entirely tilted. In such configuration, it is crucial that:
    - the RealSense pose allows the object to fall entirely into the field of view;  
    - the RealSense pose allows to have a view covering most of the object, rather than (for example) a top view, which might be problematic for grasping;
- ideally, the RealSense should not fall into the cameras field of view.

To this aim, the test-bench we considered includes an object located on the table in front of iCub, taken from the YCB dataset (specifically the mustard bottle). The 3D mesh of the RealSense was imported in the Gazebo simulation environment and moved within a range around the head:

<p align="center">
<img src="https://user-images.githubusercontent.com/9716288/99791296-b7101180-2b25-11eb-8904-013c99960185.jpg" width="600">
</p>

The analysis carried out relies on the following steps:
- the 3D point cloud of the scene is acquired from the RealSense and segmented from the table;
- the retrieved point cloud is used to construct a compact 3D representation based on superquadrics, relying on [superquadric-lib](https://github.com/robotology/superquadric-lib);
- the optimal superquadric is retrieved beforehand considering a reasonable view of the object, fully inside the field of view;
- for each pose the superquadric is extracted and compared to the optimal.

The camera pose is finally retrieved in order to minimize the difference between the dimensions of the optimal and the computed superquadric. 


## 📊📉🎯 Report on Outcome and Results
A full report describing the performed analysis can be found [here](report.md).

<!---
## ☁ Instructions to run the sandbox on the web
We make use of the [Gitpod Cloud IDE](https://gitpod.io) as infrastructure. Find out more on [YARP-enabled Gitpod workspaces][1].

Here's below a quick how-to guide; please, refer to the section🚶🏻‍♂️ Walkthrough for an extensive video tutorial 🎥

1. To get started with the sandbox, click on the following badge:

    ![Gitpod](https://gitpod.io/button/open-in-gitpod.svg)

tbc

--->

## [🔽 Instructions to run the sandbox locally](./dockerfiles/README.md)

<!---
## ℹ Manual installation
If you want to install the sandbox manually to perform specific customization (e.g. enable GPU), please refer to the recipe contained in the [**`Dockerfile`**](./dockerfiles/Dockerfile).
--->

### 👨🏻‍💻 Maintainers
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/vtikha.png" width="40">](https://github.com/vtikha) | [@vtikha](https://github.com/vtikha) |
| [<img src="https://github.com/vvasco.png" width="40">](https://github.com/vvasco) | [@vvasco](https://github.com/vvasco) |


[1]: https://spectrum.chat/icub/technicalities/yarp-enabled-gitpod-workspaces-available~73ab5ee9-830e-4b7f-9e99-195295bb5e34
