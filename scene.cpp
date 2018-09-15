#include <stdio.h>
#include <stdlib.h>
#include "Pixel.h"
#include "image.h"
#include "scene.h"
#include <numeric>
#pragma GCC diagnostic ignored "-Wwrite-strings"

using namespace std;
bool pointCheck = false;

void Scene::Set_Default_Scene(){
	//camera
	cameraX = 0;
	cameraY = 0;
	cameraZ = 0;

	directionX = 0;
	directionY = 0;
	directionZ = 1;

	upX = 0;
	upY = 1;
	upZ = 0;

	ha = 45;
	//film resolution
	width = 640;
	height = 480;

	//output_image
	output = "raytraced.bmp";

	//background color
	background = Pixel(0,0,0);

	max_depth = 5;

	//ambient light
	ambientLight = Pixel(0,0,0);

	dLights = (struct directionalLight*)malloc(5*sizeof(struct directionalLight));
	dLightCount = 0;
	dLight = false;
	pLights = (struct pointLight*)malloc(15*sizeof(struct pointLight));
	pLightCount = 0;
	pLight = false;
	sLights = (struct spotLight*)malloc(10*sizeof(struct spotLight));;
	sLightCount = 0;
	sLight = false;

	//material
	materialAmbient[0] = 0;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0;

	materialDiffuse[0] = 1;
	materialDiffuse[1] = 1;
	materialDiffuse[2] = 1;

	materialSpecular[0] = 0;
	materialSpecular[1] = 0;
	materialSpecular[2] = 0;
	phongCosine = 5;

	materialTransmissive[0] = 0;
	materialTransmissive[1] = 0;
	materialTransmissive[2] = 0;
	refractionIndex = 1;

	vertexCount = 0;
	normalCount = 0;

	vertices = (float**)malloc(35000*sizeof(float*));
	for (int i = 0; i < 35000; i++)
	{
		vertices[i] = (float*)malloc(3*sizeof(float));
	}

	normals = (float**)malloc(35000*sizeof(float*));
	for (int i = 0; i < 35000; i++)
	{
		normals[i] = (float*)malloc(3*sizeof(float));
	}

	focal_length = -1;
	depth_of_field = false;
	}

void Scene::changeDefault(char* keyword, char** settings)
{
	if (strcmp(keyword, "output_image") == 0)
	{
		output = (char*)malloc(40);
		strcpy(output, settings[1]);
	}
	else if (strcmp(keyword, "camera") == 0)
	{
		cameraX = atof(settings[1]);
		cameraY = atof(settings[2]);
		cameraZ = atof(settings[3]);
		directionX = atof(settings[4]);
		directionY = atof(settings[5]);
		directionZ = atof(settings[6]);
		upX = atof(settings[7]);
		upY = atof(settings[8]);
		upZ = atof(settings[9]);
		ha = atof(settings[10]);
	}
	else if (strcmp(keyword, "film_resolution") == 0)
	{
		width = atoi(settings[1]);
		height = atoi(settings[2]);
	}
	else if (strcmp(keyword, "background") == 0)
	{
		float red = 255*atof(settings[1]);
		float green = 255*atof(settings[2]);
		float blue = 255*atof(settings[3]);
		background.Set(red,green,blue);
	}
	else if (strcmp(keyword, "max_depth") == 0)
	{
		max_depth = atoi(settings[1]);
	}
	else if (strcmp(keyword, "ambient_light") == 0)
	{
		float red = 255*atof(settings[1]);
		float green = 255*atof(settings[2]);
		float blue = 255*atof(settings[3]);
		ambientLight.Set(red,green,blue);
	}
	else if (strcmp(keyword, "directional_light") == 0)
	{
		struct directionalLight light;
		float red = 255*atof(settings[1]);
		float green = 255*atof(settings[2]);
		float blue = 255*atof(settings[3]);
		light.dcolor[0] = atof(settings[1]);
		light.dcolor[1] = atof(settings[2]);
		light.dcolor[2] = atof(settings[3]);

		light.directionalColor.Set(red,green,blue);
		light.directionalX = atof(settings[4]);
		light.directionalY = -atof(settings[5]);
		light.directionalZ = atof(settings[6]);
		dLights[dLightCount] = light;
		dLightCount++;
		dLight = true;
	}
	else if (strcmp(keyword, "point_light") == 0)
	{
		struct pointLight light;
		light.pcolor[0] = atof(settings[1]);
		light.pcolor[1] = atof(settings[2]);
		light.pcolor[2] = atof(settings[3]);
		light.pointX = atof(settings[4]);
		light.pointY = atof(settings[5]);
		light.pointZ = atof(settings[6]);
		pLights[pLightCount] = light;
		pLightCount++;
		pLight = true;
	}
	else if (strcmp(keyword, "spot_light") == 0)
	{
		struct spotLight light;
		float red = 255*atof(settings[1]);
		float green = 255*atof(settings[2]);
		float blue = 255*atof(settings[3]);
		light.scolor[0] = atof(settings[1]);
		light.scolor[1] = atof(settings[2]);
		light.scolor[2] = atof(settings[3]);
		light.spotColor.Set(red,green,blue);
		light.spotX = atof(settings[4]);
		light.spotY = atof(settings[5]);
		light.spotZ = atof(settings[6]);
		light.spotdirX = atof(settings[7]);
		light.spotdirY = atof(settings[8]);
		light.spotdirZ = atof(settings[9]);
		light.spotAngle1 = atof(settings[10]);
		light.spotAngle2 = atof(settings[11]);
		sLights[sLightCount] = light;
		sLightCount++;
	}
	else if (strcmp(keyword, "material") == 0)
	{
		materialAmbient[0] = atof(settings[1]);
		materialAmbient[1] = atof(settings[2]);
		materialAmbient[2] = atof(settings[3]);

		materialDiffuse[0] = atof(settings[4]);
		materialDiffuse[1] = atof(settings[5]);
		materialDiffuse[2] = atof(settings[6]);

		materialSpecular[0] = atof(settings[7]);
		materialSpecular[1] = atof(settings[8]);
		materialSpecular[2] = atof(settings[9]);

		phongCosine = atof(settings[10]);
		materialTransmissive[0] = atof(settings[11]);
		materialTransmissive[1] = atof(settings[12]);
		materialTransmissive[2] = atof(settings[13]);

		refractionIndex = atof(settings[14]);
	}
	else if (strcmp(keyword, "max_vertices") == 0)
	{
		max_vertices = atof(settings[1]);
	}
	else if (strcmp(keyword, "max_normals") == 0)
	{
		max_normals = atof(settings[1]);
	}
	else if (strcmp(keyword, "vertex") == 0)
	{
		vertices[vertexCount][0] = atof(settings[1]);
		vertices[vertexCount][1] = atof(settings[2]);
		vertices[vertexCount][2] = atof(settings[3]);
		vertexCount++;
	}
	else if (strcmp(keyword, "normal") == 0)
	{
		normals[normalCount][0] = atof(settings[1]);
		normals[normalCount][1] = atof(settings[2]);
		normals[normalCount][2] = atof(settings[3]);
		normalCount++;
	}
	else if (strcmp(keyword, "depth_of_field") == 0)
	{
		focal_length = atof(settings[1]);
		depth_of_field = true;
	}
	else printf("INVALID KEYWORD %s\n", settings[0]);

}

