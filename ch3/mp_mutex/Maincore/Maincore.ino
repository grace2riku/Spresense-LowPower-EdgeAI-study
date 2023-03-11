#ifdef SUBCORE
#error "Core selection is wrong!!"alignas
#endif

#include <MP.h>
#include <MPMutex.h>
MPMutex mutex(MP_MUTEX_ID0);

const int subcore = 1;
const int mem_size = 128;
int data[mem_size + 1];  // 末尾にチェックサムを追加

void setup() {
  int ret = MP.begin(subcore);
  if (ret < 0) {
    MPLog("MP.begin error: %d\n", ret);
  }
  // 未接続ピンのノイズを利用して乱数初期値を設定
  randomSeed(analogRead(0));
}

void loop() {
  int ret;
  int8_t sndid = 100; // 送信ID

  // mutexが確保できるまで待つ
  do { ret = mutex.Trylock(); } while (ret != 0);
  for (int n = 0; n < mem_size; ++n) {
    data[n] = random(0, 256); // 0〜255の乱数生成
  }
  // チェックサムを計算
  int sum = 0;
  for (int n = 0; n < mem_size; ++n) {
    sum += data[n];
  } 
  int checksum = ~sum;
  data[mem_size] = checksum;  // チェックサムを追加
  mutex.Unlock(); // mutexを解放する

  // サブコアにメモリポインターを渡す
  ret = MP.Send(sndid, &data, subcore);
  if (ret < 0) {
    MPLog("MP.Send error: %d\n", ret);
  }
  MPLog("Checksum = 0x%2X\n", checksum);
}
