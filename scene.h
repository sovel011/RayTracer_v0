#include <stdio.h>
#include <stdlib.h>
#include "Pixel.h"
#include "image.h"

class Scene {
	public:
		//camera
	    float cameraX;
		float cameraY;
		float cameraZ;

		float directionX;
		float directionY;
		float directionZ;

		float upX;
		float upY;
		float upZ;

		float ha;

		//film resolution
		float width;
		float height;

		//output_image
		char* output;

		//background color
		Pixel background;

		int max_depth;

		struct directionalLight* dLights;
		int dLightCount;
		bool dLight;
		struct pointLight* pLights;
		int pLightCount;
		bool pLight;
		struct spotLight* sLights;
		int sLightCount;
		bool sLight;

		//ambient light
		Pixel ambientLight;

		//material
		float materialAmbient[3];
		float materialDiffuse[3];
		float materialSpecular[3];
		float phongCosine;
		float materialTransmissive[3];
		float refractionIndex;

		//triangles
		int max_vertices;
		int max_normals;
		int vertexCount;
		int normalCount;
		float** vertices;
		float** normals;

		float focal_length;
		bool depth_of_field;

		void  Set_Default_Scene();
		void changeDefault(char* keyword, char** settings);
};

struct directionalLight {
public:
	Pixel directionalColor;
	float dcolor[3];
	float directionalX;
	float directionalY;
	float directionalZ;
};

struct pointLight{
public:
	Pixel pointColor;
	float pcolor[3];
	float pointX;
	float pointY;
	float pointZ;
};

struct spotLight{
public:
	Pixel spotColor;
	float scolor[3];
	float spotX;
	float spotY;
	float spotZ;
	float spotdirX;
	float spotdirY;
	float spotdirZ;
	float spotAngle1;
	float spotAngle2;
};

class Sphere
{
	public:
		float x;
		float y;
		float z;
		float radius;
		float t;
	//material
		float materialAmbient[3];
		float materialDiffuse[3];
		float materialSpecular[3];
		float phongCosine;
		float materialTransmissive[3];
		float refractionIndex;

		Sphere(char** line, Scene scene);
		Sphere();
};

class Triangle {
	public:
		float v1[3];
		float v2[3];
		float v3[3];
		float n[3];
	//material
		float materialAmbient[3];
		float materialDiffuse[3];
		float materialSpecular[3];
		float phongCosine;
		float materialTransmissive[3];
		float refractionIndex;

		Triangle(char** line, Scene scene);
		Triangle();
};

class normal_triangle { //just make this and triangle 1 thing?
	public:
		float v1[3];
		float v2[3];
		float v3[3];
		float n1[3];
		float n2[3];
		float n3[3];
	//material
		float materialAmbient[3];
		float materialDiffuse[3];
		float materialSpecular[3];
		float phongCosine;
		float materialTransmissive[3];
		float refractionIndex;
};

struct Ray {

	float pos[3];
	float dir[3];
};

class Hit {
public:
	float t;
	float normal[3];
	Ray ray;
	bool isHit;
	int depth;
	//material of object hit
	float materialAmbient[3];
	float materialDiffuse[3];
	float materialSpecular[3];
	float phongCosine;
	float materialTransmissive[3];
	float refractionIndex;
};


Ray getRay(float i, float j, Scene scene);

Pixel spotLight(Hit hit, Scene scene, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, struct spotLight sLight);

Hit hitCheck(Ray ray, Sphere* spheres, int sphereCount, Triangle* triangles, int triangleCount, Scene scene, int depth);

Hit sphereHit(Ray ray, Sphere* objects, int objectCount, Scene scene, int depth);

Hit triangleHit(Ray ray, Triangle* triangles, int triangleCount, Scene scene, int depth);

Pixel point(Hit hit, Scene scene, Sphere* objects, Triangle* triangles, int triangleCount, int objectCount, pointLight pLight);

Pixel specular(float ray[], float light[], float normal[], Hit hit, float intensity[]);

bool directionalShadow(Hit hit, Scene scene, Sphere* objects, int objectCount, Triangle* triangles, int ttriangleCount, directionalLight dLight);

Pixel directional(Hit hit, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, directionalLight dLight, Scene scene);

Pixel reflection(Hit hit, Scene scene, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount);

Pixel getTransparency(Hit hit, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, Scene scene);

Pixel getColor(Hit hit, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, Scene scene);


