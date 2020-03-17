__kernel void ProcessArray
(
	__global int* pixels,
	__global int* texture,
	int size_x,
	int size_y,
	int size_x_texture,
	int size_y_texture,
	__global double* zBuffer,
	__global double* points,
	__global double* uvs
)
{
	int triangle=get_global_id(0);
	
	double 
		v0x=points[triangle*9+0],
		v0y=points[triangle*9+1],
		v0z=points[triangle*9+2],
		v0u=uvs[triangle*6+0],
		v0v=uvs[triangle*6+1],
		v1x=points[triangle*9+3],
		v1y=points[triangle*9+4],
		v1z=points[triangle*9+5],
		v1u=uvs[triangle*6+2],
		v1v=uvs[triangle*6+3],
		v2x=points[triangle*9+6],
		v2y=points[triangle*9+7],
		v2z=points[triangle*9+8],
		v2u=uvs[triangle*6+4],
		v2v=uvs[triangle*6+5]
	;
	
	double max_x=v0x;
	if(max_x<v1x)
		max_x=v1x;
	if(max_x<v2x)
		max_x=v2x;		
	double max_y=v0y;
	if(max_y<v1y)
		max_y=v1y;
	if(max_y<v2y)
		max_y=v2y;
	
	double min_x=v0x;
	if(min_x>v1x)
		min_x=v1x;
	if(min_x>v2x)
		min_x=v2x;		
	double min_y=v0y;
	if(min_y>v1y)
		min_y=v1y;
	if(min_y>v2y)
		min_y=v2y;

	int 
		x=get_global_id(1),
		y=get_global_id(2)
	;
	
	if(x>min_x&&x<max_x&&y>min_y&&y<max_y)
	{	

	pixels[y * size_x + x]=255*255*255*255;
	
	double 
		e1 = (x - v0x) * (v1y - v0y) - (y - v0y) * (v1x - v0x),
		e2 = (x - v1x) * (v2y - v1y) - (y - v1y) * (v2x - v1x),
		e3 = (x - v2x) * (v0y - v2y) - (y - v2y) * (v0x - v2x)
	;
	
	if ((e1 >= 1 && e2 >= 1 && e3 >= 1)||(e1 == 0&& e2 >= 1 && e3 >= 1)||(e2 == 0&& e1 >= 1 && e3 >= 1)||(e3 == 0&& e2 >= 1 && e1 >= 1))
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
}
