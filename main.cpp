#include "image.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include "EasyBMP.h"
#include "scene.h"
//#include <omp.h>

using namespace std;

Sphere* objects = (Sphere*)malloc(25*sizeof(Sphere));
int objectCount;

Triangle* triangles = (Triangle*)malloc(72000*sizeof(Triangle));
int triangleCount;

char** parseLine(FILE* file, char** lineInfo)
{
	char currentChar = fgetc(file);
	int i = 0, j = 0;

	if (currentChar == '\n' || currentChar == '\r')
	{
		//line is only whitespace so there is no information
		if (currentChar == '\r')
		{
			currentChar = fgetc(file);
		}
		return NULL;
	}
	if (currentChar == '#')
	{
		while (currentChar != '\n')
		{
			currentChar = fgetc(file);
		}
		return NULL;
	}

	while (currentChar != '\n' && currentChar != EOF && currentChar != '\r')
	{

	    while (currentChar != ' ' && currentChar != EOF && currentChar != '\r')
		{
			if (currentChar == '\n')
			{
				ungetc(currentChar, file);
				break;
			}
			lineInfo[i][j] = currentChar;
			currentChar = fgetc(file);
			j++;
			continue;
		}
		currentChar = fgetc(file);
		lineInfo[i][j] = '\0';
		while (currentChar == ' ')
		{
			currentChar = fgetc(file);
		}
		i++;
		j = 0;
	}
	char nextChar = fgetc(file);
	ungetc(nextChar, file);
	if (nextChar == '\n' || nextChar == '\r')
	{
		currentChar = fgetc(file);
	}
	return lineInfo;
}

int getLineCount(FILE* file)
{
	int count = 0;
	char ch = fgetc(file);
	while (ch != EOF)
	{
		if (ch == '\n')
		{
			count++;
		}
		ch = fgetc(file);
	}
	rewind(file);
	return count+1;
}

Scene buildScene(FILE* file, int fileSize, Scene scene)
{
	char** currentLine;
	int i;
	objectCount = 0;
	triangleCount = 0;

	char** lineInfo;
	lineInfo = (char**)malloc(20*sizeof(char*));
	for (i = 0; i < 20; i++)
		lineInfo[i] = (char*)malloc(26);

	for (i = 0; i < fileSize+1; i++)
	{
		currentLine = parseLine(file, lineInfo);
		if (currentLine != NULL)
		{
			char* keyword = currentLine[0];
			if (strcmp(keyword, "sphere") == 0)
			{
				Sphere sphere = Sphere(currentLine, scene);
				objects[objectCount] = sphere;
				objectCount++;
			}
			else if (strcmp(keyword, "triangle") == 0)
			{
				Triangle triangle = Triangle(currentLine, scene);
				triangles[triangleCount] = triangle;
				triangleCount++;
			}
			else scene.changeDefault(keyword, currentLine);
		}
	}
	return scene;
}

int main( int argc, char* argv[] ){

	if (argc != 2)
	{
		printf("Usage: %s <.scn file>\n", *argv);
		return -1;
	}

	//CREATING SCENE
	Scene scene;
	printf("Setting default scene\n");
	scene.Set_Default_Scene();

	//OPENING FILE
	FILE* fp;
	fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		printf("Error opening file\n");
		return -1;
	}
	printf("Opened file: %s\n", argv[1]);

	//BUILDING SCENE FROM FILE
	printf("Building scene from scn file\n");
	int fileSize = getLineCount(fp);
	scene = buildScene(fp, fileSize, scene);

	//CREATING NEW IMAGE
	printf("Making new image\n");
	Image *img = new Image(scene.width, scene.height);

	// LOOP THROUGH EACH PIXEL AND FIND COLOR

	float N = 1.0, n = sqrt(N);
	#pragma omp parallel for
	for (int i = 0; i < img->width; i++)
	{
		for	(int j = 0; j < img->height; j++)
		{
			Pixel p, c, t;
			float red = 0, green = 0, blue = 0;
			for (int p = 0; p < n; p++)
			{
				for (int q = 0; q < n; q++)
				{
					float r = rand()/(float)(RAND_MAX);
					struct Ray ray = getRay(i + (p+r)/N, j + (q+r)/N, scene);
					Hit hit = hitCheck(ray, objects, objectCount, triangles, triangleCount, scene, 0);
					if (hit.isHit)
					{
						c = getColor(hit, objects, objectCount, triangles, triangleCount, scene);
						t = Pixel(0,0,0); //getTransparency(hit, objects, objectCount, triangles, triangleCount, scene);
						red = red + c.r + t.r, green = green + c.g + t.g, blue = blue + c.b + t.b;
					}
					else
					{
						red = red + scene.background.r, green = green + scene.background.g, blue = blue + scene.background.b;
					}
				}
			}
			p.SetClamp(red/N, green/N, blue/N);
			img->GetPixel(i,j) = p;
		}
	}

	// WRITE IMAGE TO SPECIFIED FILE
	printf("Writing file to %s\n", scene.output);
	img->Write(scene.output);
	return 0;
}
