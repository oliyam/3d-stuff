#pragma once

#pragma once

/*
mesh object class
written by myself (Jan Oliver Yameogo)
... yes I wote quite a lot of comments for my standarts ... I don't wanna get conused by my own code
... lol
*/

#include <fstream>
#include <string>

#include <vector>
#include "vectors.h"

//using std::string;
using namespace std;

class object {

public:
	
	//name
	string name = "object";

	//true if culling
	bool culling;

	//vertices in the same order as in the file
	vector<vec3> vertices;
	//vertex normals in the same order as in the file
	vector<vec3> normals;
	//faces in the same order as in the file 
	//(every index contains another list for each face with indices pointing to a vertex in the vertices list)
	vector<vector<int>> faces;
	//vertex normals in the same order as in the file 
	//(every index contains another list for each face with indices pointing to a vertex normal in the normals list)
	vector<vector<int>> vertex_normals;
	//uv texture coordinates in the same order as in the file 
	//(every index contains another list for each face with indices pointing to a uv texture coordinate in the normals list)
	vector<vector<int>> uv;
	//uv coordinates
	vector<vec2> uv_texture_coordinates;
	//list that contains each face normal at the index of its corresponding face
	vector<vec3> face_normals;
	//axes (x,y,z)
	vec3 axes[3] = { vec3(1,0,0), vec3(0,1,0), vec3(0,0,1) };

	//splits up a string into multiple smaller ones that are then saved in a vector list
	vector<string> split(string s, char c)
	{
		vector<string> group;
		int last = 0;
		for (int i = 0; i < s.length(); i++)
		{
			if (s.at(i) == c)
			{
				group.push_back(s.substr(last, i - last));
				last = i + 1;
			}
			else if (i + 1 == s.length())
			{
				group.push_back(s.substr(last, i - last + 1));
			}
		}
		return group;
	}

	void rotateObj(int axis, float angle) {
		for (int i = 0; i < vertices.size(); i++)
			vertices.at(i) = vertices.at(i).rotate(axes[axis], angle);
		for (int i = 0; i < face_normals.size(); i++)
			face_normals.at(i) = face_normals.at(i).rotate(axes[axis], angle);
		for (int i = 0; i < normals.size(); i++)
			normals.at(i) = normals.at(i).rotate(axes[axis], angle);
		for (int i = 0; i < 3; i++)
			axes[i] = axes[i].rotate(axes[axis], angle);
	}
	
	void rotateObj(vec3 axis, float angle) {
		for (int i = 0; i < vertices.size(); i++)
			vertices.at(i) = vertices.at(i).rotate(axis, angle);
		for (int i = 0; i < face_normals.size(); i++)
			face_normals.at(i) = face_normals.at(i).rotate(axis, angle);
		for (int i = 0; i < normals.size(); i++)
			normals.at(i) = normals.at(i).rotate(axis, angle);
		for (int i = 0; i < 3; i++)
			axes[i] = axes[i].rotate(axis, angle);
	}
	
	/*
	reads an .obj file from the path path and a smooth object
	*/
	void readObj(string path, string smooth_path)
	{
		int numberOfNormals;
		string line;
		ifstream objectFile("assets/objects/" + path);

		//reads the file line by line
		while (getline(objectFile, line))
		{
			//name
			if (line.substr(0, 2) == ("o "))
			{
				name = split(line, ' ').at(1);
			}
			//vertices
			else if (line.substr(0, 2) == ("v "))
			{
				vector<string> list = split(line, ' ');
				vertices.push_back(
					vec3(
					stof(list.at(1)),
					stof(list.at(2)),
					stof(list.at(3))
				));
			}
			//uv texture coordinates
			else if (line.substr(0, 3) == ("vt "))
			{
				vector<string> list = split(line, ' ');
				uv_texture_coordinates.push_back( vec2( stof(list.at(1)), stof(list.at(2)) ) );
			}
			//vertex normals (somehow face normals in blender)
			else if (line.substr(0, 3) == ("vn "))
			{
				vector<string> list = split(line, ' ');
				normals.push_back(
					vec3(
					stof(list.at(1)),
					stof(list.at(2)),
					stof(list.at(3))
				));
			}
			//faces
			else if (line.substr(0, 2) == ("f "))
			{
				vector<string> list1 = split(line, ' ');
				vector<int> face_vertices;
				vector<int> face_face_normals;
				vector<int> face_uv;
				for (int i = 1; i < list1.size(); i++)
				{
					vector<string> list2 = split(list1.at(i), '/');
					face_vertices.push_back(stoi(list2.at(0)));
					face_face_normals.push_back(stoi(list2.at(2)));
					face_uv.push_back(stoi(list2.at(1)));
				}
				faces.push_back(face_vertices);
				uv.push_back(face_uv);
				face_normals.push_back(normals.at(face_face_normals.at(0) - 1));
			}
		}
		objectFile.close();
		
		numberOfNormals = normals.size();

		//reads smooth object
		ifstream smooth_objectFile("assets/objects/" + smooth_path);
		while (getline(smooth_objectFile, line))
		{
			//vertex normals (somehow face normals in blender)
			if (line.substr(0, 3) == ("vn "))
			{
				vector<string> list = split(line, ' ');
				normals.push_back(
					vec3(
					stof(list.at(1)),
					stof(list.at(2)),
					stof(list.at(3))
				));
			}
			//vertex normals
			else if (line.substr(0, 2) == ("f "))
			{
				vector<string> list1 = split(line, ' ');
				vector<int> face_vertex_normals;
				for (int i = 1; i < list1.size(); i++)
					face_vertex_normals.push_back(stoi(split(list1.at(i), '/').at(2)) + numberOfNormals);
				vertex_normals.push_back(face_vertex_normals);
			}
		}
		smooth_objectFile.close();
		
	}

	//constructors
	object(string path, string smooth_path, bool primitive_culling)
	{
		//sets culling flag
		culling = primitive_culling;
		//reading .obj file with path to smooth object
		readObj(path, smooth_path);
	}

	//getters
	string getName()
	{return name;}

	//setters
	void setName(string n)
	{name = n;}
};