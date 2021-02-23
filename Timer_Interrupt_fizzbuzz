/*これはハードウェアタイマの割り込みを確認するためのシンプルなテストです
 * 
 * タイマーは4つまで使用することができます
 * ほぼ1秒ごとにカウントアップして
 * 1分ごとにカウントアップします
 * 3秒ごとにあほになります         print("saaaaaaaaaaaaaaaaaaaaaaaaaaaan!")
 * 5秒ごとに犬になります           print("one!")
 * 15秒毎にfizzbuzzといいます   print("fuzzBuzz")
 * 参考情報
 * https://lang-ship.com/blog/work/esp32-timer/
 * http://marchan.e5.valueserver.jp/cabin/comp/jbox/arc202/doc21105.html
 * https://55life555.blog.fc2.com/blog-entry-3194.html
 * http://marchan.e5.valueserver.jp/cabin/comp/jbox/arc202/doc21105.html
 * https://lang-ship.com/blog/work/esp32-timer/
 * ESP32でのタイマー割り込みはハードウェアタイマーとtickerクラスが考えられる 
 * 
 * @author S.Nakamura 2021-02-23 
*/


//割り込み通知用のカウンター
volatile int timeCounter1 ;    //1分後
volatile int timeCounter2;    //3秒後後
volatile int timeCounter3;    //5秒後
volatile int timeCounter4;    //15秒後

//タイマーフラグの初期化
uint8_t timer_flg_1 =0; 
uint8_t timer_flg_2 =0;
uint8_t timer_flg_3 =0;
uint8_t timer_flg_4 =0;

//ここは何をしているかよくわからないが必要
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//ハードウェアタイマーの準備
hw_timer_t *timer1 = NULL;
hw_timer_t *timer2 = NULL;
hw_timer_t *timer3 = NULL;
hw_timer_t *timer4 = NULL;

//タイマー1の割り込み間隔を分で指定
#define INTERVAL_MINUTE   1

//タイマーのインターバルを秒で指定
#define TIMER2_INTERVAL   3
#define TIMER3_INTERVAL   5
#define TIMER4_INTERVAL   15


//割り込み時に呼ばれる割り込み関数
//フラグを立てるためにカウンターをインクリメントする
void IRAM_ATTR onTimer1(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  timeCounter1++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR onTimer2(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  timeCounter2++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR onTimer3(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  timeCounter3++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR onTimer4(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  timeCounter4++;
  portEXIT_CRITICAL_ISR(&timerMux);
}


void setup() {
  Serial.begin(115200);

  //1us毎にカウンターのカウントアップ
  //第一引数：使用するタイマーの番号
  //第二引数：プリスケーラ(1us毎にカウントアップしたいのであれば80に設定(80MHzなので))
  //第三引数：割り込みカウンターのインクリメント指定（tue：カウントアップ、false：カウントダウン）
  timer1 = timerBegin(0, 80, true);
  timer2 = timerBegin(1, 80, true);
  timer3 = timerBegin(2, 80, true);
  timer4 = timerBegin(3, 80, true);

  // Attach onTimer function.
  //割り込み関数処理(ISR)の設定、ISR関数を割り込み処理に結び付ける
  //第一引数：初期化されたタイマー設定用のポインタ
  //第二引数：ISR関数のアドレス
  //第三引数：エッジ割り込み指定（true：エッジトリガー、 false：レベルトリガー）
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAttachInterrupt(timer2, &onTimer2, true);
  timerAttachInterrupt(timer3, &onTimer3, true);
  timerAttachInterrupt(timer4, &onTimer4, true);
  
  // Set alarm to call onTimer function every minute (value in microseconds).
  //タイマの動作間隔の指定（トリガー条件）
  //第一引数：タイマーへのポインタ
  //第二引数：割り込み発生までの時間（マイクロ秒で指定）
  //第三引数：カウンターのリロード指定（true:定期実行、false:1ショットの実行）
  timerAlarmWrite(timer1, (INTERVAL_MINUTE* 1000000*60), true);   //1分毎
  timerAlarmWrite(timer2, (TIMER2_INTERVAL* 1000000), true);      //3秒毎
  timerAlarmWrite(timer3, (TIMER3_INTERVAL* 1000000), true);      //5秒毎
  timerAlarmWrite(timer4, (TIMER4_INTERVAL* 1000000), true);      //15秒毎
  
  // Start an alarm
  //タイマーの有効化　timerAlarmDisable関数でストップさせることもできる
  //引数：タイマー変数
  timerAlarmEnable(timer1);
  timerAlarmEnable(timer2);
  timerAlarmEnable(timer3);
  timerAlarmEnable(timer4);
}

//ほぼ1秒でループを回す
int second_timer;
int old_time =0;
//経過時間の測定
int n = 0;    //秒数をカウントアップ
int m = 0;    //分数をカウントアップ

void loop() {
    
  //ほぼ1秒ごとにカウントアップする
  second_timer = millis();
  //1秒経過したら
  if((second_timer - old_time) >= 1000){
    //秒をカウントアップ
    n = n +1;
    
    //3秒と5秒と15秒は表示しない
    if(n%3 != 0){
      if(n%5 != 0){
        Serial.print("count is ");
        Serial.println(n);
      }
    }        
  //計測時間の基準を作成
  old_time = second_timer; 
  }

  //1分ごとに表示してカウントアップ 
  if (timeCounter1 > 0) {
    m = m+1;
    Serial.print("minute count ");
    Serial.println(m);
    //フラグを元に戻す
    timeCounter1 = 0;
  }

  //3秒ごとにあほになる
  if (timeCounter2 > 0) {
    //15秒の時は無視する
    if (n%15 != 0 ){
      Serial.println("Saaaaaaaaaaaaaaaaaaaaaaaaaaaan!");
    }
    //フラグを元に戻す
    timeCounter2 = 0;
  }

  //5秒ごとに犬になる
  if (timeCounter3 > 0) {
    //15秒の時は無視する
    if (n%15 != 0){
      Serial.println("one!");
    }
    //フラグを元に戻す
    timeCounter3 = 0;
  }
  //15秒ごとにfizzbuzz
  if (timeCounter4 > 0) {
    Serial.println("FizzBuzz!");
    //フラグを元に戻す
    timeCounter4 = 0;
  } 
}

