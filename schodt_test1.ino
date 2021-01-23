#include <FastLED.h>
#include <LEDMatrix.h>

#define DATA_PIN           4

#define STEPS              11                          // Ile schodow
#define WIDTH              10                          // Ile diod na jeden stopien (musi być podzielne przez dwa)
#define NUM_LEDS           STEPS * WIDTH
#define MATRIX_TYPE        HORIZONTAL_ZIGZAG_MATRIX

#define PIR_UP             9                          //numer pinu z czujnika pir góar
#define PIR_DOWN           8                          //numer pinu z czujnika pir dół

#define ANIM_DURATION 1500

//CRGB leds[NUM_LEDS];
cLEDMatrix<WIDTH, STEPS, MATRIX_TYPE> leds;

// Zdefiniowane kolory
const CRGB::HTMLColorCode colors[88] = {CRGB::Amethyst, CRGB::Aqua, CRGB::Blue, CRGB::BlueViolet, CRGB::Brown, CRGB::Chartreuse, CRGB::Chocolate, CRGB::DarkGray,
                                        CRGB::Coral, CRGB::CornflowerBlue, CRGB::Crimson, CRGB::Cyan, CRGB::DarkBlue, CRGB::DarkCyan, CRGB::DarkGoldenrod, CRGB::YellowGreen,
                                        CRGB::DarkGreen, CRGB::DarkMagenta, CRGB::DarkKhaki, CRGB::DarkOliveGreen, CRGB::DarkOrange, CRGB::DarkOrchid, CRGB::DarkRed,
                                        CRGB::DarkSalmon, CRGB::DarkSlateBlue, CRGB::DarkSlateGray, CRGB::DarkTurquoise, CRGB::DarkViolet, CRGB::DeepPink, CRGB::DeepSkyBlue,
                                        CRGB::DimGray, CRGB::DodgerBlue, CRGB::FireBrick, CRGB::ForestGreen, CRGB::Fuchsia, CRGB::Gold, CRGB::Goldenrod, CRGB::Green,
                                        CRGB::GreenYellow, CRGB::HotPink, CRGB::IndianRed, CRGB::Indigo, CRGB::Khaki, CRGB::LawnGreen, CRGB::LightSeaGreen, CRGB::LightSkyBlue,
                                        CRGB::LightSlateGray, CRGB::Lime, CRGB::LimeGreen, CRGB::Magenta, CRGB::Maroon, CRGB::MediumAquamarine, CRGB::MediumBlue, CRGB::MediumOrchid,
                                        CRGB::MediumPurple, CRGB::MediumSeaGreen, CRGB::MediumSlateBlue, CRGB::MediumSpringGreen, CRGB::MediumTurquoise, CRGB::MediumVioletRed,
                                        CRGB::MidnightBlue, CRGB::MintCream, CRGB::NavajoWhite, CRGB::Navy, CRGB::Olive, CRGB::OliveDrab, CRGB::Orange, CRGB::OrangeRed, CRGB::Orchid,
                                        CRGB::PaleVioletRed, CRGB::PaleGreen, CRGB::Plaid, CRGB::Purple, CRGB::Red, CRGB::RoyalBlue, CRGB::SeaGreen, CRGB::SandyBrown, CRGB::SlateBlue,
                                        CRGB::SkyBlue, CRGB::Snow, CRGB::SpringGreen, CRGB::SteelBlue, CRGB::Teal, CRGB::Tomato, CRGB::Turquoise, CRGB::Violet, CRGB::White, CRGB::Yellow
                                       };

unsigned long timeOut = 60000;   // Sygnatura czasowa do zapamiętania, kiedy PIR został uruchomiony.
unsigned long c_timer = 0;
unsigned long s_timer = millis();

int downUp = 0;                  // Kierunek poruszania się po schodach(kierunek animacji)
int ledPin = 13;                 // Dioda LED na płycie arduino miga, gdy aktywny jest PIR
int PIR_UP_V = LOW;
int PIR_DOWN_V = LOW;

