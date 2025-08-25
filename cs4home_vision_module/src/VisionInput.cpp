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

#include "cs4home_core/Afferent.hpp"
#include "cs4home_core/macros.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

/**
 * @class VisionInput
 * @brief Manages image input by creating subscribers for specified topics and
 *        handling audio messages from these sources.
 */
class VisionInput : public cs4home_core::Afferent {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(VisionInput)

  /**
   * @brief Constructs a VisionInput object and declares necessary
   * parameters.
   * @param parent Shared pointer to the lifecycle node managing this
   * VisionInput instance.
   */
  explicit VisionInput(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Afferent("vision_input", parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Afferent created: [VisionInput]");
  }

  /**
   * @brief Configures the VisionInput by creating subscribers for each
   * specified topic.
   *
   * This method retrieves the topic names from the parameter server and
   * attempts to create a subscription for each topic.
   *
   * @return True if all subscriptions are created successfully.
   */
  bool configure() override { return Afferent::configure(); }
};

/// Registers the VisionInput component with the ROS 2 class loader
CS_REGISTER_COMPONENT(VisionInput)
