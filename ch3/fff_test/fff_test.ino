#include <Audio.h>
#include <FFT.h>
#include <SDHCI.h>
SDClass SD;

#define FFT_LEN 1024
#define AVG_FILTER (8)

// モノラル、1024サンプルでFFTを初期化
FFTClass<AS_CHANNEL_MONO, FFT_LEN> FFT;

AudioClass* theAudio = AudioClass::getInstance();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while ( !SD.begin() ) { Serial.println("Insert SD card"); }

  // ハミング窓、モノラル、オーバーラップ50%
  FFT.begin(WindowHamming, AS_CHANNEL_MONO, (FFT_LEN/2));

  Serial.println("Init Audio Recorder");
  theAudio->begin();

  // 入力をマイクに設定
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC);

  // 録音設定: PCM(16ビットRAWデータ)フォーマット
  // DSPファイルは、SDカード上のBINディレクトリを使用、
  // サンプリングレート 48000Hz, モノラル入力
  int err = theAudio->initRecorder(AS_CODECTYPE_PCM, "mnt/sd0/BIN", AS_SAMPLINGRATE_48000, AS_CHANNEL_MONO);
  if (err != AUDIOLIB_ECODE_OK) {
    Serial.println("Recorder initialize error");
    while(1);
  }

  Serial.println("Start Recorder");
  theAudio->startRecorder();  // 録音開始
}

void loop() {
  // put your main code here, to run repeatedly:
  static const uint32_t buffering_time = FFT_LEN * 1000 / AS_SAMPLINGRATE_48000;
  static const uint32_t buffer_size = FFT_LEN * sizeof(int16_t);
  static const int ch_index = AS_CHANNEL_MONO - 1;
  static char buff[buffer_size];
  static float pDst[FFT_LEN];
  uint32_t read_size;

  // buffer_sizeで要求されたデータをbuffに格納する
  // 読み込みできたデータ量はread_sizeに設定される
  int ret = theAudio->readFrames(buff, buffer_size, &read_size);
  if (ret != AUDIOLIB_ECODE_OK && ret != AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA) {
    Serial.println("Error err = " + String(ret));
    while(1);
  }

  // 読み込みサイズがbuffer_sizeに満たない場合
  if (read_size < buffer_size) {
    delay(buffering_time);  // データが蓄積されるまで待つ
    return;
  }

  FFT.put((q15_t*)buff, FFT_LEN); // FETを実行
  FFT.get(pDst, ch_index);        // チャンネル0万の演算結果を取得
#ifdef AVG_FILTER
  avgFilter(pDst);  // 周波数データを平滑化
#endif
  // 周波数と最大値の近似値を算出
  float maxValue;
  float peakFs = get_peak_frequency(pDst, &maxValue);

  Serial.println("peak freq: " + String(peakFs) + " Hz");
  Serial.println("Spectrum: " + String(maxValue));  
}

#ifdef AVG_FILTER
void avgFilter(float dst[FFT_LEN]) {
  static float pAvg[AVG_FILTER][FFT_LEN/2];
  static int g_counter = 0;

  if (g_counter == AVG_FILTER) g_counter = 0;

  for (int i = 0; i < FFT_LEN/2; ++i) {
    pAvg[g_counter][i] = dst[i];
    float sum = 0;
    for (int j = 0; j < AVG_FILTER; ++j) {
      sum += pAvg[j][i];
    }
    dst[i] = sum / AVG_FILTER;
  }
  ++g_counter;
}
#endif

float get_peak_frequency(float* pData, float* maxValue) {
  uint32_t idx;
  float delta, delta_spr;
  float peakFs;

  // 周波数分解脳(delta)を算出
  const float delta_f = AS_SAMPLINGRATE_48000 / FFT_LEN;

  // 最大値と最大値のインデックスを取得
  arm_max_f32(pData, FFT_LEN/2, maxValue, &idx);
  if (idx < 1) return 0.0;

  // 周波数のピークの近似値を算出
  delta = 0.5 * (pData[idx-1]-pData[idx+1]) / (pData[idx-1]+pData[idx+1]-(2.*pData[idx]));
  peakFs = (idx + delta) * delta_f;

  // スペクトルの最大値の近似値を算出
  delta_spr = 0.125*(pData[idx-1]-pData[idx+1]) 
    *(pData[idx-1]-pData[idx+1])
    /(2.*pData[idx]-(pData[idx-1]+pData[idx+1]));
  *maxValue += delta_spr;

  return peakFs; 
}


