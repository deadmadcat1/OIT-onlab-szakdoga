#include "Geometry.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

class Sphere : protected Geometry {
public:
	int create() override {
		if (!Geometry::create()) {
			return NULL;
		}

        int nStrips = 16; //2^n strips need 2^n-1 rings of vtx
        int nVtxPerRing = 32;

        /*VERTICES*/
        std::vector<glm::vec3> vertices;
        glm::vec3 vtxProbe(0.0, 0.0, 1.0);

        vertices.push_back(vtxProbe);

        double rotAngle = 2 * M_PI / nVtxPerRing;
        glm::mat4 rot(1.0);

        for (int i = 1; i < nStrips; i++)
        {
            double pitchAngle = i * (M_PI / nStrips);
            
            rot = glm::rotate(rot, (float)pitchAngle, glm::vec3(0.0, 0.0, 1.0));
            vtxProbe *= rot;
            for (int j = 0; j < nVtxPerRing; j++) {
                vertices.push_back(vtxProbe);
                rot = glm::rotate(rot, (float)rotAngle, glm::vec3(0.0, 1.0, 0.0));
                vtxProbe *= rot;
            }
        }

        vtxProbe = glm::vec3(0.0, 0.0, -1.0);
        vertices.push_back(vtxProbe);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, nStrips * nVtxPerRing * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        /*NORMALS*/
        // le magic of the unit sphere
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, nStrips * nVtxPerRing * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        /*UVs*/
        //unused
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

        /*INDICES*/
        std::vector<unsigned int> indices;

        //TODO

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[4]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        /*LAYOUT*/
        //TODO
	}
};