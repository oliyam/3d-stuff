__kernel void ProcessArray
(
	__global int* pixels,
	__global int* texture,
	int size_x,
	int size_y,
	int size_x_texture,
	int size_y_texture,
	double v0x,
	double v0y,
	double v0z,
	double v0u,
	double v0v,
	double v1x,
	double v1y,
	double v1z,
	double v1u,
	double v1v,
	double v2x,
	double v2y,
	double v2z,
	double v2u,
	double v2v,
	__global double* zBuffer,
	int offset_x,
	int offset_y
)
{
	int 
		x=get_global_id(0)+offset_x,
		y=get_global_id(1)+offset_y
	;
	
	//pixels[y * size_x + x]=255*255*255*255;
	
	double 
		e1 = (x - v0x) * (v1y - v0y) - (y - v0y) * (v1x - v0x),
		e2 = (x - v1x) * (v2y - v1y) - (y - v1y) * (v2x - v1x),
		e3 = (x - v2x) * (v0y - v2y) - (y - v2y) * (v0x - v2x)
	;
	
	if ((e1 > 1 && e2 > 1 && e3 > 1)||(e1 == 0&& e2 > 1 && e3 > 1)||(e2 == 0&& e1 > 1 && e3 > 1)||(e3 == 0&& e2 > 1 && e1 > 1))
	{
		
		double
			u_0 = v0u / v0z,
			v_0 = v0v / v0z,
			u_1 = v1u / v1z,
			v_1 = v1v / v1z,
			u_2 = v2u / v2z,
			v_2 = v2v / v2z
		;
		
		//calculating all the interpolation weights
		double d1 = ((v1y - v2y) * (v0x - v2x) + (v2x - v1x) * (v0y - v2y));
		double
		w_v1 = ((v1y - v2y) * (x - v2x) + (v2x - v1x) * (y - v2y)) / d1,
		w_v2 = ((v2y - v0y) * (x - v2x) + (v0x - v2x) * (y - v2y)) / d1,
		w_v3 = 1 - w_v1 - w_v2
		;
		//interpolate 1/z
		double w = ((1 / v0z) * w_v1 + (1 / v1z) * w_v2 + (1 / v2z) * w_v3);
		//calculation of uv coordinates
		double
		u1 = (u_0 * w_v1 + u_1 * w_v2 + u_2 * w_v3) * 1 / w,
		v1 = (v_0 * w_v1 + v_1 * w_v2 + v_2 * w_v3) * 1 / w
		;
		int
		u = abs((int)((u1 * size_x_texture) * 1) % (size_x_texture)),
		v = abs((int)((v1 * size_y_texture) * 1) % (size_y_texture))
		;
		if (zBuffer[y * size_x + x] < w)
		{
			zBuffer[y * size_x + x] = w;
			pixels[y * size_x + x] = texture[v * size_x_texture + u];
		}
	}

}