#pragma once

//camera object ... lol
class camera {

	private:
		vec3 position;
		vec3 rotation;
		double focal_length;
		float viewfield_x = 16;
		float viewfield_y = 9;
		string name;

	public:

		//axes (x,y,z)
		vec3 axes[3] = { vec3(1,0,0), vec3(0,1,0), vec3(0,0,1) };

		camera()
		{
			position = vec3(0,0,10);
			focal_length = 1;
		}

		camera(vec3 pos, vec3 rot, double d)
		{
			position = pos;
			rotation = rot;
			focal_length = d;
		}

		//getters
		vec3 getPos()
		{return position;}
		vec3 getRot() 
		{return rotation;}
		double getFocus()
		{return focal_length;}
		float getViewX() 
		{return viewfield_x;}
		float getViewY()
		{return viewfield_y;}

		//setters
		void setPos(vec3 pos)
		{position = pos;}
		void setRot(vec3 rot)
		{rotation = rot;}
		void setFocus(double d)
		{focal_length = d;}
};