#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
Model *model = NULL;
const int width  = 200;
const int height = 200;

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color);
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
	bool steep = false;
	if (std::abs(p0.x-p1.x) < std::abs(p0.y-p1.y)) {
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}
	if (p0.x > p1.x) {
		std::swap(p0.x, p1.x);
		std::swap(p0.y, p1.y);
	}
	int dx = p1.x-p0.x;
	int dy = p1.y-p0.y;
	float derror = std::abs(dy)*2;
	float error = 0;
	int y = p0.y;
	for (int x=p0.x; x<=p1.x; x++) {
		if (steep) {
			image.set(y, x, color);
		} else {
			image.set(x, y, color);
		}
		error += derror;
		if (error>.5) {
			y += (p1.y>p0.y?1:-1);
			error -= dx*2.;
		}
	}
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) { 
    line(t0, t1, image, color); 
    line(t1, t2, image, color); 
    line(t2, t0, image, color);
	if (t0.y<t1.y) std::swap(t0, t1); 
    if (t0.y<t2.y) std::swap(t0, t2); 
    if (t1.y<t2.y) std::swap(t1, t2);

	Vec2i t3 = Vec2i(t0.x - (((float)(t2.x - t0.x)/(t2.y - t0.y))*(t0.y - t1.y)), t1.y);

	// fill bottom half

	float slope_2_3 = (float)(t3.x - t2.x) / (t3.y - t2.y);
	float slope_2_1 = (float)(t1.x - t2.x) / (t1.y - t2.y);

	float current_x1 = t2.x;
	float current_x2 = t2.x;
	for (int line_y = t2.y; line_y <= t1.y; line_y++) {
		line(Vec2i(current_x1, line_y), Vec2i(current_x2, line_y), image, color);
		current_x1 += slope_2_3;
		current_x2 += slope_2_1;
	}

	// fill top half

	float slope_1_0 = (float)(t0.x - t1.x) / (t0.y - t1.y);
	float slope_3_0 = (float)(t0.x - t3.x) / (t0.y - t3.y);

	current_x1 = t1.x;
	current_x2 = t3.x;
	for (int line_y = t3.y; line_y <= t0.y; line_y++) {
		line(Vec2i(current_x1, line_y), Vec2i(current_x2, line_y), image, color);
		current_x1 += slope_1_0;
		current_x2 += slope_3_0;
	}


}