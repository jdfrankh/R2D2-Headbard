/**
 * @file streams-sd-audiokit.ino
 * @author Phil Schatzmann
 * @brief Just a small demo, how to use files with the SD library
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */


// Which pin on the Arduino is connected to the NeoPixels?

#include <WiFi.h>
#include "EspNowEZ.h"
#include <esp_now.h>
#include <esp_system.h>
#include "esp_wifi.h"


#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

#include "controls.h"

const  String Noises[10]= {
  "/R2D2Noises/chat0.mp3",
  "/R2D2Noises/chat1.mp3",
  "/R2D2Noises/exci0.mp3",
  "/R2D2Noises/exci1.mp3",
  "/R2D2Noises/scre0.mp3",
  "/R2D2Noises/scre1.mp3",
  "/R2D2Noises/worr0.mp3",
  "/R2D2Noises/worr1.mp3",
  "/R2D2Noises/ackn0.mp3",
  "/R2D2Noises/ackn1.mp3"
}; 

struct __attribute__((packed)) sendData{
    uint8_t macHandshake[6];
    uint8_t Direction[2]; // 0,1,2,3 FWD, BWD, Left, Right

    uint8_t DcMax[2] = {255,255};
    uint8_t DcMin[2] = {0,0};
    uint8_t dcMagnitude[2] = {0,0}; //Should be between 0 and 100
    bool objectArm[6]; // For escs, arm them, for servos, set to zero degrees? or fixable value
    bool objectRun[6]; 
    float escSpeed = .25; 

    bool connectCheck = false;
};

const int chipSelect=PIN_AUDIO_KIT_SD_CARD_CS;
AudioBoardStream i2s(AudioKitEs8388V1); // final output of decoded stream
EncodedAudioStream decoder(&i2s, new MP3DecoderHelix()); // Decoding stream
StreamCopy copier; 
File audioFile;
Controls controls = Controls();
sendData sD;
uint8_t macAddress[6];
bool prevLight = false;
bool prevEmote = false;
bool prevRandomEmote = false;
int coolDownEmote = 0;
int countDownEmote = 0;


void onReceive(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  const uint8_t *recv_info = mac_addr;
   // Serial.printf("Length: %d, --- sizeofData: %d", len, sizeof(sendData));
  if (len == sizeof(sendData)) {
   // Serial.println("Data received from remote!"); 
    memcpy(&sD, incomingData, sizeof(sendData));
  

    if (!esp_now_is_peer_exist(sD.macHandshake)) {
  //  Serial.println("Adding peer");
    ESPNow.add_peer(sD.macHandshake);
    }
    
    


    if(sD.macHandshake[5] == macAddress[5] && sD.macHandshake[4] == macAddress[4] && sD.macHandshake[3] == macAddress[3] ){
      Serial.println("Successful Handshake");
    }

  } else {
    Serial.print("Unexpected message length: ");
    Serial.println(len);

  }
}

void setup(){
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  ESPNow.init();
  WiFi.macAddress(macAddress); 
  WiFi.setTxPower(WIFI_POWER_19dBm);
  ESPNow.reg_recv_cb(onReceive);



  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);  
  

  
  pinMode(PIN_AUDIO_KIT_SD_CARD_CS, OUTPUT);
  // setup audiokit before SD!
  auto config = i2s.defaultConfig(TX_MODE);
  config.sd_active = true;
  i2s.begin(config);

  // setup file
  SD.begin(chipSelect);
  audioFile = SD.open("/R2D2Noises/chat1.mp3");

  Serial.print("My MAC Address: ");
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x", macAddress[0],macAddress[1],macAddress[2],macAddress[3],macAddress[4],macAddress[5]);

  // setup I2S based on sampling rate provided by decoder
  decoder.begin();
  controls.initalizeAll();
  // begin copy
  copier.begin(decoder, audioFile); // Set the file as the active portion -- there is another component that needs to loop in loop

  controls.input.lightShow = 2;
  


}

void loop(){

  if(!copier.copy()) {
   //controls.input.emote = 0; 
   
  }

 // Serial.printf("Lights: %d", sD.objectRun[0]); Serial.println();
  //Serial.printf("Emote: %d", sD.objectArm[2]); Serial.println();

  controls.update();
  Serial.printf("Light Show: %d", controls.input.lightShow );
  Serial.println();
  if(sD.objectArm[2] && prevLight == false){
      prevLight = true;
      controls.input.lightShow++;
      if(controls.input.lightShow > 3){
        controls.input.lightShow = 0;
      }
    }
  if(!sD.objectArm[2]){
      prevLight = false;
  }
    Serial.printf("Emote: %d", controls.input.emote );
    Serial.println();
    if(sD.objectRun[0] && prevEmote == false){
      prevEmote = true;
      controls.input.emote++;
      if(controls.input.emote > 10){
        controls.input.emote = 0;
      }
        audioFile = SD.open(Noises[controls.input.emote]);
        copier.begin(decoder, audioFile );
    }
    if(!sD.objectRun[0]){
      prevEmote = false;
    }
    Serial.printf("Random Emote: %d", controls.input.emote );
    Serial.println();
    if(sD.objectRun[2] && prevRandomEmote == false){
      prevRandomEmote = true;
      controls.input.emote++;
      if(controls.input.emote > 2){
        controls.input.emote = 0;
      }
    }
    if(!sD.objectRun[2]){
      prevRandomEmote = false;
    }


    if(controls.input.emote == 1){
     if(coolDownEmote == 0){
      coolDownEmote = random(200, 700);

     }
     countDownEmote++;
     
     if(countDownEmote >= coolDownEmote){
        countDownEmote = 0;
        coolDownEmote = random(200, 700);
        audioFile = SD.open(Noises[random(0,9)]);
     }

    }


}

