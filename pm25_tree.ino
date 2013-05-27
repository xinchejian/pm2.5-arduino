/* Grove - Dust Sensor Demo v1.0
 Interface to Shinyei Model PPD42NS Particle Sensor
 Program by Christopher Nafis 
 Written April 2012
 
 http://www.seeedstudio.com/depot/grove-dust-sensor-p-1050.html
 http://www.sca-shinyei.com/pdf/PPD42NS.pdf
 
 JST Pin 1 (Black Wire)  => Arduino GND
 JST Pin 3 (Red wire)    => Arduino 5VDC
 JST Pin 4 (Yellow wire) => Arduino Digital Pin 8
 */


/* smoothing */

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average




#include <Servo.h> 
Servo myservo;  // create servo object to control a servo 
// a maximum of eight servo objects can be created 
int pos = 0;

int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sampe 30s ;
unsigned long lowpulseoccupancy = 0;

float ratio = 0;
int concentration = 0;

void setup() {
  Serial.begin(9600);
  pinMode(8,INPUT);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;          
  }
  starttime = millis();//get the current time;
  myservo.attach(9);
}

void loop() {
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) > sampletime_ms)  //if the sample time == 30s
  {

    total= total - readings[index];         

    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve

    Serial.println(concentration);

    lowpulseoccupancy = 0;

    readings[index] = concentration; 

    total= total + readings[index];       
    index = index + 1;                    

    if (index >= numReadings)  {
      index = 0;                           
    }
    average = total / numReadings;
    pm25(average);
    starttime = millis();




  }
}

void pm25(long range) {

  if (range >0 && range <= 75) {
    pos = 180;
  } 
  else if(range >75 && range <=150) {
    pos = 144;
  } 
  else if(range >150 && range <=300) {
    pos = 108;
  } 
  else if(range > 300 && range <= 1050) {
    pos = 72;
  } 
  else if(range> 1050 && range <= 3000) {
    pos = 36; 
  } 
  else {
    pos = 0;
  }

  myservo.write(pos);
} 

