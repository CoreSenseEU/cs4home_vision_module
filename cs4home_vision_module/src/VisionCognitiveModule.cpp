#include "cs4home_core/CognitiveModule.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

class VisionCognitiveModule : public cs4home_core::CognitiveModule {
public:
  explicit VisionCognitiveModule(
      const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
      : cs4home_core::CognitiveModule("vision_cognitive_module", options) {
    RCLCPP_INFO(this->get_logger(), "VisionCognitiveModule initialized");
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<VisionCognitiveModule>();
  rclcpp::spin(node->get_node_base_interface());
  rclcpp::shutdown();
  return 0;
}
