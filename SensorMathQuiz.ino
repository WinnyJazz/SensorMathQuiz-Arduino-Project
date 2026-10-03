#include <TimerOne.h>

const int BUTTON1 = A1;
const int BUTTON2 = A2;
const int BUTTON3 = A3;
const int BUZZER  = 3;

const int LATCH_PIN = 4;
const int CLK_PIN   = 7;
const int DATA_PIN  = 8;
const int IR_PIN    = 5;

volatile byte displayBuffer[4] = {0x00, 0x00, 0x00, 0x00};
volatile bool displayOn = false;

// Pola segment ACTIVE-HIGH
const byte SEG_0 = 0b00111111;
const byte SEG_1 = 0b00000110;
const byte SEG_2 = 0b01011011;
const byte SEG_3 = 0b01001111;
const byte SEG_4 = 0b01100110;
const byte SEG_5 = 0b01101101;
const byte SEG_6 = 0b01111101;
const byte SEG_7 = 0b00000111;
const byte SEG_8 = 0b01111111;
const byte SEG_9 = 0b01101111;

const byte SEG_BLANK = 0x00;

const byte SEG_T = 0x78;   // tanda tambah (tampil sebagai "t")
const byte SEG_MIN = 0x40; // tanda kurang (segment tengah "-")
const byte SEG_G = 0x3D;
const byte SEG_O = 0x5C;
const byte SEG_D = 0x5E;
const byte SEG_E = 0x79;
const byte SEG_R = 0x50;
const byte SEG_A = 0x77;
const byte SEG_L = 0x38;   
const byte SEG_S = 0x6D;  

int angkaKiri[5]  = {1, 2, 1, 3, 2};
int angkaKanan[5] = {1, 2, 3, 2, 3};
bool soalKurang[5] = {false, false, false, false, false};

// Minimal bener 3 baru menang
const int JUMLAH_SOAL = 5;
const int SKOR_MENANG = 3;  

enum GameState {
  POWER_OFF,
  SHOW_QUESTION,
  READY_TO_ANSWER,
  ANSWERING,
  SHOW_RESULT,
  GAME_FINISHED
};

GameState gameState = POWER_OFF;

int nomorSoal = 0;
int jawabanBenar = 0;
int jawabanUser = 0;
int skor = 0;

unsigned long waktuMulaiSoal = 0;

unsigned long waktuButton1 = 0;
bool button1SedangDitekan = false;

unsigned long waktuButton3 = 0;
bool button3SedangDitekan = false;

const unsigned long DURASI_POWER = 2000;
const unsigned long DURASI_CLEAR = 2000;

int kondisiIR = HIGH;
int kondisiIRSebelumnya = HIGH;
unsigned long waktuIR = 0;
const unsigned long debounceIR = 300;
bool irSiap = true;

bool button2Sebelumnya = HIGH;

// INITIAL SETUP
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  pinMode(BUZZER, OUTPUT);
  buzzerOff();

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  pinMode(IR_PIN, INPUT);

  Timer1.initialize(2500);
  Timer1.attachInterrupt(multiplexISR);

  matikanDisplay();
  gameState = POWER_OFF;
}


// LOOP FOR THE GAME
void loop() {

  if (gameState == POWER_OFF) {
    cekPowerButton();
    return;
  }

  cekButton1Exit();

  if (gameState == SHOW_QUESTION) {
    if (millis() - waktuMulaiSoal >= 2000) {
      gameState = READY_TO_ANSWER;
    }
  }
  else if (gameState == READY_TO_ANSWER) {
    cekButton1MulaiJawab();
  }
  else if (gameState == ANSWERING) {
    bacaSensorIR();
    cekButton2Submit();
    cekButton3Delete();
  }
  else if (gameState == SHOW_RESULT) {
    cekButton2Next();
  }
  else if (gameState == GAME_FINISHED) {
    cekButton2Finish();
  }
}


// BUTTONS 
void cekPowerButton() {

  if (digitalRead(BUTTON1) == LOW) {

    if (!button1SedangDitekan) {
      button1SedangDitekan = true;
      waktuButton1 = millis();
    }

    if (millis() - waktuButton1 >= DURASI_POWER) {
      nyalakanGame();
      button1SedangDitekan = false;

      while (digitalRead(BUTTON1) == LOW) {
        delay(5);
      }
    }

  } else {
    button1SedangDitekan = false;
  }
}

void cekButton1Exit() {

  if (digitalRead(BUTTON1) == LOW) {

    if (!button1SedangDitekan) {
      button1SedangDitekan = true;
      waktuButton1 = millis();
    }

    if (millis() - waktuButton1 >= DURASI_POWER) {
      matikanGame();
      button1SedangDitekan = false;

      while (digitalRead(BUTTON1) == LOW) {
        delay(5);
      }
    }

  } else {
    button1SedangDitekan = false;
  }
}

