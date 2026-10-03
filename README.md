# Arduino Math Quiz

## About The Project

**Arduino Math Quiz** adalah permainan kuis matematika sederhana berbasis Arduino. Pemain akan mendapatkan soal penjumlahan atau pengurangan secara acak, kemudian memasukkan jawaban menggunakan **IR Sensor**.

Jawaban ditampilkan pada **7-Segment Display**, sedangkan **Buzzer** digunakan sebagai feedback ketika pemain memasukkan, menghapus, atau mengirim jawaban.

Game terdiri dari **5 soal**. Pemain membutuhkan minimal **3 jawaban benar** untuk mendapatkan hasil menang.

## Hardware

Project ini menggunakan **MFS (Multi-Function Shield)** yang memiliki beberapa komponen seperti Push Button, Buzzer, 7-Segment Display, dan LED.

Pada project ini, komponen MFS yang digunakan adalah:

* Push Button 1
* Push Button 2
* Push Button 3
* Buzzer
* 4-Digit 7-Segment Display

LED pada MFS tidak digunakan.

Selain MFS, project ini menggunakan **IR Sensor** sebagai input untuk menambahkan jawaban.

## Pin Configuration

| Component | Arduino Pin |
| --------- | ----------- |
| Button 1  | A1          |
| Button 2  | A2          |
| Button 3  | A3          |
| Buzzer    | D3          |
| MFS Latch | D4          |
| IR Sensor | D5          |
| MFS Clock | D7          |
| MFS Data  | D8          |

Konfigurasi tersebut digunakan langsung dalam program Arduino.

### MFS 7-Segment

7-Segment pada MFS dikontrol menggunakan shift register melalui tiga pin:

```text
Arduino D4 → LATCH
Arduino D7 → CLOCK
Arduino D8 → DATA
```

Program menggunakan `TimerOne` untuk melakukan multiplexing pada 7-Segment sehingga setiap digit dapat ditampilkan secara bergantian.

### IR Sensor

IR Sensor digunakan untuk memasukkan jawaban.

```text
IR Sensor OUT → Arduino D5
```

Setiap kali sensor mendeteksi objek, jawaban pemain akan bertambah satu.

Untuk mencegah satu deteksi terbaca lebih dari sekali, program menggunakan debounce selama **300 ms** dan sensor harus kembali ke kondisi awal sebelum dapat mendeteksi input berikutnya.

## How The Game Works

1. Tekan dan tahan **Button 1 selama 2 detik** untuk menyalakan game.
2. Soal matematika akan muncul pada 7-Segment.
3. Tekan **Button 1** untuk mulai menjawab.
4. Gunakan **IR Sensor** untuk menambahkan angka jawaban.
5. **Button 3** digunakan untuk mengurangi jawaban.

   * Tekan sebentar → jawaban berkurang 1.
   * Tahan 2 detik → jawaban dihapus menjadi 0.
6. Tekan **Button 2** untuk mengirim jawaban.
7. Jika jawaban benar, Buzzer memainkan suara keberhasilan.
8. Jika jawaban salah, Buzzer memainkan suara kesalahan.
9. Tekan **Button 2** untuk lanjut ke soal berikutnya.
10. Setelah 5 soal selesai, sistem akan menampilkan hasil akhir.
11. Tahan **Button 1 selama 2 detik** untuk keluar dari game.

## Scoring

Game memiliki 5 soal dengan minimal 3 jawaban benar untuk menang.

```cpp
const int JUMLAH_SOAL = 5;
const int SKOR_MENANG = 3;
```

Soal terdiri dari operasi **penjumlahan dan pengurangan** yang dibuat secara acak ketika game dimulai.

## Display & Buzzer

7-Segment digunakan untuk menampilkan:

* Soal matematika
* Jawaban pemain
* `GOOD` untuk jawaban benar
* `ERR` untuk jawaban salah
* `YAY` untuk hasil menang
* `LOSE` untuk hasil kalah

Buzzer memberikan feedback pada beberapa kondisi seperti ketika jawaban bertambah, jawaban dihapus, jawaban benar, jawaban salah, dan game selesai.

## Software

Project dibuat menggunakan **Arduino IDE** dan membutuhkan library:

```cpp
#include <TimerOne.h>
```

Library `TimerOne` digunakan untuk menjalankan proses multiplexing 7-Segment.

## How To Run

1. Install Arduino IDE.
2. Install library **TimerOne**.
3. Hubungkan MFS ke Arduino.
4. Hubungkan IR Sensor ke pin D5.
5. Pastikan koneksi pin sesuai dengan konfigurasi di atas.
6. Upload program ke Arduino.
7. Tekan dan tahan Button 1 selama 2 detik untuk memulai game.

## Project Features

* Random addition and subtraction questions
* IR Sensor sebagai input jawaban
* 3 Push Buttons sebagai kontrol game
* 4-Digit 7-Segment Display
* Buzzer feedback
* 5-question quiz
* Score system
* Minimum 3 correct answers to win
* IR Sensor debounce
* 7-Segment multiplexing
