/*
  06 - Farkli Cikti Birimi ile Tarama (Print Polimorfizmi)
  I2C by meptNN

  scan(Print &p) fonksiyonu Arduino'nun Print sinifini referans alir.
  Bu sayede tarama sonuclarini yalnizca Serial'a degil; SoftwareSerial,
  LCD ekranlar veya kendi yazdiginiz cikti siniflarina da yonlendirebilirsiniz.
*/

#include <i2c_meptnn.h>
#include <SoftwareSerial.h>

MeptnnI2C i2c(2, 3);             // SDA = Pin 2, SCL = Pin 3
SoftwareSerial hariciPort(8, 9); // RX = Pin 8, TX = Pin 9 (Bluetooth / Harici Ekran)

void setup() {
  Serial.begin(9600);
  hariciPort.begin(9600);

  i2c.begin();

  // Sonuclari hem USB Seri Port'a hem de harici porta yazdir
  Serial.println(F("USB Seri Port Taramasi:"));
  i2c.scan(Serial);

  hariciPort.println(F("Harici Port Taramasi:"));
  i2c.scan(hariciPort);
}

void loop() {
}
