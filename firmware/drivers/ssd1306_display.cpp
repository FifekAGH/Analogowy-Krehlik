#include "ssd1306_display.hpp"
#include "ssd1306_display_port.hpp"
#include "ssd1306_display_registers.hpp"

#include <stdlib.h>
#include <string.h>

#define SWAP_INT16_T(a, b)                                                                                                                           \
  {                                                                                                                                                  \
    int16_t t = a;                                                                                                                                   \
    a = b;                                                                                                                                           \
    b = t;                                                                                                                                           \
  }

#define SSD1306_128X32
// #define SSD1306_128X64

#ifdef SSD1306_128X64
#define SSD1306_GEOMETRY ssd1306::GEOMETRY_128_64
// SSD1306 width in pixels
#define SSD1306_WIDTH 128
// SSD1306 LCD height in pixels
#define SSD1306_HEIGHT 64
#endif
#ifdef SSD1306_128X32
#define SSD1306_GEOMETRY ssd1306::GEOMETRY_128_32
// SSD1306 width in pixels
#define SSD1306_WIDTH 128
// SSD1306 LCD height in pixels
#define SSD1306_HEIGHT 32
#endif

// SSD1306 LCD Buffer Size
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

//
//  Get a width and height screen size
//
static const uint16_t width(void) { return SSD1306_WIDTH; };
static const uint16_t height(void) { return SSD1306_HEIGHT; };

static ssd1306::SSD1306_t SSD1306;

static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];

ssd1306::geometry_t display_geometry = SSD1306_GEOMETRY;

namespace ssd1306 {

uint16_t getWidth(void) { return SSD1306_WIDTH; }

uint16_t getHeight(void) { return SSD1306_HEIGHT; }

color_t getColor(void) { return SSD1306.Color; }

void setColor(color_t color) { SSD1306.Color = color; }

//	initialize the oled screen
bool init(void) {
  /* init LCD */
  writeCommand(DISPLAYOFF);
  writeCommand(SETDISPLAYCLOCKDIV);
  writeCommand(0xF0); // Increase speed of the display max ~96Hz
  writeCommand(SETMULTIPLEX);
  writeCommand(height() - 1);
  writeCommand(SETDISPLAYOFFSET);
  writeCommand(0x00);
  writeCommand(SETSTARTLINE);
  writeCommand(CHARGEPUMP);
  writeCommand(0x14);
  writeCommand(MEMORYMODE);
  writeCommand(0x00);
  writeCommand(SEGREMAP);
  writeCommand(COMSCANINC);
  writeCommand(SETCOMPINS);

  if (display_geometry == GEOMETRY_128_64) {
    writeCommand(0x12);
  } else if (display_geometry == GEOMETRY_128_32) {
    writeCommand(0x02);
  }

  writeCommand(SETCONTRAST);

  if (display_geometry == GEOMETRY_128_64) {
    writeCommand(0xCF);
  } else if (display_geometry == GEOMETRY_128_32) {
    writeCommand(0x8F);
  }

  writeCommand(SETPRECHARGE);
  writeCommand(0xF1);
  writeCommand(SETVCOMDETECT); // 0xDB, (additionally needed to lower the contrast)
  writeCommand(0x40);          // 0x40 default, to lower the contrast, put 0
  writeCommand(DISPLAYALLON_RESUME);
  writeCommand(NORMALDISPLAY);
  writeCommand(0x2e); // stop scroll
  writeCommand(DISPLAYON);

  // Set default values for screen object
  SSD1306.CurrentX = 0;
  SSD1306.CurrentY = 0;
  SSD1306.Color = White;

  // clearScreen screen
  clearScreen();

  // Flush buffer to screen
  update();

  return true;
}

//
//  fill the whole screen with the given color
//
void fill() {
  /* Set memory */
  uint16_t i;

  for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
    SSD1306_Buffer[i] = (SSD1306.Color == Black) ? 0x00 : 0xFF;
  }
}

