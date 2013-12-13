/* include the library code: */
#include <LiquidCrystal.h>      /* http://www.fibidi.com/?p=734 */
#include <OneWire.h>            /* DS18S20 Temperature */
#include <Stepper.h>            /* FISH FEEDER */

// TODO: define a function PrintLCD(*char, long) that splits at newline (guaranteeing width) and pauses
// TODO: To avoid using delay() everywhere, use millis(): http://arduino.cc/en/Tutorial/BlinkWithoutDelay

#define STEPS            100 /* Number of steps per revolution stepper motor FISH FEEDER */
#define STEPPER_SPEED    200
#define RELAY_ON           0 /* relay on */
#define RELAY_OFF          1 /* relay off */
#define PIN_PUMP_RELAY_1  22 /* Arduino Digital I/O pin number pump */
#define LightRelay_2      24 /* Arduino Digital I/O pin number lights */

/* 1-3-2-4 for proper sequencing of stepper motor FISH FEEDER  */
Stepper FishFeedersmall_stepper(STEPS, 10, 12, 11, 13);

int Steps2Take;                 /* step variable */
LiquidCrystal lcd(9,8,5,4,3,2); /* LCD arduino pins set */
int Water_Pin = 7;              /* DS18S20 Signal pin on digital 7 */
int Air_Pin   = 6;              /* DS18S20 Signal pin on digital 6 */

/* Temperature chip i/o */
OneWire water(Water_Pin);
OneWire air(Air_Pin);

char sean1   [17] = "Sean            ";
char sean2   [17] = "Allred          ";
char libby1  [17] = "Libby           ";
char libby2  [17] = "Glasgow         ";
char MC1     [17] = "Mary Claire     ";
char MC2     [17] = "McCarthy        ";
char alexia1 [17] = "Alexia          ";
char alexia2 [17] = "Tanski          ";
char james1  [17] = "James           ";
char james2  [17] = "Sappington      ";

long get_seconds         (  int  hours, int minutes, int seconds                   );
long get_milliseconds    (  int  hours, int minutes, int seconds, int milliseconds );
void lcd_display_message ( char  delim, char *msg                                  );
void lcd_display_welcome ( void )

void setup() {
  lcd.begin(16, 2);
  /* calls welcome message with names */
  lcd_display_welcome();

  /* set the speed of the stepper motor */
  FishFeedersmall_stepper.setSpeed(STEPPER_SPEED);
  /* for pH sensor */
  Serial1.begin (38400);
  /* Set Relays OFF */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* Set pins as outputs */
  pinMode(PIN_PUMP_RELAY_1, OUTPUT);
  pinMode(LightRelay_2, OUTPUT);
  /* Check that all relays are inactive at Reset */
  delay(4000);
}

void loop() {
  /*
    PUMP OUT TANK
    Unhook hose from back of tank and put it in a bucket.
  */
  /* delay(5000); */
  if(false) {
    pumpOutTank();
  }
  if(false) {
    test();
  }
  day();
}

void test(){
  /* for normal serial communication */
  Serial.begin (38400);
  fishFeederTest();
  lightTesting();
  pumpTesting();
  pumpLightTesting();
}

void day(){
  theMidnightHour();            /* 00:00-03:00 */
  threeToSixAM();               /* 03:00-06:00 */
  sixToNineAM();                /* 06:00-09:00 */
  nineToNoon();                 /* 09:00-12:00 */
  noonToThree();                /* 12:00-15:00 */
  threeToSixPM();               /* 15:00-18:00 */
  sixToNinePM();                /* 18:00-21:00 */
  nineToMidnight();             /* 21:00-00:00 */
}

void theMidnightHour(){
  Serial.println("The Midnight Hour: 00:00 hours - pump 2 minutes: 120,000ms")
  getWaterTemp();
  getAirTemp();
  LCDtheMidnightHour();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump for 2 minutes */
  LCDTimePump();
  /* set the Relay OFF */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("00:02\n");
  LCDTimeLightsOFF();
  getWaterTemp();
  getAirTemp();
}

void LCDTimeLightsOFF(){
  /*
     int twoMin = 120;
     int sec = 1;
     int minute = 60;
     int hour = 3600;
     15;
  */
  for(int i = get_seconds(2, 59, 50) ; i>=0 ; i--) {
    Serial.println("#");
    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.print("Lights OFF...");
    lcd.setCursor(0,1);
    lcd.print(i);
    delay(1000);
    lcd.clear();
  }
}

