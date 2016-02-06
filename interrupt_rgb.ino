//status led on pin 13 when button (pulled down with an 10k resistor) - timer 1 driven (could be more elegant)
//the mode is the mode of LED function
// the mode initially was multiplied by 4 -on, pause, off, pause for each mode
// this was too fast so divided by 8

//uses a led Monitor to give some spacing

const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// variables will change:
volatile int buttonState = 0;         // variable for reading the pushbutton status
int ledState =0;

//modes 1 is pots, 2 is swirl, 3 is pause, 4 is off
int mode = 1;

int ledMonitor = 0;//used to flash from 1 to 4 in 50 beats
int modeCounter = mode *8;

int timer1_counter;


// mainCol is the status of the swirl- maintains status when go across different modes
int mainCol = 1;

// Init the Pins used for PWM
const int redPin = 5;
const int greenPin = 6;
const int bluePin = 3;
 
// Init the Pins used for 10K pots
const int redPotPin = 2;
const int greenPotPin = 3;
const int bluePotPin = 1;
 
// Init our Vars- for the pot values
int currentColorValueRed;
int currentColorValueGreen;
int currentColorValueBlue;

void setup() {

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
//  Serial.begin(9600);

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
  
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt


  // Attach an interrupt to the ISR vector- for the button
  attachInterrupt(0, pin_ISR, FALLING);

  interrupts();             // enable all interrupts
}

void loop() {

    switch(mode){
    case(1): 
      readPot();
      break;

    case(2):
      swirl();
      break;

     case(3)://do nothing
      break;

      case(4):
        zeroColours();
        break;
  };
  
 setColor();// takes the calculated colour values and sets the LED
}

ISR(TIMER1_OVF_vect)        // interrupt service routine- clunky but works

{
  TCNT1 = timer1_counter;   // preload timer
  ledMonitor ++;// 50 beats of 50 Hz so flashes and pauses for second in total
//    Serial.println(modeCounter);
  if (ledMonitor >50){
    ledMonitor =0;
    modeCounter = mode*8;//reset modeCounter- this muliplier works - each flash .16 of a second
    }
  if (modeCounter >0){ // flash more for the higher modes
    modeCounter--;
    if(modeCounter%4 ==0){// only change states every 0.08- each flash is 0.16 secs- this is acceptable
      digitalWrite(ledPin, ledState);
      
      if (ledState ==1){ledState = 0;}
      else {ledState = 1;
//            Serial.println("change");   
      }
    }
  }
  else{digitalWrite(ledPin, 0);
  }

}


void pin_ISR() {
  static unsigned long last_interrupt_time = 0;
 unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 400) 
 {
//  change mode
 mode ++;
 if(mode>4){
  mode =1;
  }
// Serial.println("mode = " + mode);
 }
 last_interrupt_time = interrupt_time;
}

void zeroColours(){
   currentColorValueRed = 0;
  currentColorValueBlue = 0;
  currentColorValueGreen = 0;
  }

void readPot(){
  // Read the voltage on each analog pin then scale down to 0-255 and inverting the value for common anode
  currentColorValueRed = (255 - map( analogRead(redPotPin), 0, 1024, 0, 255 ) );
  currentColorValueBlue = (255 - map( analogRead(bluePotPin), 0, 1024, 0, 255 ) );
  currentColorValueGreen = (255 - map( analogRead(greenPotPin), 0, 1024, 0, 255 ) );

 if(currentColorValueRed <4){
  currentColorValueRed = 0;
 }
 if(currentColorValueGreen <4){
  currentColorValueGreen = 0;
 }
 if(currentColorValueBlue <4){
  currentColorValueBlue = 0;
 }
} 

void swirl(){
  switch(mainCol){
    case(1):
      currentColorValueGreen = currentColorValueBlue = 0;
      currentColorValueRed ++;
      if (currentColorValueRed >255){
        mainCol = 2;
        }
        break;

    case(2):
      currentColorValueRed = currentColorValueBlue = 0;
      currentColorValueGreen ++;
      if (currentColorValueGreen >255){
        mainCol = 3;
        }
        break;


    case(3):
      currentColorValueGreen = currentColorValueRed = 0;
      currentColorValueBlue ++;
      if (currentColorValueBlue >255){
        currentColorValueBlue =0;
        mainCol = 4;
        }
        break;


    case(4):
      currentColorValueBlue = 0;
      currentColorValueRed = 255;
      currentColorValueGreen ++;
      if (currentColorValueGreen >255){
        currentColorValueGreen =0;
        mainCol = 5;
        }
        break;

    case(5):
      currentColorValueBlue = 255;
      currentColorValueRed = 0;
      currentColorValueGreen ++;
      if (currentColorValueGreen >255){
        currentColorValueGreen =0;
        mainCol = 6;
        }
        break;


    case(6):
      currentColorValueGreen = 0;
      currentColorValueBlue = 255;
      currentColorValueRed ++;
      if (currentColorValueRed >255){
        currentColorValueRed =0;
        mainCol = 7;
        }
        break;
        
    case(7):
      currentColorValueGreen = 255;
      currentColorValueBlue = 0;
      currentColorValueRed ++;
      if (currentColorValueRed >255){
        currentColorValueRed =0;
        mainCol = 8;
        }
        break;
        
    case(8):
      currentColorValueGreen = 0;
      currentColorValueRed = 255;
      currentColorValueBlue ++;
      if (currentColorValueBlue >255){
        currentColorValueBlue =0;
        mainCol = 9;
        }
        break;

    case(9):
      currentColorValueGreen = 255;
      currentColorValueRed = 0;
      currentColorValueBlue ++;
      if (currentColorValueBlue >255){
        currentColorValueBlue = 0;
        mainCol = 1;
        }
        break;


    
    }
  }

void setColor(){
  // Write the color to each pin using PWM and the value gathered above
  analogWrite(redPin, currentColorValueRed);
  Serial.println(currentColorValueRed);
  analogWrite(bluePin, currentColorValueBlue);
  Serial.println(currentColorValueBlue);
  analogWrite(greenPin, currentColorValueGreen);
   Serial.println(currentColorValueGreen);
  }


