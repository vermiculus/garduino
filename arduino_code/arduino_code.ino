-*- mode: c -*-
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
#define PIN_WATER          7 /* DS18S20 Signal pin on digital 7 */
#define PIN_AIR            6 /* DS18S20 Signal pin on digital 6 */

#define _0000_0300_        1    /* midnight to 3am      */
#define _0300_0600_        2    /*      3am to 6am      */
#define _0600_0900_        3    /*      6am to 9am      */
#define _0900_1200_        4    /*      9am to noon     */
#define _1200_1500_        5    /*     noon to 3pm      */
#define _1500_1800_        6    /*      3pm to 6pm      */
#define _1800_2100_        7    /*      6pm to 9pm      */
#define _2100_2400_        8    /*      9pm to midnight */

/* 1-3-2-4 for proper sequencing of stepper motor FISH FEEDER  */
Stepper FishFeedersmall_stepper(STEPS, 10, 12, 11, 13);

LiquidCrystal lcd(9,8,5,4,3,2); /* LCD arduino pins set */

/* Temperature chip i/o */
OneWire water(PIN_WATER);
OneWire air(PIN_AIR);

long  get_seconds           ( int, int, int );
long  get_milliseconds      ( int, int, int, int );
void  lcd_display_message   ( char, char * );
void  lcd_display_welcome   ( void );
float get_air_temperature   ( void );
float get_water_temperature ( void );
int   get_hour              ( unsigned long );
int   get_minute            ( unsigned long );
int   get_second            ( unsigned long );
void  print_time            ( void );

short state;

void setup() {
  Serial1.begin(38400);
  pinMode(PIN_PUMP_RELAY_1, OUTPUT);
  pinMode(LightRelay_2    , OUTPUT);

  lcd.begin(16, 2);

  /* set the speed of the stepper motor */
  FishFeedersmall_stepper.setSpeed(STEPPER_SPEED);

  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  digitalWrite(LightRelay_2    , RELAY_OFF);

  /* calls welcome message with names */
  lcd_display_welcome();

  /* Ensure that all relays are inactive at Reset */
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

  print_time (&current_time);

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
  Serial.println("The Midnight Hour: 00:00 hours - pump 2 minutes: 120,000ms");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  LCDtheMidnightHour();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump for 2 minutes */
  LCDTimePump();
  /* set the Relay OFF */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("00:02\n");
  LCDTimeLightsOFF();
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  LCDsixToNineAM();
  Serial.println("  /* ---(06:00 hours - pump 2 minutes: 120,000ms, Lights ON, Camera takes picture. )---\n");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("06:02");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  LCDnineToNoon();
  Serial.println("---(09:00 - pump 2 minutes: 120,000ms, Lights ON)---\n");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  /* light off */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("09:02");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  LCDNoonToThree();
  Serial.println("/* ---(12:00 hours - pump 4 minutes: 120,000ms, Lights ON, Feed fish: -1500 CCW, LUNCH TIME!!. )---\n");
  LCDFishFeeder();
  /* *******************************************FISH FEEDER****************************************************************************** */
  FishFeedersmall_stepper.setSpeed(75);
  /* Rotate CCW...Adjust as seems fit...must be negative (-) integer to turn Fish Feeder Counter Clock Wise */
  /* *******************************************FISH FEEDER************************************************************************** */
  FishFeedersmall_stepper.step(-1500);
  /* light off at 12 Noon for 2 minutes while pump runs */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);
  Serial.println("12:02\n");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  LCDsixToNinePM();
  Serial.println("/* ---(18:00 hours - pump 4 minutes: 120,000ms, Lights ON. )---\n");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  LCDnineToMidnight();
  Serial.println("  /* ---(21:00 hours - pump 2 minutes: 120,000ms, Lights ON. Lights OFF at 00:00)---\n");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
  /* light off */
  digitalWrite(LightRelay_2, RELAY_OFF);
  /* set the Relay ON */
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  /* pump 2 minutes */
  LCDTimePump();
  Serial.println("21:02");
  get_temperature(PIN_WATER);
  get_temperature(PIN_AIR);
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
  FishFeedersmall_stepper.step(-100);
  Serial.println("Fish Feeder takes -100 steps Counter Clock Wise.\n");
  delay(2500);
  FishFeedersmall_stepper.setSpeed(75);
  /* Rotate CW */
  FishFeedersmall_stepper.step(100);
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