void cekButton1MulaiJawab() {

  if (digitalRead(BUTTON1) == LOW) {

    delay(30);

    if (digitalRead(BUTTON1) == LOW) {
      mulaiMenjawab();

      while (digitalRead(BUTTON1) == LOW) {
        delay(5);
      }
      button1SedangDitekan = false;
    }
  }
}

void cekButton2Submit() {

  bool button2Sekarang = digitalRead(BUTTON2);

  if (button2Sebelumnya == HIGH && button2Sekarang == LOW) {

    delay(30);

    if (digitalRead(BUTTON2) == LOW) {
      submitJawaban();
    }
  }

  button2Sebelumnya = button2Sekarang;
}

void cekButton2Next() {

  if (digitalRead(BUTTON2) == LOW) {

    delay(30);

    if (digitalRead(BUTTON2) == LOW) {

      while (digitalRead(BUTTON2) == LOW) {
        delay(5);
      }

      nomorSoal++;

      if (nomorSoal >= JUMLAH_SOAL) {
        gameState = GAME_FINISHED;

        if (skor >= SKOR_MENANG) {
          tampilkanYay();
          bunyiSelesai();
        } else {
          tampilkanLose();
          bunyiSalah();
        }

        Serial.print("Skor akhir: ");
        Serial.print(skor);
        Serial.print("/");
        Serial.println(JUMLAH_SOAL);
      } else {
        mulaiSoalBaru();
      }
    }
  }
}

void cekButton2Finish() {

  if (digitalRead(BUTTON2) == LOW) {

    delay(30);

    if (digitalRead(BUTTON2) == LOW) {

      while (digitalRead(BUTTON2) == LOW) {
        delay(5);
      }

      matikanGame();
    }
  }
}

void cekButton3Delete() {

  if (digitalRead(BUTTON3) == LOW) {

    if (!button3SedangDitekan) {
      button3SedangDitekan = true;
      waktuButton3 = millis();
    }

    if (millis() - waktuButton3 >= DURASI_CLEAR) {

      jawabanUser = 0;
      tampilkanAngka(jawabanUser);
      bunyiClear();

      while (digitalRead(BUTTON3) == LOW) {
        delay(5);
      }

      button3SedangDitekan = false;
    }

  } else {

    if (button3SedangDitekan) {

      unsigned long durasiTekan = millis() - waktuButton3;

      if (durasiTekan < DURASI_CLEAR) {
        if (jawabanUser > 0) {
          jawabanUser--;
          tampilkanAngka(jawabanUser);
          bunyiKlik();
        }
      }
    }

    button3SedangDitekan = false;
  }
}

// LOGIKA GAME
void acakSoal() {
  for (int i = 0; i < JUMLAH_SOAL; i++) {

    soalKurang[i] = (random(0, 2) == 1);   // 50% tambah, 50% kurang

    if (soalKurang[i]) {
      // KURANG: kiri 2-9, kanan 1 sampai (kiri-1) -> hasil 1-8, tidak negatif
      angkaKiri[i]  = random(2, 10);
      angkaKanan[i] = random(1, angkaKiri[i]);
    } else {
      // TAMBAH: jumlah maksimal 9
      angkaKiri[i]  = random(1, 6);
      angkaKanan[i] = random(1, 10 - angkaKiri[i]);
    }
  }
}

void nyalakanGame() {
  nomorSoal = 0;
  jawabanUser = 0;
  skor = 0;
  acakSoal();
  mulaiSoalBaru();
}

void matikanGame() {
  buzzerOff();
  matikanDisplay();

  nomorSoal = 0;
  jawabanUser = 0;
  skor = 0;

  gameState = POWER_OFF;
}

void mulaiSoalBaru() {

  if (soalKurang[nomorSoal]) {
    jawabanBenar = angkaKiri[nomorSoal] - angkaKanan[nomorSoal];
  } else {
    jawabanBenar = angkaKiri[nomorSoal] + angkaKanan[nomorSoal];
  }

  jawabanUser = 0;

  tampilkanSoal(angkaKiri[nomorSoal], angkaKanan[nomorSoal], soalKurang[nomorSoal]);

  gameState = SHOW_QUESTION;
  waktuMulaiSoal = millis();

  kondisiIRSebelumnya = digitalRead(IR_PIN);
  kondisiIR = kondisiIRSebelumnya;
  irSiap = true;
  waktuIR = millis();

  Serial.print("SOAL ");
  Serial.print(nomorSoal + 1);
  Serial.print(": ");
  Serial.print(angkaKiri[nomorSoal]);
  Serial.print(soalKurang[nomorSoal] ? " - " : " + ");
  Serial.print(angkaKanan[nomorSoal]);
  Serial.println(" = ?");
}

