
#include <Servo.h>
#include <IRremote.h>

// ====== Pin Setup ======
#define TRIG 7
#define ECHO 6
#define BUZZER 11
#define IR_PIN 9
#define SERVO_PIN 8
//#define LED_HIJAU 11
//#define LED_MERAH 12

// ====== Objek ======
Servo myservo;
IRrecv irrecv(IR_PIN);
decode_results results;

// ====== Variabel ======
long duration;
int distance;
int threshold = 10; // cm, jarak deteksi mangkuk
int pos = 0;

// Ganti sesuai kode remote (cek Serial Monitor untuk dapat HEX)
unsigned long tombol1 = 0xFF30CF; // contoh tombol "1"
unsigned long tombol2 = 0xFF18E7; // contoh tombol "2"
unsigned long tombol3 = 0xFF7A85; // contoh tombol "3"

void setup() {
  Serial.begin(9600);
  myservo.attach(SERVO_PIN);
  myservo.write(0); // posisi tutup awal
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  //pinMode(LED_HIJAU, OUTPUT);
  //pinMode(LED_MERAH, OUTPUT);
  
  irrecv.enableIRIn(); // aktifkan receiver
}

void loop() {
  // Cek remote
  if (irrecv.decode(&results)) {
    Serial.print("Kode Remote: ");
    Serial.println(results.value, HEX);

    int durasiMakan = 0;

    if (results.value == tombol1 || results.value == 0x800F0401) durasiMakan = 1000; // 1 detik
    if (results.value == tombol2 || results.value == 0x800F0402) durasiMakan = 1500; // 1.5 detik
    if (results.value == tombol3 || results.value == 0x800F0403) durasiMakan = 2000; // 2 detik

    if (durasiMakan > 0) {
      if (cekMangkuk()) {
        kasihMakan(durasiMakan);
      } else {
        warning();
      }
    }

    irrecv.resume(); // reset untuk baca berikutnya
  }
}
  
// ====== Fungsi Kasih Makan ======
void kasihMakan(int durasi) {
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  //digitalWrite(LED_HIJAU, HIGH); // nyalakan hijau
  myservo.write(100);
  delay(durasi);
  myservo.write(0);
  //digitalWrite(LED_HIJAU, LOW); // matikan hijau
  digitalWrite(BUZZER, LOW);
}

// ====== Fungsi Cek Mangkuk ======
bool cekMangkuk() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < threshold) {
    return true;  // mangkuk ada
  } else {
    return false; // mangkuk tidak ada
  }
}

// ====== Fungsi Warning ======
void warning() {
  Serial.println("Mangkuk tidak terdeteksi!");
  for (int i = 0; i < 3; i++) {
    //digitalWrite(LED_MERAH, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(300);
    //digitalWrite(LED_MERAH, LOW);
    //delay(200);
  }
  digitalWrite(BUZZER, LOW);
}