Sphere::Sphere(char** line, Scene scene)
{
	this->x = atof(line[1]);
	this->y = atof(line[2]);
	this->z = atof(line[3]);
	radius = atof(line[4]);
	t = -1;
	materialAmbient[0] = scene.materialAmbient[0];
	materialAmbient[1] = scene.materialAmbient[1];
	materialAmbient[2] = scene.materialAmbient[2];
	materialDiffuse[0] = scene.materialDiffuse[0];
	materialDiffuse[1] = scene.materialDiffuse[1];
	materialDiffuse[2] = scene.materialDiffuse[2];
	materialSpecular[0] = scene.materialSpecular[0];
	materialSpecular[1] = scene.materialSpecular[1];
	materialSpecular[2] = scene.materialSpecular[2];
	phongCosine = scene.phongCosine;
	materialTransmissive[0] = scene.materialTransmissive[0];
	materialTransmissive[1] = scene.materialTransmissive[1];
	materialTransmissive[2] = scene.materialTransmissive[2];
	refractionIndex = scene.refractionIndex;
}

Sphere::Sphere()
{

}

Triangle::Triangle(char** line, Scene scene)
{
	int index1 = atoi(line[1]);
	int index2 = atoi(line[2]);
	int index3 = atoi(line[3]);

	this->v1[0] = scene.vertices[index1][0];
	this->v1[1] = scene.vertices[index1][1];
	this->v1[2] = scene.vertices[index1][2];

	this->v2[0] = scene.vertices[index2][0];
	this->v2[1] = scene.vertices[index2][1];
	this->v2[2] = scene.vertices[index2][2];

	this->v3[0] = scene.vertices[index3][0];
	this->v3[1] = scene.vertices[index3][1];
	this->v3[2] = scene.vertices[index3][2];

	materialAmbient[0] = scene.materialAmbient[0];
	materialAmbient[1] = scene.materialAmbient[1];
	materialAmbient[2] = scene.materialAmbient[2];
	materialDiffuse[0] = scene.materialDiffuse[0];
	materialDiffuse[1] = scene.materialDiffuse[1];
	materialDiffuse[2] = scene.materialDiffuse[2];
	materialSpecular[0] = scene.materialSpecular[0];
	materialSpecular[1] = scene.materialSpecular[1];
	materialSpecular[2] = scene.materialSpecular[2];
	phongCosine = scene.phongCosine;
	materialTransmissive[0] = scene.materialTransmissive[0];
	materialTransmissive[1] = scene.materialTransmissive[1];
	materialTransmissive[2] = scene.materialTransmissive[2];
	refractionIndex = scene.refractionIndex;

	//calculating normal
	float a[3], b[3];
	a[0] = v1[0] - v2[0], a[1] = v1[1] - v2[1], a[2] = v1[2] - v2[2];
	b[0] = v3[0] - v2[0], b[1] = v3[1] - v2[1], b[2] = v3[2] - v2[2];

	n[0] = a[1]*b[2] - a[2]*b[1];
	n[1] = a[2]*b[0] - a[0]*b[2];
	n[2] = a[0]*b[1] - a[1]*b[0];

	float normalize = sqrt(inner_product(n, n+3, n, 0.0));
	n[0] = n[0]/normalize;
	n[1] = n[1]/normalize;
	n[2] = n[2]/normalize;

	float n2[3] = {-n[0], -n[1], -n[2]};
	float dir[3] = {-scene.directionX, -scene.directionY, -scene.directionZ};
	float normalizeDir = sqrt(inner_product(dir, dir+3, dir, 0.0));
	dir[0] = dir[0]/normalizeDir, dir[1] = dir[1]/normalizeDir, dir[2] = dir[2]/normalizeDir;
	float angle1 = acos(inner_product(n, n+3, dir, 0.0)), angle2 = acos(inner_product(n2, n2+3, dir, 0.0));
	if (angle2 < angle1)
	{
		n[0] = n2[0], n[1] = n2[1], n[2] = n2[2];
	}
}

