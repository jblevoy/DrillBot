# DrillBot
About: Automated production line section for a Heathkit GR150 AM radio kit component. 
Hardware: Arduino Uno controlling a 2 axis CNC drilling machine. Parts are loaded into a hopper and advanced one at a time throught the drilling stage, then ejected from line. Stepper motors drive X and Y axis. DC drill motor. 
Programs:
  drillBot_fsm_04.ino: final code update currently deployed in device. This program uses a finite state machine format in order to implement a pause button. The pause button saves the state including step count (for stepper motors). There are no delays, all timing sequences use millis() timer. 
Read more: https://jblevoy.wixsite.com/skunkworks/drillbot
Watch more: https://jblevoy.wixsite.com/skunkworks/drillbot-video
