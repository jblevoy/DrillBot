
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
#define RESET_part_15      6
#define RESET_part_2        7               // feed and drill away from home to hopper position
#define RESET_part_3        8               
#define RESET_part_4        9              
#define HOPPER_TO_HOLE1     10               // feed from hopper to first hole location
#define DRILL_DOWN          11
#define DRILL_UP            12
#define HOLE1_TO_HOLE2      13
#define HOLE2_TO_HOLE3      14
#define HOLE3_TO_EJECT      15
#define DRILL_LIFT          16
#define EJECT_TO_HOPPER     17
#define RED_LED_OFF         18
#define RED_LED_OFF_WAIT    19
#define RED_LED_ON          20
#define RED_LED_ON_WAIT     21

int array_run_states[] = {RESET_part_1, RESET_part_15, RESET_part_2, RESET_part_3, RESET_part_4, HOPPER_TO_HOLE1, DRILL_DOWN, DRILL_UP, HOLE1_TO_HOLE2, HOLE2_TO_HOLE3, HOLE3_TO_EJECT, DRILL_LIFT, EJECT_TO_HOPPER};
int qty_run_states = 13;
float timer = 0;
bool pause = 0;                             //seems to be true generally when machine is paused
bool machine_running = 0;                   //used for green led, should blink 
bool complete_01 = 0;                       // used for drill cycle in eject to hopper #16
bool drill_in_pos = 0;                      //used during reset to determine completion of state/step
bool feed_in_pos = 0;                       //used during reset to determine completion of state/step
bool green_LED_on = 0;                      //used to check time green led blinking
uint32_t hold_step = 750;
uint32_t state = LED_OFF;
uint32_t saved_state = 0;
uint32_t saved_state_02 = 0;
uint32_t prev_state = 0;
uint32_t led_timer = 0;
uint32_t pre_led_timer = 0;
uint32_t count_timer = 0;
uint32_t count_timer_02 = 0;                //pertaining to RED_LED
uint32_t count_timer_03 = 0;                //pertaining to GREEN_LED
uint32_t stepper_timer = 0;
uint32_t stepper_timer_01 = 0;
uint32_t stepper_timer_02 = 0;
uint32_t stepper_timer_03 = 0;
uint32_t stepper_timer_04 = 0;
uint32_t stepper_timer_05 = 0;
uint32_t stepper_timer_06 = 0;
uint32_t stepper_timer_07 = 0;
uint32_t stepper_timer_08 = 0;
uint32_t stepper_timer_09 = 0;
uint32_t stepper_timer_10 = 0;
uint32_t stepper_timer_11 = 0;
uint32_t stepper_timer_12 = 0;
uint32_t stepper_timer_13 = 0;
uint32_t stepper_timer_14 = 0;
uint32_t stepper_timer_15 = 0;
uint32_t stepper_timer_16 = 0;
uint32_t step_half = 0;
uint32_t step_half_01 = 0;
uint32_t step_half_02 = 0;
uint32_t step_half_03 = 0;
uint32_t step_half_04 = 0;
uint32_t step_half_05 = 0;
uint32_t step_half_06 = 0;
uint32_t step_half_07 = 0;
uint32_t step_half_08 = 0;
uint32_t step_half_09 = 0;
uint32_t step_half_10 = 0;
uint32_t step_half_11 = 0;
uint32_t step_half_12 = 0;
uint32_t step_half_13 = 0;
uint32_t step_half_14 = 0;
uint32_t step_half_15 = 0;
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
uint32_t cycle_button_timer = 0;                //stuff for board mount cycle button
uint32_t cycles = 0;
long int elapsed_time = 0;
long int press_timer = 0;
long int previous_time = 0;
bool released = 0;                          // flag indicates if board mount button has been released 
bool hopper_pause = 0;                      //controls hopper pausing at the end in between cycles

