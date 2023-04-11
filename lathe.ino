#define ROTATION_CONTROL          12
#define OIL_PRESSURE              A6
#define OIL_FAILURE_LED           13
#define MOTOR_DISABLE_INTERVAL    A4
#define MAIN_MOTOR_PHASE_ONE      2
#define MAIN_MOTOR_PHASE_TWO      3
// #define MAIN_MOTOR_PHASE_THREE       4
#define MAIN_MOTOR_PHASE_STARTER  4
#define OIL_MOTOR                 5
#define FORWARD_RELAY             6
#define REVERSE_RELAY             7
#define BRAKES_RELAY              8
#define ENABLE_FORWARD            9
#define ENABLE_REVERSE            10
#define IS_ON_OFF                 A5
// #define IS_OFF                    12

int starterDuration = 1200;
unsigned long starterEnabledTime = 0;
unsigned long motorDisableTime = 0;
bool isSpinning = false;
bool turnedOn = false;
bool isStarterActive = false;
bool motorRunning = false;
bool enableForward = false;
bool enableReverse = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(ROTATION_CONTROL, INPUT);
  pinMode(ENABLE_FORWARD, INPUT);
  pinMode(ENABLE_REVERSE, INPUT);
  pinMode(MAIN_MOTOR_PHASE_ONE, OUTPUT);
  digitalWrite(MAIN_MOTOR_PHASE_ONE, LOW);
  pinMode(MAIN_MOTOR_PHASE_TWO, OUTPUT);
  digitalWrite(MAIN_MOTOR_PHASE_TWO, LOW);
  // pinMode(MAIN_MOTOR_PHASE_THREE, OUTPUT);
  // digitalWrite(MAIN_MOTOR_PHASE_THREE, LOW);
  pinMode(MAIN_MOTOR_PHASE_STARTER, OUTPUT);
  digitalWrite(MAIN_MOTOR_PHASE_STARTER, LOW);
  pinMode(OIL_MOTOR, OUTPUT);
  digitalWrite(OIL_MOTOR, LOW);
  pinMode(FORWARD_RELAY, OUTPUT);
  digitalWrite(FORWARD_RELAY, LOW);
  pinMode(BRAKES_RELAY, OUTPUT);
  digitalWrite(BRAKES_RELAY, LOW);
  pinMode(REVERSE_RELAY, OUTPUT);
  digitalWrite(REVERSE_RELAY, LOW);
  pinMode(OIL_FAILURE_LED, OUTPUT);
  digitalWrite(OIL_FAILURE_LED, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  long motorDisableInterval;
  int isOnOff, enableDirection, oilPressure, getMotorDisableInterval;

  isSpinning = digitalRead(ROTATION_CONTROL);
  oilPressure = analogRead(OIL_PRESSURE);
  enableForward = digitalRead(ENABLE_FORWARD);
  enableReverse = digitalRead(ENABLE_REVERSE);
  isOnOff = analogRead(IS_ON_OFF);
  // isOff = digitalRead(IS_OFF);
  getMotorDisableInterval = analogRead(MOTOR_DISABLE_INTERVAL);
  getMotorDisableInterval = getMotorDisableInterval >= 200 ? getMotorDisableInterval : 200;
  motorDisableInterval = 60000 * round(getMotorDisableInterval / 200);

Serial.println(oilPressure);

  if (isOnOff <= 5) {
    turnedOn = true;
  } else if (isOnOff > 250) {
    turnedOn = false;  
  }

  if (turnedOn) {
    digitalWrite(OIL_MOTOR, HIGH);
  } else {
    motorRunning = false;

    digitalWrite(OIL_MOTOR, LOW);
    digitalWrite(MAIN_MOTOR_PHASE_STARTER, LOW);
    digitalWrite(MAIN_MOTOR_PHASE_ONE, LOW);
    digitalWrite(MAIN_MOTOR_PHASE_TWO, LOW);
    digitalWrite(FORWARD_RELAY, LOW);
    digitalWrite(REVERSE_RELAY, LOW);
    digitalWrite(BRAKES_RELAY, LOW);

    return;
  }

  // if (oilPressure < 200 || oilPressure > 800) {
  //   motorRunning = false;

  //   digitalWrite(MAIN_MOTOR_PHASE_STARTER, LOW);
  //   digitalWrite(MAIN_MOTOR_PHASE_ONE, LOW);
  //   digitalWrite(MAIN_MOTOR_PHASE_TWO, LOW);
  //   digitalWrite(FORWARD_RELAY, LOW);
  //   digitalWrite(REVERSE_RELAY, LOW);
  //   digitalWrite(BRAKES_RELAY, LOW);
    
  //   digitalWrite(OIL_FAILURE_LED, HIGH);
    
  //   return;
  // } else {
  //   digitalWrite(OIL_FAILURE_LED, LOW);
  // }

  if ((enableForward || enableReverse)) {
    if (!motorRunning) {
      starterEnabledTime = millis();
      isStarterActive = true;
      motorRunning = true;

      digitalWrite(MAIN_MOTOR_PHASE_STARTER, HIGH);
      digitalWrite(MAIN_MOTOR_PHASE_ONE, HIGH);
      digitalWrite(MAIN_MOTOR_PHASE_TWO, HIGH);      
    } else {
      if (motorDisableTime > 0) {
        motorDisableTime = 0;
      }
    }
  }

  if (isStarterActive && int(currentMillis - starterEnabledTime) >= starterDuration) {  
    isStarterActive = false;
    starterEnabledTime = 0;

    digitalWrite(MAIN_MOTOR_PHASE_STARTER, LOW);
  }

  if (enableForward && motorRunning && !isStarterActive) {
    digitalWrite(FORWARD_RELAY, HIGH);
  }

  if (enableReverse && motorRunning && !isStarterActive) {
    digitalWrite(REVERSE_RELAY, HIGH);
  }

  if (!enableForward && !enableReverse) {
    digitalWrite(FORWARD_RELAY, LOW);
    digitalWrite(REVERSE_RELAY, LOW);

    if (isSpinning) {
      digitalWrite(BRAKES_RELAY, HIGH);
    } else {
      digitalWrite(BRAKES_RELAY, LOW);
    }

    if (motorRunning) {
      if (motorDisableTime == 0) {
        motorDisableTime = millis();
      }

      if (long(currentMillis - motorDisableTime) >= motorDisableInterval) {
        digitalWrite(MAIN_MOTOR_PHASE_ONE, LOW);
        digitalWrite(MAIN_MOTOR_PHASE_TWO, LOW);

        motorRunning = false;
        motorDisableTime = 0;
      }
    }
  } 
}