void mulaiMenjawab() {

  jawabanUser = 0;

  setDisplay(SEG_BLANK, SEG_BLANK, SEG_BLANK, SEG_BLANK);

  kondisiIR = digitalRead(IR_PIN);
  kondisiIRSebelumnya = kondisiIR;
  irSiap = (kondisiIR == HIGH);
  waktuIR = millis();

  button2Sebelumnya = digitalRead(BUTTON2);

  gameState = ANSWERING;
}

void bacaSensorIR() {

  kondisiIR = digitalRead(IR_PIN);

  if (kondisiIR == HIGH) {
    irSiap = true;
  }

  if (kondisiIR == LOW &&
      kondisiIRSebelumnya == HIGH &&
      irSiap &&
      millis() - waktuIR >= debounceIR) {

    if (jawabanUser < 9) {
      jawabanUser++;
      tampilkanAngka(jawabanUser);
      bunyiKlik();

      Serial.print("Jawaban: ");
      Serial.println(jawabanUser);
    }

    waktuIR = millis();
    irSiap = false;
  }

  kondisiIRSebelumnya = kondisiIR;
}

void submitJawaban() {

  if (jawabanUser == jawabanBenar) {
    skor++;
    tampilkanGood();
    bunyiBenar();
    Serial.println("BENAR");
  } else {
    tampilkanError();
    bunyiSalah();
    Serial.println("SALAH");
  }

  gameState = SHOW_RESULT;
}

// DISPLAY
void tampilkanSoal(int kiri, int kanan, bool kurang) {
  byte operatorSeg = kurang ? SEG_MIN : SEG_T;
  setDisplay(polaAngka(kiri), operatorSeg, polaAngka(kanan), SEG_BLANK);
}

void tampilkanAngka(int angka) {
  setDisplay(SEG_BLANK, SEG_BLANK, SEG_BLANK, polaAngka(angka));
}

void tampilkanGood() {
  setDisplay(SEG_G, SEG_O, SEG_O, SEG_D);
}

void tampilkanError() {
  setDisplay(SEG_E, SEG_R, SEG_R, SEG_BLANK);
}

// YAY tampil sebagai "4 A 4"
void tampilkanYay() {
  setDisplay(SEG_4, SEG_A, SEG_4, SEG_BLANK);
}

// LOSE tampil sebagai "L O S E"
void tampilkanLose() {
  setDisplay(SEG_L, SEG_0, SEG_S, SEG_E);
}

void setDisplay(byte d0, byte d1, byte d2, byte d3) {
  noInterrupts();
  displayBuffer[0] = d0;
  displayBuffer[1] = d1;
  displayBuffer[2] = d2;
  displayBuffer[3] = d3;
  displayOn = true;
  interrupts();
}

void matikanDisplay() {
  noInterrupts();
  displayBuffer[0] = SEG_BLANK;
  displayBuffer[1] = SEG_BLANK;
  displayBuffer[2] = SEG_BLANK;
  displayBuffer[3] = SEG_BLANK;
  displayOn = false;
  interrupts();
}

byte polaAngka(int angka) {
  switch (angka) {
    case 0: return SEG_0;
    case 1: return SEG_1;
    case 2: return SEG_2;
    case 3: return SEG_3;
    case 4: return SEG_4;
    case 5: return SEG_5;
    case 6: return SEG_6;
    case 7: return SEG_7;
    case 8: return SEG_8;
    case 9: return SEG_9;
    default: return SEG_BLANK;
  }
}

// BUZZER (MFS = active-low, HIGH = diam)
void buzzerOff() {
  noTone(BUZZER);
  digitalWrite(BUZZER, HIGH);
}

void nada(unsigned int freq, unsigned int durasi) {
  tone(BUZZER, freq);
  delay(durasi);
  buzzerOff();
}

void bunyiKlik() {
  nada(3000, 60);
}

void bunyiClear() {
  nada(2500, 100);
}

void bunyiBenar() {
  nada(2500, 140);
  delay(20);
  nada(3000, 140);
  delay(20);
  nada(3500, 220);
}

void bunyiSalah() {
  nada(2200, 300);
  delay(30);
  nada(1700, 400);
}

void bunyiSelesai() {
  nada(2500, 120);
  delay(20);
  nada(2800, 120);
  delay(20);
  nada(3200, 120);
  delay(20);
  nada(3500, 300);
}


// MULTIPLEXING (urutan: SEGMENT dulu, baru DIGIT)
void multiplexISR() {

  static byte currentDigit = 0;

  digitalWrite(LATCH_PIN, LOW);

  if (!displayOn) {
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, 0x00); // semua segment mati
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, 0xFF); // tidak ada digit dipilih
    digitalWrite(LATCH_PIN, HIGH);
    return;
  }

  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, displayBuffer[currentDigit]);  // segment
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, ~(1 << currentDigit));         // digit (active-low)

  digitalWrite(LATCH_PIN, HIGH);

  currentDigit++;
  if (currentDigit >= 4) {
    currentDigit = 0;
  }
}