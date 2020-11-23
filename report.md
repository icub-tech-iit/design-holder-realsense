Design of Experiment
====================

The following report describes the procedure adopted to determine the optimal poses of the RealSense holder for the iCub.
The procedure consisted of the following steps:
1. [**Identifying the nominal pose**](#1-identifying-the-nominal-pose)
    - We identified a _nominal pose_ starting from the pose conceived for the first holder prototype.
2. [**Defining a search space**](#2-defining-a-search-space)
    - We defined a search space by _perturbing the nominal pose in position and orientation along the optical axis_.
3. [**Identifying the best poses**](#3-identifying-the-best-poses)
    - We identified the best poses with respect to the robot, optimizing for the difference between the optimal and fitted superquadric.
4. [**Choosing the final pose**](#4-choosing-the-final-pose)
    - We chose the final pose by merging the results of the previous analysis.

#### ⚠ **Important note**
Currently, [iCubGenova01](https://github.com/robotology/robots-configuration/blob/032c70ab33fff1c9da73bf19c13a11ebe58fea1d/iCubGenova01/hardware/motorControl/icub_head.xml#L54) has the neck pitch minimum value set at `-37 degrees`. However, for other robots (see for example [iCubGenova02](https://github.com/robotology/robots-configuration/blob/4f880ec1a7970626bab41061ff4594b45a08d71a/iCubGenova02/hardware/motorControl/head-eb20-j0_1-mc.xml#L15) and [iCubGenova04](https://github.com/robotology/robots-configuration/blob/4f880ec1a7970626bab41061ff4594b45a08d71a/iCubGenova04/hardware/motorControl/head-eb20-j0_1-mc.xml#L15)) the limit is set at `-30 degrees`, also shared by the iCub models in Gazebo. We thus adopted this value, in order to rely on the official limits while being consistent with different iCubs.

### 1. Identifying the nominal pose

In order to identify a _nominal pose_, we started from the one conceived for the first holder prototype, as defined here:

<p align="center">
    <img src=https://user-images.githubusercontent.com/9716288/99939642-d055cf80-2d6a-11eb-9bce-5e45a9a1ff4d.png width="550">
</p>

However, while the object is visible at high distances from the robot (~ `34 cm`), at lower distances (~ `22 cm`) the object does not fall into the field of view.
Therefore we decided to modify the orientation of the camera by tilting it of additional `30 degrees` and qualitatively evaluated it:

Finally the nominal pose identified is `(-0.084112 0.0 1.092464 2.09492 0 -1.57)`.

### 2. Defining a search space

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

The following shows an example of the scenario for a fixed pose of the RealSense, comparing the output of the iCub left camera, the Real sense and the point cloud with the fitted superquadric:

<p align="center">
<img src=https://user-images.githubusercontent.com/9716288/99948261-7d374900-2d79-11eb-82cc-648c54416ef6.jpg width="650">
</p>

| iCub camera  | RealSense |  Point Cloud + Superquadric from RealSense |
| ------------- | ------------- | ------------- |
| <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99941357-2d9f5000-2d6e-11eb-9db2-26235ec41f2f.png width="350"> </p> |   <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/99941379-3859e500-2d6e-11eb-8cff-ce78964e9caa.png width="350"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/99941432-4f98d280-2d6e-11eb-91cd-0a36b4dccda5.png width="350"> </p> |

For identifying the best poses, we adopted the following strategy:

- the pose of the camera is updated within the ranges identified in [Section 2](#2-defining-a-search-space);
- for each pose, the point cloud of the object is extracted and a superquadric fitted to it. Given the tested object is a bottle, the optimal superquadric is elongated, i.e. a dimension is higher than the other two. Therefore we compute a priori the optimal superquadric for a camera pose that maximizes the object visibility into the RealSense field of view and consequently define **a good pose** as the one which guarantees that the dimensions of the extracted superquadric are close to those of the optimal one.

Specifically, the following shows the fitted superquadric for a good view of the object, object out of the field of view and a top view, with the relative scores:

| Good  | Object out of FOV |  Top View |
| ------------- | ------------- | ------------- |
| <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99958881-6816e600-2d8a-11eb-93f2-f8a6fe150bdc.png width="350"> </p> |  <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99958417-a4961200-2d89-11eb-80fb-201ee17dc83e.png width="350"> </p> | <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99958538-d5764700-2d89-11eb-9f88-8e7e86a571a4.png width="350"> </p> |
| <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99958947-87157800-2d8a-11eb-8589-da0c0a58b47f.png width="350"> </p> |  <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99959030-a3b1b000-2d8a-11eb-83df-f87eb37307db.png width="350"> </p> | <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99959128-c17f1500-2d8a-11eb-916a-efb42c1d9b43.png width="350"> </p> |
| **`score: 0.000404`** | `score: 0.01016` | `score: 0.0065`  |

Notably, when the view is good, the score is much lower if compared to those computed when the object is out of the field of view or the view is too much from the top.


The whole procedure is repeated for:
- [object close](#3.1-object-close-(25-cm)) (i.e. at `25 cm`);
- [object far](#3.2-object-far-(34-cm)) (i.e. at `34 cm`);
- [object in the middle](#3.3-object-in-the-middle-(29.5-cm)) (i.e. at `29.5 cm`);

#### 3.1 Object close (25 cm)

The result we have when the object is as close as possible to the robot is shown in the following:

| Best pose  | Set of suitable poses |
| ------------- | ------------- |
| <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99963129-9a781180-2d91-11eb-941a-788104f85f26.jpg width="350"> </p> |   <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/99963135-9d730200-2d91-11eb-90e4-64413b1ed7a1.jpg width="350"> </p> |

The nominal pose defined [here](#1-identifying-the-nominal-pose) is shown in red.
The best pose identified is at `-6.0841    1.0000   -9.5375   -5.0746`. However, there is a _range_ of suitable poses, shown on the right, for which the computed score was below `0.003`.

#### 3.2 Object far (34 cm)

The result we have when the object is far from the robot is shown in the following:

| Best pose  | Set of suitable poses |
| ------------- | ------------- |
|<p align="center"> <img src=https://user-images.githubusercontent.com/9716288/99963251-c7c4bf80-2d91-11eb-8ece-8cf4e2c9562f.jpg width="350"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/99966394-8387ee00-2d96-11eb-8e81-4a0e563a0b51.jpg width="350"> </p> |

The best pose identified is at `-2.0841    1.0000   -9.5375  -25.0746`. However, there is a _range_ of suitable poses, shown on the right, for which the computed score was below `0.003`.

#### 3.3 Object in the middle (29.5 cm)

The result we have when the object is placed in the middle between `25 cm` and `34 cm` is shown in the following:

| Best pose  | Set of suitable poses |
| ------------- | ------------- |
|  |   |


The best pose identified is: .

### 4. Choosing the final pose
