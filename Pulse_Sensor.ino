#include <Adafruit_SSD1306.h>
#define OLED_Address 0x3C // 0x3C device address of I2C OLED. Few other OLED has 0x3D
Adafruit_SSD1306 oled(128, 64); // create our screen object setting resolution to 128x64


// Variables
unsigned long lastMillis;
int analogPin = A0;

int val;

int upperBound = 600;
int lowerBound = 500;

bool beat = false;
int count = 0;
int temp;

int i = 1;

int x=0;
int y=0;
int lastx=0;
int lasty=0;

uint32_t samples = 0;

void init_oled(){
  
}

void setup() {
  // put your setup code here, to run once: 
  oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setTextColor(WHITE); 
  Serial.begin(9600);

  cli();
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz 
  OCR1A = 15999;// = (16*10^6) / (1*1024) - 1 (must be <65536))
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();

}

ISR(TIMER1_COMPA_vect){
  if(i < 301){
    samples= samples + analogRead(analogPin);
    //Serial.println(samples);
    i++;
  }
  if(i > 299 && i < 301){
    i++;
    samples = samples/300;
    //Serial.println(samples);
  }
}



void loop() {
  // put your main code here, to run repeatedly:
  
  // read from analog a0
  val = analogRead(analogPin);
  int norm = analogRead(analogPin) - samples;
  Serial.print(val);
  Serial.print(",");
  Serial.println(norm);

  //x goes to 127 pixels then back to the first one
  if(x>127){
  oled.clearDisplay();
  x=0;
  lastx=x;
  }
  //y needs to be within 0 - 63 pixels 
  y=50-(val/20);
  oled.writeLine(lastx,lasty,x,y,WHITE);
  lasty=y;
  lastx=x;
  
  // our tresholds for calculating the beats
  if(val > upperBound ){
    if(beat){
      beat = false;
      count++;
    }
  }
  if (val < lowerBound ){
    beat = true;
  }
  //every 10 sec we calculate the bpm based on how many beats we got during the 10 sec then multiply by 6 for the bpm  
  if (millis() - lastMillis >= 10*1000UL){
    lastMillis = millis();
    temp = count * 6;
    count = 0;
  }

  oled.setCursor(0,50);
  oled.print("BPM:");
  oled.print(temp);
  oled.display();
  x++;
 
}
