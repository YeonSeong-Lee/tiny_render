#include <cmath>

#include "Model.hpp"
#include "geometry.h"
#include "tgaimage.hpp"

Model* model = NULL;
const int width = 1024;
const int height = 1024;
const int depth = 255;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

void draw_line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
  bool steep = false;
  if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
    std::swap(p0.x, p0.y);
    std::swap(p1.x, p1.y);
    steep = true;
  }
  if (p0.x > p1.x) {
    std::swap(p0.x, p1.x);
    std::swap(p0.y, p1.y);
  }
  int dx = p1.x - p0.x;
  int dy = p1.y - p0.y;
  int derror2 = std::abs(dy) * 2;
  int error2 = 0;
  int y = p0.y;
  int tempDeltaY = p1.y > p0.y ? 1 : -1;
  for (int x = p0.x; x <= p1.x; x++) {
    if (steep) {
      image.set(y, x, color);
    } else {
      image.set(x, y, color);
    }
    error2 += derror2;
    if (error2 > dx) {
      y += tempDeltaY;
      error2 -= dx * 2;
    }
  }
}

Vec3f m2v(Matrix m) {
  return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

Matrix v2m(Vec3f v) {
  Matrix m(4, 1);
  m[0][0] = v.x;
  m[1][0] = v.y;
  m[2][0] = v.z;
  m[3][0] = 1.f;
  return m;
}

Matrix viewport(int x, int y, int w, int h) {
  Matrix m = Matrix::identity(4);
  m[0][3] = x + w / 2.f;
  m[1][3] = y + h / 2.f;
  m[2][3] = depth / 2.f;

  m[0][0] = w / 2.f;
  m[1][1] = h / 2.f;
  m[2][2] = depth / 2.f;
  return m;
}

void triangle(Vec3i* pts, int* z_buffer, TGAImage& image, float* intensity,
              Vec2i* uv) {
  Vec3i t0 = pts[0];
  Vec3i t1 = pts[1];
  Vec3i t2 = pts[2];
  Vec2i uv0 = uv[0];
  Vec2i uv1 = uv[1];
  Vec2i uv2 = uv[2];
  float ity0 = intensity[0];
  float ity1 = intensity[1];
  float ity2 = intensity[2];

  if (t0.y == t1.y && t0.y == t2.y)
    return;  // i dont care about degenerate triangles
  if (t0.y > t1.y) {
    std::swap(t0, t1);
    std::swap(uv0, uv1);
    std::swap(ity0, ity1);
  }
  if (t0.y > t1.y) {
    std::swap(t0, t1);
    std::swap(uv0, uv1);
    std::swap(ity0, ity1);
  }
  if (t0.y > t2.y) {
    std::swap(t0, t2);
    std::swap(uv0, uv2);
    std::swap(ity0, ity2);
  }
  if (t0.y > t2.y) {
    std::swap(t0, t2);
    std::swap(uv0, uv2);
    std::swap(ity0, ity2);
  }
  if (t1.y > t2.y) {
    std::swap(t1, t2);
    std::swap(uv1, uv2);
    std::swap(ity1, ity2);
  }
  if (t1.y > t2.y) {
    std::swap(t1, t2);
    std::swap(uv1, uv2);
    std::swap(ity1, ity2);
  }

  int total_height = t2.y - t0.y;
  for (int i = 0; i < total_height; i++) {
    for (int i = 0; i < total_height; i++) {
      bool second_half = i > t1.y - t0.y || t1.y == t0.y;
      int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
      float alpha = (float)i / total_height;
      float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) /
                   segment_height;  // be careful: with above conditions no
                                    // division by zero here	        float
                                    // beta = (float)(i-(second_half ? t1.y-t0.y
      Vec3i A = t0 + Vec3f(t2 - t0) * alpha;
      Vec3i B =
          second_half ? t1 + Vec3f(t2 - t1) * beta : t0 + Vec3f(t1 - t0) * beta;
      float ityA = ity0 + (ity2 - ity0) * alpha;
      float ityB = second_half ? ity1 + (ity2 - ity1) * beta
                               : ity0 + (ity1 - ity0) * beta;
      Vec2i uvA = uv0 + (uv2 - uv0) * alpha;
      Vec2i uvB =
          second_half ? uv1 + (uv2 - uv1) * beta : uv0 + (uv1 - uv0) * beta;
      if (A.x > B.x) {
        std::swap(A, B);
        std::swap(uvA, uvB);
        std::swap(ityA, ityB);
      }
      for (int j = A.x; j <= B.x; j++) {
        float phi = B.x == A.x ? 1. : (float)(j - A.x) / (float)(B.x - A.x);
        Vec3i P = Vec3f(A) + Vec3f(B - A) * phi;
        Vec2i uvP = uvA + (uvB - uvA) * phi;
        float ityP = ityA + (ityB - ityA) * phi;
        int idx = P.x + P.y * width;
        if (z_buffer[idx] < P.z) {
          z_buffer[idx] = P.z;
          TGAColor color = model->diffuse(uvP);
          image.set(P.x, P.y, TGAColor(color.r, color.g, color.b) * ityP);
        }
      }
    }
  }
}

