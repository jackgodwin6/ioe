#include <SPI.h> 
#include <Ethernet.h> //include ethernet connection
#include <PubNub.h> //incllude pubnub library

byte mac[]= {0x00, 0x0e, 0xEF, 0x00, 0x00, 0xE6}; //Mac address

const int numReadings = 5; //number reading = 5

int redPin=5; // value of Red on RGB LED is 5
int bluePin=6; // value of Blue on RGB LED is 6
int greenPin=7; // value of Green on RGB LED is 6

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total

unsigned long previousMillis = 1000;   // interval at which to blink (ms)
const long interval = 1000; //each interval is 1000 milliseconds
                                                                                      
const int analogInRNG=A1; //Analog Range finder in signal port A1
const int MIC=A0; //Microphone in port A0 on arduino
int average; //integer for average



char pubkey[]="pub-c-6c8ec4e2-764d-4802-9bcb-981b0ea8570a"; //pubnub key
char subkey[]="sub-c-81028ae2-498f-11e9-bace-daeb5080f5f6"; //subcriber key
char channel[]= "channelRNGMIC"; //pubnub channel name

void setup() { //setup function

  Serial.println("Ethernet set up success");  //print line saying ethernet is set up on serial
  PubNub.begin(pubkey,subkey); //start new pubnub with publisher and subscriber key
  Serial.println("Pubnub set up"); //print line saying pubnub is set up on serial
  
  pinMode(redPin, OUTPUT); //Red pin output
  pinMode(greenPin, OUTPUT); //Green pin output
  pinMode(bluePin, OUTPUT); //Blue pin output

  pinMode(2, INPUT);  //interrupt
  digitalWrite(2, HIGH);  //enable pull up resistor
  
  Serial.begin(9600); //serial begin at 9600
  Serial.println("Serial Setup"); //print line 
  while(!Ethernet.begin(mac)){  
    Serial.println("Ethernet setup error!"); //if mac address doesn't load print "ethernet setup error"
  
  }
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0; //for loop, if this reading < number reading. increment.So this counts 5 readings
  }
}

void loop() { //loop function



  int dB = (analogRead(MIC)/1024.0*5*56);  // integer DB is an alogorithm to convert analog reading into decibel 
  unsigned long currentMillis = millis();  // current level of milliseconds is equal to millis function

  if (currentMillis - previousMillis >= interval) { If it goes over interval level
    previousMillis = currentMillis; previous level if overtook by new level

    if(analogRead(analogInRNG) > 250) { //if range on rangefinder is over 250
    blank(); //light goes off, calls blank function
    
    } else if (average < 35) { //if average reading is less than 35
      flashGreen(); //RGB led is green
      
    } else if(average > 45.01 && (average < 53.99)) { //If average is over 45 and less than 54
      flashAmber(); //RGB led is amber (warning)
      
    } else if(average > 54) { //is average is over 54
      flashRed(); //RGB led is red
    } 
  }

                                      
  total = total - readings[readIndex];    // subtract the last reading                                   
  readings[readIndex] = analogRead(MIC);    // read from the sensor                                   
  total = total + readings[readIndex];    // add the reading to the total                             
  readIndex = readIndex + 1;    // advance to the next position in the array
                                      
  if (readIndex >= numReadings) {     // if we're at the end of the array...
    readIndex = 0;    // ...wrap around to the beginning:
  }

    average = total / numReadings;    // calculate the average: 
    Serial.println(average);    // send it to the computer as ASCII digits




 
  Ethernet.maintain(); //maintain ethernet
  EthernetClient *client;
   
  char msg[64] = "{\"eon\":{\"range\":"; //print message on serial of eon and range
  sprintf(msg+ strlen(msg), "%d", analogRead(analogInRNG)); //, print message above and analog range level

  sprintf(msg+ strlen(msg), ", \"mic\":"); //print messages "mic"
  sprintf(msg+ strlen(msg), "%d",average); //print average value in message

  strcat(msg,"}}"); //closes off messages

  Serial.print("Publishing message:"); //Serial print line saying publishing message
  Serial.println(msg); //print message line again
  client=PubNub.publish(channel, msg); //client is pubnub publishers channel and msg
  if(!client){ //if there is no client
    Serial.println("publishing error");  //print publishing error
  } else {
    client->stop(); //otherwise stop client
    }
    delay(500); //wait half a second
}

void flashRed() { //for red light call red
  red();
  }

void flashAmber() { //for amber light call warning
 warning();
}

void flashGreen() { //for green light call green
  green();
}
 
void blank() { //function for no light output. turns off microphone
  analogWrite(5, 0);
  analogWrite(6, 0);
  analogWrite(7, 0);
}

void red() { //for blue Blue level = 255 no red or green light output
  analogWrite(5, 255);
  analogWrite(6, 0);
  analogWrite(7, 0);
}

void warning() { //for warning and amber light is emmitted, Green = 188, Red = 255, no blue light output
  analogWrite(6, 0);
  analogWrite(7, 188);
  analogWrite(5, 255);
}

void green() { //for green Green level = 255 no red or blue light output
  analogWrite(7, 255);
  analogWrite(5, 0);
  analogWrite(6, 0);
}






