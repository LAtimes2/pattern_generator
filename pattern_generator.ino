/* Pattern_Generator
 * Copyright (c) 2018 LAtimes2
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*  This program outputs a variety of patterns at various frequencies in
 *  order to support testing of the logic analyzer. Designed for Teensy 3.5/3.6.
 */

#include <FrequencyTimer2.h>

#define LED_PIN 13

#define TIMING_PIN_0 24
#define TIMING_PIN_1 25
#define TIMING_PIN_2 26
#define TIMING_PIN_3 27
#define TIMING_PIN_4 28
#define TIMING_PIN_5 31

int userFrequency = 1000000;

void printPinout () {
  Serial.println ("  Signal Generator                                     Logic Analyzer");
  Serial.println ("  Pin  Pattern                                         Channel  Pin");
  Serial.println ("    1   Uart \"?1fU\" at 9600 baud                           0      2");
  Serial.println ("    2   PWM 62,500 Hz, 25% duty cycle                      1     14");
  Serial.println ("    3   PWM user-specified freq, 50% duty cycle            2      7");
  Serial.println ("    4   PWM user-specified freq, 50% duty cycle            3      8");
  Serial.println ("    5   CMT 1000 Hz, 50% duty cycle                        4      6");
  Serial.println ("    6   PWM user-specified freq/2, 50% duty cycle          5     20");
  Serial.println ("    7   PWM 62,500 Hz, 49% duty cycle                      6     21");
  Serial.println ("    8   PWM 62,500 Hz, 50% duty cycle                      7      5");
  Serial.println ("    9   PWM user-specified freq/2, 12.5% duty cycle                ");
  Serial.println ("   10   PWM user-specified freq/2, 25% duty cycle      Hardware");
  Serial.println ("   14   PWM 62,500 Hz, 75% duty cycle                  Channel  Pin");
  Serial.println ("   16   PWM 5954 Hz, 50% duty cycle (3.6 only)             0      1");
  Serial.println ("   17   PWM 5954 Hz, 25% duty cycle (3.6 only)             1      8");
  Serial.println ("   20   PWM user-specified freq/2, 37.5% duty cycle");
  Serial.println ("   21   PWM user-specified freq/2, 62.5% duty cycle");
  Serial.println ("   22   PWM user-specified freq/2, 75% duty cycle");
  Serial.println ("   23   PWM user-specified freq/2, 87.5% duty cycle");
  Serial.println ("   24,25,26,27,28   Software pattern");
  Serial.println ("   29   PWM 0.1 Hz (10 second period), 99% duty cycle");
  Serial.println ("   30   PWM 0.1 Hz (10 second period),  1% duty cycle");
  Serial.println ("   32   Uart \"?1fU\" at 115200 baud");
  Serial.println ("   33   Uart \"?1fU\" at 1,843,200 baud");
  Serial.println ("   35   PWM 62,500 Hz, 99% duty cycle");
  Serial.println ("");
  Serial.print ("User Specified Frequency: ");
  Serial.print (userFrequency);
  Serial.println (" Hz");
  Serial.println ("");
}