//
//  Write the screenbuffer with changed to the screen
//
void update(void) {

  for (uint8_t i = 0; i < 8; i++) {
    writeCommand(0xB0 + i);
    writeCommand(SETLOWCOLUMN);
    writeCommand(SETHIGHCOLUMN);
    writeData(&SSD1306_Buffer[(SSD1306_WIDTH * (i))], width());
  }
}

//
//	Draw one pixel in the screenbuffer
//	X => X Coordinate
//	Y => Y Coordinate
//	color => Pixel color
//
void drawPixel(uint8_t x, uint8_t y) {
  color_t color = SSD1306.Color;

  if (x >= getWidth() || y >= getHeight()) {
    // Don't write outside the buffer
    return;
  }

  // Check if pixel should be inverted
  if (SSD1306.Inverted) {
    color = (color_t)!color;
  }

  // Draw in the right color
  if (color == White) {
    SSD1306_Buffer[x + (y / 8) * width()] |= 1 << (y % 8);
  } else {
    SSD1306_Buffer[x + (y / 8) * width()] &= ~(1 << (y % 8));
  }
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    SWAP_INT16_T(x0, y0);
    SWAP_INT16_T(x1, y1);
  }

  if (x0 > x1) {
    SWAP_INT16_T(x0, x1);
    SWAP_INT16_T(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      drawPixel(y0, x0);
    } else {
      drawPixel(x0, y0);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void drawHorizontalLine(int16_t x, int16_t y, int16_t length) {
  if (y < 0 || y >= height()) {
    return;
  }

  if (x < 0) {
    length += x;
    x = 0;
  }

  if ((x + length) > width()) {
    length = (width() - x);
  }

  if (length <= 0) {
    return;
  }

  uint8_t *bufferPtr = SSD1306_Buffer;
  bufferPtr += (y >> 3) * width();
  bufferPtr += x;

  uint8_t drawBit = 1 << (y & 7);

  switch (SSD1306.Color) {
  case White:
    while (length--) {
      *bufferPtr++ |= drawBit;
    };
    break;
  case Black:
    drawBit = ~drawBit;
    while (length--) {
      *bufferPtr++ &= drawBit;
    };
    break;
  case Inverse:
    while (length--) {
      *bufferPtr++ ^= drawBit;
    };
    break;
  }
}

void drawVerticalLine(int16_t x, int16_t y, int16_t length) {
  if (x < 0 || x >= width())
    return;

  if (y < 0) {
    length += y;
    y = 0;
  }

  if ((y + length) > height()) {
    length = (height() - y);
  }

  if (length <= 0)
    return;

  uint8_t yOffset = y & 7;
  uint8_t drawBit;
  uint8_t *bufferPtr = SSD1306_Buffer;

  bufferPtr += (y >> 3) * width();
  bufferPtr += x;

  if (yOffset) {
    yOffset = 8 - yOffset;
    drawBit = ~(0xFF >> (yOffset));

    if (length < yOffset) {
      drawBit &= (0xFF >> (yOffset - length));
    }

    switch (SSD1306.Color) {
    case White:
      *bufferPtr |= drawBit;
      break;
    case Black:
      *bufferPtr &= ~drawBit;
      break;
    case Inverse:
      *bufferPtr ^= drawBit;
      break;
    }

    if (length < yOffset)
      return;

    length -= yOffset;
    bufferPtr += width();
  }

  if (length >= 8) {
    switch (SSD1306.Color) {
    case White:
    case Black:
      drawBit = (SSD1306.Color == White) ? 0xFF : 0x00;
      do {
        *bufferPtr = drawBit;
        bufferPtr += width();
        length -= 8;
      } while (length >= 8);
      break;
    case Inverse:
      do {
        *bufferPtr = ~(*bufferPtr);
        bufferPtr += width();
        length -= 8;
      } while (length >= 8);
      break;
    }
  }

  if (length > 0) {
    drawBit = (1 << (length & 7)) - 1;
    switch (SSD1306.Color) {
    case White:
      *bufferPtr |= drawBit;
      break;
    case Black:
      *bufferPtr &= ~drawBit;
      break;
    case Inverse:
      *bufferPtr ^= drawBit;
      break;
    }
  }
}

void drawRect(int16_t x, int16_t y, int16_t width, int16_t height) {
  drawHorizontalLine(x, y, width);
  drawVerticalLine(x, y, height);
  drawVerticalLine(x + width - 1, y, height);
  drawHorizontalLine(x, y + height - 1, width);
}

void fillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height) {
  for (int16_t x = xMove; x < xMove + width; x++) {
    drawVerticalLine(x, yMove, height);
  }
}

void drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3) {
  /* Draw lines */
  drawLine(x1, y1, x2, y2);
  drawLine(x2, y2, x3, y3);
  drawLine(x3, y3, x1, y1);
}

void DrawfillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3) {
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, curpixel = 0;

  deltax = abs(x2 - x1);
  deltay = abs(y2 - y1);
  x = x1;
  y = y1;

  if (x2 >= x1) {
    xinc1 = 1;
    xinc2 = 1;
  } else {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1) {
    yinc1 = 1;
    yinc2 = 1;
  } else {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay) {
    xinc1 = 0;
    yinc2 = 0;
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;
  } else {
    xinc2 = 0;
    yinc1 = 0;
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++) {
    drawLine(x, y, x3, y3);

    num += numadd;
    if (num >= den) {
      num -= den;
      x += xinc1;
      y += yinc1;
    }
    x += xinc2;
    y += yinc2;
  }
}

void drawCircle(int16_t x0, int16_t y0, int16_t radius) {
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  do {
    if (dp < 0)
      dp = dp + 2 * (++x) + 3;
    else
      dp = dp + 2 * (++x) - 2 * (--y) + 5;

    drawPixel(x0 + x, y0 + y); // For the 8 octants
    drawPixel(x0 - x, y0 + y);
    drawPixel(x0 + x, y0 - y);
    drawPixel(x0 - x, y0 - y);
    drawPixel(x0 + y, y0 + x);
    drawPixel(x0 - y, y0 + x);
    drawPixel(x0 + y, y0 - x);
    drawPixel(x0 - y, y0 - x);

  } while (x < y);

  drawPixel(x0 + radius, y0);
  drawPixel(x0, y0 + radius);
  drawPixel(x0 - radius, y0);
  drawPixel(x0, y0 - radius);
}

void fillCircle(int16_t x0, int16_t y0, int16_t radius) {
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  do {
    if (dp < 0) {
      dp = dp + 2 * (++x) + 3;
    } else {
      dp = dp + 2 * (++x) - 2 * (--y) + 5;
    }

    drawHorizontalLine(x0 - x, y0 - y, 2 * x);
    drawHorizontalLine(x0 - x, y0 + y, 2 * x);
    drawHorizontalLine(x0 - y, y0 - x, 2 * y);
    drawHorizontalLine(x0 - y, y0 + x, 2 * y);

  } while (x < y);
  drawHorizontalLine(x0 - radius, y0, 2 * radius);
}

void drawCircleQuads(int16_t x0, int16_t y0, int16_t radius, uint8_t quads) {
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  while (x < y) {
    if (dp < 0)
      dp = dp + 2 * (++x) + 3;
    else
      dp = dp + 2 * (++x) - 2 * (--y) + 5;
    if (quads & 0x1) {
      drawPixel(x0 + x, y0 - y);
      drawPixel(x0 + y, y0 - x);
    }
    if (quads & 0x2) {
      drawPixel(x0 - y, y0 - x);
      drawPixel(x0 - x, y0 - y);
    }
    if (quads & 0x4) {
      drawPixel(x0 - y, y0 + x);
      drawPixel(x0 - x, y0 + y);
    }
    if (quads & 0x8) {
      drawPixel(x0 + x, y0 + y);
      drawPixel(x0 + y, y0 + x);
    }
  }
  if (quads & 0x1 && quads & 0x8) {
    drawPixel(x0 + radius, y0);
  }
  if (quads & 0x4 && quads & 0x8) {
    drawPixel(x0, y0 + radius);
  }
  if (quads & 0x2 && quads & 0x4) {
    drawPixel(x0 - radius, y0);
  }
  if (quads & 0x1 && quads & 0x2) {
    drawPixel(x0, y0 - radius);
  }
}

void drawProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress) {
  uint16_t radius = height / 2;
  uint16_t xRadius = x + radius;
  uint16_t yRadius = y + radius;
  uint16_t doubleRadius = 2 * radius;
  uint16_t innerRadius = radius - 2;

  setColor(White);
  drawCircleQuads(xRadius, yRadius, radius, 0b00000110);
  drawHorizontalLine(xRadius, y, width - doubleRadius + 1);
  drawHorizontalLine(xRadius, y + height, width - doubleRadius + 1);
  drawCircleQuads(x + width - radius, yRadius, radius, 0b00001001);

  uint16_t maxProgressWidth = (width - doubleRadius + 1) * progress / 100;

  fillCircle(xRadius, yRadius, innerRadius);
  fillRect(xRadius + 1, y + 2, maxProgressWidth, height - 3);
  fillCircle(xRadius + maxProgressWidth, yRadius, innerRadius);
}

// Draw monochrome bitmap
// input:
//   X, Y - top left corner coordinates of bitmap
//   W, H - width and height of bitmap in pixels
//   pBMP - pointer to array containing bitmap
// note: each '1' bit in the bitmap will be drawn as a pixel
//       each '0' bit in the will not be drawn (transparent bitmap)
// bitmap: one byte per 8 vertical pixels, LSB top, truncate bottom bits
void drawBitmap(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t *pBMP) {
  uint8_t pX;
  uint8_t pY;
  uint8_t tmpCh;
  uint8_t bL;

  pY = Y;
  while (pY < Y + H) {
    pX = X;
    while (pX < X + W) {
      bL = 0;
      tmpCh = *pBMP++;
      if (tmpCh) {
        while (bL < 8) {
          if (tmpCh & 0x01)
            drawPixel(pX, pY + bL);
          tmpCh >>= 1;
          if (tmpCh) {
            bL++;
          } else {
            pX++;
            break;
          }
        }
      } else {
        pX++;
      }
    }
    pY += 8;
  }
}

void setCursor(uint8_t x, uint8_t y) {
  SSD1306.CurrentX = x;
  SSD1306.CurrentY = y;
}

void set_pixel(int x, int y, int color) {

  if (color != 0)
    drawPixel(x, y);
  else {
    SSD1306.Color = SSD1306.Color == Black ? White : Black;
    drawPixel(x, y);
    SSD1306.Color = SSD1306.Color == Black ? White : Black;
  }
}

void drawBitmapMono(int x, int y, const image_t *image_t) {
  uint8_t value = 0;
  int16_t x0, y0;
  int16_t counter = 0;
  const uint8_t *pdata = (const uint8_t *)image_t->data;
  // rows
  for (y0 = 0; y0 < image_t->height; y0++) {
    // columns
    for (x0 = 0; x0 < image_t->width; x0++) {
      // load new data
      if (counter == 0) {
        value = *pdata++;
        counter = image_t->dataSize;
      }
      counter--;

      // set pixel
      if ((value & 0x80) != 0)
        set_pixel(x + x0, y + y0, 1);
      else
        set_pixel(x + x0, y + y0, 0);

      value = value << 1;
    }
  }
}

