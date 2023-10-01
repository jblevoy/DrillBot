
void setup() {
    Serial.begin(9600);
    pinMode(A0, OUTPUT);                    // motor relay 
    pinMode(A1, OUTPUT);                    // Green LED; should be solid when machine is ready, blink when in operation
    pinMode(A2, OUTPUT);                    // Red LED; should blink when paused
    pinMode(2, INPUT);                      // upper button - pauses and resumes blinking light -> should also be: pause drill operation and resume it if already stopped
    pinMode(5, INPUT);                      // lower button - reset and run
    pinMode(6, OUTPUT);                     // drill stepper direction
    pinMode(7, OUTPUT);                     // drill stepper step
    pinMode(8, OUTPUT);                     // feed stepper direction
    pinMode(9, OUTPUT);                     // feed stepper step
    pinMode(10, INPUT);                     // board mount button
    pinMode(11, INPUT);                     // feed stepper limit switch
    pinMode(12, INPUT);                     // drill stepper limit switch
    pinMode(13, OUTPUT);                    // builtin LED
}

#define LED_OFF             0
#define LED_OFF_WAIT        1
#define LED_ON              2
#define LED_ON_WAIT         3
#define PIN_2_HIGH          4
#define RESET_part_1        5               // feed and drill back to home position until limit switch
#define RESET_part_2        6               // feed and drill away from home to hopper position
#define RESET_part_3        7               
#define RESET_part_4        8              
#define HOPPER_TO_HOLE1     9               // feed from hopper to first hole location
#define DRILL_DOWN          10
#define DRILL_UP            11
#define HOLE1_TO_HOLE2      12
#define HOLE2_TO_HOLE3      13
#define HOLE3_TO_EJECT      14
#define DRILL_LIFT          15
#define EJECT_TO_HOPPER     16
#define RED_LED_OFF         17
#define RED_LED_OFF_WAIT    18
#define RED_LED_ON          19
#define RED_LED_ON_WAIT     20

int array_run_states[] = {RESET_part_1, RESET_part_2, RESET_part_3, RESET_part_4, HOPPER_TO_HOLE1, DRILL_DOWN, DRILL_UP, HOLE1_TO_HOLE2, HOLE2_TO_HOLE3, HOLE3_TO_EJECT, DRILL_LIFT, EJECT_TO_HOPPER};
int qty_run_states = 12;
float timer = 0;
bool pause = 0;                             //seems to be true generally when machine is paused
bool machine_running = 0;                   //used for red LED; should be on solid
bool complete_01 = 0;                       // used for drill cycle in eject to hopper #16
bool drill_in_pos = 0;                      //used during reset to determine completion of state/step
bool feed_in_pos = 0;                       //used during reset to determine completion of state/step
bool green_LED_on = 0;
uint32_t hold_step = 900;
uint32_t state = LED_OFF;
uint32_t saved_state = 0;
uint32_t saved_state_02 = 0;
uint32_t prev_state = 0;
uint32_t led_timer = 0;
uint32_t pre_led_timer = 0;
uint32_t count_timer = 0;
uint32_t count_timer_02 = 0;                //pertaining to RED_LED
uint32_t count_timer_03 = 0;
uint32_t stepper_timer = 0;
uint32_t stepper_timer_01 = 0;
uint32_t stepper_timer_02 = 0;
uint32_t stepper_timer_08 = 0;
uint32_t step_half = 0;
uint32_t step_half_01 = 0;
uint32_t step_half_02 = 0;
uint32_t step_half_08 = 0;
uint32_t count_01 = 0;
uint32_t count_02 = 0;
uint32_t count_03 = 0;
uint32_t count_04 = 0;
uint32_t count_05 = 0;
uint32_t count_06 = 0;
uint32_t count_07 = 0;
uint32_t count_08 = 0;
uint32_t count_09 = 0;
uint32_t count_10 = 0;
uint32_t count_11 = 0;
uint32_t count_12 = 0;
uint32_t count_13 = 0;                    //pertaining to RED_LED
uint32_t count_14 = 0;
uint32_t cycle_button_timer = 0;
uint32_t cycles = 0;
long int elapsed_time = 0;
long int press_timer = 0;
long int previous_time = 0;
bool released = 0;                          // flag indicates if board mount button has been released 

