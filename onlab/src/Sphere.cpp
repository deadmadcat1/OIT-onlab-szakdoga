#include "Geometry.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

Sphere::Sphere(unsigned int _nStrips = 16, unsigned int _nVtxPerRing = 16) : Geometry(){
    if (_nStrips > 1) {
        nStrips = _nStrips;
    }
    if (_nVtxPerRing > 0) {
        nVtxPerRing = _nVtxPerRing;
    }
}

bool Sphere::create() {
    if (!Geometry::create()) {
        return false;
    }

    /*VERTICES*/
    std::vector<glm::vec3> vertices;
    glm::vec3 vtxProbe(0.0f, 1.0f, 0.0f);

    vertices.push_back(vtxProbe);

    float pitchAngle = ((float)M_PI / nStrips);
    glm::quat pitchQuat(glm::vec3(pitchAngle, 0.0f, 0.0f));
    glm::normalize(pitchQuat);
    float yawAngle = 2 * (float)M_PI / nVtxPerRing;
    glm::quat yawQuat(glm::vec3(0.0f, yawAngle, 0.0f));
    glm::normalize(yawQuat);

    for (unsigned int i = 1; i < nStrips; i++)
    {
        vtxProbe = glm::normalize(pitchQuat * vtxProbe);
        for (unsigned int j = 0; j < nVtxPerRing; j++) {
            
            vertices.push_back(vtxProbe);
            vtxProbe = glm::normalize(yawQuat * vtxProbe);
        }
    }

    vtxProbe = glm::vec3(0.0, -1.0, 0.0);
    vertices.push_back(vtxProbe);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    /*NORMALS*/
    // le magic of the unit sphere
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    /*UVs*/
    //unused
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

    /*INDICES*/
    std::vector<unsigned int> indices;
        //cap
    for (unsigned int i = 1; i < nVtxPerRing; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(nVtxPerRing);
    indices.push_back(1);
        //body
    for (unsigned int i = 0; i < nStrips - 2; i++)
    {
        unsigned int oldRingOffset = i * nVtxPerRing + 1;
        unsigned int newRingOffset = oldRingOffset + nVtxPerRing;
        for (unsigned int j = 0; j < nVtxPerRing; j++)
        {
            indices.push_back(oldRingOffset + j);
            indices.push_back(newRingOffset + j);
            indices.push_back(newRingOffset + (j + 1) % nVtxPerRing);
    
            indices.push_back(newRingOffset + (j + 1) % nVtxPerRing);
            indices.push_back(oldRingOffset + (j + 1) % nVtxPerRing);
            indices.push_back(oldRingOffset + j);
        }
    }
        //endcap
    unsigned int lastRingOffset = (nStrips - 2) * nVtxPerRing + 1;
    unsigned int lastVtxIdx = lastRingOffset + nVtxPerRing;
    
    for (unsigned int i = 0; i < nVtxPerRing; i++)
    {
        indices.push_back(lastRingOffset + i);
        indices.push_back(lastVtxIdx);
        indices.push_back(lastRingOffset + (i + 1) % nVtxPerRing);
    }

    nIdx = indices.size();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    /*LAYOUT*/
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3, GL_FLOAT,
        GL_FALSE,
        0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3, GL_FLOAT,
        GL_FALSE,
        0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);  //unused
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2, GL_FLOAT,
        GL_FALSE,
        0, NULL);

    glBindVertexArray(NULL);
    return true;
}

void Sphere::draw() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);

    glDrawElements(GL_TRIANGLES, nIdx, GL_UNSIGNED_INT, NULL);
}