void setup() {

  pinMode(LED_PIN, OUTPUT);

  // setup user input
  Serial.begin (9600);
  while (!Serial);

  printPinout ();
  Serial.println ("\nEnter highest frequency (typically half the sample rate):");  

  blinkLED ();

  // UART on pin 1
  Serial1.begin (9600);  // set baud rate

  // UART on pin 32
  Serial4.begin (115200);  // set baud rate

  // UART on pin 33
  Serial5.begin (1843200);  // set baud rate

  // Pins set via software
  pinMode (TIMING_PIN_0, OUTPUT);
  pinMode (TIMING_PIN_1, OUTPUT);
  pinMode (TIMING_PIN_2, OUTPUT);
  pinMode (TIMING_PIN_3, OUTPUT);
  pinMode (TIMING_PIN_4, OUTPUT);
  pinMode (TIMING_PIN_5, OUTPUT);

  // Set up pin 5 using CMT
  pinMode (FREQUENCYTIMER2_PIN, OUTPUT);
  FrequencyTimer2::setPeriod (1000);
  FrequencyTimer2::enable ();

  // PWM signals
  //
  // Note: Each FTM has 1 clock frequency, so calling analogWriteFrequency for
  //       any channel on that FTM changes the frequency for all channels on the FTM.
  //       Duty cycle is the only variance between channels.

  // set channels 3,4,6,9,10,20,21,2,23
  setUserFrequency (userFrequency);

  // FTM2 - 29,30
  //    set to every 10 seconds
  analogWriteFrequency (29, 0.1);
  analogWrite (29, 255);  // 99% duty cycle
  analogWrite (30, 1);    //  1% duty cycle

  // FTM3 - 2,7,8,14,35,36,37,38
  //    set to a nominal frequency with various duty cycles
  analogWriteFrequency (2, 62500);
  analogWrite (2, 64);   // 25% duty cycle
  analogWrite (7, 127);  // 49% duty cycle
  analogWrite (8, 128);  // 50% duty cycle
  analogWrite (14, 192); // 75% duty cycle
  analogWrite (35, 255); // 99% duty cycle

  // TPM1 - 16,17
  //    set to a frequency that's not a multiple of anything (83.977 usec)
  analogWriteFrequency (16, 5954);
  analogWrite (16, 128);  // 50% duty cycle
  analogWrite (17,  64);  // 25% duty cycle
}

void setUserFrequency (int frequency) {

  // FTM1 - 3,4
  analogWriteFrequency (3, userFrequency);
  analogWrite (3, 128);  // 50% duty cycle
  analogWrite (4, 128);  // 50% duty cycle (duplicate)

  // FTM0 - 6,9,10,20,21,22,23
  analogWriteFrequency (6, userFrequency / 2);
  analogWrite ( 6, 128);  // 50% duty cycle
  analogWrite ( 9,  32);  // 12.5% duty cycle
  analogWrite (10,  64);  // 25% duty cycle
  analogWrite (20,  96);  // 37.5% duty cycle
  analogWrite (21, 160);  // 62.5% duty cycle
  analogWrite (22, 192);  // 75% duty cycle
  analogWrite (23, 224);  // 87.5% duty cycle
}


void loop() {

  int index = 0;

  while (1)
  {
    if (Serial.available ())
    {
      int input = Serial.parseInt ();

      // problem with extra line feed
      if (input > 0)
      {
        userFrequency = input;

        setUserFrequency (userFrequency);

        Serial.println(userFrequency);

        Serial.println ("Enter highest frequency (typically half the sample rate):");  
      }
    }

    // set a pin high briefly at start of loop
    digitalWriteFast (TIMING_PIN_2, HIGH);
    delayLoop (index);
    digitalWriteFast (TIMING_PIN_2, LOW);

    if (Serial1.availableForWrite () >= 4)
    {
      Serial1.print("?1fU");
    }
    if (Serial4.availableForWrite () >= 4)
    {
      Serial4.print("?1fU");
    }
    if (Serial5.availableForWrite () >= 4)
    {
      Serial5.print("?1fU");
    }

    for (index = 1; index < 5; ++index)
    {
      digitalWriteFast (TIMING_PIN_0, HIGH);
      delayLoop (index);
      digitalWriteFast (TIMING_PIN_0, LOW);

      digitalWriteFast (TIMING_PIN_1, HIGH);
      digitalWriteFast (TIMING_PIN_3, HIGH);
      delayLoop (index);
      digitalWriteFast (TIMING_PIN_3, LOW);
      digitalWriteFast (TIMING_PIN_1, LOW);
    }
  }
}

void blinkLED() {
  digitalWriteFast(LED_PIN, HIGH);
  delay(15);
  digitalWriteFast(LED_PIN, LOW);
  delay(10);
}

void delayLoop (int index) {
      for (int count = 0; count < index; ++count)
      {
        delayMicroseconds(1);
      }
}

