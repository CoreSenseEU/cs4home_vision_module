std_msgs/Header header

string fused_from           # "audio+vision", "audio", "vision"

# --- Severity factors ---
float32 harm_potential
float32 social_inappropriateness
float32 task_criticality
float32 vulnerability_of_being
float32 emotional_disruption

# --- Occurrence factors ---
float32 dynamic_obstruction_risk
float32 interaction_level
float32 attentiveness_inverse

# --- Detection factors ---
float32 perceptual_blindness_score
float32 signal_quality_inverse
float32 entity_observability_inverse
float32 sensor_latency
float32 reaction_window_inverse