Triangle::Triangle()
{

}

Ray getRay(float i, float j, Scene scene)
{
	Ray ray;
	float distance;
	if (scene.depth_of_field)
	{
		float N = .5, n = N/2;
		float rand_x = N*rand()/(float)RAND_MAX;
		float rand_y = N*rand()/(float)RAND_MAX;
		ray.pos[0] = (scene.cameraX - n) + rand_x;
		ray.pos[1] = (scene.cameraY - n) + rand_y;
		ray.pos[2] = scene.cameraZ;
		distance = scene.focal_length;
	}
	else
	{
		ray.pos[0] = scene.cameraX;
		ray.pos[1] = scene.cameraY;
		ray.pos[2] = scene.cameraZ;
		distance = scene.height/(2*tan((scene.ha*M_PI)/180));
	}
	float offset[] = {ray.pos[0] - scene.cameraX, ray.pos[1] - scene.cameraY, ray.pos[2] - scene.cameraZ};
	float right[3];
	right[0] = scene.upY*scene.directionZ - scene.upZ*scene.directionY;
	right[1] = scene.upZ*scene.directionX - scene.upX*scene.directionZ;
	right[2] = scene.upX*scene.directionY - scene.upY*scene.directionX;

	float t = scene.cameraY  + scene.height/2;
	float b = scene.cameraY - scene.height/2;
	float ratio = scene.width / scene.height;
	float unitHeight = t - b;
	float unitWidth = unitHeight*ratio;
	float widthAngle = atan((unitWidth/2)/distance);
	widthAngle = (widthAngle*180)/M_PI;
	float l = scene.cameraX  - scene.width/2;
	float r = scene.cameraX  + scene.width/2;
	float u = (l + (((r - l)*(i))/scene.width));
	float v = (b  + (((t - b)*(j))/scene.height));

	ray.dir[0] = (u * right[0] + distance*scene.directionX + v * scene.upX);
	ray.dir[1] = (-v * scene.upY + -u * right[1] + distance*scene.directionY);
	ray.dir[2] = (distance*scene.directionZ + u * right[2] + v * scene.upZ);

	float normalizeDir = inner_product(ray.dir, ray.dir+3, ray.dir, 0.0);
	ray.dir[0] = ray.dir[0]/normalizeDir;
	ray.dir[1] = ray.dir[1]/normalizeDir;
	ray.dir[2] = ray.dir[2]/normalizeDir;

	if (scene.depth_of_field)
	{
		u = u + scene.focal_length*ray.dir[0] ;
		v = v + scene.focal_length*ray.dir[1];
		//distance =  distance + scene.focal_length;
		ray.dir[0] = u - ray.pos[0];
		ray.dir[1] = v - ray.pos[1];
		ray.dir[2] = distance - ray.pos[2];
		float normalizeDir = inner_product(ray.dir, ray.dir+3, ray.dir, 0.0);
		ray.dir[0] = ray.dir[0]/normalizeDir;
		ray.dir[1] = ray.dir[1]/normalizeDir;
		ray.dir[2] = ray.dir[2]/normalizeDir;
	}
	return ray;
}

Hit hitCheck(Ray ray, Sphere* spheres, int sphereCount, Triangle* triangles, int triangleCount, Scene scene, int depth)
{
	Hit hit1 = sphereHit(ray, spheres, sphereCount, scene, depth);
	Hit hit2 = triangleHit(ray, triangles, triangleCount, scene, depth);
	if (hit1.t < hit2.t)
	{
		return hit1;
	}
	else return hit2;
}