void draw_bitmap_mono_rle(int x, int y, const image_t *image_t) {
  uint8_t value = 0;
  int16_t x0, y0;
  int16_t counter = 0;
  int8_t sequence = 0;
  int8_t nonsequence = 0;
  const uint8_t *pdata = (const uint8_t *)image_t->data;
  // rows
  for (y0 = 0; y0 < image_t->height && (y0 + y) < 320; y0++) {
    // columns
    for (x0 = 0; x0 < image_t->width; x0++) {
      // load new data
      if (counter == 0) {
        if ((sequence == 0) && (nonsequence == 0)) {
          sequence = *pdata++;
          if (sequence < 0) {
            nonsequence = -sequence;
            sequence = 0;
          }
        }
        if (sequence > 0) {
          value = *pdata;

          sequence--;

          if (sequence == 0)
            pdata++;
        }
        if (nonsequence > 0) {
          value = *pdata++;

          nonsequence--;
        }
        counter = image_t->dataSize;
      }
      counter--;

      // set pixel
      if ((value & 0x80) != 0)
        set_pixel(x + x0, y + y0, 1);
      else
        set_pixel(x + x0, y + y0, 0);

      value = value << 1;
    }
  }
}

const character_t *find_char_by_code(int code, const font_t *font_t) {
  int16_t count = font_t->length;
  int16_t first = 0;
  int16_t last = count - 1;
  int16_t mid = 0;

  if (count > 0) {
    if ((code >= font_t->chars[0].code) && (code <= font_t->chars[count - 1].code)) {
      while (last >= first) {
        mid = first + ((last - first) / 2);

        if (font_t->chars[mid].code < code)
          first = mid + 1;
        else if (font_t->chars[mid].code > code)
          last = mid - 1;
        else
          break;
      }

      if (font_t->chars[mid].code == code)
        return (&font_t->chars[mid]);
    }
  }

  return (0);
}

int utf8_next_char(const char *str, int16_t start, int16_t *resultCode, int16_t *nextIndex) {
  uint16_t len = 0;
  uint16_t index = 0;
  *resultCode = 0;

  while (*(str + index) != 0) {
    len++;
    index++;
  }

  uint8_t c;
  uint8_t code = 0;
  int8_t result = 0;
  uint8_t skip = 0;

  *resultCode = 0;
  *nextIndex = -1;

  if (start >= 0 && start < len) {
    index = start;

    while (index < len) {
      c = *(str + index);
      index++;

      // msb
      if (skip == 0) {
        // if range 0x00010000-0x001fffff
        if ((c & 0xf8) == 0xf0) {
          skip = 3;
          code = c;
        }
        // if range 0x00000800-0x0000ffff
        else if ((c & 0xf0) == 0xe0) {
          skip = 2;
          code = c;
        }
        // if range 0x00000080-0x000007ff
        else if ((c & 0xe0) == 0xc0) {
          skip = 1;
          code = c;
        }
        // if range 0x00-0x7f
        else { // if ((c & 0x80) == 0x00)
          skip = 0;
          code = c;
        }
      } else { // not msb
        code = code << 8;
        code |= c;
        skip--;
      }
      if (skip == 0) {
        // completed
        *resultCode = code;
        *nextIndex = index;
        result = 1;
        break;
      }
    }
  }
  return (result);
}

void drawString(const char *str, int x, int y, const font_t *font_t) {
  int16_t len = strlen(str);
  int16_t index = 0;
  int16_t code = 0;
  int16_t x1 = x;
  int16_t nextIndex;

  while (index < len) {
    if (utf8_next_char(str, index, &code, &nextIndex) != 0) {
      const character_t *ch = find_char_by_code(code, font_t);
      if (ch != 0) {
        drawBitmapMono(x1, y, ch->image);
        x1 += ch->image->width;
      }
    }
    index = nextIndex;
    if (nextIndex < 0)
      break;
  }
}

void displayOn(void) { writeCommand(DISPLAYON); }

void displayOff(void) { writeCommand(DISPLAYOFF); }

void invertDisplay(void) { writeCommand(INVERTDISPLAY); }

void normalDisplay(void) { writeCommand(NORMALDISPLAY); }

void resetOrientation() {
  writeCommand(SEGREMAP);
  writeCommand(COMSCANINC); // Reset screen rotation or mirroring
}

void flipScreenVertically() {
  writeCommand(SEGREMAP | 0x01);
  writeCommand(COMSCANDEC); // Rotate screen 180 Deg
}

