#include "dht11.h"
dht11 DHT11;
#define DHT11PIN 5
#include <IRremote.h>
int RECV_PIN = 3;   //红外线接收器OUTPUT端接在pin 10
IRrecv irrecv(RECV_PIN);  //定义IRrecv对象来接收红外线信号
decode_results results;   //解码结果放在decode_results构造的对象results里

// 接 74HC595 的 ST_CP
int latchPin1 = 4;
int latchPin2 = 7;
int latchPin3 = 8;
// 接 74HC595 的 SH_CP
int clockPin1 = 6;
int clockPin2 = 12;
int clockPin3 = 13;
// 接 74HC595 的 DS
int dataPin1 = 9;
int dataPin2 = 10;
int dataPin3 = 11;

//分别将十位数和个位数转换为二进制
int tens(int num)
{
  int result;
  result = 1 << num / 10;
  if (num / 10 == 0) result = 1 << 11;
  else if (num / 10>7) result <<= 1;
  return result;
}

int single(int num)
{
  int result;
  result = 1 << num % 10;
  if (num % 10 == 0) result = 1 << 11;
  else if (num % 10>7) result <<= 1;
  else if (num % 10 == 1) result = 1 << 1;
  return result;
}

void setup() {
  Serial.begin(9600);
 irrecv.enableIRIn(); // 启动红外解码
  // 將 latchPin, clockPin, dataPin 設置為輸出
  pinMode(latchPin1, OUTPUT);
  pinMode(latchPin2, OUTPUT);
  pinMode(latchPin3, OUTPUT);
  pinMode(clockPin1, OUTPUT);
  pinMode(clockPin2, OUTPUT);
  pinMode(clockPin3, OUTPUT);
  pinMode(dataPin1, OUTPUT);
  pinMode(dataPin2, OUTPUT);
  pinMode(dataPin3, OUTPUT);
}

void loop() {

  int chk=DHT11.read(DHT11PIN);

  for(int HR=0;HR<24;HR++){
  digitalWrite(latchPin1, LOW);

  shiftOut(dataPin1, clockPin1, MSBFIRST, (single(HR) >> 8));//将三十二位的数据分四次输入移位寄存器
  shiftOut(dataPin1, clockPin1, MSBFIRST, single(HR));
  shiftOut(dataPin1, clockPin1, MSBFIRST, (tens(HR) >> 8));
  shiftOut(dataPin1, clockPin1, MSBFIRST, tens(HR));

  digitalWrite(latchPin1, HIGH);

  for(int MIN=0;MIN<60;MIN++){
  digitalWrite(latchPin2, LOW);

  shiftOut(dataPin2, clockPin2, MSBFIRST, (single(MIN) >> 8));
  shiftOut(dataPin2, clockPin2, MSBFIRST, single(MIN));
  shiftOut(dataPin2, clockPin2, MSBFIRST, (tens(MIN) >> 8));
  shiftOut(dataPin2, clockPin2, MSBFIRST, tens(MIN));

  digitalWrite(latchPin2, HIGH);
  if(MIN==59) {
    MIN=0;
    HR++;
  }

  for (int SEC = 0; SEC<60; SEC++) {
    digitalWrite(latchPin3, LOW);

    shiftOut(dataPin3, clockPin3, MSBFIRST, (single(SEC) >> 8));
    shiftOut(dataPin3, clockPin3, MSBFIRST, single(SEC));
    shiftOut(dataPin3, clockPin3, MSBFIRST, (tens(SEC) >> 8));
    shiftOut(dataPin3, clockPin3, MSBFIRST, tens(SEC));

    digitalWrite(latchPin3, HIGH);

   

    if (irrecv.decode(&results)) {
      if(results.value==33456255)//接收到按键A的信号
          for(int N=0;N<10;N++) {
          Serial.println((float)DHT11.humidity,2);
          digitalWrite(latchPin3, LOW);
          shiftOut(dataPin3, clockPin3, MSBFIRST,(single(DHT11.humidity)>>8));
          shiftOut(dataPin3, clockPin3, MSBFIRST,single(DHT11.humidity)); 
          shiftOut(dataPin3, clockPin3, MSBFIRST,(tens(DHT11.humidity)>>8)); 
          shiftOut(dataPin3, clockPin3, MSBFIRST,tens(DHT11.humidity));
          digitalWrite(latchPin3, HIGH);
          irrecv.resume();
          delay(1000);
          SEC++;
          if(SEC==60)
          {
            SEC=0;
            MIN++;
          }
       }
      if (results.value == 33439935)//接收到按键B的信号
          for(int N=0;N<10;N++) {
          Serial.println((float)DHT11.temperature,2);
          digitalWrite(latchPin3, LOW);
          shiftOut(dataPin3, clockPin3, MSBFIRST,(single(DHT11.temperature)>>8));
          shiftOut(dataPin3, clockPin3, MSBFIRST,single(DHT11.temperature)); 
          shiftOut(dataPin3, clockPin3, MSBFIRST,(tens(DHT11.temperature)>>8)); 
          shiftOut(dataPin3, clockPin3, MSBFIRST,tens(DHT11.temperature));
          digitalWrite(latchPin3, HIGH);
          irrecv.resume();
          delay(1000);
          SEC++;
          if(SEC==60)
          {
            SEC=0;
            MIN++;
          }
          }
        }
    
    delay(1000);
  }}}


}