void loop() {

    //Serial.println(state);
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
        if (count_14 >= 100){
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
        
    if (state == 0 or state == 1 or state == 2 or state == 3) {
        analogWrite(A1, 255);
    }
    else if (machine_running == 0) {
        analogWrite(A1, 0);
    }
    if ((pause == 0) || (machine_running == 1)){
      analogWrite(A2, 0); 
    }
    if (digitalRead(5) == HIGH) {
        state = RESET_part_1;                                                               //state 5
        prev_state = 0;
    }
    
    if (digitalRead(12) == HIGH) {                                                          //drill limit switch
        if (prev_state == DRILL_LIFT) {
            state = EJECT_TO_HOPPER;
        }
    }

    if (digitalRead(2) == HIGH && pause == 0 && millis() - led_timer > 700) {               //pause/resume function pt 1: if top button (pin 2) is pressed, pause = 1, state set to 'pin_2_high' (led on) until button pressed again
        saved_state = state;
        state = PIN_2_HIGH;
        //Serial.println("are we ever here");
        pause = 1;
        led_timer = millis();
    }
    else if (digitalRead(2) == HIGH && pause == 1 && millis() - led_timer > 700) {          //pause/resume function pt 2: if top button (pin 2) is pressed again, pause = 0, state set back to whatever it was
        if (saved_state == 0 || saved_state == 1)
            state = 0;
        else if (saved_state == 2 || saved_state == 3)
            state = 2;
        else state = saved_state;
        pause = 0;
        led_timer = millis();
    }
    if (digitalRead(10) == HIGH) {                                                          //these next three functions 
        if (press_timer > 0) {
            elapsed_time = millis() - previous_time;
            previous_time = millis();
            press_timer += elapsed_time;
        }
        if (press_timer == 0) {
            press_timer += 1;
            previous_time = millis();
            elapsed_time = 0;
        }
        if (press_timer > 1000) {
            cycles = 0;
        }
    }
    if (digitalRead(10) == LOW) {
        released = 1;
        previous_time = 0;
        press_timer = 0;
        elapsed_time = 0;
    }
    if (digitalRead(10) == HIGH && millis() - cycle_button_timer > 200 && released == 1) {
        if (press_timer < 1000) {
            cycles++;
        }
        released = 0;
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

        case PIN_2_HIGH:
            digitalWrite(13, HIGH);
            if ((machine_running == 0) && (pause == 1)) {
                state = RED_LED_OFF;
            }
            break;

        case RESET_part_1:                                                                                                                  // state 5
            digitalWrite(6, LOW);                                                                                                           //drill direction toward home
            if ((digitalRead(12) == LOW) && (micros() - stepper_timer_03 > hold_step) && (step_half_03 == 0)) {                                   //go until limit switch = HIGH
                digitalWrite(7, HIGH);
                stepper_timer_03 = micros();
                step_half_03 = 1;
            }
            if ((digitalRead(12) == LOW) && (micros() - stepper_timer_03 > hold_step) && (step_half_03 == 1)) {
                digitalWrite(7, LOW);
                stepper_timer_03 = micros();
                step_half_03 = 0;
            }
            if (digitalRead(12) == HIGH) {
              state = RESET_part_15;
            }
            break;

        case RESET_part_15:
            digitalWrite(8, LOW);                                                                                                               //feed direction toward home
            if ((digitalRead(11) == LOW) && (micros() - stepper_timer_04 > hold_step) && (step_half_04 == 0)) {                                       //feed switch not yet reached and not yet time
                digitalWrite(9, HIGH);                                                                                                          //step pin high
                stepper_timer_04 = micros();
                step_half_04 = 1;
            }
            if ((digitalRead(11) == LOW) && (micros() - stepper_timer_04 > hold_step) && (step_half_04 == 1)) {                                       //feed switch not yet reached and not yet time to switch
                digitalWrite(9, LOW);                                                                                                           //step pin low
                stepper_timer_04 = micros();
                step_half_04 = 0;
            }
            if (digitalRead(11) == HIGH) {
            state = RESET_part_2;
            }
            break;
                 
        case RESET_part_2:                                                                  // state 6
            digitalWrite(6, HIGH);                                                          // this moves the drill head away from home
            if (((micros() - stepper_timer_05) > hold_step) && (step_half_05 == 0) && (drill_in_pos == 0)) {
                digitalWrite(7, HIGH);
                stepper_timer_05 = micros();
                step_half_05 = 1;
            }
            if (((micros() - stepper_timer_05) > hold_step) && (step_half_05 == 1) && (drill_in_pos == 0)) {
                digitalWrite(7, LOW);
                stepper_timer_05 = micros();
                step_half_05 = 0;
                count_03 ++;
                if (count_03 == 1670) {
                    count_03 = 0;
                    drill_in_pos = 1;
                }
            }
            digitalWrite(8, HIGH);                                                                                                               // feed direction away from home
            if (((micros() - stepper_timer_06) > hold_step) && (step_half_06 == 0) && (feed_in_pos == 0)) {
                digitalWrite(9, HIGH);
                stepper_timer_06 = micros();
                step_half_06 = 1;
            }
            if (((micros() - stepper_timer_06) > hold_step) && (step_half_06 == 1) && (feed_in_pos == 0)) {
                digitalWrite(9, LOW);
                stepper_timer_06 = micros();
                step_half_06 = 0;
                count_01 ++;
                if (count_01 == 1550) {
                    count_01 = 0;
                    feed_in_pos = 1;
                }
            }
            if ((drill_in_pos == 1) && (feed_in_pos == 1)){
              state = HOPPER_TO_HOLE1;
              drill_in_pos = 0;
              feed_in_pos = 0;
            }
            break;


        case HOPPER_TO_HOLE1:                                                               // feed from hopper to first hole location
            digitalWrite(8, HIGH);                                                          // this moves the feed away from home
            if (((micros() - stepper_timer_07) > hold_step) && (step_half_07 == 0)) {
                digitalWrite(9, HIGH);
                stepper_timer_07 = micros();
                step_half_07 = 1;
                break;
            }
            if (((micros() - stepper_timer_07) > hold_step) && (step_half_07 == 1)) {
                digitalWrite(9, LOW);
                stepper_timer_07 = micros();
                step_half_07 = 0;
                count_04 ++;
                if (count_04 == 5620) {
                    count_04 = 0;
                    state = DRILL_DOWN;
                    prev_state = HOPPER_TO_HOLE1;
                }
            }
            break;

        case DRILL_DOWN:
            digitalWrite(6, HIGH); // this moves the drill head away from home
            if (((micros() - stepper_timer_08) > 2800) && (step_half_08 == 0)) {
                digitalWrite(7, HIGH);
                stepper_timer_08 = micros();
                step_half_08 = 1;
            }
            if (((micros() - stepper_timer_08) > 2800) && (step_half_08 == 1)) {
                digitalWrite(7, LOW);
                stepper_timer_08 = micros();
                step_half_08 = 0;
                count_05 ++;
                if (count_05 == 400) {
                    digitalWrite(A0, HIGH);                                                                                       //turns motor on
                }
                if (count_05 == 1500) {
                    count_05 = 0;
                    state = DRILL_UP;
                }
            }

            break;

        case DRILL_UP:
            digitalWrite(6, LOW); // this moves the drill head toward home
            if (((micros() - stepper_timer_09) > hold_step) && (step_half_09 == 0)) {
                digitalWrite(7, HIGH);
                stepper_timer_09 = micros();
                step_half_09 = 1;
            }
            if (((micros() - stepper_timer_09) > hold_step) && (step_half_09 == 1)) {
                digitalWrite(7, LOW);
                stepper_timer_09 = micros();
                step_half_09 = 0;
                count_06 ++;
                if (count_06 == 700) {
                    digitalWrite(A0, LOW);                                                                                       //turns motor off
                }
                if (count_06 == 1500 && prev_state == HOPPER_TO_HOLE1)  {                                                        //if (count_06 == 1500 && prev_state == HOPPER_TO_HOLE1)
                    count_06 = 0;
                    state = HOLE1_TO_HOLE2;
                }
                if (count_06 == 1500 && prev_state == HOLE1_TO_HOLE2) {
                    count_06 = 0;
                    state = HOLE2_TO_HOLE3;
                }
                if (count_06 == 1500 && prev_state == HOLE2_TO_HOLE3) {
                    count_06 = 0;
                    state = HOLE3_TO_EJECT;
                }
            }
            break;

        case HOLE1_TO_HOLE2:                                                                                                      // first hole location to second
            digitalWrite(8, HIGH);                                                                                                // this moves the feed away from home
            if ((micros() - stepper_timer_10 > hold_step) && (step_half_10 == 0)) {
                digitalWrite(9, HIGH);
                stepper_timer_10 = micros();
                step_half_10 = 1;
            }
            if ((micros() - stepper_timer_10 > hold_step) && (step_half_10 == 1)) {
                digitalWrite(9, LOW);
                stepper_timer_10 = micros();
                step_half_10 = 0;
                count_06 ++;
                if (count_06 == 1000) {
                    count_06 = 0;
                    state = DRILL_DOWN;
                    prev_state = HOLE1_TO_HOLE2;
                }
            }
            break;

        case HOLE2_TO_HOLE3:                                                                // first hole location to second
            digitalWrite(8, HIGH);                                                          // this moves the feed away from home
            if ((micros() - stepper_timer_11 > hold_step) && (step_half_11 == 0)) {
                digitalWrite(9, HIGH);
                stepper_timer_11 = micros();
                step_half_11 = 1;
            }
            if ((micros() - stepper_timer_11 > hold_step) && (step_half_11 == 1)) {
                digitalWrite(9, LOW);
                stepper_timer_11 = micros();
                step_half_11 = 0;
                count_07 ++;
                if (count_07 == 7000) {
                    count_07 = 0;
                    state = DRILL_DOWN;
                    prev_state = HOLE2_TO_HOLE3;
                }
            }
            break;

        case HOLE3_TO_EJECT:
            digitalWrite(8, HIGH);                                                          // feed plate from hole 3 to eject position
            if ((micros() - stepper_timer_12 > hold_step) && (step_half_12 == 0)) {
                digitalWrite(9, HIGH);
                stepper_timer_12 = micros();
                step_half_12 = 1;
            }
            if ((micros() - stepper_timer_12 > hold_step) && (step_half_12 == 1)) {
                digitalWrite(9, LOW);
                stepper_timer_12 = micros();
                step_half_12 = 0;
                count_08 ++;
                if (count_08 == 2000) {
                    count_08 = 0;
                    state = DRILL_LIFT;
                }
            }
            break;

        case DRILL_LIFT:
            digitalWrite(6, LOW);                                                           // switch 15. returns drill head to home to give part clearance to fall away
            if ((digitalRead(12) == LOW) && (micros() - stepper_timer_13 > hold_step) && (step_half_13 == 0)) {
                digitalWrite(7, HIGH);
                stepper_timer_13 = micros();
                step_half_13 = 1;
            }
            if ((digitalRead(12) == LOW) && (micros() - stepper_timer_13 > hold_step) && (step_half_13 == 1)) {
                digitalWrite(7, LOW);
                stepper_timer_13 = micros();
                step_half_13 = 0;
                prev_state = DRILL_LIFT;
                complete_01 = 0;
            }
            break;

        case EJECT_TO_HOPPER:                                                               // switch 16
            digitalWrite(6, HIGH);                                                          // lowers drill head back to ready pos
            if ((complete_01 == 0) && (micros() - stepper_timer_14 > hold_step) && (step_half_14 == 0)) {
                digitalWrite(7, HIGH);
                stepper_timer_14 = micros();
                step_half_14 = 1;
            }
            if ((complete_01 == 0 ) && (micros() - stepper_timer_14 > hold_step) && (step_half_14 == 1)) {
                digitalWrite(7, LOW);
                stepper_timer_14 = micros();
                step_half_14 = 0;
                count_09 ++;
                if (count_09 == 1670) {
                    count_09 = 0;
                    complete_01 = 1;
                }
            }
            digitalWrite(8, LOW);                                                           // feed plate from eject position to hopper
            if ((micros() - stepper_timer_15 > hold_step) && (step_half_15 == 0) && count_10 < 15630) {
                digitalWrite(9, HIGH);
                stepper_timer_15 = micros();
                step_half_15 = 1;
            }
            if ((micros() - stepper_timer_15 > hold_step) && (step_half_15 == 1) && count_10 < 15630) {
                digitalWrite(9, LOW);
                stepper_timer_15 = micros();
                step_half_15 = 0;
                count_10 ++;
            }
            if (count_10 >= 15630 && hopper_pause == 0) {
                    count_10 = 0;
                    stepper_timer_16 = millis();
                    hopper_pause = 1;
            }
            if (cycles > 0 && (millis() - stepper_timer_16 > 2000)) { 
                    cycles --;
                    hopper_pause = 0;
                    state = HOPPER_TO_HOLE1;
            }
            if (cycles == 0) {
                    hopper_pause = 0;
                    state = LED_OFF;
            }
            break;
        
        case RED_LED_OFF:
            analogWrite(A2, 0);
            state = RED_LED_OFF_WAIT;
            break;

        case RED_LED_OFF_WAIT:
            if (count_13 >= 20) {
                state = RED_LED_ON;
                count_13 = 0;
            }
            else if (millis() - count_timer_02 > 20){
                count_13 ++;
                count_timer_02 = millis();
            }
            break;

        case RED_LED_ON:
            if (pause == 1){
                analogWrite(A2, 255);
                Serial.println(state);
                state = RED_LED_ON_WAIT;                
            }
            break;

        case RED_LED_ON_WAIT:
            if (count_13 >= 20) {
                state = RED_LED_OFF;
                count_13 = 0;
            }
            else if (millis() - count_timer_02 > 20) {
              count_13++;
              count_timer_02 = millis();
            }
            break;
            
            

        default:
            break;
    }
}
