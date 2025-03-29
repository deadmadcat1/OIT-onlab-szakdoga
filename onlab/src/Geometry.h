#pragma once
#include <stdio.h>
#include <vector>
#include <GL/glew.h>

class Geometry {
public:
	Geometry();
	unsigned int VAO;
	unsigned int EBO;
	unsigned int vtxVBO;
	unsigned int uvVBO;
	unsigned int normalVBO;
	
	virtual void draw() = 0;
	virtual int create();
	virtual ~Geometry();
};

class Sphere : protected Geometry {
	unsigned int nStrips = 16; //2^n strips need 2^n-1 rings of vtx
	unsigned int nVtxPerRing = 32;
	unsigned int nIdx = 0;
public:
	Sphere(unsigned int _nStrips, unsigned int _nVtxPerRing);
	int create() override;
	void draw() override;
};

class Plane : protected Geometry {
	unsigned int nIdx = 0;
public:
	int create() override;
	void draw() override;
};
