#pragma once

class light {

	public:
		vec3 point;

		light(vec3 vec) {
			point = vec;
		}
};