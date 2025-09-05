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

#include <iomanip>
#include <sstream>
#include <ctime>

#include "cs4home_core/Core.hpp"
#include "cs4home_core/macros.hpp"

#include "yolo_msgs/msg/detection_array.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/string.hpp"
#include "cs4home_msgs/msg/entity.hpp"
#include "cs4home_msgs/msg/context_description.hpp"
#include "ros_typedb_msgs/srv/query.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

#include "llama_msgs/action/generate_response.hpp"

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
    context_ = std::make_shared<cs4home_msgs::msg::ContextDescription>();
    context_->source = "vision";
  }

  void process_vision_data(
      std::shared_ptr<yolo_msgs::msg::DetectionArray> yolo_msg,
      std::shared_ptr<sensor_msgs::msg::Image> image_msg) {

    for (int i=0; i<yolo_msg->detections.size(); i++){
      cs4home_msgs::msg::Entity entity;
      entity.class_name = yolo_msg->detections[i].class_name;
      entity.location.position.x = yolo_msg->detections[i].bbox.center.position.x;
      entity.location.position.y = yolo_msg->detections[i].bbox.center.position.y;
      context_->entities.push_back(entity);
    }

    RCLCPP_INFO(parent_->get_logger(), "[VisionRecognition]: Processing data...");
    llama_msgs::action::GenerateResponse::Goal goal;
    goal.prompt = R"(<__media__>
    This image was captured by a robot's camera.
    Describe briefly in this exact format:
    Environment: [Which room in the apartment]
    Objects: [List object types present, no counts]
    People: [Present/None]
    Activity: [If obvious, else 'None']
    Risk: [If obvious, else 'None']
    Keep it short.)";
    goal.images.push_back(*image_msg);
    goal.sampling_config.temp = 0.7;
    goal.reset = true;

    rclcpp_action::Client<llama_msgs::action::GenerateResponse>::SendGoalOptions opts;
    opts.goal_response_callback =
      [this](std::shared_ptr<rclcpp_action::ClientGoalHandle<llama_msgs::action::GenerateResponse>> gh) {
        if (!gh) {
          RCLCPP_ERROR(parent_->get_logger(), "Goal rechazado por el servidor.");
        } else {
          RCLCPP_INFO(parent_->get_logger(), "Goal aceptado. Esperando resultado...");
        }
      };

    opts.result_callback =
      [this](const rclcpp_action::ClientGoalHandle<llama_msgs::action::GenerateResponse>::WrappedResult & result) {
        using rclcpp_action::ResultCode;
        if (result.code != ResultCode::SUCCEEDED) {
          RCLCPP_ERROR(parent_->get_logger(), "Action terminó con estado: %d", static_cast<int>(result.code));
          return;
        }

        // Se asume que el result tiene campo 'response.text'
        const auto & res = result.result;
        if (res && !res->response.text.empty()) {
          RCLCPP_INFO(parent_->get_logger(), "Context description - %s", res->response.text.c_str());

          context_->scene_description = res->response.text.c_str();

          efferent_->publish(0, context_);
          RCLCPP_INFO(parent_->get_logger(), "Efferent sent");

          // DATABASE
          auto request = std::make_shared<ros_typedb_msgs::srv::Query::Request>();
          request->query_type = ros_typedb_msgs::srv::Query::Request::INSERT;

          RCLCPP_INFO(parent_->get_logger(), "Create request");

          auto now = parent_->now();
          std::time_t t = now.seconds();
          std::tm tm = *std::gmtime(&t);
          std::ostringstream ts;
          ts << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
          // start building the TypeQL insert
          std::stringstream ss;
          ss << "insert "
          << "$sc isa scene, has description \"" << context_->scene_description
          << "\", has timestamp " << ts.str() << "; ";



          RCLCPP_INFO(parent_->get_logger(), "Scene");

          // now entities from YOLO
          for (size_t i = 0; i < context_->entities.size(); i++) {
            const auto & ent = context_->entities[i];
            std::string var = "$e" + std::to_string(i);

            ss << var << " isa object, has object-label \"" << ent.class_name << "\", "
              << "has x-pos " << ent.location.position.x << ", "
              << "has y-pos " << ent.location.position.y << ", "
              << "has z-pos 0.0; ";


            ss << "(context: $sc, observed: " << var
              << ") isa perception, has source-type \"vision\"; ";

          }

          RCLCPP_INFO(parent_->get_logger(), "entities");

          RCLCPP_INFO(parent_->get_logger(), "query: ", ss.str().c_str());

          request->query = ss.str();
          RCLCPP_INFO(parent_->get_logger(), "sending");

          auto future_request = typedb_client_->async_send_request(request);

        } else {
          RCLCPP_WARN(parent_->get_logger(), "Resultado recibido pero sin texto en response.");
        }
      };

    // Enviar goal (asíncrono)
    auto future_goal_handle = client_->async_send_goal(goal, opts);
    
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
    if(image_msg){
      RCLCPP_INFO(parent_->get_logger(), "[VisionRecognition] image information");
    }
    if(detections_msg){
      RCLCPP_INFO(parent_->get_logger(), "[VisionRecognition] 3d information");
    }
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
    client_ = rclcpp_action::create_client<llama_msgs::action::GenerateResponse>(parent_, "/llama/generate_response");
    typedb_client_ = parent_->create_client<ros_typedb_msgs::srv::Query>("/ros_typedb_interface/query");
    RCLCPP_INFO(parent_->get_logger(), "Esperando servidor de acción en /llama/generate_response...");
    if (!client_->wait_for_action_server(10s)) {
      RCLCPP_WARN(parent_->get_logger(), "Servidor de acción no disponible tras 10s. Aún puedes dejar el nodo corriendo.");
      return false;
    }
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
        10000ms, std::bind(&VisionRecognition::timer_callback, this));
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
  rclcpp_action::Client<llama_msgs::action::GenerateResponse>::SharedPtr client_;
  rclcpp::Client<ros_typedb_msgs::srv::Query>::SharedPtr typedb_client_;
  std::shared_ptr<cs4home_msgs::msg::ContextDescription> context_;
  const double TIME_SYNC_TOLERANCE = 1.0;
};

/// Registers the VisionRecognition component with the ROS 2 class loader
CS_REGISTER_COMPONENT(VisionRecognition)
