#ifndef SSD1306_DISPLAY_REGISTERS_H_
#define SSD1306_DISPLAY_REGISTERS_H_

#define SSD1306_I2C_ADDR 0x3C // 0x3C default

// Display commands
#define CHARGEPUMP 0x8D
#define COLUMNADDR 0x21
#define COMSCANDEC 0xC8
#define COMSCANINC 0xC0
#define DISPLAYALLON 0xA5
#define DISPLAYALLON_RESUME 0xA4
#define DISPLAYOFF 0xAE
#define DISPLAYON 0xAF
#define EXTERNALVCC 0x1
#define INVERTDISPLAY 0xA7
#define MEMORYMODE 0x20
#define NORMALDISPLAY 0xA6
#define PAGEADDR 0x22
#define SEGREMAP 0xA0
#define SETCOMPINS 0xDA
#define SETCONTRAST 0x81
#define SETDISPLAYCLOCKDIV 0xD5
#define SETDISPLAYOFFSET 0xD3
#define SETHIGHCOLUMN 0x10
#define SETLOWCOLUMN 0x00
#define SETMULTIPLEX 0xA8
#define SETPRECHARGE 0xD9
#define SETSEGMENTREMAP 0xA1
#define SETSTARTLINE 0x40
#define SETVCOMDETECT 0xDB
#define SWITCHCAPVCC 0x2

#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

#define SSD1306_CMD_SCRL_HR 0x26   // Setup continuous horizontal scroll right
#define SSD1306_CMD_SCRL_HL 0x27   // Setup continuous horizontal scroll left
#define SSD1306_CMD_SCRL_VHR 0x29  // Setup continuous vertical and horizontal scroll right
#define SSD1306_CMD_SCRL_VHL 0x2A  // Setup continuous vertical and horizontal scroll left
#define SSD1306_CMD_SCRL_STOP 0x2E // Deactivate scroll
#define SSD1306_CMD_SCRL_ACT 0x2F  // Activate scroll

namespace ssd1306 {

typedef enum {
  GEOMETRY_128_64 = 0,
  GEOMETRY_128_32 = 1,
} geometry_t;

// Screen scroll interval enumeration
enum {
  LCD_SCROLL_IF2 = 0x07,   // 2 frames
  LCD_SCROLL_IF3 = 0x04,   // 3 frames
  LCD_SCROLL_IF4 = 0x05,   // 4 frames
  LCD_SCROLL_IF5 = 0x00,   // 5 frames
  LCD_SCROLL_IF25 = 0x06,  // 25 frames
  LCD_SCROLL_IF64 = 0x01,  // 64 frames
  LCD_SCROLL_IF128 = 0x02, // 128 frames
  LCD_SCROLL_IF256 = 0x03  // 256 frames

};

enum {
  LCD_SCROLL_RIGHT = 0, // Scroll right
  LCD_SCROLL_LEFT = 1   // Scroll left

};

} // namespace ssd1306

#endif // SSD1306_DISPLAY_REGISTERS_H_