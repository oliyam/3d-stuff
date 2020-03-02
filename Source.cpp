#pragma once

#include <SDL.h>
#include <CL/cl.hpp>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>

#include <ctime>
#include <chrono>

#include "scene.h"
#include "pipeline.h"
#include "vectors.h"
#include "object.h"

using std::string;
using namespace cl;
using namespace std;

typedef chrono::high_resolution_clock Clock;

double multiplikator = 3;
const int SCREEN_WIDTH = 192 * multiplikator, SCREEN_HEIGHT = 108 * multiplikator;

//console stuff
void HideConsole()
{::ShowWindow(::GetConsoleWindow(), SW_HIDE);}

void ShowConsole()
{::ShowWindow(::GetConsoleWindow(), SW_SHOW);}

bool IsConsoleVisible()
{return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;}

//main ... lol
int main(int argc, char* args[]) 
{
	bool stop = false;
	int frame = 0;
	double angle = 0;
	int shift = 8;
	bool quit = false;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	SDL_Window* window = SDL_CreateWindow("3D Stuff yo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);// SDL_RENDERER_ACCELERATED);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

	bool leftMouseButtonDown = false;
	bool hidden = false;
	int active_camera=0;
	string pictures = "globe";
	int number = 1;

	Scene scene = Scene();
	Pipeline pipe = Pipeline(pictures, number, SCREEN_WIDTH, SCREEN_HEIGHT);

	//declare pixel array accommodating all pixels on the screen
	Uint32* pixels = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
	Uint32* pixels_background = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
	//initializing every pixel to be 0 (black)
	//memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
	//memset(pixels_background, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

	int mouseX, mouseY;
	Uint8 GREEN[4] = { 0,0,255,0 };
	Uint8 BLACK[4] = { 0,0,0,0 };

	//HideConsole();
	ShowConsole();

	//opencl stuff
	/*
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	_ASSERT(platforms.size() > 0);
	*/

	//game start time
	auto lastTime = Clock::now();

	SDL_UpdateTexture(texture, NULL, pixels_background, SCREEN_WIDTH * sizeof(Uint32));

	while (!quit)
	{
		//measure start time
		auto startTime = Clock::now();

			//event handling
			SDL_PollEvent(&event);
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case(SDL_WINDOWEVENT_MINIMIZED):
					hidden = true;
					break;
				case(SDL_WINDOWEVENT_RESTORED):
					hidden = false;
					break;
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = true;
				break;
			case SDL_MOUSEMOTION:
				/*
				if (leftMouseButtonDown)
				{
					mouseX = event.motion.x;
					mouseY = event.motion.y;
					Uint8 a = 0, r = 255, g = 165, b = 0;
					pixels[mouseY * SCREEN_WIDTH + mouseX] = a * 256 * 256 * 256 + r * 256 * 256 + g * 256 + b;
				}
				*/
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_l:
					pipe = Pipeline(pictures, number, SCREEN_WIDTH, SCREEN_HEIGHT);
					scene.setObj(0, object("test.obj", "test_smooth.obj", true));
					break;
				case SDLK_a:
					scene.rotate(0, 0, 3);
					break;
				case SDLK_s:
					scene.rotate(0, 1, 3);
					break;
				case SDLK_q:
					scene.rotate(0, 2, 3);
					break;
				case SDLK_d:
					scene.rotate(0, 0, -3);
					break;
				case SDLK_w:
					scene.rotate(0, 1, -3);
					break;
				case SDLK_e:
					scene.rotate(0, 2, -3);
					break;
				case SDLK_LSHIFT:
					ShowConsole();
					break;
				case SDLK_LCTRL:
					HideConsole();
					break;
				case SDLK_KP_PLUS:
					shift++;
					break;
				case SDLK_KP_MINUS:
					shift--;
					break;
				case SDLK_SPACE:
					stop = !stop;
					break;
				}
				break;
			}

			if (!hidden && !stop) {
				memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));	
				/*
				for (int x = 0; x < SCREEN_WIDTH; x++)
					for (int y = 0; y < SCREEN_HEIGHT; y++)
						pixels[y * SCREEN_WIDTH + x] = 0 * 256 * 256 * 256 + 70 * 256 * 256 + 70 * 256 + 70;
				*/
				
				//cool gradient
				for (int x = 0; x < SCREEN_WIDTH; x++)
					for (int y = 0; y < SCREEN_HEIGHT; y++)
					{
						Uint8 a = 0, r = y * 256 / SCREEN_HEIGHT, g = 255, b = 255;
						//Uint8 a = 0, r =255, g = 255, b = 255;
						pixels[y * SCREEN_WIDTH + x] = a * 256 * 256 * 256 + r * 256 * 256 + g * 256 + b;
					}
				
				//draw the spinning line thingy
				//memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
				//pixels = drawLine(GREEN, pixels, SCREEN_WIDTH, vec2(SCREEN_WIDTH / 2 + cos(angle * 3.14 / 180)*10, SCREEN_HEIGHT / 2 + sin(angle * 3.14 / 180) * 10), vec2(SCREEN_WIDTH/2+cos(angle * 3.14 / 180)* SCREEN_HEIGHT / 2, SCREEN_HEIGHT/2+sin(angle * 3.14 / 180)* SCREEN_HEIGHT / 2));
				//angle++;
				scene.setActiveCam(abs(shift) % scene.getCameras().size());
				pipe.draw(scene, pixels, SCREEN_WIDTH, SCREEN_HEIGHT);

				SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
			}
			

		//measure current time
		auto currentTime = Clock::now();
		//calculate delta time
		chrono::duration<double> time_span = currentTime - startTime;
		chrono::duration<double> time_span_fps_print = currentTime - lastTime;
		if (time_span_fps_print.count() >= 1){
			cout << "FPS: " << 1 / time_span.count() << "; render stop: " << stop << endl;
			lastTime = Clock::now();
		}
	}

	delete[] pixels;
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0x1a4;
}