#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>

#define TFT_CS D8
#define TFT_RST D4
#define TFT_DC D3

#define TS_CS D2

#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 40

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define TS_MINX 100
#define TS_MAXX 900

#define TS_MINY 100
#define TS_MAXY 900

#define NUM_BUTTONS 5

const int buttonPins[NUM_BUTTONS] = {D5, D6, D7, D9, D10};

const String coffeeNames[NUM_BUTTONS] = {"Kava 1", "Kava 2", "Kava 3", "Kava 4", "Kava 5"};
const int coffeePrices[NUM_BUTTONS] = {10, 15, 12, 8, 20};

TouchScreen ts = TouchScreen(TS_CS, TS_MINX, TS_MAXX, TS_MINY, TS_MAXY);

volatile bool newCardDetected = false;  // Varijabla koja označava je li detektiran novi RFID žeton
int temporaryAmount = 0;  // Privremeni iznos za trošenje

void setup() {
  Serial.begin(9600);

  tft.begin();
  tft.setRotation(3);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], OUTPUT);
    digitalWrite(buttonPins[i], HIGH);
  }

  attachInterrupt(digitalPinToInterrupt(D11), handleNewCard, RISING);  // Postavljanje prekidača na pin D11

  drawCoffeeButtons();
  displayTemporaryAmount();
}

void loop() {
  if (newCardDetected) {
    // Povećaj privremeni iznos za trošenje na temelju RFID žetona
    increaseTemporaryAmount();

    // Resetiranje varijable
    newCardDetected = false;
  }

  TSPoint touch = ts.getPoint();

  if (touch.z >= 10 && touch.z <= 1000) {
    int touchX = map(touch.y, TS_MINY, TS_MAXY, 0, tft.width());
    int touchY = map(touch.x, TS_MINX, TS_MAXX, 0, tft.height());

    for (int i = 0; i < NUM_BUTTONS; i++) {
      if (touchX >= i * BUTTON_WIDTH && touchX <= (i + 1) * BUTTON_WIDTH && touchY >= 0 && touchY <= BUTTON_HEIGHT) {
        if (temporaryAmount >= coffeePrices[i]) {
          activateCoffeeButton(i);
        }
        break;
      }
    }
  }
}

void drawCoffeeButtons() {
  tft.fillScreen(ILI9341_BLACK);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    tft.fillRect(i * BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9341_BLUE);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(i * BUTTON_WIDTH + 10, 10);
    tft.println(coffeeNames[i]);
    tft.setCursor(i * BUTTON_WIDTH + 10, 30);
    tft.print("Cijena: ");
    tft.println(coffeePrices[i]);
  }
}

void activateCoffeeButton(int buttonIndex) {
  // Isključi sve gumbe
  for (int i = 0; i < NUM_BUTTONS; i++) {
    digitalWrite(buttonPins[i], HIGH);
  }

  // Aktiviraj odabrani gumb
  digitalWrite(buttonPins[buttonIndex], LOW);

  // Smanji privremeni iznos za trošenje za odabrani napitak
  decreaseTemporaryAmount();

  // Implementirajte aktivaciju odabranog napitka
  // Ovdje možete izvršiti potrebne radnje na aparatu za kavu

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.println("Odabrana kava:");
  tft.setCursor(10, 40);
  tft.println(coffeeNames[buttonIndex]);
}

void handleNewCard() {
  // Detektiran novi RFID žeton
  newCardDetected = true;
}

void increaseTemporaryAmount() {
  // Implementirajte logiku za povećanje privremenog iznosa za trošenje na temelju RFID žetona
  // Ovdje možete izvršiti potrebne radnje na aparatu za kavu

  temporaryAmount += 10;  // Povećaj privremeni iznos za trošenje za 10
  displayTemporaryAmount();
}

void decreaseTemporaryAmount() {
  // Implementirajte logiku za smanjivanje privremenog iznosa za trošenje
  // Ovdje možete izvršiti potrebne radnje na aparatu za kavu

  temporaryAmount -= 10;  // Smanji privremeni iznos za trošenje za 10
  displayTemporaryAmount();
}

void displayTemporaryAmount() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.println("Iznos za trošenje:");
  tft.setCursor(10, 110);
  tft.println(temporaryAmount);
}