void loop() {

    if (digitalRead(5) == HIGH) {
        state = DRILL_DOWN;                                                               //state 5
        prev_state = 0;
    }
    for (int i=0; i<qty_run_states; i++){
        if (state == array_run_states[i]){
          machine_running = 1;
          break;
        }
        else machine_running = 0;
    }

    if ((machine_running == 1) && (millis() - count_timer_03 > 20) && (green_LED_on == 0)) {
        count_14++;
        count_timer_03 = millis();        
        if (count_14 >= 70){
          green_LED_on = 1;
          analogWrite(A1, 255);
          count_14 = 0;
        }
    }
    if ((machine_running == 1) && (millis() - count_timer_03 > 20) && (green_LED_on == 1)) {
        count_14++;
        count_timer_03 = millis();
        if (count_14 >= 20){
            green_LED_on = 0;
            analogWrite(A1, 0);       
            count_14 = 0;
        }
    }


//########################################################################################### SWITCH ###########################################################################################
    switch (state) {

        case LED_OFF:
            digitalWrite(13, LOW);
            state = LED_OFF_WAIT;
            break;

        case LED_OFF_WAIT:
            if (count_02 >= 20) {
                state = LED_ON;
                count_02 = 0;
                break;
            }
            if (millis() - count_timer > 20) {
                count_02 ++;
                count_timer = millis();
            }
            break;

        case LED_ON:
            digitalWrite(13, HIGH);
            state = LED_ON_WAIT;
            break;

        case LED_ON_WAIT:
            if (count_02 >= 20) {
                state = LED_OFF;
                count_02 = 0;
                break;
            }
            if (millis() - count_timer > 20) {
                count_02 ++;
                count_timer = millis();
            }
            break;
            
        case RESET_part_1:                                                                                                                  // state 5
            digitalWrite(6, LOW);                                                                                                           //drill direction toward home
            if ((digitalRead(12) == LOW) && (micros() - stepper_timer_01 > hold_step) && (step_half_01 == 0)) {                                   //go until limit switch = HIGH
                digitalWrite(7, HIGH);
                stepper_timer_01 = micros();
                step_half_01 = 1;
            }
            if ((digitalRead(12) == LOW) && (micros() - stepper_timer_01 > hold_step) && (step_half_01 == 1)) {
                digitalWrite(7, LOW);
                stepper_timer_01 = micros();
                step_half_01 = 0;
            }
            digitalWrite(8, LOW);                                                                                                               //feed dir toward home
            if ((digitalRead(11) == LOW) && (micros() - stepper_timer_02 > hold_step) && (step_half_02 == 0)) {                                       //feed switch not yet reached and not yet time
                digitalWrite(9, HIGH);                                                                                                          //step pin high
                stepper_timer_02 = micros();
                step_half_02 = 1;
            }
            if ((digitalRead(11) == LOW) && (micros() - stepper_timer_02 > hold_step) && (step_half_02 == 1)) {                                       //feed switch not yet reached and not yet time to switch
                digitalWrite(9, LOW);                                                                                                           //step pin low
                stepper_timer_02 = micros();
                step_half_02 = 0;
            }
            if ((digitalRead(11) == HIGH) && (digitalRead(12) == HIGH)) {
              Serial.println("are we here");
              state = RESET_part_2;
            }
            break;
        
        case DRILL_DOWN:
            digitalWrite(6, HIGH); // this moves the drill head away from home
            if ((micros() - stepper_timer_08 > 1800) && (step_half_08 == 0)) {
                digitalWrite(7, HIGH);
                stepper_timer_08 = micros();
                step_half_08 = 1;
            }
            if ((micros() - stepper_timer_08 > 1800) && (step_half_08 == 1)) {
                digitalWrite(7, LOW);
                stepper_timer_08 = micros();
                step_half_08 = 0;
                count_05 ++;
                if (count_05 == 700) {
                    digitalWrite(A0, HIGH);                                                                                       //turns motor on
                }
                if (count_05 == 1500) {
                    count_05 = 0;
                    state = DRILL_UP;
                }
            }

            break;

        default:
            break;
    }
}
