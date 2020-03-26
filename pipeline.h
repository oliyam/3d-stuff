#pragma once

#include <SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>  
#include <fstream>  
#include <vector>
#include <bitset>

#include <ctime>
#include <chrono>

#include "CLSetup.h"
#include "vectors.h"
#include "object.h"
#include "scene.h"

using std::string;
using namespace std;

typedef chrono::high_resolution_clock Clock;


class Pipeline {

private:
	/*
	draws a line between two vec2 objects p1 and p2 onto a given pixel array pixels and returs a pointer to that very array
	size_x is the width of the frame (pixel array)
	size_y is the height of the frame (pixel array)
	*/
	void drawLine(Uint32 color_int, double x1, double y1, double x2, double y2) {
		int y, x;
		float k = (y1 - y2) / (x1 - x2);
		if (abs(x1 - x2) > abs(y1 - y2))
		{
			if (x1 < x2) {
				swap(x1, x2);
				swap(y1, y2);
			}
			if (x2 < 0)
				x2 = 0;
			for (int x = x2; x < size_x && x < x1; x++)
			{
				y = k * (x - x1) + y1;
				if (y >= 0 && y < size_y)
					pixels[y * size_x + x] = color_int;
			}
		}
		else
		{
			if (y1 < y2) {
				swap(x1, x2);
				swap(y1, y2);
			}
			if (y2 < 0)
				y2 = 0;
			for (int y = y2; y < size_y && y < y1; y++)
			{
				x = (y - y1) / k + x1;
				if (x >= 0 && x < size_x)
					pixels[y * size_x + x] = color_int;
			}
		}
	}
	void drawLine(bool* pixels, Uint32 color_int, double x1, double y1, double x2, double y2) {
		int y, x;
		float k = (y1 - y2) / (x1 - x2);
		if (abs(x1 - x2) > abs(y1 - y2))
		{
			if (x1 < x2) {
				swap(x1, x2);
				swap(y1, y2);
			}
			if (x2 < 0)
				x2 = 0;
			for (int x = x2; x < size_x && x < x1; x++)
			{
				y = k * (x - x1) + y1;
				if (y >= 0 && y < size_y)
					pixels[y * size_x + x] = true;
			}
		}
		else
		{
			if (y1 < y2) {
				swap(x1, x2);
				swap(y1, y2);
			}
			if (y2 < 0)
				y2 = 0;
			for (int y = y2; y < size_y && y < y1; y++)
			{
				x = (y - y1) / k + x1;
				if (x >= 0 && x < size_x)
					pixels[y * size_x + x] = true;
			}
		}
	}
	/*
	draws a face ... its simple ...
	*/
	void drawFace(Uint32 color, double points[])
	{
		drawLine(color, points[0], points[1], points[3], points[4]);
		drawLine(color, points[3], points[4], points[6], points[7]);
		drawLine(color, points[6], points[7], points[0], points[1]);
	}
	/*
	fills a face ... its simple ...
	*/
	void fillFace(Uint32 color, double points[], int l)
	{
		int
		min_x = min(min(points[0], points[3]), points[6]),
		max_x = max(max(points[0], points[3]), points[6]),
		min_y = min(min(points[1], points[4]), points[7]),
		max_y = max(max(points[1], points[4]), points[7])
		;
		double
			d0 = (points[4] - points[1]),
			d1 = (points[7] - points[4]),
			d2 = (points[1] - points[7]),
			d3 = (points[3] - points[0]),
			d4 = (points[6] - points[3]),
			d5 = (points[0] - points[6])
		;
		if (min_x < 0)
			min_x = 0;
		if (min_y < 0)
			min_y = 0;
		for (int y = min_y; y < max_y && y < size_y; y++) {
			for (int x = min_x; x < max_x && x < size_x; x++) {
				double
					e1 = (x - points[0]) * d0 - (y - points[1]) * d3,
					e2 = (x - points[3]) * d1 - (y - points[4]) * d4,
					e3 = (x - points[6]) * d2 - (y - points[7]) * d5
				;
				if(e1 > 0 && e2 > 0 && e3 > 0) 
					pixels[y * size_x + x] = color;
			}
		}
	}
	/*
	fills a face ... its simple ...
	*/
	void fillFace(Uint32 color, double points[])
	{
		int
			min_x = min(min(points[0], points[3]), points[6]),
			max_x = max(max(points[0], points[3]), points[6]),
			min_y = min(min(points[1], points[4]), points[7]),
			max_y = max(max(points[1], points[4]), points[7])
		;
		double
			d1 = points[4] - points[7],
			d2 = points[6] - points[3],
			d3 = points[7] - points[1],
			d4 = points[0] - points[6],
			d0 = d1 * d4 + d2 * (points[1] - points[7])
		;
		if (min_x < 0)
			min_x = 0;
		if (min_y < 0)
			min_y = 0;
		for (int y = min_y; y < max_y && y < size_y; y++) {
			double y1 = y - points[7];
			int i = y * size_x;
			for (int x = min_x; x < max_x && x < size_x; x++) {
				double
					x1 = x - points[6],
					w_v0 = (d1 * x1 + d2 * y1) / d0,
					w_v1 = (d3 * x1 + d4 * y1) / d0,
					w_v2 = 1 - w_v0 - w_v1
				;
				if (w_v0 > 0 && w_v1 > 0 && w_v2 > 0) {
					int o = i + x;
					double w = ((1 / points[2]) * w_v0 + (1 / points[5]) * w_v1 + (1 / points[8]) * w_v2);
					if (zBuffer[o] < w)
					{
						zBuffer[o] = w;
						pixels[o] = ((int)(red * w_v0) & 0x00FF0000) | ((int)(green * w_v1) & 0x0000FF00) | ((int)(blue * w_v2) & 0x000000FF);
					}
				}
			}
		}
	}
	/*
	draws a line between two vec2 objects p1 and p2 onto a given pixel array pixels and returs a pointer to that very array
	size_x is the width of the frame (pixel array)
	size_y is the height of the frame (pixel array)
	*/
	void drawLine(Uint8* color, Uint32* pixels, vec2 p1, vec2 p2) {
		int y, x;
		float k = (p1.getY() - p2.getY()) / (p1.getX() - p2.getX());
		Uint32 color_int = color[0] * 256 * 256 * 256 + color[1] * 256 * 256 + color[2] * 256 + color[3];
		if (abs(p1.getX() - p2.getX()) > abs(p1.getY() - p2.getY()))
		{
			if (p1.getX() < p2.getX())
				swap(p1, p2);
			if (p2.getX() < 0)
				p2.setX(0);
			for (int x = p2.getX(); x < size_x && x < p1.getX(); x++)
			{
				y = k * (x - p1.getX()) + p1.getY();
				if (y >= 0 && y < size_y)
					pixels[y * size_x + x] = color_int;
			}
		}
		else
		{
			if (p1.getY() < p2.getY())
				swap(p1, p2);
			if (p2.getY() < 0)
				p2.setY(0);
			for (int y = p2.getY(); y < size_y && y < p1.getY(); y++)
			{
				x = (y - p1.getY()) / k + p1.getX();
				if (x >= 0 && x < size_x)
					pixels[y * size_x + x] = color_int;
			}
		}
	}
	/*
	something ... something ...
	*/
	static void drawLineToBoolBuffer(bool* pixels, int size_x, int size_y, vec2 p1, vec2 p2) {
		int y, x;
		float k = (p1.getY() - p2.getY()) / (p1.getX() - p2.getX());
		if (abs(p1.getX() - p2.getX()) > abs(p1.getY() - p2.getY()))
		{
			if (p1.getX() < p2.getX())
				swap(p1, p2);
			if (p2.getX() < 0)
				p2.setX(0);
			for (int x = p2.getX(); x < size_x && x < p1.getX(); x++)
			{
				y = k * (x - p1.getX()) + p1.getY();
				if (y >= 0 && y < size_y)
					pixels[y * size_x + x] = true;
			}
		}
		else
		{
			if (p1.getY() < p2.getY())
				swap(p1, p2);
			if (p2.getY() < 0)
				p2.setY(0);
			for (int y = p2.getY(); y < size_y && y < p1.getY(); y++)
			{
				x = (y - p1.getY()) / k + p1.getX();
				if (x >= 0 && x < size_x)
					pixels[y * size_x + x] = true;
			}
		}
	}
	/*
	checks if p3 is on the line between p1 and p2
	*/
	bool pointOnLine(vec2 p1, vec2 p2, vec2 p3)
	{
		if (p1.getX() < p2.getX())
			swap(p1, p2);
		return p3.getX() < p1.getX() && p3.getX() > p2.getX() && p3.getY() == floor((p1.getY() - p2.getY()) / (p1.getX() - p2.getX()) * (p3.getX() - p1.getX()) + p1.getY());
	}
	/*
	draws a face ... its simple ...
	*/
	void drawFace(Uint8* color, Uint32* &pixels, vector<vec2> points)
	{
		for (int i = 0; i < points.size() - 1; i++)
			drawLine(color, pixels, points.at(i), points.at(i + 1));
		drawLine(color, pixels, points.at(0), points.at(points.size() - 1));
	}
	/*
	draws a circle ... not ... its simple ...
	*/
	void drawCircle(Uint8* color, Uint32* pixels, int size_x, int size_y, int radius, int pos_x, int pos_y)
	{
		Uint32 color_int = color[0] * 256 * 256 * 256 + color[1] * 256 * 256 + color[2] * 256 + color[3];
		for (int i = 0; i <= radius * 2; i++)
			pixels[(int)(sqrt(4 - (i - 2) * (i - 2))) * size_x + i] = color_int;
	}
	/*
	fills a face ... its simple ...
	*/
	void fillFace(Uint32 color_int, double points[], bool l)
	{
		bool* buffer = new bool[size_x * size_y];
		memset(buffer, 0, size_x * size_y);

		drawLine(buffer, color_int, points[0], points[1], points[3], points[4]);
		drawLine(buffer, color_int, points[3], points[4], points[6], points[7]);
		drawLine(buffer, color_int, points[6], points[7], points[0], points[1]);
		
		int first = 0, last = 0;
		bool first_time;
		for (int y = 0; y < size_y; y++)
		{
			first_time = true;
			for (int x = 0; x < size_x; x++)
			{
				if (first_time && buffer[y * size_x + x]) {
					first = x;
					first_time = false;
				}
				else if (buffer[y * size_x + x])
					last = x;
			}
			if (!first_time)
				for (int x = first; x <= last; x++)
					pixels[y * size_x + x] = color_int;
		}
		delete[] buffer;
	}
	/*
	fills a face ... with interpolation stuff going on
	*/
	void fillFace(Uint32* pixels, int size_x, int size_y, vector<vec2> points, Uint8 color[3][4], vector<double> z)
	{
		bool* buffer = new bool[size_x * size_y];
		memset(buffer, 0, size_x * size_y);
		for (int i = 0; i < points.size() - 1; i++)
			drawLineToBoolBuffer(buffer, size_x, size_y, points.at(i), points.at(i + 1));
		drawLineToBoolBuffer(buffer, size_x, size_y, points.at(0), points.at(points.size() - 1));
		//coordinates of points
		double
			x_v1 = points.at(0).getX(),
			y_v1 = points.at(0).getY(),
			x_v2 = points.at(1).getX(),
			y_v2 = points.at(1).getY(),
			x_v3 = points.at(2).getX(),
			y_v3 = points.at(2).getY()
			;
		int first = 0, last = 0;
		bool first_time;
		for (int y = 0; y < size_y; y++)
		{
			first_time = true;
			for (int x = 0; x < size_x; x++)
			{
				if (first_time && buffer[y * size_x + x]) {
					first = x;
					first_time = false;
				}
				else if (buffer[y * size_x + x])
					last = x;
			}
			if (!first_time) {
				vector<double> pointbuffer = {
					x_v1,
					y_v1,
					x_v2,
					y_v2,
					x_v3,
					y_v3
				};
				for (int x = first; x <= last; x++) {
					//calculating all the interpolation weights
					double
						w_v1 = ((y_v2 - y_v3) * (x - x_v3) + (x_v3 - x_v2) * (y - y_v3)) / ((y_v2 - y_v3) * (x_v1 - x_v3) + (x_v3 - x_v2) * (y_v1 - y_v3)),
						w_v2 = ((y_v3 - y_v1) * (x - x_v3) + (x_v1 - x_v3) * (y - y_v3)) / ((y_v2 - y_v3) * (x_v1 - x_v3) + (x_v3 - x_v2) * (y_v1 - y_v3)),
						w_v3 = 1 - w_v1 - w_v2
						;
					//calculation of all color channels (argb)
					int
						a = ((color[0][0] * w_v1 + color[1][0] * w_v2 + color[2][0] * w_v3) * 256 * 256 * 256),
						r = ((color[0][1] * w_v1 + color[1][1] * w_v2 + color[2][1] * w_v3) * 256 * 256),
						g = ((color[0][2] * w_v1 + color[1][2] * w_v2 + color[2][2] * w_v3) * 256),
						b = ((color[0][3] * w_v1 + color[1][3] * w_v2 + color[2][3] * w_v3))
						;
					//interpolate 1/z
					double w = ((1 / z.at(0)) * w_v1 + (1 / z.at(1)) * w_v2 + (1 / z.at(2)) * w_v3);
					if (zBuffer[y * size_x + x] < w)
					{
						zBuffer[y * size_x + x] = w;
						pixels[y * size_x + x] = (a & 0xFF000000) | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
					}
				}
			}
		}

		delete[] buffer;
	}
	/*
	draws texture to face ... with interpolation stuff going on
	*/
	void drawTextureToFace(Uint32* pixels, int size_x, int size_y, vector<vec2> points, vector<vec2> uv_coords, vector<double> z, int texture_multiplier_x, int texture_multiplier_y, Uint32* texture, int size_x_texture, int size_y_texture, vector<double> vertex_normal_angle)
	{
		bool* buffer = new bool[size_x * size_y];
		memset(buffer, 0, size_x * size_y);
		for (int i = 0; i < points.size() - 1; i++)
			drawLineToBoolBuffer(buffer, size_x, size_y, points.at(i), points.at(i + 1));
		drawLineToBoolBuffer(buffer, size_x, size_y, points.at(0), points.at(points.size() - 1));
		//all vertex coordinates
		double
			x_v1 = points.at(0).getX(),
			y_v1 = points.at(0).getY(),
			x_v2 = points.at(1).getX(),
			y_v2 = points.at(1).getY(),
			x_v3 = points.at(2).getX(),
			y_v3 = points.at(2).getY()
			;
		int first = 0, last = 0;
		bool first_time;
		double d1 = ((y_v2 - y_v3) * (x_v1 - x_v3) + (x_v3 - x_v2) * (y_v1 - y_v3));
		//uv coordinates at vertices divided by the z value of the vertex
		vec2
			uv_1 = uv_coords.at(0) / z.at(0),
			uv_2 = uv_coords.at(1) / z.at(1),
			uv_3 = uv_coords.at(2) / z.at(2)
			;
		//loop through rows
		for (int y = 0; y < size_y; y++)
		{

			first_time = true;
			for (int x = 0; x < size_x; x++)
			{
				if (first_time && buffer[y * size_x + x]) {
					first = x;
					first_time = false;
				}
				else if (buffer[y * size_x + x])
					last = x;
			}

			if (!first_time)
				for (int x = first; x <= last; x++) {
					//calculating all the interpolation weights
					double
						w_v1 = ((y_v2 - y_v3) * (x - x_v3) + (x_v3 - x_v2) * (y - y_v3)) / d1,
						w_v2 = ((y_v3 - y_v1) * (x - x_v3) + (x_v1 - x_v3) * (y - y_v3)) / d1,
						w_v3 = 1 - w_v1 - w_v2
						;

					//interpolate 1/z
					double w = ((1 / z.at(0)) * w_v1 + (1 / z.at(1)) * w_v2 + (1 / z.at(2)) * w_v3);
					//calculation of uv coordinates
					double
						u1 = (uv_1.getX() * w_v1 + uv_2.getX() * w_v2 + uv_3.getX() * w_v3) * 1 / w,
						v1 = (uv_1.getY() * w_v1 + uv_2.getY() * w_v2 + uv_3.getY() * w_v3) * 1 / w
						;

					int
						u = abs((int)((u1 * size_x_texture) * texture_multiplier_x) % (size_x_texture)),
						v = abs(((int)(v1 * size_y_texture) * texture_multiplier_y) % (size_y_texture))
						;
					//cout << "u: "<< u << "; v:" << v << endl;
					//calculation of all color channels (argb)
					Uint32 color[4] = {
						(texture[v * size_x_texture + u] & 0xFF000000) * (vertex_normal_angle.at(0) * w_v1 + vertex_normal_angle.at(1) * w_v2 + vertex_normal_angle.at(2) * w_v3),
						(texture[v * size_x_texture + u] & 0x00FF0000) * (vertex_normal_angle.at(0) * w_v1 + vertex_normal_angle.at(1) * w_v2 + vertex_normal_angle.at(2) * w_v3),
						(texture[v * size_x_texture + u] & 0x0000FF00) * (vertex_normal_angle.at(0) * w_v1 + vertex_normal_angle.at(1) * w_v2 + vertex_normal_angle.at(2) * w_v3),
						(texture[v * size_x_texture + u] & 0x000000FF) * (vertex_normal_angle.at(0) * w_v1 + vertex_normal_angle.at(1) * w_v2 + vertex_normal_angle.at(2) * w_v3)
					};
					if (zBuffer[y * size_x + x] < w)
					{
						zBuffer[y * size_x + x] = w;
						pixels[y * size_x + x] = (color[0] & 0xFF000000) | (color[1] & 0x00FF0000) | (color[2] & 0x0000FF00) | (color[3] & 0x000000FF);
					}
				}
		}
		delete[] buffer;
	}
	/*
	projection ... lel
	vec3 point (point to project in world space)
	camera cam (camera object)
	*/
	vec2 project(vec3 point, camera cam, vec3* axes_og) {
		vec3 axes[] = { axes_og[0],axes_og[1],axes_og[2] };
		point -= cam.getPos();
		point = point.rotate(axes[0], cam.getRot().getX());
		for (int i = 0; i < 3; i++)
			axes[i] = axes[i].rotate(axes[0], cam.getRot().getX());
		point = point.rotate(axes[1], cam.getRot().getY());
		for (int i = 0; i < 3; i++)
			axes[i] = axes[i].rotate(axes[1], cam.getRot().getY());
		point = point.rotate(axes[2], cam.getRot().getZ());
		if (point.getZ() < 1)
			point.setZ(1);
		return vec2(point.getX() * cam.getFocus() / point.getZ(), point.getY() * cam.getFocus() / point.getZ());
	}

