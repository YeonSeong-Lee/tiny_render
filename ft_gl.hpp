#ifndef __FT_GL_HPP__
#define __FT_GL_HPP__

#include "geometry.h"
#include "tgaimage.hpp"

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

void viewport(int x, int y, int w, int h);
void projection(float coeff = 0.f);  // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

struct IShader {
  virtual ~IShader();
  virtual Vec3i vertex(int iface, int nthvert) = 0;
  virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

void triangle(Vec3i *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer);
template <typename T>
T CLAMP(const T &value, const T &low, const T &high) {
  return value < low ? low : (value > high ? high : value);
}

#endif  // __FT_GL_HPP__