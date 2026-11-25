#ifndef I2C_MEPTNN_H
#define I2C_MEPTNN_H

#include <Arduino.h>

// Hata kodlari: her fonksiyon sonucunu bunlardan biri olarak dondurur.
// "enum class" oldugu icin kullanimda Error::Ok seklinde yazilir;
// boylece baska kutuphanelerdeki Ok, Timeout gibi isimlerle cakismaz.
enum class Error : uint8_t {
  Ok = 0,
  AddressNack,   // cihaz yok ya da adres yanlis
  DataNack,      // cihaz veriyi kabul etmedi
  Timeout        // SCL hatti serbest kalmadi
};

class BenimI2C {
public:
  // --- Kullanicinin gordugu yuzey (API) ---
  BenimI2C(uint8_t sdaPin, uint8_t sclPin);
  Error begin();
  Error readReg(uint8_t adres, uint8_t reg, uint8_t &deger);
  Error writeReg(uint8_t adres, uint8_t reg, uint8_t deger);
  Error readReg(uint8_t adres, uint8_t reg, uint8_t *tampon, uint8_t uzunluk);
  Error writeReg(uint8_t adres, uint8_t reg, const uint8_t *tampon, uint8_t uzunluk);
  uint8_t scan(Print &p);   // bulunan adresleri p'ye yazar, bulunan sayiyi dondurur

  // Hatanin Turkce aciklamasini dondurur. Hicbir yere yazdirmaz:
  // Serial.println(BenimI2C::aciklama(h));
  static const __FlashStringHelper* aciklama(Error e);

private:
  // --- Uye degiskenler ---
  uint8_t  _sda;
  uint8_t  _scl;
  uint8_t  _yarimPeriyot;   // mikrosaniye
  uint32_t _zamanAsimiUs;   // mikrosaniye

  // --- Hat kontrolu (open-drain mantigi) ---
  void sdaBirak();
  void sdaCek();
  void sclBirak();
  void sclCek();
  Error sclBirakBekle();

  // --- Protokol ---
  Error iletisimStart();
  Error iletisimTekrarStart();
  Error iletisimStop();

  // --- Bit / bayt ---
  Error bitGonder(uint8_t bit);
  Error bitOku(uint8_t &bit);
  Error ackOku();
  Error baytGonder(uint8_t bayt);
  Error baytOku(uint8_t &bayt, bool ackVer);

  // --- readReg / writeReg'in ic fonksiyonlari ---
  Error yazIc(uint8_t adres, uint8_t reg, uint8_t deger);
  Error okuIc(uint8_t adres, uint8_t reg, uint8_t &deger);
  Error yazCokIc(uint8_t adres, uint8_t reg, const uint8_t *tampon, uint8_t uzunluk);
  Error okuCokIc(uint8_t adres, uint8_t reg, uint8_t *tampon, uint8_t uzunluk);

  // --- scan'in yardimcisi: tek adrese seslenir ---
  Error adresDene(uint8_t adres);
};

#endif
