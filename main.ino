
#include <Wire.h>
#include <Adafruit_CCS811.h>
#include <Adafruit_SHT31.h>

// ================== CẤU HÌNH ==================
#define I2C_SDA     6
#define I2C_SCL     5

#define LDR_PIN     2        // ADC LDR
#define DUST_ADC    3        // ADC DUST (chung chân theo mạch bạn)
#define LED_DUST   18     // GPIO điều khiển LED bụi

Adafruit_CCS811 ccs;
Adafruit_SHT31 sht30 = Adafruit_SHT31();

#define DUST_V0 0.5  // chỉnh sau khi đo

float readDustGP2Y() {
  digitalWrite(LED_DUST, LOW);
  delayMicroseconds(280);

  int adc = analogRead(DUST_ADC);

  delayMicroseconds(40);
  digitalWrite(LED_DUST, HIGH);
  delayMicroseconds(9680);

  float voltage = adc * (3.3 / 4095.0);
  float dust = (voltage - DUST_V0) * 1000.0 / 0.5;

  if (dust < 0) dust = 0;
  return dust;
}


// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(I2C_SDA, I2C_SCL);

  pinMode(LED_DUST, OUTPUT);
  digitalWrite(LED_DUST, HIGH);   // LED OFF

  // ===== SHT30 =====
  if (!sht30.begin(0x44)) {
    Serial.println("❌ Không tìm thấy SHT30");
    while (1);
  }
  Serial.println("✅ SHT30 OK");

  // ===== CCS811 =====
  if (!ccs.begin()) {
    Serial.println("❌ Không tìm thấy CCS811");
    while (1);
  }
  while (!ccs.available()) delay(100);
  Serial.println("✅ CCS811 OK");

  analogReadResolution(12);
}

// ================== LOOP ==================
void loop() {
  // ===== SHT30 =====
  float temperature = sht30.readTemperature();
  float humidity    = sht30.readHumidity();

  Serial.print("🌡 ");
  Serial.print(temperature);
  Serial.print(" °C | 💧 ");
  Serial.print(humidity);
  Serial.println(" %");

  // ===== CCS811 =====
  if (ccs.available() && !ccs.readData()) {
    Serial.print("🫁 eCO2: ");
    Serial.print(ccs.geteCO2());
    Serial.print(" ppm | TVOC: ");
    Serial.print(ccs.getTVOC());
    Serial.println(" ppb");
  }

  // ===== GP2Y1010 =====
  float dust = readDustGP2Y();
  Serial.print("🌫 Bụi PM: ");
  Serial.print(dust, 1);
  Serial.println(" ug/m3");

  // ===== LDR =====
  int ldrRaw = analogRead(LDR_PIN);
  float ldrPercent = (ldrRaw / 4095.0) * 100.0;

  Serial.print("💡 LDR: ");
  Serial.print(ldrRaw);
  Serial.print(" (");
  Serial.print(ldrPercent, 1);
  Serial.println(" %)");

  Serial.println("--------------------------");
  delay(2000);
}
