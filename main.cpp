#include <cmath>

#include "Model.hpp"
#include "geometry.hpp"
#include "tgaimage.hpp"

Model* model = NULL;
const int width = 1024;
const int height = 1024;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

Vec3f barycentric(Vec3f* pts, Vec3f P) {
  Vec3f u =
      cross(Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x),
            Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));
  if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
  return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

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

void triangle(Vec3f* pts, float* z_buffer, TGAImage& image, TGAColor color) {
  Vec2f bboxmin(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());
  Vec2f bboxmax(-std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max());
  Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  Vec3f P;
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(pts, P);
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
      P.z = 0;
      for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
      if (z_buffer[int(P.x + P.y * width)] < P.z) {
        z_buffer[int(P.x + P.y * width)] = P.z;
        image.set(P.x, P.y, color);
      }
    }
  }
}

Vec3f world2screen(Vec3f v) {
  return Vec3f(int((v.x + 1.) * width / 2. + .5),
               int((v.y + 1.) * height / 2. + .5), v.z);
}

int main(int argc, char** argv) {
  if (2 == argc) {
    model = new Model(argv[1]);
  } else {
    model = new Model("obj/african_head.obj");
  }

  float* z_buffer = new float[width * height];
  for (int i = width * height; i == 0; i--) {
    z_buffer[i] = -std::numeric_limits<float>::max();
  }

  TGAImage image(width, height, TGAImage::RGB);
  Vec3f light_dir(0, 0, -1);  // define light_dir

  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i);
    Vec3f world_coords[3];
    Vec3f screen_coords[3];
    for (int j = 0; j < 3; j++) {
      Vec3f v = model->vert(face[j]);
      world_coords[j] = v;
      for (int i = 0; i < 3; i++) {
        screen_coords[i] = world2screen(model->vert(face[i]));
      }
    }
    Vec3f n = cross((world_coords[2] - world_coords[0]),
                    (world_coords[1] - world_coords[0]));
    n.normalize();

    float intensity = n * light_dir;
    if (intensity > 0) {
      triangle(
          screen_coords, z_buffer, image,
          TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
    }
  }
  image.flip_vertically();  // i want to have the origin at the left bottom
                            // corner of the image
  image.write_tga_file("output.tga");
  delete model;
  return 0;
}