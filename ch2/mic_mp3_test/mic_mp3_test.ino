#include <Audio.h>
#include <SDHCI.h>

SDClass SD;
File myFile;

AudioClass* theAudio = AudioClass::getInstance();
const int32_t recording_time_ms = 10000;  // 録音時間10秒
int32_t start_time_ms;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  while (!SD.begin()) { Serial.println("Insert SD card."); }

  theAudio->begin();
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC);
  theAudio->initRecorder(AS_CODECTYPE_MP3, "mnt/sd0/BIN", AS_SAMPLINGRATE_48000, AS_CHANNEL_MONO);
  
  if (SD.exists("Sound.mp3")) {
    SD.remove("Sound.mp3");
  }

  myFile = SD.open("Sound.mp3", FILE_WRITE);
  if (!myFile) {
    Serial.println("File open error\n");
    while(1);
  }

  theAudio->startRecorder();
  start_time_ms = millis();

  Serial.println("Start Recording");
}


void loop() {
  // put your main code here, to run repeatedly:
  uint32_t duration_ms = millis() - start_time_ms;

  err_t err = theAudio->readFrames(myFile);
  if (duration_ms > recording_time_ms || err != AUDIOLIB_ECODE_OK) {
      theAudio->stopRecorder();
      theAudio->closeOutputFile(myFile);
      theAudio->setReadyMode();
      theAudio->end();

      Serial.println("End Recording");
      while(1);
    }
}
