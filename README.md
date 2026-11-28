# I2C by meptNN (v1.0.1)

Arduino platformu için sıfırdan geliştirilmiş, donanımsal I2C pinlerine (`A4`/`A5` vb.) bağımlı kalmadan **herhangi iki dijital pin** üzerinden haberleşme sağlayan yazılımsal (*bit-banging*) I2C kütüphanesidir.

## Projenin Amacı ve Vizyonu
**I2C by meptNN**, Türkiye'deki gömülü sistem geliştiricileri, öğrenciler ve maker topluluğu için **Türkçe bir açık kaynak donanım/yazılım ekosistemi** oluşturma amacıyla geliştirilmiştir. Amacımız; protokol katmanının nasıl çalıştığını anlaşılır, sade ve Türkçe dokümantasyonla sunarken hem standart İngilizce API alışkanlıklarını korumak hem de tamamen Türkçe komutlarla kod yazabilme özgürlüğü sağlamaktır.

---

## Teknik Altyapı ve Mimari
Kütüphane, Arduino'nun standart `Wire` kütüphanesinden bağımsız olarak I2C fiziksel ve veri bağı katmanını yazılım seviyesinde gerçekleştirir:

1. **Gerçek Open-Drain Hat Emülasyonu:**
   * I2C standartlarına uygun olarak hatlara hiçbir zaman doğrudan `HIGH` (5V/3.3V) sürülmez; kısa devre riski önlenir.
   * `begin()` aşamasında pinlerin `PORT` çıkış kaydedicileri `LOW` olarak hazırlanır. Hat serbest bırakılırken (`sdaBirak`, `sclBirak`) pin `INPUT` (yüksek empedans) moduna alınır ve hattı harici pull-up direnci yukarı çeker; hat sıfıra çekilirken (`sdaCek`, `sclCek`) pin `OUTPUT` moduna geçirilerek anında `GND` seviyesine indirilir.
2. **Clock Stretching ve Zaman Aşımı Koruması:**
   * Yavaş çalışan slave cihazların `SCL` hattını `LOW` seviyesinde tutarak master'ı bekletmesi (*clock stretching*) desteklenir.
   * Hattın kopuk olması veya pull-up direnci bulunmaması durumunda sonsuz döngüye girmeyi önlemek için `micros()` tabanlı **1000 µs zaman aşımı (`Error::Timeout`)** koruması bulunur.
3. **Repeated Start (`Sr`) Desteği:**
   * Register okuma işlemlerinde araya `STOP` koşulu sokmadan doğrudan `Repeated Start` sinyali üretilir; böylece cihazın register işaretçisi korunur ve okuma tek bir kesintisiz işlemde tamamlanır.
4. **Düşük RAM Tüketimi (`F()` Makrosu):**
   * Tüm Türkçe hata açıklamaları SRAM yerine doğrudan Flash bellekte (`__FlashStringHelper`) saklanır; Arduino UNO gibi 2 KB RAM'e sahip mikrodenetleyicilerde bellek daralmasına yol açmaz.
5. **Polimorfik Çıktı (`Print &p`):**
   * `scan()` fonksiyonu Arduino'nun temel `Print` sınıfını kullanır; tarama sonuçları `Serial` dışında `SoftwareSerial`, LCD veya OLED ekran nesnelerine doğrudan aktarılabilir.

---

## Donanım Gereksinimi
Open-drain mimarisi gereği **SDA** ve **SCL** hatları ile **VCC (5V veya 3.3V)** arasına harici **4.7kΩ pull-up** dirençleri bağlanmalıdır.

## Kurulum
1. Bu depoyu sağ üstteki **Code > Download ZIP** butonundan (veya **Releases** bölümünden son sürümü seçerek) indirin.
2. Arduino IDE'yi açın.
3. **Taslak (Sketch) > Library Ekle (Include Library) > .ZIP Kitaplığı Ekle...** menüsünden indirdiğiniz `.zip` dosyasını seçin.

---

## Kullanım Seçenekleri

### 1. Standart Kullanım (`i2c_meptnn.h`)
Hata kontrolü zorunlu değildir; normal akışta fonksiyonlar doğrudan çağrılabilir, yalnızca sorun yaşandığında `Error` dönüş değeri kontrol edilebilir:

