std_msgs/Header header

uint8 decision
uint8 DECISION_NO_CHANGE=0
uint8 DECISION_DEGRADE=1
uint8 DECISION_REPLAN=2
uint8 DECISION_STOP=3
uint8 DECISION_ASK_HUMAN=4

string[] activated_modules
string[] deactivated_modules

string reason
float32 rpn              




