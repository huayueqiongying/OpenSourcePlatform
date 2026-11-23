// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
//
// 修改说明：
// 1. 温度变化 → LED 闪烁快慢（温度越高闪得越快，越低闪得越慢）
// 2. 串口输出传感器数据前，先输出学号

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"

#define DHTPIN 7     // Digital pin connected to the DHT sensor（数据脚接 D7）
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#define LEDPIN 13         // 指示灯：D13 是 UNO 板上自带的 LED
#define STUDENT_ID 23301155  // 学号（想只输出后四位就把这里改成 1155）

// 温度映射范围：TEMP_MIN 时闪得最慢，TEMP_MAX 时闪得最快
const float TEMP_MIN = 15.0;
const float TEMP_MAX = 35.0;

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

const unsigned long READ_INTERVAL = 2000;  // 每 2 秒读一次传感器
unsigned long lastRead = 0;

unsigned long lastBlink = 0;
bool ledState = false;
unsigned long blinkInterval = 1000;  // 闪烁间隔，单位毫秒

void setup() {
  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  Serial.println(F("DHTxx test!"));

  dht.begin();
}

void loop() {
  unsigned long now = millis();

  // —— 每 2 秒读一次传感器，并更新闪烁速度 ——
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
    } else {
      // 温度越高 → 间隔越小（闪得越快）；温度越低 → 间隔越大（闪得越慢）
      float temp = constrain(t, TEMP_MIN, TEMP_MAX);
      blinkInterval = map((long)(temp * 10),
                          (long)(TEMP_MIN * 10), (long)(TEMP_MAX * 10),
                          1000, 100);  // 1000ms ~ 100ms

      // 先输出学号，再输出传感器数据
      Serial.print(F("Student ID: "));
      Serial.println(STUDENT_ID);

      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.println(F("°C"));

      // 把当前温度和对应的闪烁间隔一起打印，方便观察
      Serial.print(F("blink interval: "));
      Serial.print(blinkInterval);
      Serial.println(F(" ms"));
    }
  }

  // —— LED 闪烁（非阻塞，不卡住读传感器）——
  if (now - lastBlink >= blinkInterval) {
    lastBlink = now;
    ledState = !ledState;
    digitalWrite(LEDPIN, ledState);
  }
}
