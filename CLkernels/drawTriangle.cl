 void drawLine(uint* pixels, int size_x, int size_y, double x1, double y1, double x2, double y2) {
		float k = (y1 - y2) / (x1 - x2);
		if (abs(x1 - x2) > abs(y1 - y2))
		{
			if (x1 < x2)
			{
				double temp_x=x1,temp_y=y1;
				x1=x2;
				y1=y2;
				x2=temp_x;
				y2=temp_y;
			}
			if (x2 < 0)
				x2=0;
			for (int x = x2; x < size_x && x < x1; x++)
			{
				int y = k * (x - x1) + y1;
				if (y >= 0 && y < size_y)
					pixels[y * size_x + x] = 255*255*255*255;
			}
		}
		else
		{
			if (y1 < y2)
			{
				double temp_x=x1,temp_y=y1;
				x1=x2;
				y1=y2;
				x2=temp_x;
				y2=temp_y;
			}
			if (y2 < 0)
				y2=0;
			for (int y = y2; y < size_y && y < y1; y++)
			{
				int x = (y - y1) / k + x1
				if (x >= 0 && x < size_x)
					pixels[y * size_x + x] = 255*255*255*255;
			}
		}
}
__kernel void drawTriangle(
	__global int* frame,
	int size_x,
	int size_y,
	double* points_x,
	double* points_y
)
{
		drawLine(frame, size_x, size_y, points_x[0], points_y[0], points_x[1], points_y[1], color);
		drawLine(frame, size_x, size_y, points_x[1], points_y[1], points_x[2], points_y[2], color);
		drawLine(frame, size_x, size_y, points_x[2], points_y[2], points_x[0], points_y[0], color);
}