uint8_t hue = 0;
int counter = 0;


void setup() {
  Serial.begin(115200);

  pinMode(PIR_UP, INPUT);
  pinMode(PIR_DOWN, INPUT);

  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds[0], leds.Size()).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(64);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  FastLED.clear();
  FastLED.show();

}

void loop() {

  PIR_UP_V    = digitalRead(PIR_UP);                                 // Stale sprawdzaj PIR na górze schodów
  PIR_DOWN_V  = digitalRead(PIR_DOWN);                               // Stale sprawdzaj PIR na dole schodów
  // c_timer = millis();
  if (PIR_UP_V == HIGH && downUp != 2)  {                            //  Drugi termin pozwala na ciągłe resetowanie timeOut, jeśli jeden z nich pozostaje na dole schodów przed zejściem, ale nie pozwala górnemu PIR zresetować timeOut, gdy zejdziesz po nim.
    Serial.println("Wykryto ruch u góry");
    c_timer = 0;
    timeOut = millis();                                              // Sygnatura czasowa, kiedy PIR jest wyzwalany. Następnie rozpocznie się cykl LED.
    downUp = 1;
    topdown(random8(0, 14));                                          // Podświetla schody od góry do dołu
  }

  if (PIR_DOWN_V == HIGH && downUp != 1)  {                          // Drugi termin pozwala na ciągłe resetowanie timeOut, jeśli jeden z nich pozostaje na dole schodów przed zejściem, ale nie pozwala górnemu PIR zresetować timeOut, gdy zejdziesz po nim.
    Serial.println("Wykryto ruch u dołu");
    c_timer = 0;
    timeOut = millis();                                              // Sygnatura czasowa, kiedy PIR jest wyzwalany. Następnie rozpocznie się cykl LED.
    downUp = 2;
    topdown(random8(0, 14));                                          // Podświetla schody od dołu do góry
  }

  if (timeOut + 10000 < millis() && timeOut + 15000 < millis()) {    // wyłącz diody LED w kierunku ruchu.
    if (downUp == 1) {
      ledoff(random8(0, 8));
    }
    if (downUp == 2)  {
      ledoff(random8(0, 8));
    }
    downUp = 0;
  }
}

void topdown(int mode) {
  Serial.println("Efekt nr:");
  Serial.println(mode);
  //switch (14) {
      switch (mode) {
    case 0:
      turn_on_0();
      break;
    case 1:
      turn_on_1 (colors[random8(0, 90)]);
      break;
    case 2:
      while ( c_timer++ < ANIM_DURATION / 2) {
        //  Serial.println(c_timer);
        turn_on_2();
      }
      break;
    case 3:
      while ( c_timer++ < ANIM_DURATION / 10 ) {
        turn_on_3();
      }
      break;
    case 4:
      while (c_timer++ < ANIM_DURATION / 60 ) {
        turn_on_4();
      }
      break;
    case 5:
      while (c_timer++ < ANIM_DURATION / 60 ) {
        turn_on_5();
      }
      break;
    case 6:
      turn_on_off_0(CRGB::White, CRGB::Black);
      break;
    case 7:
      turn_on_off_1(colors[random8(0, 88)]);
      break;
    case 8:
      turn_on_off_2(colors[random8(0, 88)]);
      break;
    case 9:
      while (c_timer++ < ANIM_DURATION ) {
        turn_on_6();
      }
      break;
    case 10:
      turn_on_7();
      break;
    case 11:
      turn_on_off_3(colors[random8(0, 88)]);
      break;
    case 12:
      turn_on_off_4(colors[random8(0, 88)]);
      break;
    case 13:
      turn_on_off_5(colors[random8(0, 88)]);
      break;
    case 14:
      turn_on_off_6(colors[random8(0, 88)]);
      break;
  }
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
}