Hit sphereHit(Ray ray, Sphere* objects, int objectCount, Scene scene, int depth)
{
	Hit hit;
	float t;
	float t2;
	hit.t = 1000000;
	hit.ray = ray;
	hit.isHit = false;
	hit.depth = depth;
	for (int i = 0; i < objectCount; i++)
	{
		Sphere sphere = objects[i];
		float vector[] = {ray.pos[0] - sphere.x, ray.pos[1] - sphere.y, ray.pos[2] - sphere.z};
		float discrim1 = inner_product(ray.dir, ray.dir+3, vector, 0.0);
		discrim1 = pow(discrim1,2);
		float discrim2 = inner_product(ray.dir, ray.dir+3, ray.dir, 0.0);
		discrim2 = discrim2*(inner_product(vector,vector+3,vector,0.0) - pow(sphere.radius,2));
		discrim1 = discrim1 - discrim2;
		if (discrim1 >= 0)
		{
			discrim1 = sqrt(discrim1);
			float negD[] = {-ray.dir[0], -ray.dir[1], -ray.dir[2]};
			float B = inner_product(negD, negD+3, vector, 0.0);
			t = (B + discrim1)/(inner_product(negD,negD+3,negD,0.0));
			if (t < 0) {t = 1000000;}
			t2 = (B - discrim1)/(inner_product(negD,negD+3,negD,0.0));
			if (t2 < 0) {t2 = 1000000;}
			t = min(t,t2);
			sphere.t = t;
			if (sphere.t < hit.t)
			{
				hit.isHit = true;
				hit.t = sphere.t;
				float point[] = {ray.pos[0] + ray.dir[0]*t, ray.pos[1] + ray.dir[1]*t, ray.pos[2] + ray.dir[2]*t };
				hit.normal[0] = (point[0]-sphere.x)/sphere.radius;
				hit.normal[1] = (point[1]-sphere.y)/sphere.radius;
				hit.normal[2] = (point[2]-sphere.z)/sphere.radius;
				hit.materialAmbient[0] = sphere.materialAmbient[0];
				hit.materialAmbient[1] = sphere.materialAmbient[1];
				hit.materialAmbient[2] = sphere.materialAmbient[2];
				hit.materialDiffuse[0] = sphere.materialDiffuse[0];
				hit.materialDiffuse[1] = sphere.materialDiffuse[1];
				hit.materialDiffuse[2] = sphere.materialDiffuse[2];
				hit.materialSpecular[0] = sphere.materialSpecular[0];
				hit.materialSpecular[1] = sphere.materialSpecular[1];
				hit.materialSpecular[2] = sphere.materialSpecular[2];
				hit.phongCosine = sphere.phongCosine;
				hit.materialTransmissive[0] = sphere.materialTransmissive[0];
				hit.materialTransmissive[1] = sphere.materialTransmissive[1];
				hit.materialTransmissive[2] = sphere.materialTransmissive[2];
				hit.refractionIndex = sphere.refractionIndex;
			}
		}
	}
	return hit;
}

Hit triangleHit(Ray ray, Triangle* triangles, int triangleCount, Scene scene, int depth)
{
	Hit hit;
	hit.t = 1000000;
	hit.isHit = false;
	hit.ray = ray;
	hit.depth = depth;
	float a[3], b[3], c[3];
	for (int i = 0; i < triangleCount; i++)
	{
		a[0] = triangles[i].v1[0], a[1] = triangles[i].v1[1], a[2] = triangles[i].v1[2];
		b[0] = triangles[i].v2[0], b[1] = triangles[i].v2[1], b[2] = triangles[i].v2[2];
		c[0] = triangles[i].v3[0], c[1] = triangles[i].v3[1], c[2] = triangles[i].v3[2];

		float ei_minus_hf = ((a[1] - c[1])*(ray.dir[2])) - ((ray.dir[1])*(a[2] - c[2]));
		float gf_minus_di = ((ray.dir[0])*(a[2] - c[2])) - ((a[0] - c[0])*(ray.dir[2]));
		float dh_minus_eg = ((a[0] - c[0])*(ray.dir[1])) - ((a[1] - c[1])*(ray.dir[0]));
		float M = (a[0] - b[0])*ei_minus_hf + (a[1] - b[1])*(gf_minus_di) + (a[2] - b[2])*(dh_minus_eg);

		float beta = (a[0] - ray.pos[0])*(ei_minus_hf) + (a[1] - ray.pos[1])*(gf_minus_di) + (a[2] - ray.pos[2])*(dh_minus_eg);
		beta = beta/M;

		float ak_minus_jb = ((a[0] - b[0])*(a[1] - ray.pos[1])) - ((a[0] - ray.pos[0])*(a[1] - b[1]));
		float jc_minus_al = ((a[0] - ray.pos[0])*(a[2] - b[2])) - ((a[0] - b[0])*(a[2] - ray.pos[2]));
		float bl_minus_kc = ((a[1] - b[1])*(a[2] - ray.pos[2])) - ((a[1] - ray.pos[1])*(a[2] - b[2]));

		float gamma = (ray.dir[2])*(ak_minus_jb) + (ray.dir[1])*(jc_minus_al) + (ray.dir[0])*(bl_minus_kc);
		gamma = gamma/M;
		float t = (a[2] - c[2])*(ak_minus_jb) + (a[1] - c[1])*(jc_minus_al) + (a[0] - c[0])*(bl_minus_kc);
		t = -t/M;

		if ((beta >= 0) && (gamma >= 0) && (beta + gamma <= 1))
		{
			if (t < hit.t && t > 0)
			{
				hit.isHit = true;
				hit.t = t;
				hit.normal[0] = triangles[i].n[0];
				hit.normal[1] = triangles[i].n[1];
				hit.normal[2] = triangles[i].n[2];
				hit.materialAmbient[0] = triangles[i].materialAmbient[0];
				hit.materialAmbient[1] = triangles[i].materialAmbient[1];
				hit.materialAmbient[2] = triangles[i].materialAmbient[2];
				hit.materialDiffuse[0] = triangles[i].materialDiffuse[0];
				hit.materialDiffuse[1] = triangles[i].materialDiffuse[1];
				hit.materialDiffuse[2] = triangles[i].materialDiffuse[2];
				hit.materialSpecular[0] = triangles[i].materialSpecular[0];
				hit.materialSpecular[1] = triangles[i].materialSpecular[1];
				hit.materialSpecular[2] = triangles[i].materialSpecular[2];
				hit.phongCosine = triangles[i].phongCosine;
				hit.materialTransmissive[0] = triangles[i].materialTransmissive[0];
				hit.materialTransmissive[1] = triangles[i].materialTransmissive[1];
				hit.materialTransmissive[2] = triangles[i].materialTransmissive[2];
				hit.refractionIndex = triangles[i].refractionIndex;
			}
		}
	}

	return hit;
}

