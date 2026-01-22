std_msgs/Header header

float32 severity            # S
float32 occurrence          # O
float32 detection           # D

# Final score
float32 rpn                 # Risk Priority Number 

# Categorization
uint8 risk_level
uint8 RISK_LEVEL_LOW=0
uint8 RISK_LEVEL_MEDIUM=1
uint8 RISK_LEVEL_HIGH=2
uint8 RISK_LEVEL_CRITICAL=3