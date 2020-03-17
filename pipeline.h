#pragma once

#include <SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>  
#include <fstream>  
#include <vector>
//#include <bitset>

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
	void drawLine(Uint8* color, Uint32* pixels, vec2 p1, vec2 p2) {
		//int y, x;
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
				int y = k * (x - p1.getX()) + p1.getY();
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
				int x = (y - p1.getY()) / k + p1.getX();
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
	draws a triangle
	*/
	void drawTriangle(Uint8* color, Uint32* pixels, int size_x, int size_y, vector<vec2> points) {
		Uint32 color_int = color[0] * 256 * 256 * 256 + color[1] * 256 * 256 + color[2] * 256 + color[3];
		for (int x = 0; x < size_x; x++)
			for (int y = 0; y < size_y; y++)
			{
				double e1 = (x - points.at(0).getX()) * (points.at(1).getY() - points.at(0).getY()) - (y - points.at(0).getY()) * (points.at(1).getX() - points.at(0).getX());
				double e2 = (x - points.at(1).getX()) * (points.at(2).getY() - points.at(1).getY()) - (y - points.at(1).getY()) * (points.at(2).getX() - points.at(1).getX());
				double e3 = (x - points.at(2).getX()) * (points.at(0).getY() - points.at(2).getY()) - (y - points.at(2).getY()) * (points.at(0).getX() - points.at(2).getX());

				if (e1 >= 1 && e2 >= 1 && e3 >= 1)
					pixels[y * size_x + x] = color_int;
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
	void drawFace(Uint8* color, Uint32* pixels, int size_x, int size_y, vector<vec2> points)
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
	void fillFace(Uint8* color, Uint32* pixels, int size_x, int size_y, vector<vec2> points)
	{
		Uint32 color_int = color[0] * 256 * 256 * 256 + color[1] * 256 * 256 + color[2] * 256 + color[3];
		bool* buffer = new bool[size_x * size_y];
		memset(buffer, 0, size_x * size_y);
		for (int i = 0; i < points.size() - 1; i++)
			drawLineToBoolBuffer(buffer, size_x, size_y, points.at(i), points.at(i + 1));
		drawLineToBoolBuffer(buffer, size_x, size_y, points.at(0), points.at(points.size() - 1));

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
	fills a face ... with interpolation stuff
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
	draws texture to face ... with interpolation stuff
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
						v = abs((int)((v1 * size_y_texture) * texture_multiplier_y) % (size_y_texture))
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
	Uint8 FACES[4] = { 0,255,255,255 }, NORMALS[4] = { 0,255,0,255 }, RGB_COLORS[3][4] = { { 0,255,0,0 }, { 0,0,255,0 }, { 0,0,0,255 } }, CMY_COLORS[3][4] = { { 0,0,255,255 }, { 0,255,0,255 }, { 0,255,255,0 } }, CAMERA[4] = { 0,0,0,0 }, OUTLINE[4] = { 0,255,165,0 };
	Uint8 colors[3][4] = {
					{ CMY_COLORS[1][0], CMY_COLORS[1][1], CMY_COLORS[1][2], CMY_COLORS[1][3] },
					{ CMY_COLORS[2][0], CMY_COLORS[2][1], CMY_COLORS[2][2], CMY_COLORS[2][3] },
					{ CMY_COLORS[0][0], CMY_COLORS[0][1], CMY_COLORS[0][2], CMY_COLORS[0][3] }
	};
	Uint8* color = NORMALS;
	int size_x, size_y;
	double* zBuffer;

	//opencl attributes
	Program halloWelt;
	Context context;
	Buffer outBuf, inBuf;
	Device device;
	vector<int> vec;
	CommandQueue queue;
	Kernel kernel;
	Buffer cl_pixels, cl_texture, cl_zBuffer;
	Kernel yeet;


public:
	//constructor
	Pipeline(string path, int number, int x, int y) {
		size_x = x;
		size_y = y;
		zBuffer = new double[x * y];
		memset(zBuffer, 0, x * y * sizeof(double));

		//OpenCL stuff
		halloWelt = getProgram("CLkernels/helloWorld.cl");
		context = halloWelt.getInfo<CL_PROGRAM_CONTEXT>();
		vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
		device = devices.front();

		lastTime = Clock::now();

		vec = vector<int>(10);
		//cout << "size: " << vec.size() << endl;
		fill(vec.begin(), vec.end(), 1);

		//memory buffers
		cl_pixels = Buffer(context, CL_MEM_READ_ONLY, sizeof(Uint32) * x * y);
		cl_zBuffer = Buffer(context, CL_MEM_READ_ONLY, sizeof(double) * x * y);

		queue = CommandQueue(context, device);

		//kernel
		yeet = Kernel(halloWelt, "ProcessArray");
		yeet.setArg(0, cl_pixels);
		yeet.setArg(2, (cl_int)x);
		yeet.setArg(3, (cl_int)y);
		yeet.setArg(21, cl_zBuffer);

		//queue.enqueueWriteBuffer(cl_pixels, CL_TRUE, 0, sizeof(Uint32) * frame_width * frame_height, frame);

		currentTime = Clock::now();
		chrono::duration<double> time_span = currentTime - lastTime;
		//cout << "OpenCL execution time: "<< time_span.count() << "s" << endl;

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
		cl_texture = Buffer(context, CL_MEM_READ_ONLY, sizeof(Uint32) * size[0][0] * size[0][1]);
		queue.enqueueWriteBuffer(cl_texture, CL_TRUE, 0, sizeof(Uint32) * size[0][0] * size[0][1], texture[0]);
		yeet.setArg(1, cl_texture);
		yeet.setArg(4, (cl_int)size[0][0]);
		yeet.setArg(5, (cl_int)size[0][1]);
	}
	void draw(Scene scene, Uint32* frame, int frame_width, int frame_height)
	{
		queue.enqueueReadBuffer(cl_pixels, CL_TRUE, 0, sizeof(Uint32) * frame_width * frame_height, frame);
		queue.enqueueFillBuffer(cl_pixels, 0, 0, sizeof(Uint32) * frame_width * frame_height, NULL, 0);
		if (1)
		{
			//test if there is an active camera
			if (scene.activeCam() != -1) {
				//memset(zBuffer, 0, size_x * size_y * sizeof(double));
				queue.enqueueFillBuffer(cl_zBuffer, 0, 0, sizeof(double) * frame_width * frame_height, NULL, 0);
				vec2 center = vec2(frame_width / 2, frame_height / 2);
				camera cam = scene.getActiveCam();
				//draw all objects in the scene
				//animation stuff
				currentTime = Clock::now();
				chrono::duration<double> time_span = currentTime - lastTime;
				if (time_span.count() >= interval) {
					lastTime = Clock::now();
					current_texture++;
				}
				int t = current_texture % number_of_textures;
				//loop through all objects
				for (object o : scene.getObjects()) {
					//draw all faces
					for (int i = 0; i < o.faces.size(); i++) {
						vector<int> f = o.faces.at(i);
						if (0 < o.face_normals.at(i) * (cam.getPos() - o.vertices.at(f.at(0) - 1)))
						{
							vector<vec2> points;
							for (int v = 0; v < f.size(); v++) {
								//push vertex coordinates into point array
								points.push_back(project(o.vertices.at(f.at(v) - 1), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center);
							}

							vec3
								light_vector1 = o.vertices.at(f.at(0) - 1) - scene.getLights().at(0).point,
								light_vector2 = o.vertices.at(f.at(1) - 1) - scene.getLights().at(0).point,
								light_vector3 = o.vertices.at(f.at(2) - 1) - scene.getLights().at(0).point
								;

							//for (int i = 0; i < 3; i++)
							//	drawLine(NORMALS, frame, frame_width, frame_height, project(scene.getLights().at(0).point, cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center, project(o.vertices.at(f.at(i) - 1), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center);

							//compute angle of vertex normal to light vector

							double
								angle1 = (o.normals.at(o.vertex_normals.at(i).at(0) - 1).angle(light_vector1)) / 180,
								angle2 = (o.normals.at(o.vertex_normals.at(i).at(1) - 1).angle(light_vector2)) / 180,
								angle3 = (o.normals.at(o.vertex_normals.at(i).at(2) - 1).angle(light_vector3)) / 180
								;
							vector<double> angles = { angle1,angle2,angle3 };
							//compute color of every vertex

							Uint8 colors[3][4] = {
								{color[0] * angle1, color[1] * angle1, color[2] * angle1, color[3] * angle1},
								{color[0] * angle2, color[1] * angle2, color[2] * angle2, color[3] * angle2},
								{color[0] * angle3, color[1] * angle3, color[2] * angle3, color[3] * angle3}
							};

							//get uv coordinates of every vertex
							vector<vec2> uvs = {
								o.uv_texture_coordinates.at(o.uv.at(i).at(0) - 1),
								o.uv_texture_coordinates.at(o.uv.at(i).at(1) - 1),
								o.uv_texture_coordinates.at(o.uv.at(i).at(2) - 1)
							};
							//get the z component of every vertex (for perspective correct interpolation)
							vector<double> z = {
								(project_w(o.vertices.at(f.at(0) - 1), cam, scene.getAxes())).getZ(),
								(project_w(o.vertices.at(f.at(1) - 1), cam, scene.getAxes())).getZ(),
								(project_w(o.vertices.at(f.at(2) - 1), cam, scene.getAxes())).getZ()
							};
							for (int p = 0; p < points.size(); p++) {
								yeet.setArg(6 + 5 * p, (cl_double)points.at(p).getX());
								yeet.setArg(6 + 5 * p + 1, (cl_double)points.at(p).getY());
								yeet.setArg(6 + 5 * p + 2, (cl_double)z.at(p));
								yeet.setArg(6 + 5 * p + 3, (cl_double)uvs.at(p).getX());
								yeet.setArg(6 + 5 * p + 4, (cl_double)uvs.at(p).getY());
							}
							int
								min_x = min(min(points.at(0).getX(), points.at(1).getX()), points.at(2).getX()),
								max_x = max(max(points.at(0).getX(), points.at(1).getX()), points.at(2).getX()),
								min_y = min(min(points.at(0).getY(), points.at(1).getY()), points.at(2).getY()),
								max_y = max(max(points.at(0).getY(), points.at(1).getY()), points.at(2).getY())
								;
							yeet.setArg(22, (cl_int)min_x);
							yeet.setArg(23, (cl_int)min_y);
							queue.enqueueNDRangeKernel(yeet, NullRange, NDRange(max_x - min_x, max_y - min_y), NullRange);
							//queue.enqueueReadBuffer(cl_pixels, CL_TRUE, 0, sizeof(Uint32) * frame_width * frame_height, frame);
							//draw the face
							//drawTextureToFace(frame, frame_width, frame_height, points, uvs, z, 1, 1, texture[t], size[t][0], size[t][1], angles);
							//if(!o.culling)
							//	fillFace(OUTLINE, frame, frame_width, frame_height, points);
							//drawFace(OUTLINE, frame, frame_width, frame_height, points);
							//drawTriangle(OUTLINE, frame, frame_width, frame_height, points);
							//else
							//fillFace(frame, frame_width, frame_height, points, colors, z);
							//fillFace(OUTLINE, frame, frame_width, frame_height, points);

							/*
							//draw normals
							for (int v = 0; v < f.size(); v++) {
								//draw vertex normals
								drawLine(NORMALS, frame, frame_width, frame_height,
									project(o.vertices.at(f.at(v) - 1), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center,
									project(o.normals.at(o.vertex_normals.at(i).at(v) - 1) * 3 + o.vertices.at(f.at(v) - 1), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center
								);
								//draw face normals
								drawLine(CAMERA, frame, frame_width, frame_height,
									project(o.vertices.at(f.at(v) - 1), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center,
									project(o.face_normals.at(i) * 3 + o.vertices.at(f.at(v) - 1), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center
								);
							}
							*/

						}
					}

					//draw axes
					/*
					for (int i = 0; i < 3; i++) {
						drawLine(RGB_COLORS[i], frame, frame_width, frame_height, project(vec3(0, 0, 0), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center, project(o.axes[i], cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center);
					}
					*/

				}

				//draws texture into top left corner of screen
				/*
				for (int x = 0; x < size_x; x++)
					for (int y = 0; y < size_y; y++)
						frame[y * frame_width + x] = texture[y * size_x + x];
				*/

				/*
				//draw all lights in the scene
				for (light i : scene.getLights())
					drawCircle(RGB_COLORS[1], frame, frame_width, frame_height, 10, 10, 10);
				*/
				/*
				//draw all cameras in the scene (except active one)
				for (int i = 0; i < scene.getCameras().size(); i++)
					if (i != scene.activeCam())
					{
						vec3 axes[] = { scene.getAxes()[0], scene.getAxes()[1], scene.getAxes()[2] };
						camera c = scene.getCameras().at(i);
						double focal_length_view = 1, view_x = focal_length_view * c.getViewX() / c.getFocus(), view_y = focal_length_view * c.getViewY() / c.getFocus();
						vec3 points[4] =
						{
						vec3(-view_x / 2, view_y / 2, focal_length_view),
						vec3(view_x / 2, view_y / 2, focal_length_view),
						vec3(view_x / 2, -view_y / 2, focal_length_view),
						vec3(-view_x / 2, -view_y / 2, focal_length_view)
						};
						vec3 triangle[3] =
						{
							vec3(view_x / 2, -view_y / 2 + -view_y / 4, focal_length_view),
							vec3(-view_x / 2, -view_y / 2 + -view_y / 4, focal_length_view),
							vec3(0, -view_y + -view_y / 4, focal_length_view)
						};
						for (int p = 0; p < 4; p++)
							points[p] = points[p].rotate(c.axes[0], -c.getRot().getX());
						for (int p = 0; p < 3; p++)
							triangle[p] = triangle[p].rotate(c.axes[0], -c.getRot().getX());
						for (int p = 0; p < 4; p++)
							points[p] = points[p].rotate(c.axes[1], -c.getRot().getY());
						for (int p = 0; p < 3; p++)
							triangle[p] = triangle[p].rotate(c.axes[1], -c.getRot().getY());
						for (int p = 0; p < 4; p++)
							points[p] = points[p].rotate(c.axes[2], -c.getRot().getZ());
						for (int p = 0; p < 3; p++)
							triangle[p] = triangle[p].rotate(c.axes[2], -c.getRot().getZ());
						for (vec3 point : points)
							drawLine(CAMERA, frame, frame_width, frame_height, project(c.getPos() + point, cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center, project(c.getPos(), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center);
						vector<vec2> points_projected;
						vector<vec2> triangle_projected;
						for (int v = 0; v < 4; v++)
							points_projected.push_back(project(c.getPos() + points[v], cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center);
						for (int v = 0; v < 3; v++)
							triangle_projected.push_back(project(c.getPos() + triangle[v], cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center);
						drawFace(CAMERA, frame, frame_width, frame_height, points_projected);
						fillFace(CAMERA, frame, frame_width, frame_height, triangle_projected);
						//draw axes
						for (int i = 0; i < 3; i++) {
							drawLine(RGB_COLORS[i], frame, frame_width, frame_height, project(c.getPos(), cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center, project(c.getPos() + c.axes[i], cam, scene.getAxes()) * vec2(frame_width / cam.getViewX(), frame_height / cam.getViewY()) + center);
						}
					}
					*/
			}
		}
	}
};