#ifndef TGAIMAGE_HPP
#define TGAIMAGE_HPP

#include <fstream>
#include <iostream>

#pragma pack(push, 1)
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
#pragma pack(pop)

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

  TGAColor(unsigned char R, unsigned char G, unsigned char B,
           unsigned char A = 255)
      : b(B), g(G), r(R), a(A), bytespp(4) {}

  TGAColor(unsigned int v, unsigned int bpp = 1) : val(v), bytespp(bpp) {}

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
  
  unsigned char &operator[](const int i) { return raw[i]; }

  TGAColor operator*(float intensity) const {
    TGAColor res = *this;
    intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
    for (int i = 0; i < 4; i++) {
      res.raw[i] = raw[i] * intensity;
    }
    return res;
  
  }
} TGAColor;

class TGAImage {
 public:
  enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };
  TGAImage();
  TGAImage(int w, int h, int bpp);
  TGAImage(const TGAImage &img);
  ~TGAImage();
  TGAImage &operator=(const TGAImage &img);

  bool read_tga_file(const char *filename);
  bool write_tga_file(const char *filename, bool rle = true);
  bool flip_horizontally();
  bool flip_vertically();
  bool scale(int w, int h);

  TGAColor get(int x, int y);
  bool set(int x, int y, TGAColor c);
  int get_width();
  int get_height();
  int get_bytespp();
  unsigned char *buffer();
  void clear();

 protected:
  unsigned char *data;
  int width;
  int height;
  int bytespp;

  bool load_rle_data(std::ifstream &in);
  bool unload_rle_data(std::ofstream &out);
};

#endif