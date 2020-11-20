/******************************************************************************
* Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option) any later
* version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.                                                                     *
 ******************************************************************************/
/**
 * @authors: Valentina Vasco <valentina.vasco@iit.it>
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <random>

#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcClient.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/GazeControl.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>
#include <yarp/eigen/Eigen.h>

//#include <pcl/point_cloud.h>
//#include <pcl/point_types.h>
//#include <pcl/kdtree/kdtree_flann.h>
//#include <pcl/sample_consensus/ransac.h>
//#include <pcl/sample_consensus/sac_model_plane.h>

#include <SuperquadricLibModel/superquadricEstimator.h>
#include <SuperquadricLibVis/visRenderer.h>
#include <SuperquadricLibGrasp/graspComputation.h>

#include "src/rpc_IDL.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::eigen;
using namespace Eigen;
using namespace SuperqModel;
using namespace SuperqGrasp;
using namespace SuperqVis;

/****************************************************************/
class Evaluator : public RFModule, public rpc_IDL
{
    double period{0.1};
    PolyDriver gaze;

    // Ports
    BufferedPort<ImageOf<PixelRgb>> rgbPort;
    BufferedPort<ImageOf<PixelFloat>> depthPort;
    RpcServer rpcPort;

    // Optimal superquadric
    vector<Superquadric> optimal;

    // PointCloud class
    PointCloud point_cloud;
    vector<Vector> pc_scene_yarp;
    vector<Vector> pc_table_yarp;
    vector<Vector> pc_object_yarp;

    // Object Superquadric
    vector<Superquadric> superqs;
    vector<vector<unsigned char>> all_colors;

    // Superq Estimator
    SuperqEstimatorApp estim;

    // Grasping pose Estimator
    GraspEstimatorApp grasp_estim;

    // Grasp pose
    GraspResults grasp_res;
    vector<GraspPoses> grasp_poses;

    PointCloud points_hand;

    // VTK visualizer
    Visualizer vis;

public:

    /********************************************************/
    bool attach(RpcServer &source)
    {
        return this->yarp().attachAsServer(source);
    }

    /****************************************************************/
    bool configure(ResourceFinder& rf) override
    {
        const string name = "evaluate-superquadric";
        period=rf.check("period",Value(0.1)).asDouble();

        Property gaze_options;
        gaze_options.put("device", "gazecontrollerclient");
        gaze_options.put("local", "/"+name+"/gaze");
        gaze_options.put("remote", "/iKinGazeCtrl");
        if (!gaze.open(gaze_options)) {
            yError() << "Unable to open gaze driver!";
            return false;
        }

        rgbPort.open("/"+name+"/rgb:i");
        depthPort.open("/"+name+"/depth:i");
        rpcPort.open("/"+name+"/rpc");
        attach(rpcPort);

        optimal.resize(1);
        Vector11d params;
        params <<0.100759, 0.0474147, 0.0256408, 0.625482, 1.07882, -0.180689,
                0.0434923, -0.353306, 2.98514, 1.55354, 3.15846;
        optimal[0].setSuperqParams(params);
//        Vector3d center;
//        center <<-0.180322, 0.0436625, -0.354931;
//        optimal[0].setSuperqCenter(center);
//        Vector3d euler;
//        euler << 2.98945, 1.55579, 3.16136;
//        optimal[0].setSuperqOrientation(euler);

        Vector3d dims;
        dims << 0.102822, 0.0458641, 0.024837;
        optimal[0].setSuperqDims(dims);

        vis.visualize();

        return true;
    }

    /****************************************************************/
    double getPeriod() override
    {
        return period;
    }

    /****************************************************************/
    bool updateModule() override
    {
        return true;
    }

