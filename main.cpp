#include <iostream>
#include <limits>
#include <vector>

#include "Model.hpp"
#include "ft_gl.hpp"
#include "geometry.h"
#include "tgaimage.hpp"

Model *model = NULL;
float *shadowbuffer = NULL;

const int width = 800;
const int height = 800;

Vec3f eye(1.2, -.8, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

float max_elevation_angle(float *zbuffer, Vec2f p, Vec2f dir) {
  float maxangle = 0;
  for (float t = 0.0f; t < 1000.f; t += 1.0f) {
    Vec2f cur = p + dir * t;
    if (cur.x >= width || cur.y >= height || cur.x < 0 || cur.y < 0) {
      return maxangle;
    }
    float distance = (p - cur).norm();
    if (distance < 1.0f) {
      continue;
    }
    float elevation =
        zbuffer[int(cur.x) + int(cur.y) * width] - zbuffer[int(p.x) + int(p.y) * width];
    maxangle = std::max(maxangle, atanf(elevation / distance));
  }
  return maxangle;
}

struct ZShader : public IShader {
  mat<4, 3, float> varying_tri;

  virtual Vec4f vertex(int iface, int nthvert) {
    Vec4f gl_Vertex =
        Projection * ModelView * embed<4>(model->vert(iface, nthvert));
    varying_tri.set_col(nthvert, gl_Vertex);
    return gl_Vertex;
  }

  virtual bool fragment(Vec3f gl_FragCoord, Vec3f bar, TGAColor &color) {
    color = TGAColor(0, 0, 0);
    return false;
  }
};

int main(int argc, char **argv) {
  if (argc == 2) {
    model = new Model(argv[1]);
  } else {
    model = new Model("obj/diablo3_pose/diablo3_pose.obj");
  }

  float *zbuffer = new float[width * height];
  for (int i = width * height; i--;
       zbuffer[i] = -std::numeric_limits<float>::max())
    ;
  TGAImage image(width, height, TGAImage::RGB);
  lookat(eye, center, up);
  viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
  projection(-1.f / (eye - center).norm());

  ZShader zshader;
  for (int i = 0; i < model->nfaces(); i++) {
    for (int j = 0; j < 3; j++) {
      zshader.vertex(i, j);
    }
    triangle(zshader.varying_tri, zshader, image, zbuffer);
  }

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (zbuffer[x + y * width] < -1e5) {
        continue;
      }
      float total = 0;
      for (float a = 0; a < M_PI * 2 - 1e-4; a += M_PI / 4) {
        Vec2f dir(cos(a), sin(a));
        total += M_PI / 2 - max_elevation_angle(zbuffer, Vec2f(x, y), dir);
      }
      total /= (M_PI / 2) * 8;
      total = pow(total, 100.f);
      image.set(x, y, TGAColor(total * 255, total * 255, total * 255));
    }
  }

  image.flip_vertically();
  image.write_tga_file("output.tga");

  delete model;
  delete[] zbuffer;
  return 0;
}