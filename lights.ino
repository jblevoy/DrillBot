void setup() {
    Serial.begin(9600);
    pinMode(2, INPUT);        //upper button
    pinMode(5, INPUT);        // lower button - reset and run
    pinMode(6, OUTPUT);      //drill stepper dir
    pinMode(7, OUTPUT);      //drill stepper step
    pinMode(11, INPUT);       //feed limit switch
    pinMode(12, INPUT);       //drill limit switch
    pinMode(13, OUTPUT);        //board mount button
    pinMode(A0, OUTPUT);       //this shouldn't work
    pinMode(A2, OUTPUT);
}


void loop() {

    analogWrite(A0, 255);
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
   /* if (digitalRead(2) == HIGH) {
        Serial.println("upper button pressed");
        analogWrite
    }
    if (digitalRead(5) == HIGH) {
        Serial.println("lower button pressed");
    }
    if (digitalRead(10) == HIGH) {
        Serial.println("board mount pressed");
    }
    if (digitalRead(11) == HIGH) {
        Serial.println("feed limit reached");
    }
    if (digitalRead(12) == HIGH) {
        Serial.println("drill limit reached");
    }
    */
}
