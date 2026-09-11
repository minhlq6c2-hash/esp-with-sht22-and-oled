#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// =========================
// SHT22
// =========================
#define SHT22_PIN 15
#define DHT_TYPE DHT22

DHT sht22(SHT22_PIN, DHT_TYPE);

// =========================
// OLED SSD1306
// =========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 5
#define OLED_SCL 4
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1
);

// =========================
// MSL
// =========================
#define MSL_LEVEL 3

const char* getFloorLife(int msl)
{
    switch (msl)
    {
        case 1:
            return "UNLIMITED";

        case 2:
            return "1 YEAR";

        case 3:
            return "168 H";

        case 4:
            return "72 H";

        case 5:
            return "48 H";

        case 6:
            return "TOL";

        default:
            return "UNKNOWN";
    }
}

// =========================
// SETUP
// =========================
void setup()
{
    Serial.begin(115200);

    // Start SHT22
    sht22.begin();

    // Start I2C
    Wire.begin(OLED_SDA, OLED_SCL);

    // Start OLED
    if (!display.begin(
            SSD1306_SWITCHCAPVCC,
            OLED_ADDR))
    {
        Serial.println("OLED NOT FOUND!");

        while (1)
        {
            delay(1000);
        }
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(20, 25);
    display.println("MSL MONITOR");

    display.display();

    delay(2000);
}

// =========================
// LOOP
// =========================
void loop()
{
    float temperature = sht22.readTemperature();
    float humidity = sht22.readHumidity();

    // Check sensor
    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println("Failed to read SHT22!");

        display.clearDisplay();

        display.setTextSize(2);
        display.setCursor(10, 25);
        display.println("SENSOR ERR");

        display.display();

        delay(500);

        return;
    }

    // =========================
    // Serial Monitor
    // =========================

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("MSL: ");
    Serial.println(MSL_LEVEL);

    Serial.print("Floor Life: ");
    Serial.println(getFloorLife(MSL_LEVEL));

    // =========================
    // OLED
    // =========================

    display.clearDisplay();

    display.setTextSize(1);

    // Temperature
    display.setCursor(0, 0);
    display.print("TEMP: ");

    display.setTextSize(2);
    display.setCursor(45, 0);
    display.print(temperature, 1);

    display.setTextSize(1);
    display.print(" C");

    // Humidity
    display.setCursor(0, 17);
    display.print("RH:   ");

    display.setTextSize(2);
    display.setCursor(45, 17);
    display.print(humidity, 1);

    display.setTextSize(1);
    display.print(" %");

    // MSL
    display.setCursor(0, 36);
    display.print("MSL:  ");

    display.setTextSize(2);
    display.setCursor(45, 34);
    display.print(MSL_LEVEL);

    // Floor life
    display.setTextSize(1);
    display.setCursor(0, 53);
    display.print("FLOOR: ");

    display.setTextSize(1);
    display.print(getFloorLife(MSL_LEVEL));

    display.display();

    // =========================
    // WARNING
    // =========================

    if (temperature > 30.0 || humidity > 60.0)
    {
        Serial.println("WARNING: Environment above reference condition!");
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.print(" WARNING: Environment above reference condition!");
        display.display();
    }

    delay(2000);
}