	/*
	projection ... lel ... and returns camera space z component
	vec3 point (point to project in world space)
	camera cam (camera object)
	*/
	vec3 project_w(vec3 point, camera cam, vec3* axes_og) {
		vec3 axes[] = { axes_og[0],axes_og[1],axes_og[2] };
		point -= cam.getPos();
		point = point.rotate(axes[0], cam.getRot().getX());
		for (int i = 0; i < 3; i++)
			axes[i] = axes[i].rotate(axes[0], cam.getRot().getX());
		point = point.rotate(axes[1], cam.getRot().getY());
		for (int i = 0; i < 3; i++)
			axes[i] = axes[i].rotate(axes[1], cam.getRot().getY());
		point = point.rotate(axes[2], cam.getRot().getZ());
		if (point.getZ() < 1)
			point.setZ(1);
		return vec3(point.getX() * cam.getFocus() / point.getZ(), point.getY() * cam.getFocus() / point.getZ(), point.getZ());
	}


	/*
	reads a file to a char buffer
	*/
	char* readFileBytes(string path) {
		ifstream file(path, ios::binary | ios::ate);
		streamsize length = file.tellg();
		file.seekg(0, ios::beg);
		char* ret = new char[length];
		//memset(ret, 0, length * sizeof(char));
		file.read(ret, length);
		file.close();
		return ret;
	}

