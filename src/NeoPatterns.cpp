#include <Adafruit_NeoPixel.h>
#include "NeoPatterns.h"

// Constructor - calls base-class constructor to initialize strip
NeoPatterns :: NeoPatterns(uint16_t pixels, 
                           uint8_t pin, 
                           uint8_t type, 
                           void (*callback)()) : Adafruit_NeoPixel(pixels, pin, type) {
  on_patternComplete = callback;
}

// Update the pattern
void NeoPatterns :: updatePattern() {
  if ((millis() - m_lastUpdate) > m_interval) { // time to update
    m_lastUpdate = millis();
    switch (m_activePattern) {
    case RAINBOW_CYCLE:
      updateRainbowCycle();
      break;
    case THEATER_CHASE:
      updateTheaterChase();
      break;
    case COLOR_WIPE:
      updateColorWipe();
      break;
    case SCANNER:
      updateScanner();
      break;
    case FADE:
      updateFade();
      break;
    default:
      break;
    }
  }
}

// Increment the Index and reset at the end
void NeoPatterns :: increment() {
  if (m_direction == FORWARD) {
    m_index++;
    if (m_index >= m_totalSteps) {
      m_index = 0;
      if (on_patternComplete != NULL) {
        on_patternComplete(); // call the comlpetion callback
      }
    }
  } else { // m_direction == REVERSE
    --m_index;
    if (m_index <= 0) {
      m_index = m_totalSteps - 1;
      if (on_patternComplete != NULL) {
        on_patternComplete(); // call the comlpetion callback
      }
    }
  }
}

// Reverse pattern direction
void NeoPatterns :: reverse() {
  if (m_direction == FORWARD) {
    m_direction = REVERSE;
    m_index = m_totalSteps - 1;
  } else {
    m_direction = FORWARD;
    m_index = 0;
  }
}

// Initialize for a RainbowCycle
void NeoPatterns :: initRainbowCycle(uint8_t interval, direction dir) {
  m_activePattern = RAINBOW_CYCLE;
  m_interval = interval;
  m_totalSteps = 255;
  m_index = 0;
  m_direction = dir;
}

// Update the Rainbow Cycle Pattern
void NeoPatterns :: updateRainbowCycle() {
  for (int i = 0; i < numPixels(); i++) {
    setPixelColor(i,wheel(((i * 256 / numPixels()) + m_index) & 255));
  }
  show();
  increment();
}

// Initialize for a Theater Chase
void NeoPatterns :: initTheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir) {
  m_activePattern = THEATER_CHASE;
  m_interval = interval;
  m_totalSteps = numPixels();
  m_color1 = color1;
  m_color2 = color2;
  m_index = 0;
  m_direction = dir;
}

// Update the Theater Chase Pattern
void NeoPatterns :: updateTheaterChase() {
  for (int i = 0; i < numPixels(); i++) {
    if ((i + m_index) % 3 == 0) {
      setPixelColor(i, m_color1);
    } else {
      setPixelColor(i, m_color2);
    }
  }
  show();
  increment();
}

// Initialize for a ColorWipe
void NeoPatterns :: initColorWipe(uint32_t color, uint8_t interval, direction dir) {
  m_activePattern = COLOR_WIPE;
  m_interval = interval;
  m_totalSteps = numPixels();
  m_color1 = color;
  m_index = 0;
  m_direction = dir;
}

// Update the Color Wipe Pattern
void NeoPatterns :: updateColorWipe() {
  setPixelColor(m_index, m_color1);
  show();
  increment();
}

// Initialize for a SCANNNER
void NeoPatterns :: initScanner(uint32_t color1, uint8_t interval) {
  m_activePattern = SCANNER;
  m_interval = interval;
  m_totalSteps = (numPixels() - 1) * 2;
  m_color1 = color1;
  m_index = 0;
}

// Update the Scanner Pattern
void NeoPatterns :: updateScanner() {
  for (int i = 0; i < numPixels(); i++) {
    if (i == m_index) { // Scan Pixel to the right
      setPixelColor(i, m_color1);
    } else if (i == m_totalSteps - m_index) { // Scan Pixel to the left
      setPixelColor(i, m_color1);
    } else { // Fading tail
      setPixelColor(i, dimColor(getPixelColor(i)));
    }
  }
  show();
  increment();
}

// Initialize for a Fade
void NeoPatterns :: initFade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir) {
  m_activePattern = FADE;
  m_interval = interval;
  m_totalSteps = steps;
  m_color1 = color1;
  m_color2 = color2;
  m_index = 0;
  m_direction = dir;
}

// Update the Fade Pattern
void NeoPatterns :: updateFade() {
  // Calculate linear interpolation between Color1 and Color2
  // Optimise order of operations to minimize truncation error
  uint8_t r = ((red(m_color1) * (m_totalSteps - m_index))
               + (red(m_color2) * m_index)) / m_totalSteps;
  uint8_t g = ((green(m_color1) * (m_totalSteps - m_index))
               + (green(m_color2) * m_index)) / m_totalSteps;
  uint8_t b = ((blue(m_color1) * (m_totalSteps - m_index))
               + (blue(m_color2) * m_index)) / m_totalSteps;

  setColor(Color(r,g,b));
  show();
  increment();
}

// Calculate 50% dimmed version of a color (used by ScannerUpdate)
uint32_t NeoPatterns :: dimColor(uint32_t color) {
  // Shift R, G and B components one bit to the right
  return Color(red(color) >> 1, green(color) >> 1, blue(color) >> 1);
}

// Set all pixels to a color (synchronously)
void NeoPatterns :: setColor(uint32_t color) {
  for (int i = 0; i < numPixels(); i++) {
    setPixelColor(i, color);
  }
  show();
}

// Returns the Red component of a 32-bit color
uint8_t NeoPatterns :: red(uint32_t color) {
  return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t NeoPatterns :: green(uint32_t color) {
  return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t NeoPatterns :: blue(uint32_t color) {
  return color & 0xFF;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t NeoPatterns :: wheel(byte pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return Color(255 - pos * 3, 0, pos * 3);
  } else if (pos < 170) {
    pos -= 85;
    return Color(0, pos * 3, 255 - pos * 3);
  } else {
    pos -= 170;
    return Color(pos * 3, 255 - pos * 3, 0);
  }
}
