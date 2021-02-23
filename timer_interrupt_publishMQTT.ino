/*ESP32のタイマー割り込みをしてMQTTを送信するシンプルなプログラム
 * 
 * 大体1秒間隔でカウントアップしていって
 * 1分間隔でタイマ割り込みをしてMQTTを送信するシンプルなプログラム
 * 
 * Wi-Fiとサーバーは自分で設定する必要があります。
 * 
 * タイマについて詳しくは
 * https://github.com/shinrinakamura/TimerInterruptESP32/blob/main/Timer_Interrupt_fizzbuzz.ino
 * を参照する
 * 
 * @author S.Nakamura 2021-02-23
*/


//送信関係のライブラリの読み込み
#include <WiFi.h>
#include <PubSubClient.h>

//ssidとパスワードの設定
const char *ssid = "your_ssid";
const char *password = "your_pass";

// Pub/Subの設定
// Pub/Sub
const char* mqttHost = "your_server_domain";                  //ipアドレスかドメインで指定する
const int mqttPort = 1883;                                    //通常は1883か8883

//mqttのユーザー認証を使用する際に使用する
//const char* mqtt_username = "your name";                //ユーザーネーム
//const char* mqtt_password = "your publish password";    //mqttのパスワード

const char* topic = "your_topic_name";                         // 送信するトピック名（変更）
//char* payload;                                               // 送信するデータ

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
//ここまで通信関係の設定--------------------------------------------------------------

//割り込みの設定-------------------------------------------------------------
volatile int timeCounter1;
uint8_t time_flg =0;    //時間フラグの初期化
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t *timer1 = NULL;    //ハードウェアタイマの準備
int iIntervalTime =1;   //割り込みの間隔（分で設定）

//割り込み処理
void IRAM_ATTR onTimer1(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  timeCounter1++;
  portEXIT_CRITICAL_ISR(&timerMux);
}
//ここまで割り込みの設定---------------------------------------------------------

//プロトタイプ宣言
void connectWiFi();
void connectMqtt();
void MqttPublish(const char *payload);


void setup() {

  //PCとの通信を始める
  //引数：ボーレート
  Serial.begin(115200);

  //1us毎にカウンターのカウントアップ
  timer1 = timerBegin(0, 80, true);

  // Attach onTimer function.
  timerAttachInterrupt(timer1, &onTimer1, true);
  // Set alarm to call onTimer function every minute (value in microseconds).
  timerAlarmWrite(timer1, (iIntervalTime * 1000000*60), true);
  // Start an alarm
  timerAlarmEnable(timer1);
  
  //Ｗｉ-Ｆｉの接続確認
  connectWiFi();
  //mQTTサーバーへの接続確認
  connectMqtt();
  //MQTT送信確認
  //引数：送信したい文字の配列(const char)
  MqttPublish("start");
}

//1秒ごとにカウントアップするためのカウンター
int timer;
int old_time =0;
int n = 0;  

void loop() {
  
    //テストとして実行しておく処理
    //1秒ごとにカウントアップする
    timer = millis();
    if((timer - old_time) >= 1000){
      n = n +1;
      Serial.print("count is ");
      Serial.println(n);
      
    //計測時間の基準作成
    old_time = timer; 
    }
    
    //フラグを確認してMQTTの送信 
    if (timeCounter1 > 0) {

      //mqtを送信する
      //引数：送信したい文字の配列(const char)
      MqttPublish("test");
     
      //フラグを戻す
      timeCounter1 = 0;
    }
}


//通信関係の処理
//--------------------------------------------------------
//Wi-Fiを接続する
void connectWiFi(){

  //Wi-Fiのアクセスポイントに接続
  //第一引数：ssid
  //第二引数：パスワード      
  WiFi.begin(ssid, password);
  Serial.print("WiFi connecting...");

  int i = 0 ;   //接続確認の時間
  //Wi-Fiの接続確認
  while(WiFi.status() != WL_CONNECTED) {
  i += 1;
  Serial.print(".");
  delay(1000);
  //10秒間Wi-Fiが接続できないときは接続をやり直す
  if (i == 10){
    Serial.println("WiFi reset");
    connectWiFi();
      }
  }
  //Wi-Fiの接続が確認出来たらコンソールに表示して確認する      
  Serial.print(" connected. ");
  Serial.println(WiFi.localIP());

}

//MQTTの接続
//mqttに接続する関数
void connectMqtt(){
  
  //Wi-Fiの接続確認
  while(WiFi.status() != WL_CONNECTED) {
    
  Serial.print("WiFi is not connect");
  connectWiFi();

  }
  
  //brokerサーバーに接続する
  //第一引数：ブローカーサーバーのドメインもしくはipアドレス
  //第二引数：接続するポート（通常は1883か8883）
  mqttClient.setServer(mqttHost, mqttPort);

  //clientIDを作成してサーバーに接続する
  while( ! mqttClient.connected() ) {

    Serial.println("Connecting to MQTT...");
    
    //MacアドレスからクライアントIDを作成する
    String clientId = "ESP32-" +  getMacAddr();
    //確認用の表示
    Serial.print("clientID : "); 
    Serial.println(clientId.c_str()); 
    
    //接続の確認
    //if ( mqttClient.connect(clientId.c_str(),mqtt_username, mqtt_password) ) {  //ユーザー認証を行う時はこちらを利用する
    if ( mqttClient.connect(clientId.c_str())) { 
      Serial.println("connected"); 

    }
  }
}

//Mqttの送信
void MqttPublish(const char *payload){

  //mqttの接続を確認
  while( ! mqttClient.connected() ) {

    Serial.println("Mqtt is not connect");
    connectMqtt();

    }
  
  
  
  //mqttの送信
  //第一引数：トピック名
  //第二引数：送信するデータ
  mqttClient.publish(topic, payload);
  //コンソールに送信するデータを表示して確認
  Serial.print("published ");
  Serial.println(payload);

}

// Macアドレスを文字列で取得する
String getMacAddr()
{
    byte mac[6];
    char buf[50];
    WiFi.macAddress(mac);
    sprintf(buf, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
}
