/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <cmath>

#include <gazebo/common/Plugin.hh>
#include <gazebo/physics/Model.hh>
#include <gazebo/physics/Link.hh>
#include <gazebo/physics/Joint.hh>
#include <gazebo/common/Events.hh>
#include <ignition/math/Pose3.hh>

#include <boost/bind.hpp>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>

namespace gazebo {

/******************************************************************************/
class ModelMover : public gazebo::ModelPlugin
{
    gazebo::physics::ModelPtr model;
    gazebo::event::ConnectionPtr renderer_connection;
    yarp::os::BufferedPort<yarp::os::Bottle> port;
    ignition::math::Pose3d starting_pos;

    /**************************************************************************/
    void onWorldFrame()
    {
        if (auto* b = port.read(false))
        {
            if (b->size() >= 4)
            {
                if (model->GetJoint("realsense_joint"))
                {
                    if (model->RemoveJoint("realsense_joint"))
                    {
                        yInfo() << "Removed fixed_to_ground joint";
                    }
                }

                ignition::math::Pose3d curr_pos = starting_pos; // model->WorldPose();
                const auto x = curr_pos.Pos().X() + b->get(0).asDouble();
                const auto y = curr_pos.Pos().Y() + b->get(1).asDouble();
                const auto z = curr_pos.Pos().Z() + b->get(2).asDouble();
                const auto roll = curr_pos.Rot().Roll() + (M_PI/180.0)*b->get(3).asDouble();
                const auto pitch = curr_pos.Rot().Pitch(); // + (M_PI/180.0)*b->get(4).asDouble();
                const auto yaw = curr_pos.Rot().Yaw(); // + (M_PI/180.0)*b->get(5).asDouble();
                ignition::math::Pose3d new_pose(x, y, z, roll, pitch, yaw);
                yInfo() << "New pose:" << x << y << z << roll << pitch << yaw;
                model->SetWorldPose(new_pose);

                physics::LinkPtr child = model->GetLink("icub_head_realsense_holder::head");
                physics::LinkPtr parent = model->GetLink("realsense_camera_D415::link");
                if (child || parent)
                {
                    if (model->CreateJoint("realsense_joint", "fixed", parent, child))
                    {
                        yInfo() << "Added realsense_joint";
                    }
                }
            }
        }
    }

public:
    /**************************************************************************/
    void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr)
    {
        this->model = model;
        starting_pos = this->model->WorldPose();
//        auto model_sdf = model->GetSDF();
//        if( model_sdf->HasElement("pose") )
//        {
//            starting_pos = model_sdf->Get<ignition::math::Pose3d>("pose");
//        }
//        else
//        {
//            starting_pos = ignition::math::Pose3d(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
//        }
        port.open("/" + model->GetName() + "/mover:i");
        auto bind = boost::bind(&ModelMover::onWorldFrame, this);
        renderer_connection = gazebo::event::Events::ConnectWorldUpdateBegin(bind);
    }

    /**************************************************************************/
    virtual ~ModelMover()
    {
        if (!port.isClosed())
        {
            port.close();
        }
    }
};

}

GZ_REGISTER_MODEL_PLUGIN(gazebo::ModelMover)
