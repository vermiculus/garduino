/* -*- mode: c -*- */
/* include the library code: */
#include <LiquidCrystal.h>      /* http://www.fibidi.com/?p=734 */
#include <OneWire.h>            /* DS18S20 Temperature */
#include <Stepper.h>            /* FISH FEEDER */

#define STEPS            100 /* Number of steps per revolution stepper motor FISH FEEDER */
#define STEPPER_SPEED    200
#define RELAY_ON           0 /* relay on */
#define RELAY_OFF          1 /* relay off */
#define PIN_PUMP_RELAY_1  22 /* Arduino Digital I/O pin number pump */
#define LightRelay_2      24 /* Arduino Digital I/O pin number lights */
#define PIN_WATER          7 /* DS18S20 Signal pin on digital 7 */
#define PIN_AIR            6 /* DS18S20 Signal pin on digital 6 */

#define STATE_0000_0300    1    /* midnight to 3am      */
#define STATE_0300_0600    2    /*      3am to 6am      */
#define STATE_0600_0900    3    /*      6am to 9am      */
#define STATE_0900_1200    4    /*      9am to noon     */
#define STATE_1200_1500    5    /*     noon to 3pm      */
#define STATE_1500_1800    6    /*      3pm to 6pm      */
#define STATE_1800_2100    7    /*      6pm to 9pm      */
#define STATE_2100_2400    8    /*      9pm to midnight */

#define STATE_PUMP_ON
#define STATE_PUMP_OFF
#define STATE_LIGHTS_ON
#define STATE_LIGHTS_OFF
#define STATE_FEEDER_ON
#define STATE_FEEDER_OFF

#define EX_INVALID_HOUR    0

#define PRINT_TIME_INTERVAL 1000

/* 1-3-2-4 for proper sequencing of stepper motor FISH FEEDER  */
Stepper FishFeedersmall_stepper(STEPS, 10, 12, 11, 13);

LiquidCrystal lcd(9, 8, 5, 4, 3, 2); /* LCD arduino pins set */

long     get_seconds           ( int, int, int );      /* h m s */
long     get_milliseconds      ( int, int, int, int ); /* h m s ms */
void     lcd_display_message   ( char, char * );       /* delim msg */
float    get_temperature       ( int );                /* pin */
int      get_hour              ( unsigned long );      /* ms */
int      get_minute            ( unsigned long );      /* ms */
int      get_second            ( unsigned long );      /* ms */
void     delay_for_time        ( int, int, int, int ); /* h m s ms */
void     set_light             ( int );
void     set_pump              ( int );
void     feed_fish             ( void );
void     exec_state            ( int );  /* takes a time state */
void     throw_exception       ( int ); /* makes necessary print for exception */
int      get_current_state     ( void ); /* gets state from curr time */
void     normalize_time        ( void );
void     maybe_print_time      ( void );
void     lcd_display_welcome   ( void );
void     fmtDouble             ( double val, byte precision, char *buf, unsigned bufLen = 0xffff);
unsigned fmtUnsigned           ( unsigned long val, char *buf, unsigned bufLen = 0xffff, byte width = 0);

int light_state;

int time_state;
long time_offset;               /* TODO: read input from serial to normalize time */
long previous_print_time;
long previous_time;

void setup() {
  /* Serial1.begin(38400); */
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
  delay_for_time(0,0,4,0);
}

/*
  States to control what we want to do

  1: pump out tank
  2: run testing functions
  3: do actual stuff
*/
int func = 1;
void loop() {
  /*
    PUMP OUT TANK
    Unhook hose from back of tank and put it in a bucket.
  */

  maybe_print_time();

  switch (func) {
  case 1:
    digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
    break;
  case 2:
    test();
    func = 3;
    break;
  case 3:
    int next_state = get_current_state();
    if (time_state != next_state) {
      time_state = next_state;
      exec_state(time_state);
    }
  }
}

void test(){
  /* for normal serial communication */
  Serial.begin (38400);

  feed_fish(1500);
  set_light(RELAY_ON);
  delay_for_time(0,0,2,0);
  set_light(RELAY_OFF);
  pump_water(2);
}