void ledoff(int mode) {
  Serial.println("Efekt nr:");
  Serial.println(mode);
  switch (mode) {
  //switch (8) {
    case 0:
      turn_off_0 ();
      break;
    case 1:
      turn_off_1 ();
      break;
    case 2:
      turn_on_off_0(CRGB::Black , CRGB::White);
      break;
    case 3:
      turn_on_off_1(CRGB::Black);
      break;
    case 4:
      turn_on_off_2(CRGB::Black);
      break;
    case 5:
      turn_on_off_3(CRGB::Black);
      break;
    case 6:
      turn_on_off_4(CRGB::Black);
      break;
    case 7:
      turn_on_off_5(CRGB::Black);
      break;
    case 8:
      turn_on_off_6(CRGB::Black);
      break;
  }
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
}

void turn_on_0 () {
  if (downUp != 1)
    for (uint16_t i = 0; i < STEPS + 1 ; i++) {
      leds.DrawLine(0, i, WIDTH, i, CHSV(100, 255, 255));
      FastLED.delay(100);
      if (i >= STEPS)
        for (uint16_t i = 0; i < STEPS + 1 ; i++) {
          leds.DrawLine(WIDTH / 5, i, WIDTH - (WIDTH / 5) - 1, i, CHSV(0, 255, 255));
          FastLED.delay(100);
        }
      FastLED.show();
    }
  if (downUp != 2)
    for (uint16_t i = 0; i < STEPS + 1 ; i++) {
      leds.DrawLine(0, STEPS - i, WIDTH, STEPS - i, CHSV(100, 255, 255));
      FastLED.delay(100);
      if (i >= STEPS)
        for (uint16_t i = 0; i < STEPS + 1 ; i++) {
          leds.DrawLine(WIDTH / 5, STEPS - i, WIDTH - (WIDTH / 5) - 1, STEPS - i, CHSV(0, 255, 255));
          FastLED.delay(100);
        }
      FastLED.show();
    }
}

void turn_off_0 () {
  if (downUp != 1)
    for (uint16_t i = 0; i < STEPS + 1 ; i++) {
      leds.DrawLine(0, i, WIDTH, i, CHSV(0, 0, 0));
      FastLED.delay(100);
    }
  FastLED.show();
  if (downUp != 2)
    for (uint16_t i = 0; i < STEPS + 1 ; i++) {
      leds.DrawLine(0, STEPS - i, WIDTH, STEPS - i, CHSV(0, 0, 0));
      FastLED.delay(100);
    }
  FastLED.show();
}

void turn_off_1 () {
  int h = 0;
  for (int j = 0; j < STEPS / 2; j++) {
    int start =  WIDTH * j;
    for (int i = 0; i < WIDTH / 2; i++) {
      leds.DrawPixel(j * STEPS, i, CHSV(hue++, 255, 255));
      FastLED.delay(50);
      FastLED.show();
      leds.QuadrantTopTriangleMirror();
      leds.HorizontalMirror();
      leds.ShiftRight();
    }
  }
}

void turn_on_1 (uint32_t color) {
  for (int j = 0; j < STEPS; j++)  {
    for (int i = 0; i < WIDTH / 2 ; i++) {
      leds.DrawPixel(i , j, color);
      // leds.HorizontalMirror();
      leds.QuadrantTopTriangleMirror();
      FastLED.delay(50);
    }
  }
  FastLED.show();
}

void turn_on_2() {
  uint8_t h;
  h = hue;
  if (downUp != 1) {
    for (int y = STEPS; y > 0; y--) {
      leds.DrawLine(0, y - 1, WIDTH - 1, y - 1, CHSV(h, 255, 255));
      delay(2);
      h += STEPS;
    }
  }
  if (downUp != 2) {
    for (int y = 0; y < STEPS; y++) {
      leds.DrawLine(0, y - 1, WIDTH - 1, y - 1, CHSV(h, 255, 255));
      delay(2);
      h += STEPS;
    }
  }
  hue += 2;
  FastLED.show();
}

