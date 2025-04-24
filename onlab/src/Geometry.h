#pragma once
#include <array>
#include <GL/glew.h>

class Geometry {
public:
	Geometry();
	std::array<unsigned int, 4> buffers;	//EBO, vtx, normal, uv
	unsigned int VAO = 0;
	
	virtual void draw() = 0;
	virtual bool create();
	virtual ~Geometry();
};

class Sphere : public Geometry {
	unsigned int nStrips = 16; //2^n strips need 2^n-1 rings of vtx
	unsigned int nVtxPerRing = 32;
	unsigned int nIdx = 0;
public:
	Sphere(unsigned int _nStrips, unsigned int _nVtxPerRing);
	bool create() override;
	void draw() override;
};

class Plane : public Geometry {
	unsigned int nIdx = 0;
public:
	Plane();
	bool create() override;
	void draw() override;
};
