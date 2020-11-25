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
#include <gazebo/physics/PhysicsEngine.hh>
#include <gazebo/physics/World.hh>
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
    gazebo::physics::WorldPtr world;
    gazebo::physics::ModelPtr model;
    gazebo::physics::ModelPtr icub;
    gazebo::physics::JointPtr fixed_joint;
    gazebo::physics::LinkPtr child, parent;
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

                if (fixed_joint)
                {
                    yInfo() << "Detaching joint";
                    fixed_joint->Detach();
                }

                ignition::math::Pose3d curr_pos = starting_pos;
                const auto x = curr_pos.Pos().X() + b->get(0).asDouble();
                const auto y = curr_pos.Pos().Y() + b->get(1).asDouble();
                const auto z = curr_pos.Pos().Z() + b->get(2).asDouble();
                const auto roll = curr_pos.Rot().Roll() + (M_PI/180.0)*b->get(3).asDouble();
                const auto pitch = curr_pos.Rot().Pitch();
                const auto yaw = curr_pos.Rot().Yaw();
                ignition::math::Pose3d new_pose(x, y, z, roll, pitch, yaw);
                yInfo() << "New pose:" << x << y << z << roll << pitch << yaw;
                model->SetWorldPose(new_pose);

                child = icub->GetLink("icub_head_realsense_holder::head");
                parent = model->GetLink("realsense_camera_D415::link");
                if (child && parent)
                {
                    yInfo() << "Reattaching joint";
                    fixed_joint->Attach(parent,child);
                }
                else
                {
                    yError() << "icub_head_realsense_holder::head or realsense_camera_D415::link not found";
                }
            }
        }
    }

public:
    /**************************************************************************/
    void Load(gazebo::physics::ModelPtr model, sdf::ElementPtr)
    {
        this->model = model;
        this->world = model->GetWorld();

        // this creates the fixed joint, not attached to anything yet
        this->fixed_joint = world->Physics()->CreateJoint("fixed", model);

        fixed_joint->SetName(model->GetName() + "_fixed_to_icub");
        fixed_joint->SetModel(model);

        icub = world->ModelByName("iCub");
        if (!icub)
        {
            yError() << "Model for icub not found";
        }
        child = icub->GetLink("icub_head_realsense_holder::head");
        parent = model->GetLink("realsense_camera_D415::link");
        if (!child)
        {
            yError() << "icub_head_realsense_holder::head link not found";
        }
        if (!parent)
        {
            yError() << "realsense_camera_D415::link not found";
        }
        fixed_joint->Load(parent, child, ignition::math::Pose3d());
        fixed_joint->Attach(parent,child);

        starting_pos = this->model->WorldPose();
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
