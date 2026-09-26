# cs4home_vision_module

This repository implements the CoreSense4Home visual-context cognitive module. It combines camera images and YOLO detections, uses a vision-language model to describe the scene and publishes a structured `ContextDescription` for downstream modules.

The existing package and namespace names are retained unchanged.

## CoreSense role

The terms below follow the [CoreSense Ontology (CSO)](https://w3id.org/coresense/cso).

- The camera is a [Sensor](https://w3id.org/coresense/cso#Sensor) that acquires visual information about the environment.
- Detection fusion and scene description form a [Cognitive Function](https://w3id.org/coresense/cso#CognitiveFunction).
- The module provides a visual-context [Cognitive Capability](https://w3id.org/coresense/cso#CognitiveCapability).
- The result is [Context](https://w3id.org/coresense/cso#Context): information retained because it is relevant to interpretation, evaluation and action.
- The scene description assigns [Meaning](https://w3id.org/coresense/cso#Meaning) to the visual evidence for use by other cognitive modules.

## Data flow

~~~mermaid
flowchart LR
    camera["Camera image"] --> vision["VisionRecognition"]
    detections["YOLO detections"] --> vision
    model["Vision-language model"] --> vision
    vision --> entities["Structured entities"]
    vision --> description["Scene description"]
    entities --> context["ContextDescription"]
    description --> context
    context --> downstream["Contextualizer or robot task"]
~~~

## Requirements and build

Use Ubuntu 22.04 and ROS 2 Humble. The workspace must contain YOLO ROS, `llama_ros`, `cs4home_architecture` and the TypeDB ROS interface used by the module. A GPU is optional but recommended for local vision-language model execution.

~~~bash
mkdir -p ~/cs4home_ws/src
cd ~/cs4home_ws/src
git clone https://github.com/CoreSenseEU/cs4home_vision_module.git
vcs import --recursive < cs4home_vision_module/thirparty.repos
cd ..
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
source install/setup.bash
~~~

The dependency manifest is named `thirparty.repos` in the current repository and is referenced without renaming it.

## Run

Start a `llama_ros` model that provides the `/llama/generate_response` action, then start YOLO:

~~~bash
ros2 launch yolo_bringup yolo.launch.py
~~~

Launch and activate the visual-context module:

~~~bash
ros2 launch cs4home_vision_module launch_vision.launch.py
ros2 lifecycle set /vision_recognition configure
ros2 lifecycle set /vision_recognition activate
~~~

Once active, the module periodically combines the latest image and detections into a contextual description.

## Module configuration

The vision cognitive module uses the following structure:

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
      types: ["cs4home_msgs/msg/ContextDescription"]
    meta: vision_meta
    coupling: vision_coupling
```

## Acknowledgement

<img src="https://github.com/user-attachments/assets/b11da974-9201-4f79-902e-c9c20e8aa7a4" alt="Funded by the European Union" width="240"/>

This work has received funding from the European Union's Horizon Europe research and innovation programme under grant agreement No 101070254 ([CORESENSE](https://coresense.eu)). Views and opinions expressed are however those of the author(s) only and do not necessarily reflect those of the European Union or the European Commission. Neither the European Union nor the granting authority can be held responsible for them.
