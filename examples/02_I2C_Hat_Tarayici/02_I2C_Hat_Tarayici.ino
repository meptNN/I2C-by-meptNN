/*
  02 - I2C Hat Tarayici (Bus Scanner)
  I2C by meptNN

  0x08 ile 0x77 arasindaki tum gecerli 7-bit I2C adreslerini tarar
  ve yanit veren (ACK gonderen) cihazlari Seri Port ekranina yazar.
*/

#include <i2c_meptnn.h>

MeptnnI2C i2c(2, 3); // SDA = Pin 2, SCL = Pin 3

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println(F("I2C by meptNN - Hat Tarayici Baslatiliyor..."));

  if (i2c.begin() != Error::Ok) {
    Serial.println(F("HATA: SCL hatti yukselmiyor! 4.7k pull-up direnclerini kontrol edin."));
    while (1);
  }

  uint8_t bulunanSayisi = i2c.scan(Serial);
  Serial.print(F("Tarama tamamlandi. Bulunan cihaz sayisi: "));
  Serial.println(bulunanSayisi);
}

void loop() {
  delay(5000);
  Serial.println(F("--- Yeniden Taraniyor ---"));
  i2c.scan(Serial);
}
