#!/bin/bash

# to have dot separated values instead of comma separated values
#LANG=en_US

DSTFILE=$ROBOT_CODE/design-holder-realsense/result.txt

MINX=-0.03
MAXX=0.01
STEPX=0.01

MINY=-0.06
MAXY=0.06
STEPY=0.02

MINZ=-0.1 #at this height, the realsense enters the icub camera FOV when eyes' tilt go beyond 2.0 degrees
MAXZ=0.0
STEPZ=0.02

MINTHETA=0.0
MAXTHETA=25.0 #at 30, the realsense sees part of the icub head
STEPTHETA=5.0

# launch the demo
run() {
    yarpserver --write --silent &

    gazebo design-holder-realsense.sdf &

    yarp wait /realsense/mover:i

    yarpmanager-console --application ${ICUBcontrib_DIR}/share/ICUBcontrib/applications/design-holder-realsense/design-holder-realsense-app.xml --run --connect --exit --silent
         
    update_pose 
        
    sleep 5
    declare -a modules=("design-holder-realsense" "yarpview")
    for module in ${modules[@]}; do
        killall ${module}
    done

    sleep 5
    declare -a modules=("iKinGazeCtrl" "iKinCartesianSolver" "yarprobotinterface")
    for module in ${modules[@]}; do
        killall ${module}
    done

    sleep 5
    declare -a modules=("gzclient" "gzserver" "yarpserver")
    for module in ${modules[@]}; do
        killall ${module}
    done
}

# update camera pose
update_pose() {
    for x in $(seq $MINX $STEPX $MAXX)
    do
        for y in $(seq $MINY $STEPY $MAXY)
        do
            for z in $(seq $MINZ $STEPZ $MAXZ)
            do
                for theta in $(seq $MINTHETA $STEPTHETA $MAXTHETA)
                do
                    echo "updating pose with delta: $x $y $z $theta" | sed 's/,/./g' 
                    echo "$x $y $z $theta" | sed 's/,/./g' | yarp write ... /realsense/mover:i
                    sleep 2
                    score=$(echo "go" | yarp rpc /evaluate-superquadric/rpc | grep -Eo '[+-]?[0-9]+([.][0-9]+)?') 
                    echo "fitting score: $score"
                    echo "$x $y $z $theta $score" | sed 's/,/./g' >> $DSTFILE
                    sleep 2
                done
            done
        done
    done
}

# clean up hanging up resources
clean() {
    declare -a modules=("design-holder-realsense" "yarpview" \
                        "iKinGazeCtrl" "iKinCartesianSolver" \
                        "yarprobotinterface" "gzclient" "gzserver" "yarpserver")
    for module in ${modules[@]}; do
        killall -9 ${module}
    done
}

# main
if [[ $# -eq 0 ]]; then
    echo "demo is starting up..."
    run
    echo "...demo done"
elif [ "$1" == "clean" ]; then
    echo "cleaning up resources..."
    clean
    echo "...cleanup done"
else
    echo "unknown option!"
fi
