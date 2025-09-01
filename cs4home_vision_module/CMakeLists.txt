cmake_minimum_required(VERSION 3.20)
project(cs4home_vision_module)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(cs4home_core REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(yolo_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(llama_msgs REQUIRED)
find_package(cs4home_msgs REQUIRED)

set(dependencies
  ament_cmake
  rclcpp
  rclcpp_lifecycle
  rclcpp_action
  cs4home_core
  yolo_msgs
  std_msgs
  sensor_msgs
  llama_msgs
  cs4home_msgs
)

set(SOURCES
  src/${PROJECT_NAME}/VisionCognitiveModule.cpp
)

install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}
)

# CORE
add_library(vision_recognition SHARED src/VisionRecognition.cpp)
ament_target_dependencies(vision_recognition ${dependencies} ${test_dependencies})

# AFFERENT
add_library(vision_input SHARED src/VisionInput.cpp)
ament_target_dependencies(vision_input ${dependencies} ${test_dependencies})

# EFFERENT
add_library(vision_output SHARED src/VisionOutput.cpp)
ament_target_dependencies(vision_output ${dependencies} ${test_dependencies})

# META
add_library(vision_meta SHARED src/VisionMeta.cpp)
ament_target_dependencies(vision_meta ${dependencies} ${test_dependencies})

# COUPLING
add_library(vision_coupling SHARED src/VisionCoupling.cpp)
ament_target_dependencies(vision_coupling ${dependencies} ${test_dependencies})

add_executable(vision_cognitive_module src/VisionCognitiveModule.cpp)
ament_target_dependencies(vision_cognitive_module ${dependencies})
target_link_libraries(vision_cognitive_module vision_recognition vision_input vision_output vision_meta vision_coupling)

install(TARGETS
  vision_cognitive_module
  vision_recognition
  vision_input
  vision_output
  vision_meta
  vision_coupling
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin}
)

install(TARGETS
  vision_cognitive_module
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_libraries(vision_recognition vision_input vision_output vision_meta vision_coupling)
ament_export_dependencies(${dependencies})
ament_package()
