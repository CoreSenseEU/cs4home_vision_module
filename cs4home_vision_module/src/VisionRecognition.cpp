// Copyright 2024 Intelligent Robotics Lab
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "cs4home_core/Core.hpp"
#include "cs4home_core/macros.hpp"

#include "yolo_msgs/msg/detection_array.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/string.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;

/**
 * @class VisionRecognition
 * @brief Core component that process incoming audio messages.
 */
class VisionRecognition : public cs4home_core::Core {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(VisionRecognition)

  /**
   * @brief Constructs an VisionRecognition object and initializes the parent
   * lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this
   * VisionRecognition instance.
   */

  explicit VisionRecognition(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Core("vision_recognition", parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Core created: [VisionRecognition]");
    
  }

  void process_vision_data(
      std::shared_ptr<yolo_msgs::msg::DetectionArray> yolo_msg,
      std::shared_ptr<sensor_msgs::msg::Image> image_msg) {

    RCLCPP_INFO(parent_->get_logger(), "[VisionRecognition]: Processing data...");
  }

  

  /**
   * @brief Timer callback function that retrieves vision information from YOLO detections and the image message, and
   * processes it.
   *
   * This function is called periodically and attempts to retrieve visual detections
   * message from the afferent component. If a message is received, it is
   * passed to `process_vision_data`.
   */
  void timer_callback() {

    auto detections_msg = afferent_->get_msg<yolo_msgs::msg::DetectionArray>(0);
    auto image_msg = afferent_->get_msg<sensor_msgs::msg::Image>(1);
    if (detections_msg && image_msg){
      RCLCPP_INFO(parent_->get_logger(), "[VisionRecognition] Visual information");
      process_vision_data(detections_msg, image_msg);
    }

  }

  /**
   * @brief Configures the VisionRecognition component.
   * @return True if configuration is successful.
   */
  bool configure() override {
    RCLCPP_DEBUG(parent_->get_logger(), "Core configured");
    return true;
  }

  /**
   * @brief Activates the VisionRecognition component by initializing a timer.
   *
   * The timer is set to call `timer_callback` every 50 milliseconds.
   *
   * @return True if activation is successful.
   */
  bool activate() override {
    timer_ = parent_->create_wall_timer(
        1000ms, std::bind(&VisionRecognition::timer_callback, this));
    return true;
  }

  /**
   * @brief Deactivates the VisionRecognition component by disabling the timer.
   *
   * The timer is reset to null, stopping periodic message processing.
   *
   * @return True if deactivation is successful.
   */
  bool deactivate() override {
    timer_ = nullptr;
    return true;
  }

private:
  rclcpp::TimerBase::SharedPtr
      timer_; /**< Timer for periodic execution of `timer_callback`. */
  const double TIME_SYNC_TOLERANCE = 1.0;
};

/// Registers the VisionRecognition component with the ROS 2 class loader
CS_REGISTER_COMPONENT(VisionRecognition)
