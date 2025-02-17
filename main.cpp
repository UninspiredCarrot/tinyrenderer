#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color);
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
bool in_triangle(Vec2i P, Vec2i A, Vec2i B, Vec2i C);

int main(int argc, char** argv) {
	if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

	TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir(0,0,-1); // define light_dir

	for (int i=0; i<model->nfaces(); i++) { 
		std::vector<int> face = model->face(i); 
		Vec2i screen_coords[3]; 
		Vec3f world_coords[3]; 
		for (int j=0; j<3; j++) { 
			Vec3f v = model->vert(face[j]); 
			screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.); 
			world_coords[j]  = v; 
		} 
		Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
		n.normalize(); 
		float intensity = n*light_dir; 
		if (intensity>0) { 
			triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255)); 
		} 
	}


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

	// find bounding box
	int bbox_left = std::min(t0.x, std::min(t1.x, t2.x));
	int bbox_right = std::max(t0.x, std::max(t1.x, t2.x));
	int bbox_bottom = std::min(t0.y, std::min(t1.y, t2.y));
	int bbox_top = std::max(t0.y, std::max(t1.y, t2.y));

	Vec2i P;

	for (P.x = bbox_left; P.x <= bbox_right; P.x++) {
		for (P.y = bbox_bottom; P.y <= bbox_top; P.y++) {
			if (in_triangle(P, t0, t1, t2)) {
				image.set(P.x, P.y, color);
			}
		}
	}

}

bool in_triangle(Vec2i P, Vec2i A, Vec2i B, Vec2i C) {
	// P = A + w1(B - A) + w2(C - A)

	// Px = Ax + w1(Bx - Ax) + w2(Cx - Ax)
	// Py = Ay + w1(By - Ay) + w2(Cy - Ay)

	// w2 = Py - Ay - w1(By - Ay) / (Cy - Ay) *

	// Px = Ax + w1(Bx - Ax) + (Py - Ay - w1(By - Ay) / (Cy - Ay))(Cx - Ax)
	// Px(Cy - Ay) = Ax(Cy - Ay) + w1(Bx - Ax)(Cy - Ay) + (Py - Ay)(Cx - Ax) -  w1(By - Ay)(Cx - Ax)
	// w1(By - Ay)(Cx - Ax) - w1(Bx - Ax)(Cy - Ay) = Ax(Cy - Ay)  + (Py - Ay)(Cx - Ax)  - Px(Cy - Ay)
	// w1[(By - Ay)(Cx - Ax) - (Bx - Ax)(Cy - Ay)] = Ax(Cy - Ay)  + (Py - Ay)(Cx - Ax)  - Px(Cy - Ay)
	// w1 = (Ax(Cy - Ay)  + (Py - Ay)(Cx - Ax)  - Px(Cy - Ay))/[(By - Ay)(Cx - Ax) - (Bx - Ax)(Cy - Ay)] *

	// Point P is inside the triangle ABC if:
	//	w1 >= 0 and w1 >= 0 and (w1 + w2) <= 1

	// Convert integers to floats to avoid integer division issues.
    float denominator = (float)((B.y - A.y) * (C.x - A.x) - (B.x - A.x) * (C.y - A.y));

    // Check for degenerate (zero-area) triangle
    if (denominator == 0) return false;

    float w1 = (A.x * (C.y - A.y) + (P.y - A.y) * (C.x - A.x) - P.x * (C.y - A.y)) / denominator;
    float w2 = (P.y - A.y - w1 * (B.y - A.y)) / (float)(C.y - A.y);

    // Check if P is inside the triangle
    return (w1 >= 0) && (w2 >= 0) && (w1 + w2 <= 1);

}