void turn_on_3() {
  if (downUp != 1) {
    for (uint16_t i = 0; i < STEPS  ; i++) {
      leds.DrawPixel(random8(WIDTH * 2), random8(1), CHSV(random8(), 255, 255));
    }
    FastLED.delay(100);
    leds.ShiftUp();
    FastLED.show();
  }
  if (downUp != 2) {
    for (uint16_t i = 0; i < STEPS ; i++) {
      leds.DrawPixel(random8(WIDTH * 2), random8(STEPS + 2), CHSV(random8(), 255, 255));
    }
    FastLED.delay(100);
    leds.ShiftDown();
    FastLED.show();
  }
}

void turn_on_4() {
  hue += 4;
  if (downUp != 1) {
    for (byte i = 0; i < WIDTH; i++) {
      CHSV thisColor = CHSV((byte)(hue + i * float(255 / WIDTH)), 255, 255);
      leds.DrawPixel(i, 0 , thisColor);
      leds.DrawPixel(i + 1, 0 , thisColor);
      leds.DrawPixel((WIDTH - 1) - i, 0 , thisColor);
      leds.DrawPixel((WIDTH - 2) - i, 0 , thisColor);
      FastLED.delay(60);
      leds.ShiftUp();
    }
    FastLED.show();
  }
  if (downUp != 2) {
    for (byte i = 0; i < WIDTH ; i++) {
      CHSV thisColor = CHSV((byte)(hue + i * float(255 / WIDTH)), 255, 255);
      leds.DrawPixel(i, STEPS - 1 , thisColor);
      leds.DrawPixel(i + 1, STEPS - 1 , thisColor);
      leds.DrawPixel((WIDTH - 2) - i, STEPS - 1 , thisColor);
      leds.DrawPixel((WIDTH - 1) - i, STEPS - 1 , thisColor);
      FastLED.delay(60);
      leds.ShiftDown();
    }
    FastLED.show();
  }
}

void turn_on_5() {
  hue += 100;
  if (downUp != 1) {
    for (uint16_t i = 0; i < WIDTH - 2 ; i++) {
      CHSV thisColor = CHSV((byte)(hue + i * float(255 / WIDTH)), 255, 255);
      leds.DrawPixel(0, 0, CRGB::Blue);
      leds.DrawPixel(i + 1, 0, thisColor);
      leds.DrawPixel((WIDTH - 1) , 0, CRGB::Blue);
      leds.DrawPixel((WIDTH - 2) - i, 0, thisColor);
      FastLED.delay(100);
      leds.ShiftUp();
    }
    FastLED.show();
  }
  if (downUp != 2) {
    for (uint16_t i = 0; i < WIDTH - 2 ; i++) {
      CHSV thisColor = CHSV((byte)(hue + i * float(255 / WIDTH)), 255, 255);
      leds.DrawPixel(0, STEPS - 1, CRGB::Blue);
      leds.DrawPixel(i + 1, STEPS - 1, thisColor);
      leds.DrawPixel((WIDTH - 1) , STEPS - 1, CRGB::Blue);
      leds.DrawPixel((WIDTH - 2) - i, STEPS - 1, thisColor);
      FastLED.delay(100);
      leds.ShiftDown();
    }
    FastLED.show();
  }
}

