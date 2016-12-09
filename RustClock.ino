
#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68


int buttonCountLimit;

int buttonCountMinute;
int buttonCount10Minute;
int buttonCountHour;

int secBuffer; 

int state = 0;

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  
  buttonCountLimit = 20;
  buttonCountMinute = 0;
  buttonCount10Minute = 0;
  buttonCountHour = 0;

  
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  //setDS3231time(30,42,21,4,5,12,16);

  // 3 buttons, bottom to top.
  pinMode(13, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);

  // top 2 buttons - not used initially, but feel free to wire them up to something...
  pinMode(10, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

  // 3 switches, I don't know what order so figure it out, smartie pants. 
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  // ten minute lights
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(30, OUTPUT);

  // minute lights
  pinMode(23, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(31, OUTPUT);

  // hour lights
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(38, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(43, OUTPUT);

}

// I didn't write this part. I copied it from 
// http://tronixstuff.com/2014/12/01/tutorial-using-ds1307-and-ds3231-real-time-clock-modules-with-arduino/
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,&year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(year, DEC);


  Serial.print(" ");
  Serial.println(digitalRead(13));

}


void evalButtonCount(int buttonChannel, int &buttonCount, bool &increment)
{
  increment = false;

  int val = 1;
  val = digitalRead(buttonChannel);
  
  if (val == 0) 
  {
    buttonCount += 1;
  } else {
    // We want to reset it so we don't add up 3 half button presses to get to 1 press.
    buttonCount = 0;
  }

  if (buttonCount > buttonCountLimit)
  {
    buttonCount = 0;
    increment = true;
  }  

}


void loop()
{

  bool incrementMinute; 
  bool increment10Minute; 
  bool incrementHour; 

  incrementMinute = false;
  increment10Minute = false; 
  incrementHour = false;

  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231 every time - We need to know if we need to change the time or not.
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  

  // evaluate to see if the set button has been pressed a few times. If it has been pressed a bunch, then 
  // we want to increment the unit for that button. 
  evalButtonCount(13, buttonCountMinute, incrementMinute);
  evalButtonCount(12, buttonCount10Minute, increment10Minute);
  evalButtonCount(11, buttonCountHour, incrementHour);

  displayTime(); // display the real-time clock data on the Serial Monitor

  setHourLights(hour);
  set10MinuteLights(minute);
  setMinuteLights(minute);


  if (incrementMinute or increment10Minute or incrementHour) 
  {
    byte targetMinute;
    byte targetHour;
  
    targetMinute = minute;
    targetHour = hour;

    if (incrementMinute) {
      targetMinute = minute + 1;
      if (targetMinute > 59) {
        targetMinute = 0;
      } 
    }

    if (increment10Minute) {
      targetMinute = minute + 10;
      if ((targetMinute) > 59) {
        targetMinute = targetMinute - 60;
      } 
    }

    if (incrementHour) {
      targetHour = targetHour + 1;
      if (targetHour > 23) {
        targetHour = 0;
      } 
    }
    
    setDS3231time(0, targetMinute, targetHour, dayOfWeek, dayOfMonth, month, year);
  }  

  // only do this once a second.
  if (secBuffer != second) 
  {
    secBuffer = second;

    int sw7 = 0;
    sw7 = digitalRead(7);
    if (sw7 == 1)
    {
      pulseOffAllLights(1);
    }
    
    int sw6 = 0;
    sw6 = digitalRead(6);
    if (sw6 == 1)
    {
      pulseOnAllLights(2);
    }

  
  }


}


void setHourLights(int hour)
{

  // Deal with all that 24 hour clock bullshit.
  if (hour > 12) {
    hour = hour - 12;
  }

  switch (hour) {
      case 1:
        digitalWrite(32, LOW);
        digitalWrite(33, LOW);
        digitalWrite(34, LOW);
        digitalWrite(35, LOW);
        digitalWrite(36, LOW);
        digitalWrite(37, LOW);
        digitalWrite(38, LOW);
        digitalWrite(39, LOW);
        digitalWrite(40, LOW);
        digitalWrite(41, HIGH);
        digitalWrite(42, LOW);
        digitalWrite(43, LOW);
        break;
      case 2:
        digitalWrite(32, LOW);
        digitalWrite(33, LOW);
        digitalWrite(34, LOW);
        digitalWrite(35, LOW);
        digitalWrite(36, LOW);
        digitalWrite(37, LOW);
        digitalWrite(38, LOW);
        digitalWrite(39, LOW);
        digitalWrite(40, HIGH);
        digitalWrite(41, HIGH);
        digitalWrite(42, LOW);
        digitalWrite(43, LOW);
        break;
      case 3:
        digitalWrite(32, LOW);
        digitalWrite(33, LOW);
        digitalWrite(34, LOW);
        digitalWrite(35, LOW);
        digitalWrite(36, HIGH);
        digitalWrite(37, LOW);
        digitalWrite(38, LOW);
        digitalWrite(39, LOW);
        digitalWrite(40, LOW);
        digitalWrite(41, HIGH);
        digitalWrite(42, HIGH);
        digitalWrite(43, LOW);
        break;
      case 4:
        digitalWrite(32, LOW);
        digitalWrite(33, LOW);
        digitalWrite(34, LOW);
        digitalWrite(35, LOW);
        digitalWrite(36, HIGH);
        digitalWrite(37, LOW);
        digitalWrite(38, LOW);
        digitalWrite(39, LOW);
        digitalWrite(40, HIGH);
        digitalWrite(41, HIGH);
        digitalWrite(42, HIGH);
        digitalWrite(43, LOW);
        break;
      case 5:
        digitalWrite(32, LOW);
        digitalWrite(33, HIGH);
        digitalWrite(34, LOW);
        digitalWrite(35, LOW);
        digitalWrite(36, LOW);
        digitalWrite(37, LOW);
        digitalWrite(38, HIGH);
        digitalWrite(39, LOW);
        digitalWrite(40, HIGH);
        digitalWrite(41, HIGH);
        digitalWrite(42, LOW);
        digitalWrite(43, HIGH);
        break;
      case 6:
        digitalWrite(32, HIGH);
        digitalWrite(33, LOW);
        digitalWrite(34, HIGH);
        digitalWrite(35, LOW);
        digitalWrite(36, HIGH);
        digitalWrite(37, LOW);
        digitalWrite(38, LOW);
        digitalWrite(39, HIGH);
        digitalWrite(40, LOW);
        digitalWrite(41, LOW);
        digitalWrite(42, HIGH);
        digitalWrite(43, HIGH);
        break;
      case 7:
        digitalWrite(32, HIGH);
        digitalWrite(33, LOW);
        digitalWrite(34, HIGH);
        digitalWrite(35, LOW);
        digitalWrite(36, HIGH);
        digitalWrite(37, LOW);
        digitalWrite(38, LOW);
        digitalWrite(39, HIGH);
        digitalWrite(40, LOW);
        digitalWrite(41, HIGH);
        digitalWrite(42, HIGH);
        digitalWrite(43, HIGH);
        break;
      case 8:
        digitalWrite(32, LOW);
        digitalWrite(33, HIGH);
        digitalWrite(34, HIGH);
        digitalWrite(35, HIGH);
        digitalWrite(36, LOW);
        digitalWrite(37, HIGH);
        digitalWrite(38, HIGH);
        digitalWrite(39, HIGH);
        digitalWrite(40, LOW);
        digitalWrite(41, LOW);
        digitalWrite(42, HIGH);
        digitalWrite(43, HIGH);
        break;
      case 9:
        digitalWrite(32, HIGH);
        digitalWrite(33, HIGH);
        digitalWrite(34, HIGH);
        digitalWrite(35, HIGH);
        digitalWrite(36, HIGH);
        digitalWrite(37, HIGH);
        digitalWrite(38, LOW);
        digitalWrite(39, HIGH);
        digitalWrite(40, LOW);
        digitalWrite(41, LOW);
        digitalWrite(42, HIGH);
        digitalWrite(43, HIGH);
        break;
      case 10:
        digitalWrite(32, HIGH);
        digitalWrite(33, HIGH);
        digitalWrite(34, HIGH);
        digitalWrite(35, LOW);
        digitalWrite(36, HIGH);
        digitalWrite(37, LOW);
        digitalWrite(38, HIGH);
        digitalWrite(39, HIGH);
        digitalWrite(40, HIGH);
        digitalWrite(41, HIGH);
        digitalWrite(42, HIGH);
        digitalWrite(43, HIGH);
        break;
      case 11:
        digitalWrite(32, HIGH);
        digitalWrite(33, HIGH);
        digitalWrite(34, HIGH);
        digitalWrite(35, HIGH);
        digitalWrite(36, HIGH);
        digitalWrite(37, HIGH);
        digitalWrite(38, HIGH);
        digitalWrite(39, HIGH);
        digitalWrite(40, HIGH);
        digitalWrite(41, LOW);
        digitalWrite(42, HIGH);
        digitalWrite(43, HIGH);
        break;
      case 12:
        digitalWrite(32, HIGH);
        digitalWrite(33, HIGH);
        digitalWrite(34, HIGH);
        digitalWrite(35, HIGH);
        digitalWrite(36, HIGH);
        digitalWrite(37, HIGH);
        digitalWrite(38, HIGH);
        digitalWrite(39, HIGH);
        digitalWrite(40, HIGH);
        digitalWrite(41, HIGH);
        digitalWrite(42, HIGH);
        digitalWrite(43, HIGH);
        break;
    }
  
}

void set10MinuteLights(int minute)
{

  int tenMinute;

  tenMinute = minute/10;

  switch (tenMinute) {
      case 0:
        digitalWrite(22, LOW);
        digitalWrite(24, LOW);
        digitalWrite(26, LOW);
        digitalWrite(28, LOW);
        digitalWrite(30, LOW);
        break;
      case 1:
        digitalWrite(22, HIGH);
        digitalWrite(24, LOW);
        digitalWrite(26, LOW);
        digitalWrite(28, LOW);
        digitalWrite(30, LOW);
        break;
      case 2:
        digitalWrite(22, HIGH);
        digitalWrite(24, HIGH);
        digitalWrite(26, LOW);
        digitalWrite(28, LOW);
        digitalWrite(30, LOW);
        break;
      case 3:
        digitalWrite(22, HIGH);
        digitalWrite(24, HIGH);
        digitalWrite(26, HIGH);
        digitalWrite(28, LOW);
        digitalWrite(30, LOW);
        break;
      case 4:
        digitalWrite(22, HIGH);
        digitalWrite(24, HIGH);
        digitalWrite(26, HIGH);
        digitalWrite(28, HIGH);
        digitalWrite(30, LOW);
        break;
      case 5:
        digitalWrite(22, HIGH);
        digitalWrite(24, HIGH);
        digitalWrite(26, HIGH);
        digitalWrite(28, HIGH);
        digitalWrite(30, HIGH);
        break;
  }
  
}


void setMinuteLights(int minute)
{

  int minuteDigit; 

  minuteDigit = minute % 10;

  // 23 is the 5 minute channel 
  switch (minuteDigit) {
    case 0:
      digitalWrite(23, LOW);
      digitalWrite(25, LOW);
      digitalWrite(27, LOW);
      digitalWrite(29, LOW);
      digitalWrite(31, LOW);
      break;
    case 1:
      digitalWrite(23, LOW);
      digitalWrite(25, LOW);
      digitalWrite(27, LOW);
      digitalWrite(29, LOW);
      digitalWrite(31, HIGH);
      break;
    case 2:
      digitalWrite(23, LOW);
      digitalWrite(25, LOW);
      digitalWrite(27, LOW);
      digitalWrite(29, HIGH);
      digitalWrite(31, HIGH);
      break;
    case 3:
      digitalWrite(23, LOW);
      digitalWrite(25, LOW);
      digitalWrite(27, HIGH);
      digitalWrite(29, HIGH);
      digitalWrite(31, HIGH);
      break;
    case 4:
      digitalWrite(23, LOW);
      digitalWrite(25, HIGH);
      digitalWrite(27, HIGH);
      digitalWrite(29, HIGH);
      digitalWrite(31, HIGH);
      break;
    case 5:
      digitalWrite(23, HIGH);
      digitalWrite(25, LOW);
      digitalWrite(27, LOW);
      digitalWrite(29, LOW);
      digitalWrite(31, LOW);
      break;
    case 6:
      digitalWrite(23, HIGH);
      digitalWrite(25, HIGH);
      digitalWrite(27, LOW);
      digitalWrite(29, LOW);
      digitalWrite(31, LOW);
      break;
    case 7:
      digitalWrite(23, HIGH);
      digitalWrite(25, HIGH);
      digitalWrite(27, HIGH);
      digitalWrite(29, LOW);
      digitalWrite(31, LOW);
      break;
    case 8:
      digitalWrite(23, HIGH);
      digitalWrite(25, HIGH);
      digitalWrite(27, HIGH);
      digitalWrite(29, HIGH);
      digitalWrite(31, LOW);
      break;
    case 9:
      digitalWrite(23, HIGH);
      digitalWrite(25, HIGH);
      digitalWrite(27, HIGH);
      digitalWrite(29, HIGH);
      digitalWrite(31, HIGH);
      break;
  }
  
}

// This was just to make sure that the thing actually worked. 
void flashAllLights(int buttonChannel, int &buttonCount, bool &increment)
{
  digitalWrite(22, (state) ? HIGH : LOW);
  digitalWrite(24, (state) ? LOW : HIGH);
  digitalWrite(26, (state) ? HIGH : LOW);
  digitalWrite(28, (state) ? LOW : HIGH);
  digitalWrite(30, (state) ? HIGH : LOW);
  //digitalWrite(24, HIGH);
  
  digitalWrite(23, (state) ? HIGH : LOW);
  digitalWrite(25, (state) ? LOW : HIGH);
  digitalWrite(27, (state) ? HIGH : LOW);
  digitalWrite(29, (state) ? LOW : HIGH);
  digitalWrite(31, (state) ? HIGH : LOW);
  
  digitalWrite(32, (state) ? HIGH : LOW);
  digitalWrite(33, (state) ? LOW : HIGH);
  digitalWrite(34, (state) ? HIGH : LOW);
  digitalWrite(35, (state) ? LOW : HIGH);
  digitalWrite(36, (state) ? HIGH : LOW);
  digitalWrite(37, (state) ? HIGH : LOW);
  digitalWrite(38, (state) ? LOW : HIGH);
  digitalWrite(39, (state) ? HIGH : LOW);
  digitalWrite(40, (state) ? LOW : HIGH);
  digitalWrite(41, (state) ? HIGH : LOW);
  digitalWrite(42, (state) ? LOW : HIGH);
  digitalWrite(43, (state) ? HIGH : LOW);
  
  
  state = !state;
    
  // We want the loop to just run its little heart out.
  delay(500); // every second
  
}


void pulseOffAllLights(int delayMsec)
{

  for (int i=22; i <= 43; i++){
    digitalWrite(i, LOW);
   } 
  delay(delayMsec);
      
}

void pulseOnAllLights(int delayMsec)
{

  for (int i=22; i <= 43; i++){
    digitalWrite(i, HIGH);
   } 
  delay(delayMsec);
      
}