void LCDTimeLightsON(){
  /* 
     int twoMin = 120;
     int sec = 1;
     int minute = 60;
     int hour = 3600;
  */
  /* 15; */
  for(int i = get_seconds(2, 59, 50);i>=0;i--){
    Serial.println("*");
    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.print("Lights ON...");
    lcd.setCursor(0,1);
    lcd.print(i);
    delay(1000);
    lcd.clear();
  }
}

void LCDTimePump(){
  /* used to be 12 */
  /* int sec = 1; */
  /*
    int minute = 60; 
    int hour = 3600; 
    int threehoursMinusTwo = 10798;
  */
  for(int i = get_seconds(0, 2, 0); i >= 0; i--) {
    Serial.println("$");
    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.print("Pump ON...");
    lcd.setCursor(0,1);
    lcd.print(i);
    delay(1000);
    lcd.clear();
  }
}

void LCDtheMidnightHour(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("AquaGarduinoMini");
  lcd.setCursor(0,1);
  lcd.print("Day Begins!");
  delay(5000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Midnight");
  lcd.setCursor(0,1);
  lcd.print("Pump 2 minutes");
  delay(2000);
  lcd.clear();
}

void threeToSixAM(){
  getWaterTemp();
  getAirTemp();
  LCDthreeToSixAM();
  Serial.println("/* ---(03:00 hours - pump 2 minutes: 120,000ms)---\n");
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump for 2 minutes */
  LCDTimePump();
  /* set the Relay OFF */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("03:02");
  /* delay(10798000); */
  /* delay ~3 hours */
  LCDTimeLightsOFF();
  getWaterTemp();
  getAirTemp();
}

void LCDthreeToSixAM(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("03:00-06:00");
  lcd.setCursor(0,1);
  lcd.print("Pump 2 minutes");
  delay(2000);
  lcd.clear();
}

void sixToNineAM(){
  getWaterTemp();
  getAirTemp();
  LCDsixToNineAM();
  Serial.println("  /* ---(06:00 hours - pump 2 minutes: 120,000ms, Lights ON, Camera takes picture. )---\n");
  getWaterTemp();
  getAirTemp();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("06:02");
  getWaterTemp();
  getAirTemp();
  /* set the Relay On */
  digitalWrite(LightRelay_2, RELAY_ON);
  /* delay(10798000); */
/* delay ~3 hours */
  LCDTimeLightsON();
}

void LCDsixToNineAM(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("06:00-09:00");
  lcd.setCursor(0,1);
  lcd.print("Pump 2 minutes");
  delay(2000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("06:02-09:00");
  lcd.setCursor(0,1);
  lcd.print("Lights ON");
  delay(2000);
  lcd.clear();
}

void nineToNoon(){
  getWaterTemp();
  getAirTemp();
  LCDnineToNoon();
  Serial.println("---(09:00 - pump 2 minutes: 120,000ms, Lights ON)---\n");
  getWaterTemp();
  getAirTemp();
  /* light off */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("09:02");
  getWaterTemp();
  getAirTemp();
  /* set the Relay On */
  digitalWrite(LightRelay_2, RELAY_ON);
  /* delay(10798000);  lights on for 3 hours */
  /* delay ~3 hours */
  LCDTimeLightsON();
}

void LCDnineToNoon(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("09:00-12:00");
  lcd.setCursor(0,1);
  lcd.print("Pump 2 minutes");
  delay(2000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("06:02-09:00");
  lcd.setCursor(0,1);
  lcd.print("Lights ON");
  delay(2000);
  lcd.clear();
}

void noonToThree(){
  getWaterTemp();
  getAirTemp();
  LCDNoonToThree();
  Serial.println("/* ---(12:00 hours - pump 4 minutes: 120,000ms, Lights ON, Feed fish: -1500 CCW, LUNCH TIME!!. )---\n");
  LCDFishFeeder();
  /* *******************************************FISH FEEDER****************************************************************************** */
  FishFeedersmall_stepper.setSpeed(75);
  /* Rotate CCW...Adjust as seems fit...must be negative (-) integer to turn Fish Feeder Counter Clock Wise */
  Steps2Take = -1500;
  /* *******************************************FISH FEEDER************************************************************************** */
  FishFeedersmall_stepper.step(Steps2Take);
  /* light off at 12 Noon for 2 minutes while pump runs */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("12:02\n");
  getWaterTemp();
  getAirTemp();
  /* set the light Relay On */
  digitalWrite(LightRelay_2, RELAY_ON);
  /* elay(10798000);  lights on for 3 hours */
  /* delay ~3 hours */
  LCDTimeLightsON();
}

void LCDFishFeeder(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Lunch time!!");
  lcd.setCursor(0,1);
  lcd.print("Stepper Motor");
  delay(2000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Takes -1500 CCW");
  lcd.setCursor(0,1);
  lcd.print("at speed 75");
  delay(2000);
  lcd.clear();
}

void LCDNoonToThree(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("12:00-12:02");
  lcd.setCursor(0,1);
  lcd.print("Pump 2 minutes");
  delay(2000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("12:02-15:00");
  lcd.setCursor(0,1);
  lcd.print("Lights ON");
  delay(2000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("12:02 Lunch");
  lcd.setCursor(0,1);
  lcd.print("Feed Fish!!");
  delay(2000);
  lcd.clear();
}

void threeToSixPM(){
  Serial.println("  /* ---(15:00 hours - pump 2 minutes: 120,000ms, Lights ON. )---\n");
  getWaterTemp();
  getAirTemp();
  LCDthreeToSix();
  /* light off at 15:00 for 2 minutes while pump runs */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("15:02\n");
  /* set the Relay On */
  digitalWrite(LightRelay_2, RELAY_ON);
  getWaterTemp();
  getAirTemp();
  /* lay(10798000);  lights on for 3 hours */
  /* delay ~3 hours */
  LCDTimeLightsON();
}

void LCDthreeToSix(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("15:00-15:02");
  lcd.setCursor(0,1);
  lcd.print("Pump 2 minutes");
  delay(2000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("15:02-18:00");
  lcd.setCursor(0,1);
  lcd.print("Lights ON");
  delay(2000);
  lcd.clear();
}

void sixToNinePM(){
  getWaterTemp();
  getAirTemp();
  LCDsixToNinePM();
  Serial.println("/* ---(18:00 hours - pump 4 minutes: 120,000ms, Lights ON. )---\n");
  getWaterTemp();
  getAirTemp();
  /* light off at 18:00 for 2 minutes while pump runs */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  /* set the Relay OFF */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("18:02");
  /* set the Relay On */
  digitalWrite(LightRelay_2, RELAY_ON);
  getWaterTemp();
  getAirTemp();
  /* elay(10798000);  lights on for 3 hours */
  /* delay ~3 hours */
  LCDTimeLightsON();
}

void LCDsixToNinePM(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("18:00-18:02");
  lcd.setCursor(0,1);
  lcd.print("Pump 2 minutes");
  delay(2000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("18:02-21:00");
  lcd.setCursor(0,1);
  lcd.print("Lights ON");
  delay(2000);
  lcd.clear();
}

void nineToMidnight(){
  getWaterTemp();
  getAirTemp();
  LCDnineToMidnight();
  Serial.println("  /* ---(21:00 hours - pump 2 minutes: 120,000ms, Lights ON. Lights OFF at 00:00)---\n");
  getWaterTemp();
  getAirTemp();
  /* light off */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  Serial.println("21:02");
  getWaterTemp();
  getAirTemp();
  /* set the Relay OFF */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  /* set the Relay ON */
    digitalWrite(LightRelay_2, RELAY_ON);
    /* lay(10798000); lights on for 3 hours */
    /* delay ~3 hours */
    LCDTimeLightsON();
     /* ---(00:00 hours - Lights OFF. )--- */
/* set the Relay OFF */
  digitalWrite(LightRelay_2, RELAY_OFF);
}

void LCDnineToMidnight(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("12:00-12:02");
  lcd.setCursor(0,1);
  lcd.print("Pump 2 minutes");
  delay(2000);
  lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Midnight!!");
  lcd.setCursor(0,1);
  lcd.print("Party Time!!");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDTesting(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Testing, testing.");
  lcd.setCursor(0,1);
  lcd.print("Unit testing");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDFishFeederTestON(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Feeder Stepper.");
  lcd.setCursor(0,1);
  lcd.print("Motor testing");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDFishFeederTestOFF(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("-100 steps CCW");
  lcd.setCursor(0,1);
  lcd.print("100 steps CW");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDLightTestON(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Lights ON");
  lcd.setCursor(0,1);
  lcd.print("2.5 seconds");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDLightTestOFF(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Lights OFF");
  lcd.setCursor(0,1);
  lcd.print("2.5 seconds");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDPumpTestON(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Pump ON");
  lcd.setCursor(0,1);
  lcd.print("2.5 seconds");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDPumpTestOFF(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Pump OFF");
  lcd.setCursor(0,1);
  lcd.print("2.5 seconds");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDLightsPumpTestON(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Lights on 120V");
  lcd.setCursor(0,1);
  lcd.print("Pump on 5V");
  delay(2000);
  lcd.clear();
}
/* prints to LCD */
void LCDLightsPumpTestOFF(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Pump R_1 OFF");
  lcd.setCursor(0,1);
  lcd.print("Lights R_2 OFF");
  delay(2000);
  lcd.clear();
}
/* tests for fish feeder pins 10-12-11-13 */
void fishFeederTest(){
  LCDTesting();
  LCDFishFeederTestON();
  Serial.println("Testing, testing.\n");
  FishFeedersmall_stepper.setSpeed(75);
  /* Rotate CCW */
  Steps2Take = -100;
  FishFeedersmall_stepper.step(Steps2Take);
  Serial.println("Fish Feeder takes -100 steps Counter Clock Wise.\n");
  delay(2500);
  FishFeedersmall_stepper.setSpeed(75);
  /* Rotate CW */
  Steps2Take = 100;
  FishFeedersmall_stepper.step(Steps2Take);
  LCDFishFeederTestOFF();
  Serial.println("Fish Feeder takes 100 steps Clock Wise.\n");
  delay(2500);
}
/* tests lights */
void lightTesting(){
  LCDLightTestON();
  /* set the Relay On */
  digitalWrite(LightRelay_2, RELAY_ON);
  Serial.println("Lights ON for 2.5 seconds.\n");
  delay(2500);
  lcd.begin(16,2);
  LCDLightTestOFF();
  /* set the Relay On */
  digitalWrite(LightRelay_2, RELAY_OFF);
  Serial.println("Lights OFF.\n");
  delay(2500);
}
/* tests pump */
void pumpTesting(){
  LCDPumpTestON();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  Serial.println("Pump ON for 2.5 seconds.\n");
  delay(2500);
  lcd.begin(16,2);
  /* set the Relay OFF */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("Pump OFF.\n");
  LCDPumpTestOFF();
  delay(2500);
}

void pumpLightTesting(){
  LCDLightsPumpTestON();
  /* set the Relay ON */
  digitalWrite(LightRelay_2, RELAY_ON);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  Serial.println("Pump and Light ON together for 5 seconds.\n");
  delay(5000);
  /* set the Relay OFF */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay OFF */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("Pump and Light OFF.\n");
  LCDLightsPumpTestOFF();
  delay(2500);
}

void pumpOutTank(){
  /* set the Relay On */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 10 minutes; pump out tank */
  delay(600000000);
}

/* Print a Welcome message to the lcd with names of group members */
void LCDSetUp(){
  Serial.begin(38400);
  lcd.setCursor(0,0);
  lcd.print("Welcome to the ");
  lcd.setCursor(0,1);
  lcd.print("AquaGarduinoMini! ");
  delay(5000);
  lcd.clear();
  Serial.println("Welcome to the AquaGarduinoMini!\n");
  Serial.println("Begining setup, please wait.\n");
  delay(5000);
  lcd.setCursor(0, 0);
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 15; thisChar++) {
    lcd.print(sean1[thisChar]);
    Serial.println(".\n");
    delay(75);
    Serial.println(".\n");
  }
  Serial.println("I present Sean Allred!!\n");
  /* set the cursor to (16,1): */
  lcd.setCursor(16,1);
  /* set the display to automatically scroll: */
  lcd.autoscroll();
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 16; thisChar++) {
    lcd.print(sean2[thisChar]);
    delay(250);
  }
  /* turn off automatic scrolling */
  lcd.noAutoscroll();
  /* clear screen for the next loop: */
  lcd.clear();
  lcd.setCursor(0, 0);
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 15; thisChar++) {
    lcd.print(libby1[thisChar]);
    Serial.println(".\n");
    delay(75);
    Serial.println(".\n");
  }
  Serial.println("I present Libby Glasgow!!\n");
  /* set the cursor to (16,1): */
  lcd.setCursor(16,1);
  /* set the display to automatically scroll: */
  lcd.autoscroll();
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 16; thisChar++) {
    lcd.print(libby2[thisChar]);
    delay(250);
  }
  /* turn off automatic scrolling */
  lcd.noAutoscroll();
  /* clear screen for the next loop: */
  lcd.clear();
  lcd.setCursor(0, 0);
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 15; thisChar++) {
    lcd.print(MC1[thisChar]);
    Serial.println(".\n");
    delay(75);
    Serial.println(".\n");
  }
  Serial.println("I present M.C. McCarthy!!\n");
  /* set the cursor to (16,1): */
  lcd.setCursor(16,1);
  /* set the display to automatically scroll: */
  lcd.autoscroll();
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 16; thisChar++) {
    lcd.print(MC2[thisChar]);
    delay(250);
  }
  /* turn off automatic scrolling */
  lcd.noAutoscroll();
  /* clear screen for the next loop: */
  lcd.clear();
  lcd.setCursor(0, 0);
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 15; thisChar++) {
    lcd.print(alexia1[thisChar]);
    Serial.println(".\n");
    delay(75);
    Serial.println(".\n");
  }
  Serial.println("I present Alexia Tanski!!\n");
  /* set the cursor to (16,1): */
  lcd.setCursor(16,1);
  /* set the display to automatically scroll: */
  lcd.autoscroll();
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 16; thisChar++) {
    lcd.print(alexia2[thisChar]);
    delay(250);
  }
  /* turn off automatic scrolling */
  lcd.noAutoscroll();
  /* clear screen for the next loop: */
  lcd.clear();
  lcd.setCursor(0, 0);
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 15; thisChar++) {
    lcd.print(james1[thisChar]);
    Serial.println(".\n");
    delay(75);
    Serial.println(".\n");
  }
  Serial.println("I present James Sappington!!\n");
  /* set the cursor to (16,1): */
  lcd.setCursor(16,1);
  /* set the display to automatically scroll: */
  lcd.autoscroll();
  /* print from 0 to 9: */
  for (int thisChar = 0; thisChar < 16; thisChar++) {
    lcd.print(james2[thisChar]);
    delay(250);
  }
  for(int i =0; i<6;i++){
    Serial.println(".\n");
  }
  /* turn off automatic scrolling */
  lcd.noAutoscroll();
  /* clear screen for the next loop: */
  lcd.clear();
}

void getAirTemp() {
  /* bildr.org/2011/07/ds18b20-arduino/"}}{\fldrslt{\ul\cf1 http:bildr.org/2011/07/ds18b20-arduino/}}}\f0\fs22  prints the */
  /* adapted from {\field{\*\fldinst{HYPERLINK "http:    air temperature from one DS18S20 in degrees Celsius and
     Fahrenheit */
  Serial1.begin (38400);
  float AirTemperatureSum;
  byte data[12];
  byte addr[8];
  if (!air.search(addr)) {
    /* no more sensors on chain, reset search */
    air.reset_search();
    AirTemperatureSum = -1000;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!\n");
    AirTemperatureSum = -1000;
  }
  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized\n");
    AirTemperatureSum = -1000;
  }
                 /* start conversion, with parasite power on at the end */
                 air.write(0x44, 1);
  air.reset();
  air.select(addr);
                 /* start conversion, with parasite power on at the end */
                 air.write(0x44, 1);
  byte present = air.reset();
  air.select(addr);
  /* Read Scratchpad */
  air.write(0xBE);
  /* we need 9 bytes */
  for (int j = 0; j < 9; j++) {
    data[j] = air.read();
  }
  air.reset_search();
  byte MSB = data[1];
  byte LSB = data[0];
  /* using two's complement */
  float tempRead = ((MSB << 8)| LSB);
  AirTemperatureSum = tempRead / 16;
  /* getAirTemp(); */
  float AirTemperatureCelsius = AirTemperatureSum;
  float AirTemperatureFahrenheit = ((AirTemperatureCelsius * 9) / 5) + 32;
  /*
    Serial.print("Air Temp: \n");
    Serial.print(AirTemperatureCelsius);
    Serial.println(" degrees (C)");
    /* Multiply by 9, then divide by 5, then add 32
    Serial.print(AirTemperatureFahrenheit);
    Serial.println(" degrees (F)\n");
  */
  Serial.print("!");
  Serial.print(AirTemperatureCelsius);
  Serial.print(" ");
                                /* prints to LCD */
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Air Temp (C)");
  lcd.setCursor(0,1);
  lcd.print(AirTemperatureCelsius);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Air Temp (F)");
  lcd.setCursor(0,1);
  lcd.print(AirTemperatureFahrenheit);
  delay(2000);
  lcd.clear();
}

/* returns the temperature from one DS18S20 in degree Celsius and Fahrenheit */
void getWaterTemp() {
  Serial1.begin (38400);
  float WaterTemperatureSum;
  byte data[12];
  byte addr[8];
  if (!water.search(addr)) {
    /* no more sensors on chain, reset search */
    water.reset_search();
    WaterTemperatureSum = -1000;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!\n");
    WaterTemperatureSum = -1000;
  }
  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized.\n");
    WaterTemperatureSum = -1000;
  }
  water.reset();
  water.select(addr);
  /* start conversion, with parasite power on at the end */
  water.write(0x44, 1);
  byte present = water.reset();
  water.select(addr);
  /* Read Scratchpad */
  water.write(0xBE);
  /* we need 9 bytes */
  for (int i = 0; i < 9; i++) {
    data[i] = water.read();
  }
  water.reset_search();
  byte MSB = data[1];
  byte LSB = data[0];
  /* using two's complement */
  float tempRead = ((MSB << 8)| LSB);
  WaterTemperatureSum = tempRead / 16;
  /* getWaterTemp(); */
  float WaterTemperatureCelsius = WaterTemperatureSum;
  float WaterTemperatureFahrenheit = ((WaterTemperatureCelsius * 9) / 5) + 32;
  /* Serial.print("Water Temp: \n"); */
  Serial.println(WaterTemperatureCelsius);
  /*
    Serial.println(" degrees (C)");
    Serial.print(WaterTemperatureFahrenheit);  Multiply by 9, then divide by 5, then add 32
    Serial.println(" degrees (F)\n");
    /* prints to LCD
  */
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Water Temp (C)");
  lcd.setCursor(0,1);
  lcd.print(WaterTemperatureCelsius);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Water Temp (F)");
  lcd.setCursor(0,1);
  lcd.print(WaterTemperatureFahrenheit);
  delay(2000);
  lcd.clear();
}
}
long get_seconds(int h, int m, int s) {
  return (h*60+m)*60+s;
}
long get_milliseconds(int h, int m, int s, int ms) {
  return get_seconds(h, m, s) * 1000 + ms;
}

char *get_spaces(int n) {
  return malloc(n*sizeof(char))
}

/* Prints a two-line `message` to the LCD screen using a lime
   delimiter `delim`. */
void lcd_display_message(char delim, char *message) {
  char *line_1 = get_spaces(17);
  char *line_2 = get_spaces(17);

  /*
    Set the cell of the first line to the character currently pointed
    to by `message`.  When the statement finished, both the current
    column and the `message` are incremented.  (Note that incrementing
    `message` will cause `message` to point to the next character in
    the screen.  The next is the same as previous, but checking
    against EOF (null terminator used by default in C strings) instead
    of newline.
  */
  int col = 0;
  while(*message != delim) {
    line_1[col] = *message;

    col     += 1;
    message += 1;
  }
  col = 0;
  message++;
  while(*message != '\0') {
    line_2[col] = *message;

    col     += 1;
    message += 1;
  }

  /* Clear the screen so we can write cleanly */
  lcd.clear();

  /* Set the insertion point at row=0, col=0 and print `line_1` */
  lcd.setCursor(0, 0);
  lcd.print(line_1);
  /* Set the insertion point at row=0, col=1 and print `line_2` */
  lcd.setCursor(0, 1);
  lcd.print(line_2);

  /* Since these lines were returned by `get_spaces` which uses
     `malloc`, they must be `free`d. */
  free(line_1);
  free(line_2);
}

void lcd_display_welcome() {
  const long DELAY_TIME = get_seconds(0, 0, 5);
  lcd_display_message(':', "Sean:Allred");
  delay(DELAY_TIME);
  lcd_display_message(':', "Libby:Glasgow");
  delay(DELAY_TIME);
  lcd_display_message(':', "Mary Claire:McCarthy");
  delay(DELAY_TIME);
  lcd_display_message(':', "Alexia:Tanski");
  delay(DELAY_TIME);
  lcd_display_message(':', "James:Sappington");
}

/* Local Variables: */
/* indent-tabs-mode: nil */
/* End: */
