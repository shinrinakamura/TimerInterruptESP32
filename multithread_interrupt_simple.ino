/*マルチスレッドと割り込みの動きを確認するための簡単なテストプログラムです
 * メインループとマルチスレッドでそれぞれカウントアップします
 * メインスレッドとマルチスレッドそれぞれに割り込みをかけて動作を確認します
 * 
 * 
 * メインループで1秒に1回カウントアップ                     count1
 * 割り込みで10秒に1回カウントアップ                       count2
 * マルチスレッドで5秒に1回カウントアップ                   count3
 * マルチスレッドのほうの割り込みで20秒に1回カウントアップ     count4
 * 
 * 
 * @author S.Nakamura 2021-05-18 
*/


int count1 = 0;
//int count2 = 0;
int count3 = 0;
//int count4 = 0;

//ほぼ1秒でループを回すために時間を測定する
int second_timer;
int old_time =0;

//マルチスレッドの時間を測る
int second_timer2;
int old_time2 =0;

//割り込み通知用のカウンター
volatile int timeCounter1;
//マルチスレッド割り込み通知用のカウンター
volatile int timeCounter3;

//タイマーフラグの初期化
uint8_t timer_flg_1 =0; 
//マルチスレッドタイマーフラグの初期化
uint8_t timer_flg_3 =0; 

//同期用変数：portMUX_TYPE型の変数で、メインループとISRで共用変数を変更する時の同期処理に使用する
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//ハードウェアタイマーの準備
hw_timer_t *timer1 = NULL;
//マルチスレッドハードウェアタイマーの準備
hw_timer_t *timer3 = NULL;

//タイマー1の割り込み間隔を秒で指定
#define INTERVAL_SECONDS   10
//マルチスレッドのタイマー1の割り込み間隔を秒で指定
#define INTERVAL_SECONDS_3   20

//割り込み時に呼ばれる割り込み関数
//フラグを立てるためにカウンターをインクリメントする
void IRAM_ATTR onTimer1(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  timeCounter1++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

//マルチスレッドのフラグを立てるためにカウンターをインクリメントする
void IRAM_ATTR onTimer3(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  timeCounter3++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

//マルチスレッドの中身------------------------------------------------------------------
void multitaskCountup(void *pvParameters){
  
  while(1){

    //割り込みタイマーのスタート
    timerAlarmEnable(timer3);
    
    //ほぼ5秒ごとにカウントアップしていく処理
    second_timer2 = millis();
    
    //5秒経過したら
    if((second_timer2 - old_time2) >= 5000){
      count3 += 1;
      Serial.print("count3 is ");
      Serial.println(count3);
  
      //計測時間
      old_time2 = second_timer2; 
      
    }
    
    //タイマー割り込み
    if (timeCounter3 != 0){
      //割り込みフラグの回収
      timeCounter3 = 0;
      //割り込みが発動したことを表示して確認
      Serial.println("thread inter rupt");
    }
  }
}

//タイマーセットの関数
void setTimer();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("timer interrupt test start");

  //タイマーセット
  setTimer();  

  //マルチタスクのループ関数を起動
  //第一引数：タスク入力機能へのポインタ
  //第二引数：タスクのわかりやすい名前
  //第三引数：タスクスタックのサイズ
  //第四引数：作成中のタスクのパラメータとして使用されるポインター
  //第五引数：タスクの優先度
  //第六引数：作成したタスクを参照できるバンドルを返すために使用される
  //第七引数：使用するcpuコア
  xTaskCreatePinnedToCore(multitaskCountup, "multitaskCountup", 4096, NULL, 1, NULL, 1);
  
}



void loop() {


  //ほぼ1秒ごとにカウントアップする
  second_timer = millis();
  //1秒経過したら
  if((second_timer - old_time) >= 1000){
    count1 += 1;
    Serial.print("count1 is ");
    Serial.println(count1);

    //計測時間の基準を作成
    old_time = second_timer; 
  }

  //タイマー割り込み
  if (timeCounter1 != 0){
    //フラグの回収
    timeCounter1 = 0;
    //割り込みされたことを表示して確認
    Serial.println("count2trrigger");
  }
    
}


void setTimer(){

  //1us毎にカウンターのカウントアップ
  //第一引数：使用するタイマーの番号
  //第二引数：プリスケーラ(1us毎にカウントアップしたいのであれば80に設定(80MHzなので))
  //第三引数：割り込みカウンターのインクリメント指定（tue：カウントアップ、false：カウントダウン）
  timer1 = timerBegin(0, 80, true);
  timer3 = timerBegin(2, 80, true);

  // Attach onTimer function.
  //割り込み関数処理(ISR)の設定、ISR関数を割り込み処理に結び付ける
  //第一引数：初期化されたタイマー設定用のポインタ
  //第二引数：ISR関数のアドレス
  //第三引数：エッジ割り込み指定（true：エッジトリガー、 false：レベルトリガー）
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAttachInterrupt(timer3, &onTimer3, true);
  
  // Set alarm to call onTimer function every minute (value in microseconds).
  //タイマの動作間隔の指定（トリガー条件）
  //第一引数：タイマーへのポインタ
  //第二引数：割り込み発生までの時間（マイクロ秒で指定）
  //第三引数：カウンターのリロード指定（true:定期実行、false:1ショットの実行）
  timerAlarmWrite(timer1, (INTERVAL_SECONDS* 1000*1000), true);   //10秒毎
  timerAlarmWrite(timer3, (INTERVAL_SECONDS_3* 1000000), true);      //20秒毎
  
  // Start an alarm
  //タイマーの有効化　timerAlarmDisable関数でストップさせることもできる
  //引数：タイマー変数
  timerAlarmEnable(timer1);
  
}
