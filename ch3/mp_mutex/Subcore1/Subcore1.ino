#if (SUBCORE != 1)
#error "Core selection is wrong!!"
#endif
#include <MP.h>
#include <MPMutex.h>
MPMutex mutex(MP_MUTEX_ID0);

const int mem_size = 128;
int* data;  // メインコアが確保したメモリーへのポインター

void setup() {
  // put your setup code here, to run once:
  int ret = MP.begin();
  if (ret < 0) { MPLog("MP.begin error: %d\n", ret); }
  // データ受信をポーリングモードで監視
  MP.RecvTimeout(MP_RECV_POLLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  int ret;
  int8_t rcvid;
  // メモリーエリアのアドレスを受信
  ret = MP.Recv(&rcvid, &data);
  if (ret < 0) return;  // データがない場合は負の値
  // mutecxが確保できるまで待つ
  do { ret = mutex.Trylock(); } while (ret != 0);
  // 受信データのチェックサムを計算
  int sum = 0;
  for (int n = 0; n < mem_size; ++n) {
    sum += data[n];
  }
  int checksum = ~sum;
  mutex.Unlock(); // mutexを解放
  // チェックサムを比較
  if (checksum != data[mem_size]) {
    MPLog("Error = 0x%2X\n", checksum);
  } else {
    MPLog("   Ok = 0x%2X\n", checksum);
  }
}
