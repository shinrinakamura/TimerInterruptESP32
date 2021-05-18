# TimerInterruptESP32<br>

## Timer_Interrupt_fizzbuzz.ino
これはESP32のハードウェアタイマーのタイマー割り込みを試すプログラムです。<br>
だいたい1秒ごとにカウントアップしていきます<br>
三の倍数の時にあほになって<br>
5の倍数の時犬になります<br>
15の倍数の時はFizzBuzzと表示します<br>
1分経過ごとに経過した分数を教えてくれます。<br>
ダウンロードしたプログラムはArduinoIDEを利用してESP32に書き込むことができます。

## publishMQTT_timer_interrupt.ino<br>
これはESP32のハードウェアタイマー割り込みで少し重めの処理を行うテストプログラムです<br>
だいたい1秒ごとにカウントアップしていきます<br>
指定した時間ごとにタイマー割り込みを行いMQTTを送信します<br>
ダウンロードしたプログラムはArduinoIDEを利用してESP32に書き込むことができますが<br>
mqtt用のライブラリが必要になります<br>
おそらくこのライブラリで動きます（未試験）
https://github.com/knolleary/pubsubclient<br>

## multithreadInterrupt_simple<br>
これはESP32のハードウェアタイマーと、マルチスレッドを同時に行うテストプログラムです。<br>
メインループでは大体1秒に1回カウントアップして、10秒に1回タイマー割り込みが入ります。<br>
マルチスレッドは大体5秒に1回カウントアップして、20秒に1回タイマー割り込みが入ります。<br>
使用する際はタイマーの指定に気を付けてください。特にtimerbeginの際に指定を忘れがちなので注意します。<br>
サンプルコードはそのままビルド、書き込みをすると動くようになっています。<br>

## English explanation<br>
This is a program that tries the timer interrupt of the ESP32 hardware timer<br>
Counts up almost every second<br>
It's stupid when it's a multiple of 3<br>
When it becomes a multiple of 5, it becomes a dog<br>
If it is a multiple of 15, it will be displayed as FizzBuzz.<br>
The reason why saaaaaaaaaaaaaaaaaaaaaaaaaaaan is stupid<br>
It's a Japanese joke<br>
The reason why one is dog<br>
That's because the one sound is the barking of a Japanese dog<br>
That's because the "one" sound is the barking of a Japanese dog<br>
It will tell you how many minutes have passed every minute<br>
The downloaded program can be written to the ESP32 using the Arduino IDE

