#ifndef I2C_MEPTNN_TR_H
#define I2C_MEPTNN_TR_H

#include "i2c_meptnn.h"

// Turkce hata tipi ve sabit takma adlari (sifir bellek maliyeti)
using Hata = Error;

constexpr Error Tamam      = Error::Ok;
constexpr Error AdresYok   = Error::AddressNack;
constexpr Error VeriReddi  = Error::DataNack;
constexpr Error ZamanAsimi = Error::Timeout;

// Orijinal MeptnnI2C sinifini degistirmeden Turkce kisa API sunan sarmalayici
class I2C_TR : public MeptnnI2C {
public:
  I2C_TR(uint8_t sdaPin, uint8_t sclPin) : MeptnnI2C(sdaPin, sclPin) {}

  inline Hata baslat() {
    return begin();
  }

  inline Hata oku(uint8_t adres, uint8_t reg, uint8_t &deger) {
    return readReg(adres, reg, deger);
  }

  inline Hata yaz(uint8_t adres, uint8_t reg, uint8_t deger) {
    return writeReg(adres, reg, deger);
  }

  inline Hata oku(uint8_t adres, uint8_t reg, uint8_t *tampon, uint8_t uzunluk) {
    return readReg(adres, reg, tampon, uzunluk);
  }

  inline Hata yaz(uint8_t adres, uint8_t reg, const uint8_t *tampon, uint8_t uzunluk) {
    return writeReg(adres, reg, tampon, uzunluk);
  }

  inline uint8_t tara(Print &p) {
    return scan(p);
  }

  inline uint8_t tara() {
    return scan(Serial);
  }
};

#endif