/* Output Routines */

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

/* Sensor Data Retrieval */

/* Calculates and returns the water temperature as a float. */
float get_temperature ( OneWire sensor ) {
  Serial1.begin (38400);
  float temperature;
  byte data[12];
  byte addr[8];
  if (!sensor.search(addr)) {
    /* no more sensors on chain, reset search */
    sensor.reset_search();
    temperature = -1000;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!\n");
    temperature = -1000;
  }
  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized.\n");
    temperature = -1000;
  }
  if (sensor == air) {
    sensor.write(0x44, 1); /// TODO: Why?
  }
  sensor.reset();
  sensor.select(addr);
  /* start conversion, with parasite power on at the end */
  sensor.write(0x44, 1);
  byte present = sensor.reset();
  sensor.select(addr);
  /* Read Scratchpad */
  sensor.write(0xBE);
  /* we need 9 bytes */
  for (int i = 0; i < 9; i++) {
    data[i] = sensor.read();
  }
  sensor.reset_search();
  byte MSB = data[1];
  byte LSB = data[0];
  /* using two's complement */
  float tempRead = ((MSB << 8)| LSB);
  temperature = tempRead / 16;
  /* get_temperature(PIN_WATER); */
  return temperature;
}

/* Utility Functions */

char *get_spaces(int n) {
  return malloc(n*sizeof(char))
}

long get_seconds(int h, int m, int s) {
  return (h*60+m)*60+s;
}

long get_milliseconds(int h, int m, int s, int ms) {
  return get_seconds(h, m, s) * 1000 + ms;
}

enum STATE {
};

void update_time(Time *time) {
  unsigned long m = millis();
  time->hour = m / 1000*60*60;
  m = m % 1000*60*60;
  time->minute = m / 1000*60;
  m = m % 1000*60;
  time->second = m / 1000;
  m = m % 1000;
  time->millisecond = m;
}

int get_hour   ( unsigned long m ) { return m / 1000 * 60 * 60 }
int get_minute ( unsigned long m ) { return m / 1000 * 60      }
int get_second ( unsigned long m ) { return m / 1000           }

void print_time( void ) {
  unsigned long m = millis();
  Serial.print(get_hour(m)); Serial.print(':');
  Serial.print(get_minute(m)); Serial.print(':');
  Serial.print(get_second(m)); Serial.print(':');
  Serial.print(m);
}

/*
  Format a floating point value with number of decimal places.  The
  `precision` parameter is a number from 0 to 6 indicating the desired
  decimal places.  The `buf` parameter points to a buffer to receive
  the formatted string.  This must be sufficiently large to contain
  the resulting string.  The buffer's length may be optionally
  specified.  If it is given, the maximum length of the generated
  string will be one less than the specified value.
  
  example:

      fmtDouble(3.1415, 2, buf);

  will have `buf` be '3.14'.
*/
void fmtDouble(double val, byte precision, char *buf, unsigned bufLen) {
  if (!buf || !bufLen) {
    return;
  }
  
  /* limit the precision to the maximum allowed value */
  const byte maxPrecision = 6;
  if (precision > maxPrecision) {
    precision = maxPrecision;
  }

  if (--bufLen > 0) {
    /* check for a negative value */
    if (val < 0.0) {
      val = -val;
      *buf = '-';
      bufLen--;
    }

    /* compute the rounding factor and fractional multiplier */
    double roundingFactor = 0.5;
    unsigned long mult = 1;
    for (byte i = 0; i < precision; i++) {
      roundingFactor /= 10.0;
      mult *= 10;
    }

    if (bufLen > 0) {
      /* apply the rounding factor */
      val += roundingFactor;

      /* add the integral portion to the buffer */
      unsigned len = fmtUnsigned((unsigned long)val, buf, bufLen);
      buf += len;
      bufLen -= len;
    }

    /* handle the fractional portion */
    if ((precision > 0) && (bufLen > 0)) {
      *buf++ = '.';
      if (--bufLen > 0) {
        buf += fmtUnsigned((unsigned long)((val - (unsigned long)val) * mult), buf, bufLen, precision);
      }
    }
  }

  /* null-terminate the string */
  *buf = '\0';
}


/* Local Variables: */
/* indent-tabs-mode: nil */
/* End: */