Pixel specular(float ray[], float light[], float normal[], Hit hit, float intensity[])
{

	Pixel s;
	float red = hit.materialSpecular[0] * intensity[0]*255;
	float green = hit.materialSpecular[1] * intensity[1]*255;
	float blue = hit.materialSpecular[2] * intensity[2]*255;

	float h[] = {ray[0] + light[0], ray[1] + light[1], ray[2] + light[2]};
	float normalize = sqrt(h[0]*h[0] + h[1]*h[1] + h[2]*h[2]);
	h[0] = h[0]/normalize;
	h[1] = h[1]/normalize;
	h[2] = h[2]/normalize;
	if (false){
		printf("%f %f %f\n", h[0], h[1], h[2]);
		printf("%f %f %f\n\n", normal[0], normal[1], normal[2]), pointCheck = false;}
	float spec = inner_product(h,h+3,normal,0.0);
	spec = fmax(0.0, spec);
	if (hit.phongCosine > 1)
	{
		spec = pow(spec, 3*hit.phongCosine);
	}
	else spec = 0;

	red = spec * red;
	green = spec * green;
	blue = spec * blue;
	s.SetClamp(red,green,blue);
	return s;
}

Pixel spotLight(Hit hit, Scene scene, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, struct spotLight sLight) //STILL DOESNT WORK
{
	Pixel p;
	Ray ray = hit.ray;
	float t = hit.t;

	float red = 255 * sLight.scolor[0] * hit.materialDiffuse[0];
	float green = 255 * sLight.scolor[1] * hit.materialDiffuse[1];
	float blue = 255 * sLight.scolor[2] * hit.materialDiffuse[2];

	float point[] = {ray.pos[0] + ray.dir[0]*t, ray.pos[1] + ray.dir[1]*t, ray.pos[2] + ray.dir[2]*t};
	float vector[] = {sLight.spotX - point[0], sLight.spotY - point[1], sLight.spotZ - point[2]};
	float normal[] = {hit.normal[0], hit.normal[1], hit.normal[2]};
	float distance = inner_product(vector, vector+3, vector, 0.0);
	float normalizeVector = sqrt(inner_product(vector, vector+3, vector, 0.0));
	vector[0] = vector[0]/normalizeVector;
	vector[1] = vector[1]/normalizeVector;
	vector[2] = vector[2]/normalizeVector;

	float dir[] = {-sLight.spotdirX, -sLight.spotdirY, -sLight.spotdirZ};
	float normalizeDir = sqrt(inner_product(dir, dir+3, dir, 0.0));
	dir[0] = dir[0]/normalizeDir;
	dir[1] = dir[1]/normalizeDir;
	dir[2] = dir[2]/normalizeDir;

	float angle = acos(inner_product(dir, dir+3, vector, 0.0));
	angle = (angle*180)/M_PI;
	if (angle < sLight.spotAngle1)
	{
		float scale = fmax(0.0, inner_product(normal, normal+3, vector, 0.0));
		red = (red*scale)/distance;
		green = (green*scale)/distance;
		blue = (blue*scale)/distance;
		p.SetClamp(red,green,blue);
	}
	else if (angle < sLight.spotAngle2 && angle > sLight.spotAngle1)
	{
		float falloff = (angle - sLight.spotAngle1)/(sLight.spotAngle2 - sLight.spotAngle1);
		float scale = falloff*fmax(0.0, inner_product(normal, normal+3, vector, 0.0));
		red = (red*scale)/distance;
		green = (green*scale)/distance;
		blue = (blue*scale)/distance;
		p.SetClamp(red,green,blue);
	}
	else
	{
		p.Set(0,0,0);
	}




	struct Ray newRay;
	newRay.pos[0] = point[0] + .001*normal[0];
	newRay.pos[1] = point[1] + .001*normal[1];
	newRay.pos[2] = point[2] + .001*normal[2];
	newRay.dir[0] = vector[0];
	newRay.dir[1] = vector[1];
	newRay.dir[2] = vector[2];
	Hit newHit = hitCheck(newRay, objects, objectCount, triangles, triangleCount, scene, hit.depth);
	float intersect = newHit.t;
//	if (intersect == 1000000 || intersect < 0)
//	{
//		float negPoint[] = {ray.pos[0] - point[0], ray.pos[1] - point[1], ray.pos[2] - point[2]};
//		float normalizePoint = sqrt(inner_product(negPoint, negPoint+3, negPoint, 0.0));
//		negPoint[0] = negPoint[0]/normalizePoint;
//		negPoint[1] = negPoint[1]/normalizePoint;
//		negPoint[2] = negPoint[2]/normalizePoint;
//		float normal[] = {newHit.normal[0], newHit.normal[1], newHit.normal[2]};
//		Pixel spec = specular(negPoint, vector, normal, newHit, sLight.scolor);
//		return p + (spec* (1/distance));
//	}
//	else
//		{
//		return Pixel(0,0,0);
//		}
	return p;
}