Vec3f world2screen(Vec3f v) {
  return Vec3f(int((v.x + 1.) * width / 2. + .5),
               int((v.y + 1.) * height / 2. + .5), v.z);
}

Matrix viewport(int x, int y, int w, int h, int depth) {
  Matrix m = Matrix::identity(4);
  m[0][3] = x + w / 2.f;
  m[1][3] = y + h / 2.f;
  m[2][3] = depth / 2.f;

  m[0][0] = w / 2.f;
  m[1][1] = h / 2.f;
  m[2][2] = depth / 2.f;
  return m;
}

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
  Vec3f z = (eye - center).normalize();
  Vec3f x = (up ^ z).normalize();
  Vec3f y = (z ^ x).normalize();
  Matrix res = Matrix::identity(4);
  for (int i = 0; i < 3; i++) {
    res[0][i] = x[i];
    res[1][i] = y[i];
    res[2][i] = z[i];
    res[i][3] = -center[i];
  }
  return res;
}

int main(int argc, char** argv) {
  if (argc == 2) {
    model = new Model(argv[1]);
  } else {
    model = new Model("obj/african_head.obj");
  }

  int* z_buffer = new int[width * height];
  for (int i = 0; i < width * height; i++) {
    z_buffer[i] = std::numeric_limits<int>::min();
  }

  TGAImage image(width, height, TGAImage::RGB);
  Vec3f light_dir = Vec3f(1, 1, 1).normalize();  // define light_dir
  Vec3f eye(1, 1, 3);
  Vec3f center(0, 0, 0);

  Matrix ModelView = lookat(eye, center, Vec3f(0, 1, 0));
  Matrix Projection = Matrix::identity(4);
  Matrix ViewPort =
      viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4, depth);
  Projection[3][2] = -1.f / (eye - center).norm();

  std::cerr << ModelView << std::endl;
  std::cerr << Projection << std::endl;
  std::cerr << ViewPort << std::endl;
  Matrix z = (ViewPort * Projection * ModelView);
  std::cerr << z << std::endl;

  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i);
    Vec3f world_coords[3];
    Vec3i screen_coords[3];
    float intensity[3];

    for (int j = 0; j < 3; j++) {
      Vec3f v = model->vert(face[j]);
      world_coords[j] = v;
      screen_coords[j] = m2v(ViewPort * Projection * ModelView * v2m(v));
      intensity[j] = model->norm(i, j) * light_dir;
    }
    Vec2i uv[3];
    for (int k = 0; k < 3; k++) {
      uv[k] = model->uv(i, k);
    }
    triangle(screen_coords, z_buffer, image, intensity, uv);
  }
  image.flip_vertically();  // i want to have the origin at the left bottom
                            // corner of the image
  image.write_tga_file("output.tga");

  {  // dump z-buffer (debugging purposes only)
    TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        zbimage.set(i, j, TGAColor(z_buffer[i + j * width]));
      }
    }
    zbimage.flip_vertically();  // i want to have the origin at the left bottom
                                // corner of the image
    zbimage.write_tga_file("zbuffer.tga");
  }

  delete model;
  delete[] z_buffer;
  return 0;
}