    /****************************************************************/
    bool segment() override
    {
        pc_scene_yarp.clear();
        pc_table_yarp.clear();
        pc_object_yarp.clear();

        // get image data
        auto* rgbImage = rgbPort.read();
        auto* depthImage = depthPort.read();

        if ((rgbImage == nullptr) || (depthImage == nullptr)) {
            yError() << "Unable to receive image data!";
            return false;
        }

        if ((rgbImage->width() != depthImage->width()) ||
            (rgbImage->height() != depthImage->height()) ) {
            yError() << "Received image data with wrong size!";
            return false;
        }

        const auto w = rgbImage->width();
        const auto h = rgbImage->height();

        // get camera extrinsics
        IGazeControl* igaze;
        gaze.view(igaze);
        Vector cam_x, cam_o;
        igaze->getLeftEyePose(cam_x, cam_o);
        yarp::sig::Matrix Teye(4, 4);
        Teye = axis2dcm(cam_o);
        Teye.setSubcol(cam_x, 0, 3);

        // get camera intrinsics
        Bottle info;
        igaze->getInfo(info);
        const auto fov_h = info.find("camera_intrinsics_left").asList()->get(0).asDouble();

        // aggregate image data in the point cloud of the whole scene
        Vector x{0., 0., 0., 1., 0, 0, 0};
        for (int v = 0; v < h; v++) {
            for (int u = 0; u < w; u++) {
                const auto rgb = (*rgbImage)(u, v);
                const auto depth = (*depthImage)(u, v);
                if (depth > 0.F) {
                    x[0] = depth * (u - .5 * (w - 1)) / fov_h;
                    x[1] = depth * (v - .5 * (h - 1)) / fov_h;
                    x[2] = depth;
                    x.setSubvector(0, Teye * x.subVector(0, 2));
                    x[4] = rgb.r;
                    x[5] = rgb.g;
                    x[6] = rgb.b;
                    pc_scene_yarp.push_back(x);
                }
            }
        }

        segment_color(pc_scene_yarp, pc_table_yarp, pc_object_yarp);
        if (pc_object_yarp.size() > 0)
        {
            // Convert to point cloud
            deque<Eigen::Vector3d> points_eigen = vectorYarptoEigen(pc_object_yarp);
            point_cloud.setPoints(points_eigen);
            point_cloud.setColors(all_colors);

            // Add points to visualizer
            // (true/false to show downsample points used for superq estimation)
            vis.addPoints(point_cloud, false);

            return true;
        }
        else
        {
            return false;
        }
    }

    /****************************************************************/
    bool fit() override
    {
        // Compute superq
        estim.SetIntegerValue("optimizer_points", 2000);
        superqs = estim.computeSuperq(point_cloud);


        // Add superquadric to visualizer
        if (superqs.size() > 0)
        {
            std::cout << "OPTIMAL: " << optimal[0].getSuperqDims() << " " << optimal[0].getSuperqExps() << std::endl;
            std::cout << "DIMS: " << superqs[0].getSuperqDims() << " " << superqs[0].getSuperqExps() << std::endl;
//            // Compute grasp pose for right hand
//            grasp_estim.SetStringValue("left_or_right", "left");
//            grasp_res = grasp_estim.computeGraspPoses(superqs);

//            // Add poses for grasping
//            vis.addPoses(grasp_res.grasp_poses);

//            vis.highlightBestPose("left", "left", 0);

//            // Add hands in final pose for grasping
//            vis.addSuperqHands(grasp_res.hand_superq);

//            if (grasp_res.points_on.size()>0)
//            {
//                points_hand.setPoints(grasp_res.points_on[0]);
//                vis.addPointsHands(points_hand);
//            }

            vis.addSuperq(superqs);
            return true;
        }
        else
        {
            return false;
        }
    }

