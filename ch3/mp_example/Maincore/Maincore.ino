#ifdef SUBCORE
#erroe "Core selection is wrong!!"
#endif

#include <MP.h>
int subcore = 1;  // サブコア1を使用

void setup() {
  // put your setup code here, to run once:
  MP.begin(subcore);  // サブコア1を起動

  // 未接続ピンのノイズを利用して乱数初期値を設定
  randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:
  int ret;
  uint32_t snddata; // サブコアへ送信するデータ
  uint32_t rcvdata; // サブコアからの受信データ
  int8_t sndid = 100; // 送信ID(100)
  int8_t rcvid; // 受信ID

  snddata = random(32767);  // 乱数を生成
  MPLog("send: data= %d\n", snddata);

  // 生成した乱数をサブコアへ送信
  ret = MP.Send(sndid, snddata, subcore);
  if (ret < 0) {
    MPLog("MP.Send error = %d\n", ret);
  }

  // サブコアからのデータが到着するまで待つ
  MP.RecvTimeout(MP_RECV_BLOCKING);
  ret = MP.Recv(&rcvid, &rcvdata, subcore);
  if (ret < 0) {
    MPLog("MP.Recv error = %d\n", ret);
  }
  MPLog("Recv data = %d\n", rcvdata);
  delay(1000);  // 1秒待つ
}
