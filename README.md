# Arduino Math Quiz

## About The Project

**Arduino Math Quiz** is a simple mathematics quiz game built with Arduino. The player receives random addition or subtraction questions and enters the answer using an **IR Sensor**.

The answer is displayed on a **4-Digit 7-Segment Display**, while the **Buzzer** provides feedback when the player interacts with the game.

The game consists of **5 questions**, and the player needs at least **3 correct answers** to win.

## Hardware

This project uses an **MFS (Multi-Function Shield)**, which includes Push Buttons, a Buzzer, a 4-Digit 7-Segment Display, and LEDs.

Only the following MFS components are used:

* Push Button 1
* Push Button 2
* Push Button 3
* Buzzer
* 4-Digit 7-Segment Display

The LEDs on the MFS are not used.

An external **IR Sensor** is also used as the input for entering answers.

## Pin Configuration

| Component     | Arduino Pin |
| ------------- | ----------- |
| Button 1      | A1          |
| Button 2      | A2          |
| Button 3      | A3          |
| Buzzer        | D3          |
| MFS Latch     | D4          |
| IR Sensor OUT | D5          |
| MFS Clock     | D7          |
| MFS Data      | D8          |

The pin configuration is defined directly in the Arduino program.

### MFS 7-Segment Connection

The 4-Digit 7-Segment Display on the MFS is controlled using a shift register through three pins:

```text
Arduino D4 → LATCH
Arduino D7 → CLOCK
Arduino D8 → DATA
```

The program uses `TimerOne` for 7-Segment multiplexing to display the digits smoothly.

### IR Sensor Connection

The external IR Sensor is connected to:

```text
IR Sensor OUT → Arduino D5
```

Whenever the sensor detects an object, the player's answer increases by 1.

The program also uses a **300 ms debounce** and requires the sensor to return to its initial state before another detection can be counted. This prevents a single detection from being counted multiple times.

## How The Game Works

1. Press and hold **Button 1 for 2 seconds** to start the game.
2. A random mathematics question will be displayed.
3. Press **Button 1** to start answering.
4. Use the **IR Sensor** to increase the answer by 1.
5. Use **Button 3** to decrease the answer.

   * Short press → decrease the answer by 1.
   * Hold for 2 seconds → reset the answer to 0.
6. Press **Button 2** to submit the answer.
7. If the answer is correct, the Buzzer plays a success sound.
8. If the answer is incorrect, the Buzzer plays an error sound.
9. Press **Button 2** to continue to the next question.
10. After 5 questions, the final result is displayed.
11. Press and hold **Button 1 for 2 seconds** to exit the game.

## Scoring

The game contains 5 questions, with a minimum of 3 correct answers required to win.

```cpp
const int JUMLAH_SOAL = 5;
const int SKOR_MENANG = 3;
```

The questions are randomly generated using addition or subtraction.

## Display & Buzzer

The 7-Segment Display is used to show:

* Mathematics questions
* Player's answer
* `GOOD` for a correct answer
* `ERR` for an incorrect answer
* `YAY` for winning
* `LOSE` for losing

The Buzzer provides feedback for actions such as increasing the answer, clearing the answer, submitting a correct or incorrect answer, and finishing the game.

## Software

This project was developed using **Arduino IDE** and requires the following library:

```cpp
#include <TimerOne.h>
```

The `TimerOne` library is used to control the 7-Segment multiplexing.

## How To Run

1. Install Arduino IDE.
2. Install the **TimerOne** library.
3. Connect the MFS to the Arduino.
4. Connect the IR Sensor OUT to **D5**.
5. Make sure all connections match the pin configuration above.
6. Upload the program to the Arduino.
7. Press and hold **Button 1 for 2 seconds** to start the game.

## Features

* Random addition and subtraction questions
* IR Sensor as answer input
* Three Push Buttons for game control
* 4-Digit 7-Segment Display
* Buzzer feedback
* 5-question quiz
* Score system
* Minimum 3 correct answers to win
* IR Sensor debounce
* Smooth 7-Segment multiplexing