Pixel point(Hit hit, Scene scene, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, struct pointLight pLight)
{
	Pixel p;
	Ray ray = hit.ray;
	float t = hit.t;

	float red = 255*pLight.pcolor[0]* hit.materialDiffuse[0];
	float green = 255*pLight.pcolor[1] * hit.materialDiffuse[1];
	float blue = 255*pLight.pcolor[2] * hit.materialDiffuse[2];
	p.Set(red, green, blue);

	float point[] = {ray.pos[0] + ray.dir[0]*t, ray.pos[1] + ray.dir[1]*t, ray.pos[2] + ray.dir[2]*t };
	float vector[] = {pLight.pointX - point[0], pLight.pointY - point[1], pLight.pointZ - point[2]};
	float normal[] = {hit.normal[0], hit.normal[1], hit.normal[2]};
	float distance = inner_product(vector, vector+3, vector, 0.0);

	float normalizeVector = sqrt(inner_product(vector, vector+3, vector, 0.0));
	vector[0] = vector[0]/normalizeVector;
	vector[1] = vector[1]/normalizeVector;
	vector[2] = vector[2]/normalizeVector;

	float scale = fmax(0, inner_product(normal, normal+3, vector, 0.0));
	red = (red*scale)/distance;
	green = (green*scale)/distance;
	blue = (blue*scale)/distance;
	p.SetClamp(red,green,blue);

	struct Ray newRay;
	newRay.pos[0] = point[0] + .001*normal[0];
	newRay.pos[1] = point[1] + .001*normal[1];
	newRay.pos[2] = point[2] + .001*normal[2];
	newRay.dir[0] = vector[0];
	newRay.dir[1] = vector[1];
	newRay.dir[2] = vector[2];
	Hit newHit = hitCheck(newRay,objects, objectCount, triangles, triangleCount, scene, hit.depth);
	float intersect = newHit.t;

	if (intersect == 1000000 || intersect < 0 || sqrt(distance) < intersect)
	{
		float negPoint[] = {ray.pos[0] - point[0], ray.pos[1] - point[1], ray.pos[2] - point[2]};
		float normalizePoint = sqrt(inner_product(negPoint, negPoint+3, negPoint, 0.0));
		negPoint[0] = negPoint[0]/normalizePoint;
		negPoint[1] = negPoint[1]/normalizePoint;
		negPoint[2] = negPoint[2]/normalizePoint;
		float normal[] = {hit.normal[0], hit.normal[1], hit.normal[2]};
		pointCheck = true;
		Pixel spec = specular(negPoint, vector, normal, hit, pLight.pcolor);
		return p + (spec* (1/distance));
	}
	else
		{
			return Pixel(0,0,0);
		}
}

bool directionalShadow(Hit hit, Scene scene, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, directionalLight dLight)
{
	if (!scene.dLight)
	{
		return false;
	}
	//Sphere sphere = hit.object;
	Ray ray = hit.ray;

	bool shadow = false;
	float t = hit.t;
	float dir[] = {-dLight.directionalX, dLight.directionalY, -dLight.directionalZ};
	float point[] = {ray.pos[0] + ray.dir[0]*t, ray.pos[1] + ray.dir[1]*t, ray.pos[2] + ray.dir[2]*t};
	struct Ray newRay;
	newRay.pos[0] = point[0] + .0001*hit.normal[0];
	newRay.pos[1] = point[1] + .0001*hit.normal[1];
	newRay.pos[2] = point[2] + .0001*hit.normal[2];
	newRay.dir[0] = dir[0];
	newRay.dir[1] = dir[1];
	newRay.dir[2] = dir[2];
	float intersect = hitCheck(newRay, objects, objectCount, triangles, triangleCount, scene, hit.depth).t;
	if ((intersect != 1000000) && (intersect > 0))
	{
		shadow = true;
	}
	return shadow;
}

Pixel directional(Hit hit, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, directionalLight dLight, Scene scene)
{
	float t = hit.t;
	Ray ray = hit.ray;

	Pixel p = dLight.directionalColor;
	float red = p.r * hit.materialDiffuse[0];
	float green = p.g * hit.materialDiffuse[1];
	float blue = p.b * hit.materialDiffuse[2];
	p.Set(red,green,blue);
	float point[] = {ray.pos[0] + ray.dir[0]*t, ray.pos[1] + ray.dir[1]*t, ray.pos[2] + ray.dir[2]*t };
	float normal[] = {hit.normal[0], hit.normal[1], hit.normal[2]};
	float light[] = {-dLight.directionalX, dLight.directionalY, -dLight.directionalZ};
	float normalizeLight = sqrt(inner_product(light, light+3, light, 0.0));
	light[0] = light[0]/normalizeLight;
	light[1] = light[1]/normalizeLight;
	light[2] = light[2]/normalizeLight;

	if (directionalShadow(hit, scene, objects, objectCount, triangles, triangleCount, dLight))
	{
		return Pixel(0,0,0);
	}

	float scale = fmax(0, inner_product(normal, normal+3,light, 0.0));
	p = p*scale;

	//float negPoint[] = {-ray.pos[0] - ray.dir[0]*t, -ray.pos[1] - ray.dir[1]*t, -ray.pos[2] - ray.dir[2]*t };
	float negPoint[] = {ray.pos[0] - point[0], ray.pos[1] - point[1], ray.pos[2] - point[2]};
	float normalizePoint = sqrt(inner_product(negPoint, negPoint+3, negPoint, 0.0));
	negPoint[0] = negPoint[0]/normalizePoint;
	negPoint[1] = negPoint[1]/normalizePoint;
	negPoint[2] = negPoint[2]/normalizePoint;
	Pixel spec = specular(negPoint, light, normal, hit, dLight.dcolor);
	return p + spec;
}

