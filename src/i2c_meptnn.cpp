#include "i2c_meptnn.h"

// ---------------------------------------------------------------
// Constructor, begin, aciklama
// ---------------------------------------------------------------
BenimI2C::BenimI2C(uint8_t sdaPin, uint8_t sclPin) {
  _sda = sdaPin;
  _scl = sclPin;
  _yarimPeriyot = 5;      // mikrosaniye (yaklasik 100 kHz hedefi)
  _zamanAsimiUs = 1000;   // mikrosaniye
}

// Hatlari hazirlar ve SCL'nin yukselebildigini dener.
// SCL yukselmezse (pull-up yok, kablo kopuk, hat GND'ye kisa) Timeout doner.
Error BenimI2C::begin() {
  // Once PORT bitlerini LOW'a hazirla (pinler henuz INPUT iken),
  // sonra hatlari birak: boylece "cek" dedigimizde pin hemen LOW surer.
  digitalWrite(_sda, LOW);
  digitalWrite(_scl, LOW);
  sdaBirak();
  delayMicroseconds(_yarimPeriyot);
  return sclBirakBekle();
}

// Metinler flash'ta tutulur (F makrosu), Uno'nun kisitli RAM'ini harcamaz.
const __FlashStringHelper* BenimI2C::aciklama(Error e) {
  switch (e) {
    case Error::Ok:          return F("Tamam");
    case Error::AddressNack: return F("Adres NACK: cihaz yok ya da adres yanlis");
    case Error::DataNack:    return F("Veri NACK: cihaz veriyi kabul etmedi");
    case Error::Timeout:     return F("Zaman asimi: SCL hatti serbest kalmadi (pull-up var mi?)");
  }
  return F("Bilinmeyen hata");
}

// ---------------------------------------------------------------
// Hat kontrolu (open-drain)
// ---------------------------------------------------------------
void BenimI2C::sdaBirak() { pinMode(_sda, INPUT); }
void BenimI2C::sdaCek()   { pinMode(_sda, OUTPUT); }
void BenimI2C::sclBirak() { pinMode(_scl, INPUT); }
void BenimI2C::sclCek()   { pinMode(_scl, OUTPUT); }

// SCL'yi birakir ve gercekten yukselmesini bekler (clock stretching).
// Sure dolarsa Timeout dondurur.
Error BenimI2C::sclBirakBekle() {
  sclBirak();
  uint32_t baslangic = micros();
  while (digitalRead(_scl) == LOW) {
    if (micros() - baslangic > _zamanAsimiUs) {
      return Error::Timeout;
    }
  }
  return Error::Ok;
}

// ---------------------------------------------------------------
// Protokol: start, repeated start, stop
// ---------------------------------------------------------------
Error BenimI2C::iletisimStart() {
  sdaCek();
  delayMicroseconds(_yarimPeriyot);
  sclCek();
  return Error::Ok;
}

Error BenimI2C::iletisimTekrarStart() {
  sdaBirak();
  delayMicroseconds(_yarimPeriyot);
  Error h = sclBirakBekle();
  if (h != Error::Ok) return h;
  delayMicroseconds(_yarimPeriyot);
  sdaCek();
  delayMicroseconds(_yarimPeriyot);
  sclCek();
  return Error::Ok;
}

Error BenimI2C::iletisimStop() {
  sdaCek();
  delayMicroseconds(_yarimPeriyot);
  Error h = sclBirakBekle();
  if (h != Error::Ok) return h;
  delayMicroseconds(_yarimPeriyot);
  sdaBirak();
  delayMicroseconds(_yarimPeriyot);
  return Error::Ok;
}

// ---------------------------------------------------------------
// Bit / bayt
// ---------------------------------------------------------------
Error BenimI2C::bitGonder(uint8_t bit) {
  if (bit == 1) {
    sdaBirak();
  } else {
    sdaCek();
  }
  delayMicroseconds(_yarimPeriyot);
  Error h = sclBirakBekle();
  if (h != Error::Ok) return h;
  delayMicroseconds(_yarimPeriyot);
  sclCek();
  return Error::Ok;
}

