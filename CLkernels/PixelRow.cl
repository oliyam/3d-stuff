__kernel void ColorPixels(
	__global int* data,
	__global double* points,
	__global int* color
	){
	//coordinates of points
	double
		x_v1 = points[0],
		y_v1 = points[1],
		x_v2 = points[2],
		y_v2 = points[3],
		x_v3 = points[4],
		y_v3 = points[5]
	;
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
	data[y * size_x + x] = (a & 0xFF000000) | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
 }