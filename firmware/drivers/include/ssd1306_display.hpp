#ifndef SSD1306_DISPLAY_HPP_
#define SSD1306_DISPLAY_HPP_

#include <stdbool.h>
#include <stdint.h>

namespace ssd1306 {

//  Enumeration for screen colors
typedef enum {
  Black = 0x00, // Black color, no pixel
  White = 0x01, // Pixel is set. Color depends on LCD
  Inverse = 0x02,
} color_t;

typedef struct {
  const uint8_t *data;
  uint16_t width;
  uint16_t height;
  uint8_t dataSize;
} image_t;

typedef struct {
  long int code;
  const image_t *image;
} character_t;

typedef struct {
  int length;
  const character_t *chars;
} font_t;

typedef struct {
  uint16_t CurrentX;
  uint16_t CurrentY;
  uint8_t Inverted;
  color_t Color;
} SSD1306_t;

uint16_t getWidth(void);
uint16_t getHeight(void);
color_t getColor(void);
void setColor(color_t color);
bool init(void);
void fill();
void update(void);
void drawPixel(uint8_t x, uint8_t y);
void drawBitmap(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t *pBMP);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void drawVerticalLine(int16_t x, int16_t y, int16_t length);
void drawHorizontalLine(int16_t x, int16_t y, int16_t length);
void drawRect(int16_t x, int16_t y, int16_t width, int16_t height);
void drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
void fillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height);
void drawCircle(int16_t x0, int16_t y0, int16_t radius);
void fillCircle(int16_t x0, int16_t y0, int16_t radius);
void drawCircleQuads(int16_t x0, int16_t y0, int16_t radius, uint8_t quads);
void drawProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress);
void drawString(const char *str, int x, int y, const font_t *font_t);
void drawBitmapMono(int x, int y, const image_t *image);
void setCursor(uint8_t x, uint8_t y);
void displayOn(void);
void displayOff(void);
void invertDisplay(void);
void normalDisplay(void);
void resetOrientation(void);
void flipScreenVertically(void);
void mirrorScreen(void);
void clearScreen(void);

// void Start_Scroll_Right(uint8_t start, uint8_t stop);
// void SSD1306_ScrollDSetup(uint8_t dir, uint8_t start, uint8_t end, uint8_t interval, uint8_t voffs);
// void SSD1306_ScrollHSetup(uint8_t dir, uint8_t start, uint8_t end, uint8_t interval);
// void SSD1306_ScrollStart(void);
// void SSD1306_ScrollStop(void);

} // namespace ssd1306

#endif // SSD1306_DISPLAY_HPP_