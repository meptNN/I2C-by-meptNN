/*
  07 - Turkce Sade API Kullanimi (i2c_meptnn_tr.h)
  I2C by meptNN

  Orijinal kutuphane API'leri hic degistirilmeden, Turkce ekosistem icin
  eklenen i2c_meptnn_tr.h baslik dosyasi ile kisa ve tamamen Turkce kod yazimi.
*/

#include <i2c_meptnn_tr.h>

I2C_TR i2c(2, 3); // SDA = Pin 2, SCL = Pin 3

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Baslatma ve istege bagli Turkce hata kontrolu
  Hata durum = i2c.baslat();
  if (durum != Tamam) {
    Serial.println(I2C_TR::aciklama(durum));
    while (1);
  }

  // Hattaki cihazlari tara (varsayilan olarak Serial'a yazar)
  i2c.tara();

  // Tek bayt yaz ve oku
  i2c.yaz(0x68, 0x6B, 0x00);

  uint8_t kimlik = 0;
  if (i2c.oku(0x68, 0x75, kimlik) == Tamam) {
    Serial.print(F("Cihaz Kimligi: 0x"));
    Serial.println(kimlik, HEX);
  }

  // Coklu bayt oku
  uint8_t tampon[6];
  i2c.oku(0x68, 0x3B, tampon, 6);
}

void loop() {
}
