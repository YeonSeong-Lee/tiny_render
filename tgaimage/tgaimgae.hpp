#ifndef TGAIMAGE_HPP
#define TGAIMGAE_HPP

#include <fstream>
typedef struct TGAHeader {
  unsigned char id_length;
  unsigned char color_map_type;
  unsigned char data_type_code;
  short color_map_origin;
  short color_map_length;
  unsigned char color_map_depth;
  short x_origin;
  short y_origin;
  short width;
  short height;
  unsigned char bits_per_pixel;
  unsigned char image_descriptor;
} TGAHeader;

typedef struct TGAColor {
  union {
    struct {
      unsigned char b;
      unsigned char g;
      unsigned char r;
      unsigned char a;
    };
    unsigned char raw[4];
    unsigned int val;
  };
  unsigned int bytespp;

  TGAColor() : val(0), bytespp(1) {}

  TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
      : b(B), g(G), r(R), a(A), bytespp(4) {}

  TGAColor(unsigned int v, unsigned int bpp) : val(v), bytespp(bpp) {}

  TGAColor(const TGAColor &c) : val(c.val), bytespp(c.bytespp) {}

  TGAColor(const unsigned char *p, int bpp) : val(0), bytespp(bpp) {
    for (int i = 0; i < bpp; i++) {
      raw[i] = p[i];
    }
  }

  TGAColor &operator=(const TGAColor &c) {
    if (this != &c) {
      bytespp = c.bytespp;
      val = c.val;
    }
    return *this;
  }
} TGAColor;

#endif