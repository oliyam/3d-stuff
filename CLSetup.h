#pragma once
/*
//#include <CL/cl.hpp>
#include <iostream>
//#include <vector>

using std::string;
using namespace cl;
using namespace std;

Program getProgram(string path)
{
	vector<Device> devices;
	vector<Platform> platforms;
	vector<Device> devices_available;
	int n = 0; // number of available devices
	Platform::get(&platforms);
	for (int i = 0; i < (int)platforms.size(); i++) {
		devices_available.clear();
		platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices_available);
		if (devices_available.size() == 0) continue; //if no device was found jump to next platform
		for (int j = 0; j < (int)devices_available.size(); j++) {
			n++;
			devices.push_back(devices_available[j]);
		}
	}
	if (platforms.size() == 0 || devices.size() == 0) {
		cout << "Well shit ... no OpenCL devices available!" << endl;
		exit(-1);
	}
	for (int i = 0; i < n; i++)
		cout
		<< "ID: " << i
		<< ", Device: " << devices[i].getInfo<CL_DEVICE_NAME>()
		<< ", Vendor: " << devices[i].getInfo<CL_DEVICE_VENDOR>()
		<< ", Profile: " << devices[i].getInfo<CL_DEVICE_PROFILE>()
		<< ", Version: " << devices[i].getInfo<CL_DEVICE_VERSION>()
		//<< ", Extensions: " << devices[i].getInfo<CL_DEVICE_EXTENSIONS>()
		<< endl;

	Device device = devices.front();

	ifstream programfile(path);
	string src(istreambuf_iterator<char>(programfile), (istreambuf_iterator<char>()));
	
	Program::Sources sources(1, make_pair(src.c_str(), src.length() + 1));

	Context context(device);
	Program program(context, sources);

	program.build("-cl-std=CL1.2");

	return program;
}
*/