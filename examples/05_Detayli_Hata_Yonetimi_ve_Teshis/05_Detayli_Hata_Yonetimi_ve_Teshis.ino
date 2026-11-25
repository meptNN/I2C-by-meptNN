/*
  05 - Detayli Hata Yonetimi ve Teshis (Troubleshooting)
  I2C by meptNN

  Normal akista hata kontrolu zorunlu degildir; ancak sahada veya
  prototiplemede sorun yasandiginda hatanin kaynagini (pull-up eksikligi,
  yanlis cihaz adresi, veri reddi) tespit etmek icin kullanilir.
*/

#include <i2c_meptnn.h>

BenimI2C i2c(2, 3); // SDA = Pin 2, SCL = Pin 3
const uint8_t HEDEF_ADRES = 0x68;

void hataRaporla(Error h) {
  Serial.print(F("[HATA] "));
  Serial.println(BenimI2C::aciklama(h));

  if (h == Error::Timeout) {
    Serial.println(F("-> Cozum: SDA/SCL hatlarina 4.7k ohm pull-up direnci baglayin."));
  } else if (h == Error::AddressNack) {
    Serial.println(F("-> Cozum: Cihaz adresi yanlis veya kablo gevsek. Hat taraniyor:"));
    i2c.scan(Serial);
  } else if (h == Error::DataNack) {
    Serial.println(F("-> Cozum: Cihaz adresi dogru ancak gonderilen register/veri reddedildi."));
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // 1. Hat fiziksel kontrolu
  Error durum = i2c.begin();
  if (durum != Error::Ok) {
    hataRaporla(durum);
    while (1);
  }

  // 2. Haberlesme kontrolu
  uint8_t veri = 0;
  durum = i2c.readReg(HEDEF_ADRES, 0x00, veri);
  if (durum != Error::Ok) {
    hataRaporla(durum);
  } else {
    Serial.print(F("Basarili! Okunan veri: 0x"));
    Serial.println(veri, HEX);
  }
}

void loop() {
}
