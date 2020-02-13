#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

//for Digi
//#define button 2
//#define gnd 1
//for Uno
#define button 2
#define gnd 3

const int  workTimer = 1500;
const int  breakTimer = 300;
volatile int counter = 0;   // counter
uint8_t buttonState = HIGH;         // current state of the button
uint8_t lastButtonState = LOW;     // previous state of the button
const uint16_t t1_load = 0;
//const uint16_t tl_load = 31250; //(500ms)
const uint16_t t1_comp = 62500; //(1000ms)

LiquidCrystal_PCF8574 lcd(0x3f); // set the LCD address to 0x27 for a 16 chars and 2 line display

volatile boolean resetTimer = false;

void buttonInterrupt() {
  counter = 0;
  resetTimer = true;
}

ISR(TIMER1_COMPA_vect) {
  counter++;// another second passed
}

void setup()
{
  int error;
  pinMode (button, INPUT);
  digitalWrite (button, HIGH); // enable pullup
  pinMode(gnd, OUTPUT);
  digitalWrite (gnd, LOW); // use this pin as a ground
  attachInterrupt(digitalPinToInterrupt(button), buttonInterrupt, FALLING);
  //  MCUCR |= (1 << ISC01);
 

  // See http://playground.arduino.cc/Main/I2cScanner how to test for a I2C device.
  Wire.begin();
  Wire.beginTransmission(0x3f);
  error = Wire.endTransmission();
  // Serial.print("Error: ");
  // Serial.print(error);

  if (error == 0) {
    //    Serial.println(": LCD found.");
    lcd.begin(16, 2); // initialize the lcd

    lcd.setBacklight(255);
    lcd.home();
    lcd.clear();
    lcd.print("Pomo Timer");
    delay(1000);
    lcd.clear();
  } else {
    ;

    //   Serial.println(": LCD not found.");
  } // if

  // Reset Timer1 Control Reg A
  TCCR1A = 0;

  // Set CTC mode
  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12);

  // Set to prescaler of 256
  TCCR1B |= (1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS10);

  // Reset Timer1 and set compare value
  TCNT1 = t1_load;
  OCR1A = t1_comp;

  // Enable Timer1 overflow interrupt
  TIMSK1 = (1 << OCIE1A);

  // Enable global interrupts
  sei();
} // setup()

char data[16];
int timeLeft;
void loop()
{
  if (resetTimer) {
    lcd.noBlink();
    lcd.clear();
    resetTimer = false;
  }

  timeLeft = workTimer - counter;
  if (timeLeft <= 0 ) {
    lcd.setCursor(2, 0);
    //---------1234567890123456------------
    lcd.print("Take a break");
    lcd.blink();
    lcd.setCursor(0, 1);
    lcd.print("Press to reset");
  }
  else {

    lcd.setCursor(2, 0);
    lcd.print(":Work Timer:");
    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;
    sprintf(data, "%02d:%02d", minutes , seconds);
    lcd.setCursor(5, 1);
    lcd.print(data);
  }
  delay(500); // change to timer interupt
  // }

} // loop()
