#if (SUBCORE != 1)
#error "Core selection is wrong!!"
#endif

#include <MP.h>


void setup() {
  // put your setup code here, to run once:
  MP.begin(); // メインコアに起動通知
  // データが来るまで待つように設定
  MP.RecvTimeout(MP_RECV_BLOCKING);
}

void loop() {
  // put your main code here, to run repeatedly:
  int ret;
  int8_t msgid; // メインコアの送信ID
  uint32_t msgdata; // 受信データ

  // メインコアからのデータを待つ
  ret = MP.Recv(&msgid, &msgdata);
  if (ret < 0) {
    MPLog("MP.Recv Error = %d\n", ret);
  }

  // メインコアの送信IDで受信データを返す
  ret = MP.Send(msgid, msgdata);
  if (ret < 0) {
    MPLog("MP.send Error = %d\n", ret);
  }
}
