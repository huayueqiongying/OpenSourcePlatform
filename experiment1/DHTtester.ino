// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
//
// 修改说明：
// 1. 温度变化 → LED 闪烁快慢（以开机第一次读数为基准，升温闪快、降温闪慢）
// 2. 串口输出传感器数据前，先输出学号

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"

#define DHTPIN 7     // 数据脚接 D7
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#define LEDPIN 13         // 指示灯：D13 是 UNO 板上自带的 LED
#define STUDENT_ID 23301155  // 学号（想只输出后四位就把这里改成 1155）

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

const unsigned long READ_INTERVAL = 2000;  // 每 2 秒读一次传感器
unsigned long lastRead = 0;

unsigned long lastBlink = 0;
bool ledState = false;
unsigned long blinkInterval = 800;  // 基准闪烁间隔（毫秒）

// 灵敏度：温度每变化 0.1°C，闪烁间隔调整多少毫秒（数值越大越灵敏）
const float MS_PER_01C = 100.0;

float referenceTemp = -1000;  // 基准温度，开机第一次读数时自动记录

void setup() {
  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  Serial.println(F("DHTxx test!"));
  dht.begin();
}

void loop() {
  unsigned long now = millis();

  // —— 每 2 秒读一次传感器，更新闪烁速度 ——
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      // 读不到数据：保持固定慢闪，串口会提示检查型号/接线
      Serial.println(F("Failed to read from DHT sensor! 检查型号/接线"));
      blinkInterval = 1000;
    } else {
      // 第一次成功读数作为基准温度
      if (referenceTemp < -900) referenceTemp = t;

      // 温差：按 MS_PER_01C 灵敏度调整闪烁速度（升温闪快、降温闪慢）
      float delta = t - referenceTemp;
      blinkInterval = (unsigned long)constrain(800.0 - delta * 10.0 * MS_PER_01C, 50.0, 2000.0);

      // 先输出学号，再输出传感器数据
      Serial.print(F("Student ID: "));
      Serial.println(STUDENT_ID);

      Serial.print(F("基准="));
      Serial.print(referenceTemp);
      Serial.print(F("°C  当前="));
      Serial.print(t);
      Serial.print(F("°C  温差="));
      Serial.print(delta);
      Serial.print(F("°C  闪烁间隔="));
      Serial.print(blinkInterval);
      Serial.println(F(" ms"));

      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.println(F("%"));
    }
  }

  // —— LED 闪烁（非阻塞，不卡住读传感器）——
  if (now - lastBlink >= blinkInterval) {
    lastBlink = now;
    ledState = !ledState;
    digitalWrite(LEDPIN, ledState);
  }
}