void set_light (int new_state) {
  if (light_state != new_state) {
    light_state = new_state;
    digitalWrite(LightRelay_2, new_state);
  }
}
void pump_water (int seconds) {
  int old_light = digitalRead(LightRelay_2);
  digitalWrite(LightRelay_2, RELAY_OFF);
  delay_for_time(0, 0, 0, 500);

  digitalWrite(PIN_PUMP_RELAY_1, RELAY_ON);
  delay_for_time(0, 0, seconds, 0);
  digitalWrite(PIN_PUMP_RELAY_1, RELAY_OFF);

  delay_for_time(0, 0, 0, 500);
  digitalWrite(LightRelay_2, old_light);
}
void feed_fish ( int amount ) {
  FishFeedersmall_stepper.setSpeed(75);

  digitalWrite(LightRelay_2, RELAY_OFF);
  delay_for_time(0,0,1,0);

  FishFeedersmall_stepper.step(-amount);
  delay_for_time(0,2,0,0);
}

/* Output Routines */

/* Prints a two-line `message` to the LCD screen using a line
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
  lcd_display_message(':', "Sean:Allred");
  delay_for_time(0, 0, 5, 0);
  lcd_display_message(':', "Libby:Glasgow");
  delay_for_time(0, 0, 5, 0);
  lcd_display_message(':', "Mary Claire:McCarthy");
  delay_for_time(0, 0, 5, 0);
  lcd_display_message(':', "Alexia:Tanski");
  delay_for_time(0, 0, 5, 0);
  lcd_display_message(':', "James:Sappington");
}

/* Sensor Data Retrieval */