Error BenimI2C::bitOku(uint8_t &bit) {
  Error h = sclBirakBekle();
  if (h != Error::Ok) return h;
  delayMicroseconds(_yarimPeriyot);
  bit = digitalRead(_sda);
  sclCek();
  delayMicroseconds(_yarimPeriyot);
  return Error::Ok;
}

Error BenimI2C::ackOku() {
  sdaBirak();
  delayMicroseconds(_yarimPeriyot);
  Error h = sclBirakBekle();   // 9. clock
  if (h != Error::Ok) return h;
  delayMicroseconds(_yarimPeriyot);
  uint8_t deger = digitalRead(_sda);
  sclCek();                    // 9. clock bitti
  if (deger == 0) {
    return Error::Ok;          // ACK
  }
  return Error::DataNack;      // NACK
}

Error BenimI2C::baytGonder(uint8_t bayt) {
  for (int8_t i = 7; i >= 0; i--) {
    uint8_t bit = (bayt >> i) & 1;
    Error h = bitGonder(bit);
    if (h != Error::Ok) return h;
  }
  return ackOku();
}

Error BenimI2C::baytOku(uint8_t &bayt, bool ackVer) {
  uint8_t sonuc = 0;
  sdaBirak();
  for (int8_t i = 7; i >= 0; i--) {
    uint8_t b;
    Error h = bitOku(b);
    if (h != Error::Ok) return h;
    sonuc = (sonuc << 1) | b;
  }

  // 9. clock: master ACK ya da NACK verir
  if (ackVer) {
    sdaCek();     // ACK: "devam"
  } else {
    sdaBirak();   // NACK: "yeter"
  }
  delayMicroseconds(_yarimPeriyot);

  Error h = sclBirakBekle();
  if (h != Error::Ok) return h;
  delayMicroseconds(_yarimPeriyot);
  sclCek();
  delayMicroseconds(_yarimPeriyot);
  sdaBirak();

  bayt = sonuc;
  return Error::Ok;
}

// ---------------------------------------------------------------
// Ic fonksiyonlar: isi yapar, hata olursa erken cikar
// ---------------------------------------------------------------
Error BenimI2C::yazIc(uint8_t adres, uint8_t reg, uint8_t deger) {
  Error h = iletisimStart();
  if (h != Error::Ok) return h;

  h = baytGonder((adres << 1) | 0);   // yazma adresi
  if (h == Error::DataNack) h = Error::AddressNack;
  if (h != Error::Ok) return h;

  h = baytGonder(reg);
  if (h != Error::Ok) return h;

  h = baytGonder(deger);
  if (h != Error::Ok) return h;

  return Error::Ok;
}

Error BenimI2C::okuIc(uint8_t adres, uint8_t reg, uint8_t &deger) {
  Error h = iletisimStart();
  if (h != Error::Ok) return h;

  h = baytGonder((adres << 1) | 0);   // once yazma adresi
  if (h == Error::DataNack) h = Error::AddressNack;
  if (h != Error::Ok) return h;

  h = baytGonder(reg);                // hangi register
  if (h != Error::Ok) return h;

  h = iletisimTekrarStart();          // Sr: stop yok
  if (h != Error::Ok) return h;

  h = baytGonder((adres << 1) | 1);   // okuma adresi
  if (h == Error::DataNack) h = Error::AddressNack;
  if (h != Error::Ok) return h;

  h = baytOku(deger, false);          // tek bayt, NACK
  if (h != Error::Ok) return h;

  return Error::Ok;
}

