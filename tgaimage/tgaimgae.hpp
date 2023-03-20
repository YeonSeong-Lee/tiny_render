#ifndef TGAIMAGE_HPP
#define TGAIMGAE_HPP

#include <fstream>
typedef struct TGA_Header {
  unsigned char id_length;
  unsigned char color_map_type;
  unsigned char data_type_code;
  unsigned char bits_per_pixel;
  unsigned char image_descriptor;
  unsigned char color_map_depth;
  short color_map_origin;
  short color_map_length;
  short x_origin;
  short y_origin;
  short width;
  short height;
} TGA_Header;

#endif