    /****************************************************************/
    double get_inliers() override
    {
        Vector3d point = point_cloud.getBarycenter();
        auto points = point_cloud.points;
        VectorXd pose(6);
        Vector fit_scores(superqs.size());
        Vector inliers(superqs.size());
        for (size_t i=0; i<superqs.size(); i++)
        {
            int count = 0;
            Vector3d p = superqs[i].getSuperqCenter();
            Vector3d ea = superqs[i].getSuperqEulerZYZ();
            pose[0] = p[0];
            pose[1] = p[1];
            pose[2] = p[2];
            pose[3] = ea[0];
            pose[4] = ea[1];
            pose[5] = ea[2];
            for (size_t j=0; j<points.size(); j++)
            {
//                auto test = points[j];
//                test[0] += 0.03;
////                test[1] += 0.1;
////                test[2] += 0.2;
//                yDebug() << "TEST" << superqs[i].insideOutsideF(pose, test);
//                double score = superqs[i].insideOutsideF(pose, points[j]);
//                yDebug()<< __LINE__ << score << superqs[i].insideOutsideF(pose, point);
                double score = superqs[i].insideOutsideF(pose, points[j]);
//                yDebug()<< __LINE__ << score;
                if (score <= 1.0)
                {
                    fit_scores[i] += score;
                    count++;
                }
            }
            fit_scores[i] /= count;
            if (point_cloud.getNumberPoints() > 0)
            {
                inliers[i] = 100.0 * (double) count / point_cloud.getNumberPoints();
                yInfo() << "# points: " << point_cloud.getNumberPoints() << pc_object_yarp.size();
                yInfo() << "absolute # inliers: " << count;
                yInfo() << "% inliers: " << inliers[i];
            }
        }

        if (superqs.size() > 0)
        {
            yInfo()<<"Scores:" <<fit_scores.toString();
            yInfo()<<"% points inside the superquadric:" << inliers.toString();
            return inliers[0];
        }
        else
        {
            return 0.0;
        }
    }

    /****************************************************************/
    double get_score() override
    {
        double score = numeric_limits<double>::infinity();
        if (superqs.size()>0)
        {
            double d0 = superqs[0].getSuperqDims()[0]-optimal[0].getSuperqDims()[0];
            double d1 = superqs[0].getSuperqDims()[1]-optimal[0].getSuperqDims()[1];
            double d2 = superqs[0].getSuperqDims()[2]-optimal[0].getSuperqDims()[2];
            score = sqrt(d0*d0 + d1*d1 + d2*d2);
        }
        return score;
    }

    /****************************************************************/
    double go() override
    {
        if (segment())
        {
            if(fit())
            {
                return get_score();
            }
        }
        return numeric_limits<double>::infinity();
    }

    /**************************************************************************/
    void segment_color(vector<Vector> &pc_scene, vector<Vector> &pc_table, vector<Vector> &pc_object)
    {
        vector<unsigned char> colors(3);
        for (size_t i = 0; i < pc_scene.size(); i++) {
            const auto p = pc_scene[i];
            const auto r = p[4];
            const auto g = p[5];
            const auto b = p[6];
            double mean = (r + g + b) / 3;
            if (mean >= 230)
            {
                pc_table.push_back(p);
            }
            else
            {
                colors[0] = (unsigned char)r;
                colors[1] = (unsigned char)g;
                colors[2] = (unsigned char)b;
                pc_object.push_back(p);
                all_colors.push_back(colors);
            }
        }
    }

//    /**************************************************************************/
//    int RANSAC(vector<Vector> &pc_scene, vector<Vector> &pc_table, vector<Vector> &pc_object)
//    {
//        pcl::PointCloud<pcl::PointXYZRGB>::Ptr pc_scene_pcl(new pcl::PointCloud<pcl::PointXYZRGB>);
//        pc_scene_pcl->width = (int)pc_scene.size();
//        pc_scene_pcl->height = 1;
//        pc_scene_pcl->points.resize(pc_scene_pcl->width * pc_scene_pcl->height);
//        for (size_t i=0; i<pc_scene.size(); i++)
//        {
//            pc_scene_pcl->points[i].x = pc_scene[i][0];
//            pc_scene_pcl->points[i].y = pc_scene[i][1];
//            pc_scene_pcl->points[i].z = pc_scene[i][2];
//        }

////        for (size_t i=0; i<pc_scene_pcl->size(); i++)
////        {
////            yDebug() << __LINE__<< pc_scene[i][0] << pc_scene[i][1] << pc_scene[i][2];
////            yDebug() << __LINE__<< pc_scene_pcl->points[i].x << pc_scene_pcl->points[i].y << pc_scene_pcl->points[i].z;
////        }

//        std::vector<int> inliers;
//        pcl::SampleConsensusModelPlane<pcl::PointXYZRGB>::Ptr model_plane(new pcl::SampleConsensusModelPlane<pcl::PointXYZRGB> (pc_scene_pcl));

//        // get the inliers
//        pcl::RandomSampleConsensus<pcl::PointXYZRGB> ransac(model_plane);
//        ransac.setDistanceThreshold(.01);
//        ransac.computeModel(1);
//        ransac.getInliers(inliers);

//        // copy all inliers of the model computed to the table point cloud
//        for (size_t i = 0; i < pc_scene.size(); i++) {
//            const auto p = pc_scene[i];
//            if (std::find(inliers.begin(), inliers.end(), i) != inliers.end()) {
//                pc_table.push_back(p);
//            } else {
//                pc_object.push_back(p);
//            }
//        }
//        yInfo()<<"Segmented object with"<<pc_object.size()<<"points";
//        return inliers.size();
//    }

