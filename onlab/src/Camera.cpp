#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace glm;

class Camera {
private:
	mat4 V, P;
public:
	vec3 pos, lookAt, viewUp;
	float fov, asp, fp, bp;

	void update() {
		vec3 w = normalize(pos - lookAt);
		vec3 u = normalize(cross(viewUp, w));
		vec3 v = cross(w, u);
		
		V = translate(V, -pos) * mat4(u.x, u.y, u.z, 0,
			v.x, v.y, v.z, 0,
			w.x, w.y, w.z, 0,
			0, 0, 0, 1);

		P = mat4(1 / (tan(fov * 0.5) * asp), 0, 0, 0,
			0, 1 / tan(fov * 0.5), 0, 0,
			0, 0, -(fp + bp) / (bp - fp), -2 * fp * bp / (bp - fp),
			0, 0, -1, 0);
	}

	mat4 V() {
		return V;
	}

	mat4 P() {
		return P;
	}
};