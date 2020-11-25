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

Finally the nominal position identified is `(-8.4112 0.0 46.2464) cm` with a rotation of `30 degrees` around the optical axis.

#### ⚠ **Important note**
The pose is identified with respect to the robot root, with `X` pointing backward, `Y` pointing right and `Z` pointing upwards.

### 2. Defining a search space

Given the nominal position we chose as `(-8.4112 0.0 46.2464) cm` and a rotation of `30 degrees`, we perturbed its position and orientation around the optical axis within the following ranges:

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
- [object close](#3.1-object-close) (i.e. at `25 cm`);
- [object far](#3.2-object-far) (i.e. at `34 cm`);
- [object in the middle](#3.3-object-in-the-middle) (i.e. at `29.5 cm`);

#### 3.1 Object close

The result we have when the object is as close as possible to the robot is shown in the following:

| Best pose  | Set of suitable poses |
| ------------- | ------------- |
| <p align="center"> <img src=https://user-images.githubusercontent.com/9716288/100202351-7c312380-2f01-11eb-9b6c-3971fcf03348.jpg width="350"> </p> |   <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100202360-7e937d80-2f01-11eb-9e57-9203716539e5.jpg width="350"> </p> |

The nominal pose defined [here](#1-identifying-the-nominal-pose) is shown in red.
The best pose identified is at `(-7.4112 -6 36.25) cm` and  `-5.07455 degrees`. However, there is a _range_ of suitable poses, shown on the right, for which the computed score was below `0.005`.

#### 3.2 Object far

The result we have when the object is far from the robot is shown in the following:

| Best pose  | Set of suitable poses |
| ------------- | ------------- |
|<p align="center"> <img src=https://user-images.githubusercontent.com/9716288/100202378-86532200-2f01-11eb-8d3b-91d29592bddf.jpg width="350"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100202380-86ebb880-2f01-11eb-81a3-31e03143a2fd.jpg width="350"> </p> |

The best pose identified is at `(-7.4112 -2 36.25) cm` and `-25.0746 degrees`. However, there is a _range_ of suitable poses, shown on the right, for which the computed score was below `0.005`.

#### 3.3 Object in the middle

The result we have when the object is placed in the middle between `25 cm` and `34 cm` is shown in the following:

| Best pose  | Set of suitable poses |
| ------------- | ------------- |
|<p align="center"> <img src=https://user-images.githubusercontent.com/9716288/100202389-8a7f3f80-2f01-11eb-9653-c33135987536.jpg width="350"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100202390-8a7f3f80-2f01-11eb-9e03-248a0ae9995a.jpg width="350"> </p> |


The best pose identified is at `(-11.4112 -6 36.25) cm` and `-5.07455 degrees`. However, there is a _range_ of suitable poses, shown on the right, for which the computed score was below `0.005`.

### 4. Choosing the final pose

By merging the results obtained for different object's positions, the range of suitable poses for which the computed score is below `0.002` is the following:

<p align="center">  
<img src=https://user-images.githubusercontent.com/9716288/100095486-d7f4a180-2e5a-11eb-8059-2ba8b4e56b6e.jpg width="350">

Since the central poses of the camera provide a reasonable score while keeping the object in the center of the field of view when the head is centered, we further reduced the range of optimal poses, by additionally removing the one in the middle of the eyes as it covers the eyes:

<p align="center">  
<img src=https://user-images.githubusercontent.com/9716288/100095496-da56fb80-2e5a-11eb-9f09-1550f794e8e8.jpg width="350">
</p>

The final set of pose is reported in the following table, showing the extracted superquadric when the object is far, close and in the middle position.

| Suitable poses  | Far | Close | Middle |
| ------------- | ------------- | ------------- | ------------- |
| `(-9.4112 0 40.2464)cm` <br> `-25 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100098399-471fc500-2e5e-11eb-9ab2-9dcd8ecf874f.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100098015-b21ccc00-2e5d-11eb-8f71-58e58d8adb85.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100098015-b21ccc00-2e5d-11eb-8f71-58e58d8adb85.png width="200"> </p> |
| `(-8.4112 0 40.2464)cm` <br> `-25 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100097639-18edb580-2e5d-11eb-8a2e-5e6b4ef42674.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100098401-47b85b80-2e5e-11eb-867c-3b8e77c636cd.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100098089-d082c780-2e5d-11eb-95fa-d69529efad9b.png width="200"> </p> |

#### ⚠ **Important note**
The final poses are defined with respect to the [nominal one](#1-identifying-the-nominal-pose).

<!--
The following shows for each of the suitable poses, the extracted superquadric when the object is far, close and in the middle position.

| Suitable poses  | Far | Close | Middle |
| ------------- | ------------- | ------------- | ------------- |
| `(-7.4112 -4 42.25)cm` <br> `-30 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100000110-6d435780-2dc1-11eb-91d9-8d550df7106a.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100001252-25253480-2dc3-11eb-9fd2-eef51e5fd34f.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100002148-741f9980-2dc4-11eb-84f7-e5c729cb73aa.png width="200"> </p> |
| `(-8.4112 -2 36.25)cm` <br> `-30 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100000160-7b917380-2dc1-11eb-81b1-da774ac91553.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100001261-28b8bb80-2dc3-11eb-832d-ab80d6f3a285.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100002159-771a8a00-2dc4-11eb-95b8-544742545bfc.png width="200"> </p> |
| `(-7.4112 -2 42.25)cm` <br> `-30 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100000191-877d3580-2dc1-11eb-814e-c7c9de3c92ab.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100001269-2b1b1580-2dc3-11eb-9017-e3d9b2a1e2bb.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100002165-797ce400-2dc4-11eb-8b5b-ca9a38cc7444.png width="200"> </p> |
| `(-10.4112 2 36.25)cm` <br> `-25 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100000223-9663e800-2dc1-11eb-830c-099aad5a5f53.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100001281-2f473300-2dc3-11eb-98c8-6ec7f409a16e.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100002174-7d106b00-2dc4-11eb-96f6-4ea8af204f89.png width="200"> </p> |
| `(-8.4112 2 40.25)cm` <br> `-25 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100000269-a4b20400-2dc1-11eb-8030-f68b6f51bc44.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100001283-3110f680-2dc3-11eb-95b6-4101c8465a9b.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100002178-7da90180-2dc4-11eb-9e99-43c0547b548b.png width="200"> </p> |
| `(-7.4112 2 42.25)cm` <br> `-30 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100000318-b8f60100-2dc1-11eb-902a-fdbf0ae76855.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100001286-33735080-2dc3-11eb-837a-bf22d189f369.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100002181-7e419800-2dc4-11eb-8bf0-8f6aa4b8f50a.png width="200"> </p> |
| `(-7.4112 4 36.25)cm` <br> `-30 deg` | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100000352-c612f000-2dc1-11eb-8b89-06434e850a6a.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100001292-353d1400-2dc3-11eb-8700-60e74f3dc1c5.png width="200"> </p> | <p align="center">  <img src=https://user-images.githubusercontent.com/9716288/100002185-7e419800-2dc4-11eb-94e5-4edc1f111d81.png width="200"> </p> |
-->
