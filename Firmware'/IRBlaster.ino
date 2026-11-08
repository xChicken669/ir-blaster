#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.hpp>

constexpr uint8_t OLED_SDA_PIN = 6;

// gooosee!!!(random comment)
constexpr uint8_t OLED_SCL_PIN = 7;
constexpr uint8_t IR_TX_PIN = 4;
constexpr uint8_t IR_RX_PIN = 5;

constexpr uint8_t BUTTON_UP_PIN = 0;
constexpr uint8_t BUTTON_LEFT_PIN = 10;
constexpr uint8_t BUTTON_RIGHT_PIN = 3;
constexpr uint8_t BUTTON_CENTER_PIN = 1;

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t OLED_I2C_ADDRESS = 0x3C;
constexpr uint8_t SEND_REPEATS = 2;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 30;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

struct Button {
  uint8_t pin;
  bool stableState;
  bool lastReading;
  uint32_t changedAt;
};

Button buttons[] = {
    {BUTTON_UP_PIN, HIGH, HIGH, 0},
    {BUTTON_LEFT_PIN, HIGH, HIGH, 0},
    {BUTTON_RIGHT_PIN, HIGH, HIGH, 0},
    {BUTTON_CENTER_PIN, HIGH, HIGH, 0},
};

enum ButtonIndex : uint8_t {
  BTN_UP,
  BTN_LEFT,
  BTN_RIGHT,
  BTN_CENTER
};

enum Screen : uint8_t {
  BRAND_SCREEN,
  RECEIVE_SCREEN
};

enum SendProtocol : uint8_t {
  SEND_SAMSUNG,
  SEND_NEC,
  SEND_PANASONIC,
  SEND_SONY
};

const char *const BUTTON_NAMES[] = {
    "UP",
    "LEFT",
    "RIGHT",
    "CENTER"
};

struct Brand {
  const char *name;
  SendProtocol protocol;
  uint16_t address;
  uint16_t command;
};

const Brand brands[] = {
    {"SAMSUNG", SEND_SAMSUNG, 0xE0E0, 0x00A9},
    {"TCL", SEND_NEC, 0x0D5F, 0x002A},
    {"PANASONIC", SEND_PANASONIC, 0x0100, 0x00BC},
    {"SONY", SEND_SONY, 0x0005, 0x0010},
    {"LG", SEND_NEC, 0x20DF, 0x0010},
};

constexpr uint8_t BRAND_COUNT = sizeof(brands) / sizeof(brands[0]);

uint8_t selectedBrand = 0;
Screen currentScreen = BRAND_SCREEN;
char lastProtocol[16] = "Waiting";
uint16_t lastAddress = 0;
uint16_t lastCommand = 0;
bool hasReceivedCode = false;
bool screenNeedsRedraw = true;
uint32_t statusUntil = 0;
char statusLine[22] = "";

bool wasPressed(ButtonIndex index) {
  Button &button = buttons[index];
  const bool reading = digitalRead(button.pin);
  const uint32_t now = millis();

  if (reading != button.lastReading) {
    button.lastReading = reading;
    button.changedAt = now;
  }

  if (now - button.changedAt >= BUTTON_DEBOUNCE_MS &&
      reading != button.stableState) {
    button.stableState = reading;
    return button.stableState == LOW;
  }

  return false;
}

void showStatus(const char *message) {
  snprintf(statusLine, sizeof(statusLine), "%s", message);
  statusUntil = millis() + 1500;
  screenNeedsRedraw = true;
}

void drawBrandScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("IR BLASTER"));
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 17);
  display.println(brands[selectedBrand].name);

  display.setTextSize(1);
  display.setCursor(0, 41);
  display.println(F("LEFT/RIGHT: brand"));
  display.setCursor(0, 52);
  display.println(F("UP: receive  CENTER: send"));

  if (statusLine[0] != '\0' && millis() < statusUntil) {
    display.fillRect(0, 50, SCREEN_WIDTH, 14, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 53);
    display.print(statusLine);
    display.setTextColor(SSD1306_WHITE);
  }

  display.display();
}

void drawReceiveScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("IR RECEIVE MONITOR"));
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);

  if (!hasReceivedCode) {
    display.setCursor(0, 25);
    display.println(F("Point a remote at RX"));
    display.setCursor(0, 39);
    display.println(F("Waiting for signal"));
  } else if (strcmp(lastProtocol, "UNKNOWN") == 0) {
    display.setCursor(0, 25);
    display.println(F("Unknown signal"));
    display.setCursor(0, 39);
    display.println(F("Check Serial Monitor"));
  } else {
    char line[24];

    display.setCursor(0, 18);
    display.print(F("Protocol: "));
    display.println(lastProtocol);

    snprintf(line, sizeof(line), "Address: 0x%04X", lastAddress);
    display.setCursor(0, 30);
    display.println(line);

    snprintf(line, sizeof(line), "Command: 0x%04X", lastCommand);
    display.setCursor(0, 41);
    display.println(line);
  }

  display.setCursor(0, 54);
  display.println(F("UP: brands  CENTER: clear"));
  display.display();
}

