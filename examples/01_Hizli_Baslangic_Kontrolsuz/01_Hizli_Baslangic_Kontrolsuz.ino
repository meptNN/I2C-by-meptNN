/*
  01 - Hizli Baslangic (Hata Kontrolsuz Sade Kullanim)
  I2C by meptNN

  Hata kontrolu yapmadan kutuphanenin en basit kullanimi.
  Not: SDA ve SCL hatlarinda harici 4.7k ohm pull-up direnci bulunmalidir.
*/

#include <i2c_meptnn.h>

BenimI2C i2c(2, 3); // SDA = Pin 2, SCL = Pin 3

void setup() {
  Serial.begin(9600);
  i2c.begin();

  // 1. Hattaki cihazlari tara
  i2c.scan(Serial);

  // 2. Register'a tek bayt yaz (ornek: sensoru uyku modundan cikar)
  i2c.writeReg(0x68, 0x6B, 0x00);

  // 3. Register'dan tek bayt oku
  uint8_t kimlik = 0;
  i2c.readReg(0x68, 0x75, kimlik);
  Serial.print(F("Okunan kimlik degeri: 0x"));
  Serial.println(kimlik, HEX);

  // 4. Coklu bayt oku
  uint8_t tampon[6];
  i2c.readReg(0x68, 0x3B, tampon, 6);
}

void loop() {
}