    /**************************************************************************/
    double RANSAC_const(vector<Vector> &pc_scene, vector<Vector> &pc_table, vector<Vector> &pc_object,
                  const int num_points = 100)
    {
        // generate random indexes
        std::random_device rnd_device;
        std::mt19937 mersenne_engine(rnd_device());
        std::uniform_int_distribution<int> dist(0, pc_scene.size() - 1);
        auto gen = std::bind(dist, mersenne_engine);
        std::vector<int> remap(num_points);
        std::generate(std::begin(remap), std::end(remap), gen);

        // implement RANSAC
        const auto threshold_1 = .01F; // [cm]
        for (size_t i = 0; i < remap.size(); i++) {
            auto pi = pc_scene[remap[i]];
            auto h = 0.F;
            size_t n = 0;
            for (size_t j = 0; j < remap.size(); j++) {
                const auto pj = pc_scene[remap[j]];
                if (std::fabs(pj[2] - pi[2]) < threshold_1) {
                    h += pj[2];
                    n++;
                }
            }
            h /= n;

            if (n > (remap.size() >> 1)) {
                pc_table.clear();
                pc_object.clear();
                const auto threshold_2 = h + threshold_1;
                for (size_t i = 0; i < pc_scene.size(); i++) {
                    const auto p = pc_scene[i];
                    if (p[2] < threshold_2) {
                        pc_table.push_back(p);
                    } else {
                        pc_object.push_back(p);
                    }
                }
                return h;
            }
        }

        return std::numeric_limits<double>::quiet_NaN();
    }

    /****************************************************************/
    deque<Eigen::Vector3d> vectorYarptoEigen(vector<Vector> &yarp_points)
    {
        deque<Eigen::Vector3d> eigen_points;
        for (size_t i = 0; i < yarp_points.size(); i++)
        {
            eigen_points.push_back(toEigen(yarp_points[i]));
        }

        return eigen_points;
    }

    /****************************************************************/
    bool interruptModule() override
    {
        rgbPort.interrupt();
        depthPort.interrupt();
        return true;
    }

    /****************************************************************/
    bool close() override
    {
        rgbPort.close();
        depthPort.close();
        rpcPort.close();
        return true;
    }

};

/****************************************************************/
int main(int argc, char* argv[])
{
    Network yarp;
    if (!yarp.checkNetwork())
    {
        yError() << "Unable to connect to YARP server";
        return EXIT_FAILURE;
    }

    ResourceFinder rf;
    rf.configure(argc, argv);

    Evaluator evaluator;
    return evaluator.runModule(rf);
}