/* Calculates and returns the water temperature as a float. */
float get_temperature ( int pin ) {
  OneWire sensor(pin);
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
  if (pin == PIN_AIR) {
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
  return temperature;
}

/* Utility Functions */

char *get_spaces(int n) {
  return (char*) malloc(n * sizeof(char));
}

long get_seconds(int h, int m, int s) {
  return (h * 60 + m) * 60 + s;
}

long get_milliseconds(int h, int m, int s, int ms) {
  return get_seconds(h, m, s) * 1000 + ms;
}

void delay_for_time(int h, int m, int s, int ms) {
  delay(get_milliseconds(h, m, s, ms));
}

int get_hour   ( unsigned long m ) { return m / 1000 * 60 * 60 ; }
int get_minute ( unsigned long m ) { return m / 1000 * 60      ; }
int get_second ( unsigned long m ) { return m / 1000           ; }

void maybe_print_time( void ) {
  long current_time = millis();
  current_time += time_offset;
  long delta = current_time - previous_time;

  /* Print current time every PRINT_TIME_INTERVAL ms */
  if (delta > PRINT_TIME_INTERVAL) {
    previous_time = current_time;
    Serial.print(get_hour  ( current_time )); Serial.print(':');
    Serial.print(get_minute( current_time )); Serial.print(':');
    Serial.print(get_second( current_time )); Serial.print(':');
    Serial.println(current_time);

    Serial.print("!");
    char *air_temp = (char*) malloc(32*sizeof(char));
    char *water_temp = (char*) malloc(32*sizeof(char));
    fmtDouble(get_temperature(PIN_AIR), 2, air_temp);
    fmtDouble(get_temperature(PIN_AIR), 2, water_temp);
    Serial.print(air_temp);
    Serial.print(water_temp);
    Serial.println();
  }
}

/* Calculates the current time according to the current offset and
   then returns the current state */
int get_current_state( void ) {
  long current_time = millis();
  current_time += time_offset;

  int current_hour = get_hour(current_time);

  switch (current_hour) {
  case  0: case  1: case  2: return STATE_0000_0300;
  case  3: case  4: case  5: return STATE_0300_0600;
  case  6: case  7: case  8: return STATE_0600_0900;
  case  9: case 10: case 11: return STATE_0900_1200;
  case 12: case 13: case 14: return STATE_1200_1500;
  case 15: case 16: case 17: return STATE_1500_1800;
  case 18: case 19: case 20: return STATE_1800_2100;
  case 21: case 22: case 23: return STATE_2100_2400;
  default:
    throw_exception(EX_INVALID_HOUR);
    return -1;
  }
}

void exec_state ( int next_state ) {
  if (time_state == next_state) {
    return;
  }

  switch (next_state) {
  case STATE_0000_0300:
    Serial.println("00:00 :: pump 2 minutes");

    lcd_display_message(':', "Midnight:Pump 2 minutes");

    pump_water(120);

    set_light(RELAY_OFF);
    break;
  case STATE_0300_0600:
    lcd_display_message(':', "Pump 2 minutes:");

    Serial.println("03:00 :: pump 2 minutes");

    set_light(RELAY_OFF);
    pump_water(120);
    break;
  case STATE_0600_0900:
    lcd_display_message(':', "Pump 2 minutes:Lights ON");

    Serial.println("06:00 :: pump 2 minutes; Lights ON; Smile!");

    pump_water(120);
    break;
  case STATE_0900_1200:
    lcd_display_message(':', "Pump 2 minutes:Lights ON");

    Serial.println("09:00 :: Pump 2 minutes; Lights ON");

    pump_water(120);

    set_light(RELAY_ON);
    break;
  case STATE_1200_1500:
    lcd_display_message(':', "Pump 2m; Lights:Lunch time!");

    Serial.println("12:00 :: pump 4 minutes; Lights ON; "
                   "Feed fish: -1500 CCW, LUNCH TIME!!");

    feed_fish(1500);
    pump_water(120);
    break;
  case STATE_1500_1800:
    Serial.println("15:00 :: pump 2 minutes; Lights ON.");

    lcd_display_message(':', "Pump 2 minutes:Lights ON");

    pump_water(120);
    break;
  case STATE_1800_2100:
    lcd_display_message(':', "Pump 2 minutes: Lights ON");

    Serial.println("18:00 :: pump 4 minutes; Lights ON.");

    set_pump(RELAY_ON);
    break;
  case STATE_2100_2400:
    lcd_display_message(':', "Mignight!:Party time!");

    Serial.println("21:00 :: pump 2 minutes; Lights ON.");

    pump_water(120);
    set_light(RELAY_OFF);
    break;
  }
}

/* TODO */
/* Read formatted data from serial */
void normalize_time ( void ) {
  /*
    Read formatted data from serial and parse it.

    Probably should go ISO:

        THH:MM:SS.S

    The T marks the beginning of a time
 */
}

void throw_exception( int exception ) {
  if (exception) {
    Serial.println("An error has occurred!");
    switch (exception) {
    case EX_INVALID_HOUR:
      Serial.println("Invalid Hour!");
      break;
    default:
      Serial.println("Too bad we don't know what it is!");
    }
  }
}

/* `Borrowed' Functions :: Printing Floating-Point Numbers */

/*
  From http://forum.arduino.cc/index.php/topic,44216.0.html#11

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


/*
  Produce a formatted string in a buffer corresponding to the value
  provided.  If the 'width' parameter is non-zero, the value will be
  padded with leading zeroes to achieve the specified width.  The
  number of characters added to the buffer (not including the null
  termination) is returned.
*/
unsigned fmtUnsigned(unsigned long val, char *buf, unsigned bufLen, byte width) {
  if (!buf || !bufLen) {
    return(0);
  }

  /* produce the digit string (backwards in the digit buffer) */
  char dbuf[10];
  unsigned idx = 0;
  while (idx < sizeof(dbuf)) {
    dbuf[idx++] = (val % 10) + '0';
    if ((val /= 10) == 0) {
      break;
    }
  }

  /* copy the optional leading zeroes and digits to the target buffer */
  unsigned len = 0;
  byte padding = (width > idx) ? width - idx : 0;
  char c = '0';
  while ((--bufLen > 0) && (idx || padding))
  {
    if (padding) {
      padding--;
    } else {
      c = dbuf[--idx];
    }
    *buf++ = c;
    len++;
  }

  /* add the null termination */
  *buf = '\0';
  return(len);
}


/* Local Variables: */
/* indent-tabs-mode: nil */
/* truncate-lines: t */
/* End: */
