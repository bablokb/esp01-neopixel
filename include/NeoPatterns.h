#ifndef NeoPatterns_h
#define NeoPatterns_h
#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel {
  private:
    pattern   m_activePattern;  // which pattern is running
    direction m_direction;      // direction to run the pattern

    unsigned long m_interval;   // milliseconds between updates
    unsigned long m_lastUpdate; // last update of position

    uint32_t m_color1, m_color2;  // What colors are in use
    uint16_t m_totalSteps;  // total number of steps in the pattern
    uint16_t m_index;  // current step within the pattern

  public:
    void (*on_patternComplete)();  // Callback on completion of pattern


    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, 
                uint8_t pin, 
                uint8_t type, 
                void (*callback)());

    void updatePattern();
    void increment();
    void reverse();
    void initRainbowCycle(uint8_t interval, direction dir = FORWARD);
    void updateRainbowCycle();
    void initTheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD);
    void updateTheaterChase();
    void initColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD);
    void updateColorWipe();
    void initScanner(uint32_t color1, uint8_t interval);
    void updateScanner();
    void initFade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD);
    void updateFade();
    uint32_t dimColor(uint32_t color);
    void setColor(uint32_t color);
    uint8_t red(uint32_t color);
    uint8_t green(uint32_t color);
    uint8_t blue(uint32_t color);
    uint32_t wheel(byte pos);
};
#endif