	/*
	converts a bitmap file (in  form of a char buffer)
	into a texture buffer of unsigned 32bit integers
	*/
	Uint32* bitmapToTexture(char* bitmap, int size_x, int size_y) {
		Uint32* tex = new Uint32[size_x * size_y];
		int pos = 0;
		for (int i = 54; i < 54 + 4 * size_x * size_y - 3; i += 4) {
			unsigned char
				b = bitmap[i],
				g = bitmap[i + 1],
				r = bitmap[i + 2],
				a = bitmap[i + 3]
				;
			tex[pos] = pow(256, 3) * a + pow(256, 2) * r + pow(256, 1) * g + pow(256, 0) * b;
			pos++;
		}
		return tex;
	}

	/*
	some attributes
	*/
	chrono::steady_clock::time_point currentTime, lastTime = Clock::now();
	double interval = 0.125;
	int current_texture = 0;
	int number_of_textures;
	int** size;
	Uint32** texture;
	Uint32* pixels;
	Uint8 FACES[4] = { 0,255,255,255 }, NORMALS[4] = { 0,255,0,255 }, RGB_COLORS[3][4] = { { 0,255,0,0 }, { 0,0,255,0 }, { 0,0,0,255 } }, CMY_COLORS[3][4] = { { 0,0,255,255 }, { 0,255,0,255 }, { 0,255,255,0 } }, CAMERA[4] = { 0,0,0,0 }, OUTLINE[4] = { 0,255,165,0 };
	Uint32
		outline = OUTLINE[0] * 256 * 256 * 256 + OUTLINE[1] * 256 * 256 + OUTLINE[2] * 256 + OUTLINE[3],
		face = FACES[0] * 256 * 256 * 256 + FACES[1] * 256 * 256 + FACES[2] * 256 + FACES[3],
		red = 256 * 256 * 255,
		green = 256 * 255,
		blue = 255
	;
	Uint8 colors[3][4] = {
					{ CMY_COLORS[1][0], CMY_COLORS[1][1], CMY_COLORS[1][2], CMY_COLORS[1][3] },
					{ CMY_COLORS[2][0], CMY_COLORS[2][1], CMY_COLORS[2][2], CMY_COLORS[2][3] },
					{ CMY_COLORS[0][0], CMY_COLORS[0][1], CMY_COLORS[0][2], CMY_COLORS[0][3] }
	};
	Uint8* color = NORMALS;
	int size_x, size_y, length;
	double* zBuffer;

