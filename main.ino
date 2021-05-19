/**
 * @author Mostafa Wael
 * @brief 
 * Controlling a uniploar stepper motor with 200 steps per revolution (SPR).
 * get Speed form the serial monitor.
 * get Direction from the switch.
 * get Steps from the potentiometer.
 * @details 
 * In the code I'm not using the stepper library as it causes unnecessary overhead.
 * Instead, I'm dealing with the motor directly.
 */
//*********************************************
// related to the switch and motor's direction.
#define SWITCH_PIN 3
int Direction = 0; // decide the direction of the motor from the switch (initial value is 0 -clock wise-)
//*********************************************
// related to the potentiometer and motor's number of steps.
#define POTENTIOMETER_PIN A0
unsigned int stepsToMove;
//*********************************************
// related to the motor's movement
#define MOTOR_PIN_1 10
#define MOTOR_PIN_2 11
#define MOTOR_PIN_3 12
#define MOTOR_PIN_4 13
#define stepsPerRevolution 200  // total number of steps the motor can due per revolution (motor charactristic)
int currentStep = 0;            // motor's current step (position)
unsigned long stepDelay;        // delay between steps
unsigned long lastStepTime = 0; // time stamp in us of the last step taken
long Speed = 10;                // decide the speed (RPM) of the motor from the potentiometer, initial value is 10
//*********************************************
void setup()
{
  Serial.begin(9600); // Inisialize the serial to do the monitoring
  Serial.println("Configure motor's pins...");
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_PIN_3, OUTPUT);
  pinMode(MOTOR_PIN_4, OUTPUT);
  Serial.println("Configure the switch...");
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), toggleDirection, CHANGE); // use interrupt to avoid polling (pin 2 or 3 only)
}
//*********************************************
void loop()
{
  Serial.println("Enter Speed to move the motor:");
  if (Serial.available())
  {
    // set speed
    Speed = Serial.parseInt(); // read serial as int variable
    setSpeed(Speed);
    Serial.print("Speed: ");
    Serial.println(Speed);
    // set steps to move
    stepsToMove = map(analogRead(POTENTIOMETER_PIN), 0, 1024, 0, stepsPerRevolution); // map the analog values to the number of steps the motor can take per revolution
    Serial.print("stepsToMove: ");
    Serial.println(stepsToMove);
    makeStep(stepsToMove);
    // set direction
    Serial.print("Direction: ");
    Serial.println(Direction);
  }
  // add a delay just to show the motor's movement, it's not good to add a delay and there are way more options but, it's acceptable here for the sake of testing
  // delay(2000);
}
//*********************************************
/**
 * @brief This function is called when the value of the switch changes using an interrupt to avoid polling,
 * it is responsible for toggling the durection of motor's movement
 * 
 * @return void
 * 
 */
void toggleDirection()
{
  Serial.println("Toggle Direction!!");
  Direction ^= 1; // toggle direction
}
//*********************************************
/**
 * @brief This function is responsibel for seting the speed of the motor by setting the proper delay,
 *  using an equation that was taken from the arduino refernce
 * 
 * @param rpm which is the number of revolution per minute
 * 
 * @return void
 */
//*********************************************
void setSpeed(unsigned long rpm)
{
  stepDelay = 60L * 1000L * 1000L / stepsPerRevolution / rpm;
}
//*********************************************
/**
 * @brief this function is responsible for moving the motor by passing the number of steps to move
 * 
 * @param stepsToMove number of steps to move
 * 
 * @return void
 */
//*********************************************
void makeStep(int stepsToMove) // number of steps to make
{
  while (stepsToMove) // while there are steps left
  {
    unsigned long now = micros(); // get the new time

    if (now - lastStepTime >= stepDelay) // leave a stepDelay time stamp between each two steps
    {
      lastStepTime = now; // get the timeStamp of when you stepped
      // increment or decrement depending on direction
      if (currentStep == 0) // check for the boundaries
        currentStep = stepsPerRevolution;

      currentStep += -1 + Direction * 2; // increment if Direction == 1, decrement if Direction == 0, use the equation to achieve branchless programming

      if (currentStep == stepsPerRevolution) // check for the boundaries
        currentStep = 0;

      // decrement the steps left
      stepsToMove--;
      // step(move) the motor to step number 0, 1, 2, 3
      stepMotor(currentStep % 4);
    }
  }
}
//*********************************************
/**
 * @brief this function is responsible for moving the motor
 * 
 * @param thisStep the value of the currentStep, since the it's a four wired motor, it has 4 type sof steps
 * 
 * @return void
 */
void stepMotor(int thisStep)
{
  switch (thisStep)
  {
  case 0: // 1010
    digitalWrite(MOTOR_PIN_1, HIGH);
    digitalWrite(MOTOR_PIN_2, LOW);
    digitalWrite(MOTOR_PIN_3, HIGH);
    digitalWrite(MOTOR_PIN_4, LOW);
    break;
  case 1: // 0110
    digitalWrite(MOTOR_PIN_1, LOW);
    digitalWrite(MOTOR_PIN_2, HIGH);
    digitalWrite(MOTOR_PIN_3, HIGH);
    digitalWrite(MOTOR_PIN_4, LOW);
    break;
  case 2: //0101
    digitalWrite(MOTOR_PIN_1, LOW);
    digitalWrite(MOTOR_PIN_2, HIGH);
    digitalWrite(MOTOR_PIN_3, LOW);
    digitalWrite(MOTOR_PIN_4, HIGH);
    break;
  case 3: //1001
    digitalWrite(MOTOR_PIN_1, HIGH);
    digitalWrite(MOTOR_PIN_2, LOW);
    digitalWrite(MOTOR_PIN_3, LOW);
    digitalWrite(MOTOR_PIN_4, HIGH);
    break;
  }
}
