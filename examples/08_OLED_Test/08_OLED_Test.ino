/*
  OLED (SH1106 / SSD1306 uyumlu, adres 0x3C) test - I2C by meptNN
  Test edilen modul: SH1106 tabanli 128x64 OLED (SSD1306 komut dizisiyle calisti).
  SH1106'da yatay adresleme yoktur ve sutunlar 2 piksel kayik olabilir;
  ekranda kayma gorurseniz bolge() icindeki sutun degerlerine +2 ekleyin.
  Baglanti: OLED SDA -> D2, SCL -> D3, VCC -> 5V (ya da 3.3V), GND -> GND
  Not: Kutuphanenin v1.0.1 surumu (MeptnnI2C sinifi) kurulu olmali.

  SSD1306/SH1106 her I2C gonderiminde ilk bayt olarak "kontrol baytini" bekler:
    0x00 -> arkasindan gelenler KOMUT
    0x40 -> arkasindan gelenler EKRAN VERISI (piksel)
  Bu yuzden kutuphanenin writeReg(adres, reg, tampon, uzunluk) fonksiyonunda
  "reg" parametresine kontrol baytini veriyoruz.
*/
#include <i2c_meptnn.h>

MeptnnI2C bus(2, 3);          // SDA = D2, SCL = D3
const uint8_t OLED = 0x3C;

// ---- Yardimci fonksiyonlar ----
void komut(uint8_t k) {
  bus.writeReg(OLED, 0x00, k);
}

void komutlar(const uint8_t *d, uint8_t n) {
  bus.writeReg(OLED, 0x00, d, n);
}

void veri(const uint8_t *d, uint8_t n) {
  bus.writeReg(OLED, 0x40, d, n);
}

// ---- Baslatma dizisi (128x64 icin) ----
// 128x32 ekransan: 0xA8 sonrasi 0x1F, 0xDA sonrasi 0x02 yapin.
void oledBaslat() {
  const uint8_t init[] = {
    0xAE,             // ekran kapali
    0xD5, 0x80,       // saat bolucu
    0xA8, 0x3F,       // 64 satir
    0xD3, 0x00,       // dikey kaydirma yok
    0x40,             // baslangic satiri 0
    0x8D, 0x14,       // sarj pompasi acik (bunsuz ekran yanmaz)
    0x20, 0x00,       // yatay adresleme modu
    0xA1,             // sag-sol ayna
    0xC8,             // alt-ust ayna
    0xDA, 0x12,       // COM pin ayari
    0x81, 0xCF,       // parlaklik
    0xD9, 0xF1,
    0xDB, 0x40,
    0xA4,             // RAM'deki goruntuyu goster
    0xA6,             // normal (inverse degil)
    0xAF              // ekran acik
  };
  komutlar(init, sizeof(init));
}

// Yazilacak bolgeyi sec: sutun c..127, sayfa (8 piksellik satir) p..7
void bolge(uint8_t c, uint8_t p) {
  const uint8_t k[] = {0x21, c, 127, 0x22, p, 7};
  komutlar(k, sizeof(k));
}

void temizle() {
  bolge(0, 0);
  uint8_t sifir[16] = {0};
  for (uint8_t i = 0; i < 64; i++) {   // 64 x 16 = 1024 bayt
    veri(sifir, 16);
  }
}

// ---- 5x7 yazi tipi (sadece lazim olan karakterler) ----
const uint8_t* harf(char h) {
  static const uint8_t A[] = {0x7E,0x11,0x11,0x11,0x7E};
  static const uint8_t C[] = {0x3E,0x41,0x41,0x41,0x22};
  static const uint8_t E[] = {0x7F,0x49,0x49,0x49,0x41};
  static const uint8_t I[] = {0x00,0x41,0x7F,0x41,0x00};
  static const uint8_t M[] = {0x7F,0x02,0x0C,0x02,0x7F};
  static const uint8_t N[] = {0x7F,0x04,0x08,0x10,0x7F};
  static const uint8_t P[] = {0x7F,0x09,0x09,0x09,0x06};
  static const uint8_t T[] = {0x01,0x01,0x7F,0x01,0x01};
  static const uint8_t IKI[] = {0x42,0x61,0x51,0x49,0x46};
  static const uint8_t BOS[] = {0x00,0x00,0x00,0x00,0x00};
  switch (h) {
    case 'A': return A;  case 'C': return C;  case 'E': return E;
    case 'I': return I;  case 'M': return M;  case 'N': return N;
    case 'P': return P;  case 'T': return T;  case '2': return IKI;
    default:  return BOS;
  }
}

// (sutun, sayfa) konumundan baslayarak metin yazar
void yaz(uint8_t c, uint8_t p, const char *s) {
  bolge(c, p);
  while (*s) {
    uint8_t tampon[6];
    const uint8_t *g = harf(*s);
    for (uint8_t i = 0; i < 5; i++) tampon[i] = g[i];
    tampon[5] = 0x00;                  // harfler arasi bosluk
    veri(tampon, 6);
    s++;
  }
}

void setup() {
  Serial.begin(9600);

  Error h = bus.begin();
  Serial.println(MeptnnI2C::aciklama(h));
  if (h != Error::Ok) while (1) {}

  bus.scan(Serial);                    // 0x3C gorunmeli

  oledBaslat();
  komut(0xA5);                         // TEST 1: tum pikseller yanar
  delay(1500);
  komut(0xA4);                         // normale don
  temizle();

  yaz(10, 2, "MEPTNN");                // TEST 2: yazi (sutun 10, sayfa 2)
  yaz(10, 4, "I2C");
}

void loop() {
}