// Cok baytli yazma: S, adres+W, reg, bayt0, bayt1, ... P
Error BenimI2C::yazCokIc(uint8_t adres, uint8_t reg, const uint8_t *tampon, uint8_t uzunluk) {
  Error h = iletisimStart();
  if (h != Error::Ok) return h;

  h = baytGonder((adres << 1) | 0);
  if (h == Error::DataNack) h = Error::AddressNack;
  if (h != Error::Ok) return h;

  h = baytGonder(reg);                // baslangic register'i
  if (h != Error::Ok) return h;

  for (uint8_t i = 0; i < uzunluk; i++) {
    h = baytGonder(tampon[i]);
    if (h != Error::Ok) return h;
  }
  return Error::Ok;
}

// Cok baytli okuma:
// S, adres+W, reg, Sr, adres+R, bayt0(ACK), ..., sonBayt(NACK), P
Error BenimI2C::okuCokIc(uint8_t adres, uint8_t reg, uint8_t *tampon, uint8_t uzunluk) {
  if (uzunluk == 0) return Error::Ok;   // okunacak bir sey yok

  Error h = iletisimStart();
  if (h != Error::Ok) return h;

  h = baytGonder((adres << 1) | 0);
  if (h == Error::DataNack) h = Error::AddressNack;
  if (h != Error::Ok) return h;

  h = baytGonder(reg);
  if (h != Error::Ok) return h;

  h = iletisimTekrarStart();
  if (h != Error::Ok) return h;

  h = baytGonder((adres << 1) | 1);
  if (h == Error::DataNack) h = Error::AddressNack;
  if (h != Error::Ok) return h;

  for (uint8_t i = 0; i < uzunluk; i++) {
    bool sonBayt = (i == uzunluk - 1);
    h = baytOku(tampon[i], !sonBayt);   // son bayt haric hepsine ACK
    if (h != Error::Ok) return h;
  }
  return Error::Ok;
}

// ---------------------------------------------------------------
// Kullanicinin gordugu fonksiyonlar: sonuc ne olursa olsun stop atar
// ---------------------------------------------------------------
Error BenimI2C::writeReg(uint8_t adres, uint8_t reg, uint8_t deger) {
  Error h = yazIc(adres, reg, deger);
  iletisimStop();
  return h;
}

Error BenimI2C::readReg(uint8_t adres, uint8_t reg, uint8_t &deger) {
  Error h = okuIc(adres, reg, deger);
  iletisimStop();
  return h;
}

Error BenimI2C::writeReg(uint8_t adres, uint8_t reg, const uint8_t *tampon, uint8_t uzunluk) {
  Error h = yazCokIc(adres, reg, tampon, uzunluk);
  iletisimStop();
  return h;
}

Error BenimI2C::readReg(uint8_t adres, uint8_t reg, uint8_t *tampon, uint8_t uzunluk) {
  Error h = okuCokIc(adres, reg, tampon, uzunluk);
  iletisimStop();
  return h;
}

// ---------------------------------------------------------------
// Tarama: 0x08-0x77 arasindaki her adrese seslenir
// ---------------------------------------------------------------

// Tek adrese seslenir. Ok ise cihaz ACK verdi (cihaz var).
// Sonuc ne olursa olsun stop atilir, hat temiz kalir.
Error BenimI2C::adresDene(uint8_t adres) {
  Error h = iletisimStart();
  if (h == Error::Ok) {
    h = baytGonder((adres << 1) | 0);   // yazma adresi
    if (h == Error::DataNack) h = Error::AddressNack;
  }
  iletisimStop();
  return h;
}

// Bulunan adresleri p'ye yazar (Serial ya da LCD olabilir).
// Dondurdugu deger: bulunan cihaz sayisi.
uint8_t BenimI2C::scan(Print &p) {
  uint8_t bulunan = 0;
  for (uint8_t adres = 0x08; adres <= 0x77; adres++) {
    if (adresDene(adres) == Error::Ok) {
      p.print("Bulundu: 0x");
      p.println(adres, HEX);
      bulunan++;
    }
  }
  if (bulunan == 0) {
    p.println("Cihaz bulunamadi");
  }
  return bulunan;
}