void turn_on_off_0(uint32_t White, uint32_t Black) {
  if (downUp != 1) {
    for (uint16_t i = 0; i < STEPS ; i++) {
      leds.DrawLine(0 , i, WIDTH, i, White);
      FastLED.delay(random(30, 110));
      FastLED.show();
      leds.DrawLine(0 , i, WIDTH, i, Black);
      FastLED.delay(random(100, 200));
      FastLED.show();
      leds.DrawLine(0 , i, WIDTH, i, White);
      FastLED.delay(random(10, 70));
      FastLED.show();
      leds.DrawLine(0 , i, WIDTH, i, Black);
      FastLED.delay(random(50, 100));
      FastLED.show();
      leds.DrawLine(0 , i, WIDTH, i, White);
      FastLED.show();
      FastLED.delay(250);
    }
  }
  if (downUp != 2) {
    for (uint16_t i = 0; i < STEPS ; i++) {
      leds.DrawLine(0 , STEPS - 1  - i, WIDTH, STEPS - 1 - i, White);
      FastLED.delay(random(30, 110));
      FastLED.show();
      leds.DrawLine(0 , STEPS - 1 - i, WIDTH, STEPS - 1 - i, Black);
      FastLED.delay(random(100, 200));
      FastLED.show();
      leds.DrawLine(0 , STEPS - 1 - i, WIDTH, STEPS - 1 - i, White);
      FastLED.delay(random(10, 70));
      FastLED.show();
      leds.DrawLine(0 , STEPS - 1 - i, WIDTH, STEPS - 1 - i, Black);
      FastLED.delay(random(50, 100));
      FastLED.show();
      leds.DrawLine(0 , STEPS - 1 - i, WIDTH, STEPS - 1 - i, White);
      FastLED.show();
      FastLED.delay(250);
    }
  }
}

void turn_on_off_1(uint32_t color) {
  if (downUp != 1) {
    for (int j = 0; j < STEPS ; j++)
      for (int i = 0; i < WIDTH / 2; i++) {
        leds.DrawPixel(i , j, color);
        leds.DrawPixel(WIDTH - 1 - i , j, color);
        FastLED.delay(100);
        FastLED.show();
      }
  }
  if (downUp != 2) {
    for (int j = 0; j < STEPS ; j++)
      for (int i = 0; i < WIDTH / 2; i++) {
        leds.DrawPixel(i , STEPS - 1 - j, color);
        leds.DrawPixel(WIDTH - 1 - i , STEPS - 1 - j, color);
        FastLED.delay(100);
        FastLED.show();
      }
  }
}

void turn_on_off_2(uint32_t color) {
  if (downUp != 1) {
    for (int j = 0; j < STEPS ; j++)
      for (int i = 0; i < WIDTH / 2; i++) {
        leds.DrawPixel((WIDTH / 2) - 1 - i , j, color);
        leds.DrawPixel((WIDTH / 2) + i , j, color);
        FastLED.delay(80);
        FastLED.show();
      }
  }
  if (downUp != 2) {
    for (int j = 0; j < STEPS ; j++)
      for (int i = 0; i < WIDTH / 2; i++) {
        leds.DrawPixel((WIDTH / 2) - 1 - i , STEPS - 1 - j, color);
        leds.DrawPixel((WIDTH / 2) + i , STEPS - 1 - j, color);
        FastLED.delay(80);
        FastLED.show();
      }
  }
}

void turn_on_6() {
  for (int i = 0; i < STEPS; i++) {
    for (int j = 0; j < WIDTH; j++) {
      int delta = abs(STEPS - i * 2) + abs(WIDTH - j * 2);
      (*leds[i * WIDTH + j]) = CHSV(delta * 4 + counter, 255, 255);
    }
  }
  FastLED.show();
  delay(5);
  counter++;
}

void turn_on_7() {
  if (downUp != 1) {
    for (int j = 0; j < STEPS; j++) {
      for (int i = STEPS  ; i >= 0 + j; i--) {
        leds.DrawLine(0, i + 1, WIDTH, i + 1 , CRGB(0, 0, 0));
        leds.DrawLine(0, i, WIDTH, i, CRGB(100, 100, 100));
        FastLED.delay(100);
      }
    }
    FastLED.show();
  }
  if (downUp != 2) {
    for (int j = 0; j < STEPS; j++) {
      for (int i = 0  ; i <= STEPS - j - 1; i++) {
        leds.DrawLine(0, i - 1, WIDTH, i - 1 , CRGB(0, 0, 0));
        leds.DrawLine(0, i, WIDTH, i, CRGB(100, 100, 100));
        FastLED.delay(100);
      }
    }
    FastLED.show();
  }
}

