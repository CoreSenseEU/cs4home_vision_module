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

#include "cs4home_core/Coupling.hpp"
#include "cs4home_core/macros.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

/**
 * @class DefaultCoupling
 * @brief A Coupling component that provides default configuration for
 * coupling-related tasks.
 *
 * This class extends the Coupling component, initializing with basic
 * configuration. It is intended for tasks involving the coordination and
 * interaction of functional components.
 */
class DefaultCoupling : public cs4home_core::Coupling {
public:
  RCLCPP_SMART_PTR_DEFINITIONS(DefaultCoupling)

  /**
   * @brief Constructs a DefaultCoupling object and initializes the parent
   * lifecycle node.
   * @param parent Shared pointer to the lifecycle node managing this
   * DefaultCoupling instance.
   */
  explicit DefaultCoupling(rclcpp_lifecycle::LifecycleNode::SharedPtr parent)
      : Coupling("vision_coupling", parent) {
    RCLCPP_DEBUG(parent_->get_logger(), "Coupling created: [VisionCoupling]");
  }

  /**
   * @brief Configures the DefaultCoupling component.
   *
   * Logs the configuration step and prepares the component for operation.
   *
   * @return True if configuration is successful.
   */
  bool configure() {
    RCLCPP_DEBUG(parent_->get_logger(), "Coupling configured");
    return true;
  }
};

/// Registers the DefaultCoupling component with the ROS 2 class loader
CS_REGISTER_COMPONENT(DefaultCoupling)