void sendSelectedPower() {
  const Brand &brand = brands[selectedBrand];

  Serial.print(F("Power command: "));
  Serial.println(brand.name);

  switch (brand.protocol) {
    case SEND_SAMSUNG:
      IrSender.sendSamsung16BitAddressAnd8BitCommand(
          brand.address, brand.command, SEND_REPEATS);
      break;

    case SEND_NEC:
      IrSender.sendNEC(brand.address, brand.command, SEND_REPEATS);
      break;

    case SEND_PANASONIC:
      IrSender.sendPanasonic(brand.address, brand.command, SEND_REPEATS);
      break;

    case SEND_SONY:
      IrSender.sendSony(brand.address, brand.command, SEND_REPEATS);
      break;
  }

  char message[22];
  snprintf(message, sizeof(message), "SENT: %s", brand.name);
  showStatus(message);
}

void handleButtons() {
  const bool upPressed = wasPressed(BTN_UP);
  const bool leftPressed = wasPressed(BTN_LEFT);
  const bool rightPressed = wasPressed(BTN_RIGHT);
  const bool centerPressed = wasPressed(BTN_CENTER);

  if (upPressed) {
    currentScreen =
        currentScreen == BRAND_SCREEN ? RECEIVE_SCREEN : BRAND_SCREEN;
    screenNeedsRedraw = true;
  }

  if (currentScreen == BRAND_SCREEN) {
    if (leftPressed) {
      selectedBrand = (selectedBrand + BRAND_COUNT - 1) % BRAND_COUNT;
      statusLine[0] = '\0';
      screenNeedsRedraw = true;
    }

    if (rightPressed) {
      selectedBrand = (selectedBrand + 1) % BRAND_COUNT;
      statusLine[0] = '\0';
      screenNeedsRedraw = true;
    }

    if (centerPressed) {
      sendSelectedPower();
    }
  } else if (centerPressed) {
    hasReceivedCode = false;
    screenNeedsRedraw = true;
  }
}

void handleIrReceive() {
  if (!IrReceiver.decode()) {
    return;
  }

  const IRData &data = IrReceiver.decodedIRData;

  if ((data.flags & IRDATA_FLAGS_IS_REPEAT) == 0) {
    if (data.protocol == UNKNOWN) {
      snprintf(lastProtocol, sizeof(lastProtocol), "UNKNOWN");
      lastAddress = 0;
      lastCommand = 0;
    } else {
      snprintf(
          lastProtocol,
          sizeof(lastProtocol),
          "%s",
          IrReceiver.getProtocolString());
      lastAddress = data.address;
      lastCommand = data.command;
    }

    hasReceivedCode = true;

    Serial.print(F("Received: "));
    IrReceiver.printIRResultShort(&Serial);

    if (currentScreen == RECEIVE_SCREEN) {
      screenNeedsRedraw = true;
    }
  }

  IrReceiver.resume();
}

void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println();
  Serial.println(F("IR blaster"));

  for (Button &button : buttons) {
    pinMode(button.pin, INPUT_PULLUP);
    button.stableState = digitalRead(button.pin);
    button.lastReading = button.stableState;
    button.changedAt = millis();
  }

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS, true, false)) {
    Serial.println(F("OLED initialization failed :((("));
    while (true) {
      delay(100);
    }
  }

  IrSender.begin(IR_TX_PIN);
  IrReceiver.begin(IR_RX_PIN, DISABLE_LED_FEEDBACK);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(25, 16);
  display.println(F("READY"));
  display.setTextSize(1);
  display.setCursor(18, 43);
  display.println(F("ESP32-C3 IR TEST"));
  display.display();

  delay(700);
  screenNeedsRedraw = true;
}

void loop() {
  handleIrReceive();
  handleButtons();

  if (currentScreen == BRAND_SCREEN &&
      statusLine[0] != '\0' &&
      millis() >= statusUntil) {
    statusLine[0] = '\0';
    screenNeedsRedraw = true;
  }

  if (screenNeedsRedraw) {
    if (currentScreen == BRAND_SCREEN) {
      drawBrandScreen();
    } else {
      drawReceiveScreen();
    }

    screenNeedsRedraw = false;
  }
}
