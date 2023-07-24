#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <ADS1219.h>
#include <arduino-timer.h>
#define address 0x40
#define drdy 15 //data ready pin not really used here

Timer<10, micros> timer;

ADS1219 ads(drdy);
File logfile;
long adc0;
int counter = 0;
bool reconnected = false;
char filename[15];
int checkRate = 0;
int fileSize = 0;
int previousCount = 0;
int readVal = 0;
long bufferA[512];
long bufferB[512];
long *currentBuffer;

void reconnectSD(){
  while (!SD.begin(4)){
  }
  while(!reconnected){
    logfile = SD.open(filename, FILE_WRITE);
    if( logfile ) {
      reconnected = true;
      logfile.seek(logfile.size());
    }
  }
  fileSize = 0;
  Serial.println("reconnected");
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  currentBuffer = bufferA;
  ads.begin();
  Wire.begin();
  Serial.begin(9600);
  //while (!Serial);
  ads.setVoltageReference(REF_INTERNAL);
  ads.setGain(ONE);
  ads.setDataRate(DATA_RATE_1000);
  //ads.readDifferential_0_1();
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  strcpy(filename, "/ANALOG00.dat");
  
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist
    if (! SD.exists(filename)) {
      break;
    }
  }
  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    while (1);
  }
  digitalWrite(LED_BUILTIN, LOW);
  timer.every(3960, readValue); //3980
}

void loop() {
    timer.tick();
    if(counter == 512){
      checkRate = 1;
      if(currentBuffer == bufferA){
        currentBuffer = bufferB;
        counter = 0;
        if (logfile) {
          if(!logfile.write((const uint8_t *)&bufferA, sizeof(bufferA))){
              Serial.println("error writing");
              Serial.println("reconnecting...");
              digitalWrite(LED_BUILTIN, HIGH);
              reconnectSD();
           }
           logfile.flush();
        }
      }
      else{
        currentBuffer = bufferA;
        counter = 0;
        if (logfile) {
          if(!logfile.write((const uint8_t *)&bufferB, sizeof(bufferB))){
              Serial.println("error writing");
              Serial.println("reconnecting...");
              digitalWrite(LED_BUILTIN, HIGH);
              reconnectSD();
           }
           logfile.flush();
        }
      }
    }
    if(checkRate == 1){ //this is just used to help fine tune sample rate if needed. if you adjust the timer interval value then you can use this to check sample rate
      //Serial.println((millis()-previousCount));
      previousCount = millis();
      checkRate = 0;
    }
}

bool readValue(void *){
      if(counter < 512){
          currentBuffer[counter] = ads.readConversionResult();
          ads.readDifferential_0_1();
          Serial.println(currentBuffer[counter]); //comment this line out when using the checkRate function above
          counter += 1;
      }
      return true;
}
