/*
  04 - Coklu Bayt Tampon (Buffer) Okuma ve Yazma
  I2C by meptNN

  Ardisik register'lara dizi (tampon) ile veri yazma ve
  tek bir I2C cercevesinde cok sayida bayt okuma ornegi.
*/

#include <i2c_meptnn.h>

MeptnnI2C i2c(2, 3); // SDA = Pin 2, SCL = Pin 3

const uint8_t CIHAZ_ADRESI  = 0x68;
const uint8_t BASLANGIC_REG = 0x3B; // Ornek: 6 baytlik ivme sensoru verisi

void setup() {
  Serial.begin(9600);
  while (!Serial);

  i2c.begin();

  // 1. Coklu bayt yazma (2 bayt ardisik ayar)
  uint8_t ayarlar[2] = {0x07, 0x00};
  i2c.writeReg(CIHAZ_ADRESI, 0x19, ayarlar, 2);
}

void loop() {
  uint8_t tampon[6] = {0};

  // 6 bayti tek seferde oku (ilk 5 bayta ACK, son bayta NACK gonderilir)
  if (i2c.readReg(CIHAZ_ADRESI, BASLANGIC_REG, tampon, 6) == Error::Ok) {
    int16_t x = (tampon[0] << 8) | tampon[1];
    int16_t y = (tampon[2] << 8) | tampon[3];
    int16_t z = (tampon[4] << 8) | tampon[5];

    Serial.print(F("X: ")); Serial.print(x);
    Serial.print(F(" | Y: ")); Serial.print(y);
    Serial.print(F(" | Z: ")); Serial.println(z);
  }

  delay(500);
}
