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

	/*
	whatever ...
	*/
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
	void fillFace(Uint32 color, double points[])
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
	void fillFaceInterpolation(Uint32 color, double points[])
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
		int 
			red_ = red / points[2],
			green_ = green / points[5],
			blue_ = blue / points[8]
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
						pixels[o] = (((int)(red_ * w_v0 / w) & 0x00FF0000) | ((int)(green_ * w_v1 / w) & 0x0000FF00) | ((int)(blue_ * w_v2  / w) & 0x000000FF));
					}
				}
			}
		}
	}
	/*
	draws a texture to a face ... its simple ...
	*/
	void drawTextureToFace(double points[], double uvs[])
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
		double
			u_0 = uvs[0] / points[2],
			v_0 = uvs[1] / points[2],
			u_1 = uvs[2] / points[5],
			v_1 = uvs[3] / points[5],
			u_2 = uvs[4] / points[8],
			v_2 = uvs[5] / points[8]
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
				if (w_v0 >= 0 && w_v1 >= 0 && w_v2 >= 0) {
					int o = i + x;
					double w = ((1 / points[2]) * w_v0 + (1 / points[5]) * w_v1 + (1 / points[8]) * w_v2);
					if (zBuffer[o] < w)
					{

						int
							u = abs((int)((((u_0 * w_v0 + u_1 * w_v1 + u_2 * w_v2) / w) * size[0][0]) * 1) % size[0][0]),
							v = abs((int)((((v_0 * w_v0 + v_1 * w_v1 + v_2 * w_v2) / w) * size[0][1]) * 1) % size[0][1])
							;
						zBuffer[o] = w;
						pixels[o] = texture[0][v * size[0][0] + u];
					}
				}
			}
		}
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
						double uvs[6];

						vec3 point0 = obj.vertices.at(obj.faces.at(f).at(0) - 1) - pos;
						points[2] = point0.getZ();
						if (points[2] < 1)
							points[2] = 1;
						points[0] = (point0.getX() * focus / points[2]) * cam_x + center_x;
						points[1] = (point0.getY() * focus / points[2]) * cam_y + center_y;
						vec2 uv0 = obj.uv_texture_coordinates.at(obj.uv.at(f).at(0) - 1);
						uvs[0] = uv0.getX();
						uvs[1] = uv0.getY();

						vec3 point1 = obj.vertices.at(obj.faces.at(f).at(1) - 1) - pos;
						points[5] = point1.getZ();
						if (points[5] < 1)
							points[5] = 1;
						points[3] = (point1.getX() * focus / points[5]) * cam_x + center_x;
						points[4] = (point1.getY() * focus / points[5]) * cam_y + center_y;
						vec2 uv1 = obj.uv_texture_coordinates.at(obj.uv.at(f).at(1) - 1);
						uvs[2] = uv1.getX();
						uvs[3] = uv1.getY();

						vec3 point2 = obj.vertices.at(obj.faces.at(f).at(2) - 1) - pos;
						points[8] = point2.getZ();
						if (points[8] < 1)
							points[8] = 1;
						points[6] = (point2.getX() * focus / points[8]) * cam_x + center_x;
						points[7] = (point2.getY() * focus / points[8]) * cam_y + center_y;
						vec2 uv2 = obj.uv_texture_coordinates.at(obj.uv.at(f).at(2) - 1);
						uvs[4] = uv2.getX();
						uvs[5] = uv2.getY();

						if (points[0] > 0 && points[0] < size_x && points[1] > 0 && points[1] < size_y)
						{
							drawTextureToFace(points, uvs);
							//drawFace(outline, points);
						}
					}
				}
		}
	}
};