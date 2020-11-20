🔽 Instructions to run the sandbox locally
==========================================

To run the sandbox locally using [Docker](https://www.docker.com), go through the following steps:
1. Pull the docker image:
    ```sh
    $ docker pull tbc
    ```
1. Launch the container:
    ```sh
    $ docker run -it --rm -p 6080:6080 --user gitpod tbc
    ```
1. From within the container shell, launch the following scripts:
    ```sh
    $ init-icubcontrib-local.sh
    $ start-vnc-session-local.sh
    ```
1. Clone and install the project:
    ```sh
    $ git clone https://github.com/icub-tech-iit/design-holder-realsense.git /workspace/design-holder-realsense
    $ cd /workspace/design-holder-realsense 
    $ mkdir build && cd build
    $ cmake ../
    $ make install
    ```
1. Open up the browser and connect to **`localhost:6080`** to get to the workspace desktop GUI.
1. In the desktop GUI, open a terminal and run the grasping experiment:
   ```sh
   $ run-analysis.sh
   ```
1. Once done, from the container shell press **CTRL+D**.

