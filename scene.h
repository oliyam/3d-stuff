#pragma once

/*
Scene class that holds objects, cameras, lights, and world space positions of all objects
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "vectors.h"
#include "object.h"
#include "camera.h"
#include "light.h"

class Scene {

	private:
		vector<object> objects;
		vector<vec3> positions;
		vector<camera> cameras;
		vector<light> lights;

		//axes (x,y,z)
		vec3 axes[3] = { vec3(1,0,0), vec3(0,1,0), vec3(0,0,1) };

		//index of the active camera -1 if none
		int active_camera = 0;

		//checking if object name is already in use
		int object_valid(object obj)
		{
			int valid = 0;
			for (int i = 0; i < objects.size(); i++)
				if (objects.at(i).getName().compare(obj.getName()))
					valid++;
			return valid;
		}

	public:
		void rotate(int index, int axis, float angle)
		{objects.at(index).rotateObj(axis, angle);}

		void rotateCamX(int index) 
		{cameras.at(index).setPos(vec3(cameras.at(index).getPos())+vec3(0,0,-60)); }
		void rotateCamY(int index)
		{cameras.at(index).setRot(vec3(cameras.at(index).getRot().getX(), cameras.at(index).getRot().getY() + 1, cameras.at(index).getRot().getZ()));}
		void rotateCamZ(int index)
		{cameras.at(index).setRot(vec3(cameras.at(index).getRot().getX(), cameras.at(index).getRot().getY(), cameras.at(index).getRot().getZ() + 1));}

		void setObj(int index, object obj)
		{objects.at(index) = obj;}

		void setActiveCam(int index)
		{active_camera = index;}

		int activeCam() 
		{return active_camera;}

		camera getActiveCam() {
			if (active_camera != -1)
				return cameras.at(active_camera);
		}

		void moveActiveCam(vec3 vec) {
			cameras.at(active_camera).setPos(cameras.at(active_camera).getPos()+vec);
		}

		void rotateActiveCam(vec3 vec) {
			cameras.at(3).setRot(cameras.at(3).getRot() + vec);
		}

		vector<light> getLights()
		{return lights;}

		vector<camera> getCameras()
		{return cameras;}

		vector<object> getObjects()
		{return objects;}

		vector<vec3> getPositions()
		{return positions;}

		vec3* getAxes()
		{return axes;}

		//add object 
		void addObject(object obj, vec3 pos)
		{
				obj.setName(obj.getName() + "." + to_string(object_valid(obj)));
				objects.push_back(obj);
				positions.push_back(pos);
		}
		//add object at position (0,0,0)
		void addObject(object obj)
		{
				objects.push_back(obj);
				positions.push_back(vec3(0,0,0));
		}
		//add camera 
		void addCamera(camera cam)
		{cameras.push_back(cam);}
		//add light
		void addLight(light light)
		{lights.push_back(light);}

		Scene() 
		{
			string obj = "monkee";
			for (int i = 0; i < 1; i++)
				objects.push_back(object(obj+".obj", obj+"_smooth.obj", true));
				
			//objects.push_back(object("grid.obj","grid_smooth.obj", false));
			//objects.push_back(object("triangle.obj", "triangle_smooth.obj", false));
			//objects.push_back(object("triangle.obj", "triangle_smooth.obj", false));
			//objects.push_back(object("torus.obj", "torus_smooth.obj", false));
			for(int i=1;i<=3;i++)
				cameras.push_back(camera(vec3(i * -10, i * -10, i * -10), vec3(30, -45, 0), 10));
			lights.push_back(light(vec3(0,0,-10)));
			cameras.push_back(camera(vec3(0, 0, -10), vec3(0, 0, 0), 10));
			cameras.push_back(camera(vec3(0, 0, 10), vec3(180, 0, 180), 10));
			cameras.push_back(camera(vec3(10, 0, 0), vec3(0, 90, 0), 10));
			cameras.push_back(camera(vec3(-10, 0, 0), vec3(0, -90, 0), 10));
			cameras.push_back(camera(vec3(0, -10, 0), vec3(90, 0, 0), 10));
			cameras.push_back(camera(vec3(0, 10, 0), vec3(-90, 0, 0), 10));		
		}
};