Pixel reflection(Hit hit, Scene scene, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount)
{
	if (hit.depth >= scene.max_depth)
	{
		return Pixel(0,0,0);
	}
	int newDepth = hit.depth + 1;
	Pixel p;
	Ray ray = hit.ray;
	float t = hit.t;

	float point[] = {ray.pos[0] + ray.dir[0]*t, ray.pos[1] + ray.dir[1]*t, ray.pos[2] + ray.dir[2]*t };
	float normal[] = {hit.normal[0], hit.normal[1], hit.normal[2]};
	float normalizeDir = sqrt(inner_product(ray.dir, ray.dir+3, ray.dir, 0.0));
	float d[3];
	d[0] = ray.dir[0]/normalizeDir;
	d[1] = ray.dir[1]/normalizeDir;
	d[2] = ray.dir[2]/normalizeDir;
	float dDotN = 2*(inner_product(d, d+3, normal, 0.0));
	float reflectionVector[3];
	reflectionVector[0] = d[0] - normal[0]*dDotN;
	reflectionVector[1] = d[1] - normal[1]*dDotN;
	reflectionVector[2] = d[2] - normal[2]*dDotN;
	Ray reflectionRay;
	reflectionRay.pos[0] = point[0] + .001*normal[0];
	reflectionRay.pos[1] = point[1] + .001*normal[1];
	reflectionRay.pos[2] = point[2] + .001*normal[2];

	reflectionRay.dir[0] = reflectionVector[0];
	reflectionRay.dir[1]= reflectionVector[1];
	reflectionRay.dir[2] = reflectionVector[2];
	Hit reflection = hitCheck(reflectionRay, objects, objectCount, triangles, triangleCount, scene, newDepth);
	if (reflection.isHit)
	{
		Pixel color = getColor(reflection, objects, objectCount, triangles, triangleCount, scene);
		float red = hit.materialSpecular[0]*color.r;
		float green = hit.materialSpecular[1]*color.g;
		float blue = hit.materialSpecular[2]*color.b;
		p.SetClamp(red,blue,green);
		return color;
	}
	else
	{
		Pixel ret = scene.background;
		ret.r = ret.r*hit.materialSpecular[0];
		ret.g = ret.g*hit.materialSpecular[1];
		ret.b = ret.b*hit.materialSpecular[2];
		return ret;
	}
}

Ray refraction(Hit hit, float incomingIndex, float outgoingIndex)
{
	float dir[] = {hit.ray.dir[0], hit.ray.dir[1], hit.ray.dir[2]};
	float normalizeDir = inner_product(dir, dir+3, dir, 0.0);
	dir[0] = dir[0]/normalizeDir, dir[1] = dir[1]/normalizeDir, dir[2] = dir[2]/normalizeDir;
	float refraction[3];
	float dDotN = inner_product(dir, dir+3, hit.normal, 0.0);
	refraction[0] = (incomingIndex*(dir[0] - (hit.normal[0]*(dDotN))))/outgoingIndex;
	refraction[1] = (incomingIndex*(dir[1] - (hit.normal[1]*(dDotN))))/outgoingIndex;
	refraction[2] = (incomingIndex*(dir[2] - (hit.normal[2]*(dDotN))))/outgoingIndex;

	float refraction2 = (1 - ((pow(incomingIndex, 2.0)*(1 - pow(dDotN, 2.0))) / pow(outgoingIndex, 2.0)));
	if (refraction2 >= 0)
	{
		refraction2 = sqrt(refraction2);
	}
	else
	{
		refraction2 = 0;
//		Ray nullRay;
//		nullRay.pos[0] = 1000000;
//		return nullRay;
	}
	refraction[0] = refraction[0] - (hit.normal[0]*refraction2);
	refraction[1] = refraction[1] - (hit.normal[1]*refraction2);
	refraction[2] = refraction[2] - (hit.normal[2]*refraction2);
//	float normalize = inner_product(refraction, refraction+3, refraction, 0.0);
//	refraction[0] = refraction[0]/normalize;
//	refraction[1] = refraction[1]/normalize;
//	refraction[2] = refraction[2]/normalize;
	Ray ray;
	ray.pos[0] = hit.ray.pos[0], ray.pos[1] = hit.ray.pos[1], ray.pos[2] = hit.ray.pos[2];
	ray.dir[0] = refraction[0], ray.dir[1] = refraction[1], ray.dir[2] = refraction[2];
	return ray;
}