	//opencl attributes
	Program halloWelt;
	Context context;
	Buffer outBuf, inBuf;
	Device device;
	vector<int> vec;
	CommandQueue queue;
	Kernel kernel;

public:
	//constructor
	Pipeline(string path, int number, int x, int y, Uint32* lel) {
		pixels = lel;
		size_x = x;
		size_y = y;
		length = x * y;
		zBuffer = new double[x * y];
		memset(zBuffer, 0, x * y * sizeof(double));
		/*
		//OpenCL stuff
		halloWelt = getProgram("CLkernels/helloWorld.cl");
		context = halloWelt.getInfo<CL_PROGRAM_CONTEXT>();
		vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
		device = devices.front();


		vec = vector<int>(15);
		fill(vec.begin(), vec.end(), 10);

		//memory buffer as kernel arguments
		inBuf = Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * vec.size(), vec.data());
		outBuf = Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * vec.size(), nullptr);
		kernel = Kernel(halloWelt, "ProcessArray");
		kernel.setArg(0, inBuf);
		kernel.setArg(1, outBuf);
		queue = CommandQueue(context, device);

		//queue.enqueueFillBuffer(inBuf, 3, sizeof(int) * 10, sizeof(int)* (vec.size()-10));
		queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(vec.size()));
		queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeof(int) * vec.size(), vec.data());
		for (int i : vec)
			cout<< i <<endl;
		finish();
		*/

		/*
		//generates checkerboard texture
		memset(texture[0], 0, size[0][0] * size[0][1] * sizeof(Uint32));
		for (int x = 0; x < size[0][0]; x++)
			for (int y = 0; y < size[0][1]; y++)
				if ((y % 2 == 0 ? x : x + 1) % 2 == 0)
					//if (x % 2 == 0)
					texture[0][y * size[0][0] + x] = pow(256, 3) * color[0] + pow(256, 2) * color[1] + pow(256, 1) * color[2] + pow(256, 0) * color[3];
		*/
		number_of_textures = number;
		size = new int* [number_of_textures];
		for (int i = 0; i < number_of_textures; i++)
			size[i] = new int[2];
		texture = new Uint32 * [number_of_textures];
		string extension = ".bmp";
		string file_name = "assets/bitmaps/" + path;
		for (int i = 0; i < number_of_textures; i++) {
			char* texture_file = readFileBytes(file_name + to_string(i + 1) + extension);
			size[i][0] = (Uint8)texture_file[18] * pow(0x100, 0) + (Uint8)texture_file[19] * pow(0x100, 1) + (Uint8)texture_file[20] * pow(0x100, 2) + (Uint8)texture_file[21] * pow(0x100, 3);
			size[i][1] = (unsigned)((Uint8)texture_file[22] * pow(0x100, 0) + (Uint8)texture_file[23] * pow(0x100, 1) + (Uint8)texture_file[24] * pow(0x100, 2) + (Uint8)texture_file[25] * pow(0x100, 3));
			size[i][1] = abs(size[i][1]);
			texture[i] = bitmapToTexture(texture_file, size[i][0], size[i][1]);
		}
	}
	void draw(Scene& scene)
	{
		//scene.getObject(0).getName();
		//test if there is an active camera
		if (scene.activeCam() != -1) {
			memset(zBuffer, 0, size_x * size_y * sizeof(double));
			int center_x = size_x / 2;
			int center_y = size_y / 2;
			camera cam = scene.getActiveCam();
			double x = cam.getPos().getX();
			double y = cam.getPos().getY();
			double z = cam.getPos().getZ();
			vec3 pos = cam.getPos();
			int cam_x = size_x / cam.getViewX();
			int cam_y = size_y / cam.getViewY();
			double focus = cam.getFocus();
			//draw all objects in the scene
			//animation stuff

			currentTime = Clock::now();
			chrono::duration<double> time_span = currentTime - lastTime;
			if (time_span.count() >= interval) {
				lastTime = Clock::now();
				current_texture++;
			}
			int t = current_texture % number_of_textures;

				int max = scene.getFaceNumber(0);
				object& obj = scene.getObject(0);
				for (int f = 0; f < max; f++) {
							
					if (0 < (obj.face_normals.at(f) * (pos - obj.vertices.at(obj.faces.at(f).at(0) - 1))))
					{
						double points[9];
						vec3 point0 = obj.vertices.at(obj.faces.at(f).at(0) - 1) - pos;
						points[2] = point0.getZ();
						if (points[2] < 1)
							points[2] = 1;
						points[0] = (point0.getX() * focus / points[2]) * cam_x + center_x;
						points[1] = (point0.getY() * focus / points[2]) * cam_y + center_y;

						vec3 point1 = obj.vertices.at(obj.faces.at(f).at(1) - 1) - pos;
						points[5] = point1.getZ();
						if (points[5] < 1)
							points[5] = 1;
						points[3] = (point1.getX() * focus / points[5]) * cam_x + center_x;
						points[4] = (point1.getY() * focus / points[5]) * cam_y + center_y;

						vec3 point2 = obj.vertices.at(obj.faces.at(f).at(2) - 1) - pos;
						points[8] = point2.getZ();
						if (points[8] < 1)
							points[8] = 1;
						points[6] = (point2.getX() * focus / points[8]) * cam_x + center_x;
						points[7] = (point2.getY() * focus / points[8]) * cam_y + center_y;

						if (points[0] > 0 && points[0] < size_x && points[1] > 0 && points[1] < size_y)
						{
							fillFace(face, points);
							drawFace(outline, points);
						}
					}
				}
		}
	}
};