# cs4home_vision_module
Use case of the cs4home architecture prototype focused on the cognitive module of visual perception

## Prerequisites

- [YOLO](https://github.com/mgonzs13/yolo_ros)
- [Llama](https://github.com/mgonzs13/llama_ros/tree/humble?tab=readme-ov-file)

## Docker 
Build the llama_ros docker or download and image from [DockerHub](https://hub.docker.com/r/mgons/llama_ros/tags). You can choose to build llama_ros with CUDA (USE_CUDA) and choose the CUDA version (CUDA_VERSION). Remember that you have to use DOCKER_BUILDKIT=0 to compile llama_ros with CUDA when building the image.


```bash
DOCKER_BUILDKIT=0 docker build -t llama_ros --build-arg USE_CUDA=1 --build-arg CUDA_VERSION=12-6 .
```

Run the docker container with [Rocker](https://github.com/osrf/rocker)

```bash
cd ~/ros2_ws/src/cs4home_vision_module
rocker --nvidia --x11 \
  --network host --ipc host \
  --device /dev/snd \
  --device /dev/bus/usb/005/005 \
  --group-add audio \
  --volume ~/audio_ws:/root/ros2_ws \
  --env CYCLONEDDS_URI=file:///root/cyclone_config.xml \
  --volume ~/cyclone_config.xml:/root/cyclone_config.xml:ro \
  --privileged \
 llama_ros
```

## Installation

```bash
cd ~/ros2_ws/src
git clone https://github.com/CoreSenseEU/cs4home_vision_module.git
vcs import --recursive < cs4home_vision_module/thirparty.repos
cd ~/ros2_ws
colcon build --cmake-args -DGGML_CUDA=ON
```

## Launch

```bash
ros2 launch llama_bringup minicpm-2.6.launch.py
ros2 launch cs4home_vision_module launch_vision.launch.py
```

The configuration for the vision cognitive module is under the `cs4home_vision_module/config/params.yaml`

```yaml
vision_recognition:
  ros__parameters:
    core: vision_recognition
    afferent: vision_input
    vision_input:
      topics:
        [
          "/yolo/detections",
          "/head_front_camera/rgb/image_raw",
        ]
      types:
        [
          "yolo_msgs/msg/DetectionArray",
          "sensor_msgs/msg/Image",
        ]
    efferent: vision_output
    vision_output:
      topics: ["/vision_description"]
      types: ["std_msgs/msg/String"]
    meta: vision_meta
    coupling: vision_coupling
```