Pixel getTransparency(Hit hit, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, Scene scene)
{
	if (hit.depth >= scene.max_depth)
	{
		return Pixel(0,0,0);
	}
	int newDepth = hit.depth +1; //
	Pixel p;
	Ray ray = hit.ray;
	float t = hit.t;
	float point[] = {ray.pos[0] + ray.dir[0]*t, ray.pos[1] + ray.dir[1]*t, ray.pos[2] + ray.dir[2]*t};
	float normal[] = {hit.normal[0], hit.normal[1], hit.normal[2]};
	Ray newRay;
	Ray refract = refraction(hit, 1, hit.refractionIndex);

	newRay.pos[0] = point[0] - .01*normal[0];
	newRay.pos[1] = point[1] - .01*normal[1];
	newRay.pos[2] = point[2] - .01*normal[2];
	newRay.dir[0] = refract.dir[0];
	newRay.dir[1] = refract.dir[1];
	newRay.dir[2] = refract.dir[2];

	Hit throughObject = hitCheck(newRay, objects, objectCount, triangles, triangleCount, scene, newDepth); //
	if (throughObject.isHit)
	{

		t = throughObject.t;
		point[0] = newRay.pos[0] + newRay.dir[0]*t;
		point[1] = newRay.pos[1] + newRay.dir[1]*t;
		point[2] = newRay.pos[2] + newRay.dir[2]*t;

		if (hit.refractionIndex != throughObject.refractionIndex)
		{
			Pixel c = getColor(throughObject, objects, objectCount, triangles, triangleCount, scene);
			float r = c.r*hit.materialTransmissive[0];
			float g = c.g*hit.materialTransmissive[1];
			float b = c.b*hit.materialTransmissive[2];
			c.SetClamp(r,g,b);
			return c;
		}

		normal[0] = throughObject.normal[0], normal[1] = throughObject.normal[1], normal[2] = throughObject.normal[2];
		Ray newnewRay;
		newnewRay.pos[0] = point[0] + .01*normal[0];
		newnewRay.pos[1] = point[1] + .01*normal[1];
		newnewRay.pos[2] = point[2] + .01*normal[2];
		Ray refract2 = refraction(throughObject, throughObject.refractionIndex, 1);

		newnewRay.dir[0] = refract2.dir[0];
		newnewRay.dir[1] = refract2.dir[1];
		newnewRay.dir[2] = refract2.dir[2];
		Hit afterObject = hitCheck(newnewRay, objects, objectCount, triangles, triangleCount, scene, newDepth); //
		if (afterObject.isHit)
		{

			Pixel color = getColor(afterObject, objects, objectCount, triangles, triangleCount, scene);
			float red = color.r*hit.materialTransmissive[0];
			float green = color.g*hit.materialTransmissive[1];
			float blue = color.b*hit.materialTransmissive[2];
			p.Set(red,green,blue);
			return p;
		}
		else
			{
				Pixel ret = scene.background;
				float red = ret.r*hit.materialTransmissive[0];
				float green = ret.g*hit.materialTransmissive[1];
				float blue = ret.b*hit.materialTransmissive[2];
				ret.SetClamp(red,green,blue);
				return ret;
			}
	}
	else
		{
			return Pixel(0,0,0);
		}
}

Pixel getColor(Hit hit, Sphere* objects, int objectCount, Triangle* triangles, int triangleCount, Scene scene)
{
	Pixel p;
	Pixel ambient = scene.ambientLight;
	float red = ambient.r*hit.materialAmbient[0];
	float green = ambient.g*hit.materialAmbient[1];
	float blue = ambient.b*hit.materialAmbient[2];
	ambient.Set(red,green,blue);

	Pixel directionalP = Pixel(0,0,0);
	for (int i = 0; i < scene.dLightCount; i++)
	{
		Pixel currDLight = directional(hit, objects, objectCount, triangles, triangleCount, scene.dLights[i], scene);
		directionalP = directionalP + currDLight;
	}

	Pixel pointP = Pixel(0,0,0);
	for (int i = 0; i < scene.pLightCount; i++)
	{
		Pixel currPLight = point(hit, scene, objects, objectCount, triangles, triangleCount, scene.pLights[i]);
		pointP = pointP + currPLight;
	}

	Pixel spotP = Pixel(0,0,0);
	for (int i = 0; i < scene.sLightCount; i++)
	{
		Pixel currSLight = spotLight(hit, scene, objects, objectCount, triangles, triangleCount, scene.sLights[i]);
		spotP = spotP + currSLight;
	}

	Pixel transparency = getTransparency(hit, objects, objectCount, triangles, triangleCount, scene);

	Pixel reflectionP = reflection(hit, scene, objects, objectCount, triangles, triangleCount);
	reflectionP.r =  reflectionP.r*hit.materialSpecular[0];
	reflectionP.g =  reflectionP.g*hit.materialSpecular[1];
	reflectionP.b =  reflectionP.b*hit.materialSpecular[2];

	p = directionalP + ambient + pointP + spotP  + reflectionP + transparency;
	return p;
}


/**
 * STEPS TO FINISHING PART 2:
 * FIX spotlight!
 *	~~~ FIX highlights
 * 		~~~implement reflections
 * 		~~~Implement transparency
 * 		~~~Refraction
 * 		~~~Add triangle elements to parser
 * 		~~~triangle hit detection
 * 		~~~triangle lighting
 * do additional features
 *  	~~~jittered supersampling
 *  	~~~parallel execution
 * 	-depth of field
 * 	-ambient occlusion
 * 	-etc
 *
 */