```cpp
#include <i2c_meptnn.h>

MeptnnI2C i2c(2, 3); // SDA = Pin 2, SCL = Pin 3

void setup() {
  Serial.begin(9600);
  i2c.begin();

  i2c.scan(Serial);
  i2c.writeReg(0x68, 0x6B, 0x00);

  uint8_t deger = 0;
  if (i2c.readReg(0x68, 0x75, deger) != Error::Ok) {
    Serial.println(F("Okuma sirasinda hata olustu!"));
  }
}

void loop() {}
```

### 2. Türkçe Sade API Kullanımı (`i2c_meptnn_tr.h`)
Orijinal sınıfa dokunmadan eklenen sıfır maliyetli Türkçe sarmalayıcı ile tamamen Türkçe kod yazabilirsiniz:

```cpp
#include <i2c_meptnn_tr.h>

I2C_TR i2c(2, 3); // SDA = Pin 2, SCL = Pin 3

void setup() {
  Serial.begin(9600);

  Hata durum = i2c.baslat();
  if (durum != Tamam) {
    Serial.println(I2C_TR::aciklama(durum));
    while (1);
  }

  i2c.tara();
  i2c.yaz(0x68, 0x6B, 0x00);

  uint8_t deger = 0;
  i2c.oku(0x68, 0x75, deger);
}

void loop() {}
```

---

## API Karşılaştırma Tablosu

| Standart API (`i2c_meptnn.h`) | Türkçe API (`i2c_meptnn_tr.h`) | Görevi |
| :--- | :--- | :--- |
| `MeptnnI2C i2c(sda, scl)` | `I2C_TR i2c(sda, scl)` | Belirtilen pinlerle I2C nesnesi oluşturur. |
| `i2c.begin()` | `i2c.baslat()` | Hatları hazırlar ve SCL hattını test eder. |
| `i2c.readReg(adr, reg, &val)` | `i2c.oku(adr, reg, &val)` | Register'dan 1 bayt okur. |
| `i2c.writeReg(adr, reg, val)` | `i2c.yaz(adr, reg, val)` | Register'a 1 bayt yazar. |
| `i2c.readReg(adr, reg, *buf, len)` | `i2c.oku(adr, reg, *buf, len)` | Register'dan ardışık çok bayt okur. |
| `i2c.writeReg(adr, reg, *buf, len)` | `i2c.yaz(adr, reg, *buf, len)` | Register'a ardışık çok bayt yazar. |
| `i2c.scan(Serial)` | `i2c.tara()` / `i2c.tara(p)` | `0x08`–`0x77` aralığındaki cihazları tarar. |
| `Error::Ok` | `Tamam` | İşlem başarılı. |
| `Error::AddressNack` | `AdresYok` | Cihaz bulunamadı veya adres yanlış. |
| `Error::DataNack` | `VeriReddi` | Cihaz gönderilen veriyi kabul etmedi. |
| `Error::Timeout` | `ZamanAsimi` | SCL hattı yükselmedi (pull-up kontrolü). |

## Sınırlamalar
* Yalnızca **master** modu desteklenir (slave ve multi-master yok).
* Yalnızca **7-bit adres** kullanılır; R/W biti kütüphane tarafından eklenir (`0x08`–`0x77`).
* Hız yaklaşık **60 kHz** civarıdır (yarım periyot ≈ 8 µs), standart 100 kHz'in altındadır.
* Zaman aşımı (`Timeout`) olursa hat takılı kalmış olabilir; cihazı resetlemek gerekebilir.
* **Test durumu:** Wokwi simülasyonunda DS1307 ile (mantık analizörüyle) ve gerçek donanımda **Arduino Uno + SH1106 tabanlı 128x64 OLED (0x3C)** ile doğrulandı (`examples/08_OLED_Test`). Diğer cihazlar henüz denenmedi.
* Register düzeyi optimizasyon ve donanım TWI desteği sonraki sürümlerde planlanıyor.

## Lisans
Bu proje MIT Lisansı ile lisanslanmıştır. Detaylar için `LICENSE` dosyasına bakabilirsiniz.