void turn_on_off_3(uint32_t  rcolors) {
  int cx = WIDTH / 2;
  if (downUp != 1) {
    for (int y = 0; y < (STEPS / 2) + 1; y++) {
      for (int x = 0; x < cx ; x++) {
        leds.DrawPixel((WIDTH - 1) - ((cx - x) - 1), y * 2, rcolors);
        leds.DrawPixel((cx - x) - 1, y * 2, rcolors);
        FastLED.delay(100);
      }
      FastLED.show();
      for (int x = 0; x < cx; x++) {
        leds.DrawPixel(x, (y * 2) + 1, rcolors);
        leds.DrawPixel((WIDTH - 1) - x, (y * 2) + 1, rcolors);
        FastLED.delay(100);
      }
      FastLED.show();
    }
  }
  if (downUp != 2) {
    for (int y = (STEPS / 2); y >= 0; y--) {
      for (int x = 0; x < cx ; x++) {
        leds.DrawPixel((WIDTH - 1) - ((cx - x) - 1), y * 2, rcolors);
        leds.DrawPixel((cx - x) - 1, y * 2, rcolors);
        FastLED.delay(100);
      }
      FastLED.show();
      for (int x = 0; x < cx; x++) {
        leds.DrawPixel(x, (y * 2) - 1, rcolors);
        leds.DrawPixel((WIDTH - 1) - x, (y * 2) - 1, rcolors);
        FastLED.delay(100);
      }
      FastLED.show();
    }
  }
}

void turn_on_off_4(uint32_t  rcolors) {
  int cx = WIDTH / 2;
  if (downUp != 1) {
    for (int y = 0; y < (STEPS / 2) + 1; y++) {
      for (int x = 0; x < cx; x++) {
        leds.DrawPixel(x, y * 2, rcolors);
        leds.DrawPixel((WIDTH - 1) - x, y * 2, rcolors);
        FastLED.delay(100);
      }
      FastLED.show();
      for (int x = 0; x < cx ; x++) {
        leds.DrawPixel((cx - x) - 1, (y * 2) + 1, rcolors);
        leds.DrawPixel((WIDTH - 1) - ((cx - x) - 1), (y * 2) + 1, rcolors);
        FastLED.delay(100);
      }
      FastLED.show();
    }
  }
  if (downUp != 2) {
    for (int y = STEPS / 2; y >= 0; y--) {
      for (int x = 0; x < cx; x++) {
        leds.DrawPixel(x, y * 2, rcolors);
        leds.DrawPixel((WIDTH - 1) - x, y * 2, rcolors);
        FastLED.delay(100);
      }
      FastLED.show();
      for (int x = 0; x < cx ; x++) {
        leds.DrawPixel((cx - x) - 1, (y * 2) - 1, rcolors);
        leds.DrawPixel((WIDTH - 1) - ((cx - x) - 1), (y * 2) - 1, rcolors);
        FastLED.delay(100);
      }
      FastLED.show();
    }
  }
}

void turn_on_off_5(uint32_t  rcolors) {
  for (int y = STEPS; y > 0; y--) {
    leds.DrawLine(0, STEPS - y - 1 , WIDTH, STEPS - y - 1, rcolors);
    leds.DrawLine(0, y , WIDTH, y, rcolors);
    FastLED.delay(500);
  }
  FastLED.show();
}

void turn_on_off_6(uint32_t  rcolors) {
  int cy = (STEPS / 2) + 1;
  for (int y = cy; y > 0; y--) {
    leds.DrawLine(0, y - 1 , WIDTH, y - 1, rcolors);
    leds.DrawLine(0, STEPS - y , WIDTH, STEPS - y, rcolors);
    FastLED.delay(500);
  }
  FastLED.show();
}