void mirrorScreen() {
  writeCommand(SEGREMAP);
  writeCommand(COMSCANDEC); // Mirror screen
}

void clearScreen() { memset(SSD1306_Buffer, 0, SSD1306_BUFFER_SIZE); }

//
//  Send a byte to the command register
//
// static void writeCommand(uint8_t command) {
// TODO

// uint8_t buf[2] = {0x00, command};
// nrf_drv_twi_tx(&SSD1306.oled_twi_master, SSD1306_I2C_ADDR, buf, 2, false);
// }

// static void writeData(uint8_t *data, uint16_t size) {
// TODO

// uint8_t buf[size + 1];
// buf[0] = 0x40;
// memcpy(buf + 1, data, size);
// nrf_drv_twi_tx(&SSD1306.oled_twi_master, SSD1306_I2C_ADDR, buf, size + 1, false);
// }

// void Start_Scroll_Right(uint8_t start, uint8_t stop) {
//   writeCommand(SSD1306_RIGHT_HORIZONTAL_SCROLL);
//   writeCommand(0X00);
//   writeCommand(start);
//   writeCommand(0X00);
//   writeCommand(stop);
//   writeCommand(0X01);
//   writeCommand(0XFF);
//   writeCommand(SSD1306_ACTIVATE_SCROLL);
// }

// void Sop_Scroll(void) { writeCommand(SSD1306_DEACTIVATE_SCROLL); }

// // Diagonal (vertical and horizontal) scroll setup
// // input:
// //   dir - horizontal scroll direction (one of LCD_SCROLL_XXX values)
// //   start - start page address [0..7]
// //   end - end page address [0..7], must be great or equal to start value
// //   interval - time interval between scroll steps (one of LCD_SCROLL_IFXXX values)
// //   voffs - vertical scrolling offset, this value specifies how many lines will
// //           be scrolled vertically per one scroll step [1..63]
// void SSD1306_ScrollDSetup(uint8_t dir, uint8_t start, uint8_t end, uint8_t interval, uint8_t voffs) {
//   writeCommand((dir == LCD_SCROLL_RIGHT) ? SSD1306_CMD_SCRL_VHR : SSD1306_CMD_SCRL_VHL);
//   writeCommand(0x00);     // dummy byte
//   writeCommand(start);    // Start page address
//   writeCommand(interval); // Time interval between each scroll stop in terms of frame frequency
//   writeCommand(end);      // End page address
//   writeCommand(voffs);    // Vertical scrolling offset
// }

// // Horizontal scroll setup
// // input:
// //   dir - scroll direction (one of LCD_SCROLL_XXX values)
// //   start - start page address [0..7]
// //   end - end page address [0..7], must be great or equal to start value
// //   interval - time interval between scroll steps (one of LCD_SCROLL_IFXXX values)
// void SSD1306_ScrollHSetup(uint8_t dir, uint8_t start, uint8_t end, uint8_t interval) {

//   writeCommand((dir == LCD_SCROLL_RIGHT) ? SSD1306_CMD_SCRL_HR : SSD1306_CMD_SCRL_HL);
//   writeCommand(0x00);     // dummy byte
//   writeCommand(start);    // Start page address
//   writeCommand(interval); // Time interval between each scroll stop in terms of frame frequency
//   writeCommand(end);      // End page address
//   writeCommand(0x00);     // dummy byte
//   writeCommand(0xFF);     // dummy byte
// }

// // Activate scrolling
// // note: this function must be called only after scroll setup
// // note: changing of video RAM contents and scroll parameters are prohibited
// //       after the scrolling is activated
// void SSD1306_ScrollStart(void) { writeCommand(SSD1306_CMD_SCRL_ACT); }

// // Deactivate scrolling
// // note: after calling this function the graphics RAM data needs to be rewritten
// void SSD1306_ScrollStop(void) { writeCommand(SSD1306_CMD_SCRL_STOP); }

} // namespace ssd1306