/*
  03 - Tek Bayt Okuma ve Yazma (Single-Byte Register Read/Write)
  I2C by meptNN

  Bir I2C cihazinin belirli bir register'ina ayar yazma ve
  register'dan 1 bayt veri okuma ornegi (Ornek: MPU6050 / DS3231).
*/

#include <i2c_meptnn.h>

MeptnnI2C i2c(2, 3); // SDA = Pin 2, SCL = Pin 3

const uint8_t CIHAZ_ADRESI = 0x68;
const uint8_t GUC_REG      = 0x6B;
const uint8_t KIMLIK_REG   = 0x75;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  i2c.begin();

  // Cihaza konfigurasyon bayti yaz
  i2c.writeReg(CIHAZ_ADRESI, GUC_REG, 0x00);
  delay(10);

  // Cihazdan 1 bayt oku
  uint8_t okunan = 0;
  Error sonuc = i2c.readReg(CIHAZ_ADRESI, KIMLIK_REG, okunan);

  if (sonuc == Error::Ok) {
    Serial.print(F("Register 0x75 degeri: 0x"));
    Serial.println(okunan, HEX);
  } else {
    Serial.print(F("Islem basarisiz: "));
    Serial.println(MeptnnI2C::aciklama(sonuc));
  }
}

void loop() {
}
