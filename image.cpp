#include "image.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

/**
 * Image
 **/
Image::Image (int width_, int height_)
{
	bmpImg = new BMP();

    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;

	if (!bmpImg->SetSize(width, height)){
		printf("Error allocating image.");
		exit(-1);
	}
	
	pixels = new Pixel[num_pixels];
	int c = 0;
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			pixels[c].r = bmpImg->GetPixel(i,j).Red;
			pixels[c].g = bmpImg->GetPixel(i,j).Green;
			pixels[c].b = bmpImg->GetPixel(i,j).Blue;
			pixels[c].a = bmpImg->GetPixel(i,j).Alpha;
			c++;
		}
	}

    assert(pixels != NULL);
}


Image::Image (const Image& src)
{
    bmpImg = new BMP();
	
	width           = src.width;
    height          = src.height;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;

	RangedPixelToPixelCopy( *src.bmpImg, 0, width-1,
                         height-1 , 0, 
                         *bmpImg, 0,0 );

	if (!bmpImg->SetSize(width, height)){
		printf("Error allocating image.");
		exit(-1);
	}
	
	pixels = new Pixel[num_pixels];
	int c = 0;
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			pixels[c].r = bmpImg->GetPixel(i,j).Red;
			pixels[c].g = bmpImg->GetPixel(i,j).Green;
			pixels[c].b = bmpImg->GetPixel(i,j).Blue;
			pixels[c].a = bmpImg->GetPixel(i,j).Alpha;
			c++;
		}
	}

	assert(pixels != NULL);
}

Image::Image (char* fname){
	bmpImg = new BMP();

	if (!bmpImg->ReadFromFile(fname)){
		printf("Error loading image: %s", fname);
		exit(-1);
	}

	width = bmpImg->TellWidth();
	height = bmpImg->TellHeight();
	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;

	pixels = new Pixel[num_pixels];
	int c = 0;
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			pixels[c].r = bmpImg->GetPixel(i,j).Red;
			pixels[c].g = bmpImg->GetPixel(i,j).Green;
			pixels[c].b = bmpImg->GetPixel(i,j).Blue;
			pixels[c].a = bmpImg->GetPixel(i,j).Alpha;
			c++;
		}
	}
}

Image::~Image (){
	delete bmpImg;
    pixels = NULL;
}

void Image::Write(char* fname){
	int c = 0;
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			RGBApixel p;
			p.Red = pixels[c].r;
			p.Green = pixels[c].g;
			p.Blue = pixels[c].b;
			p.Alpha = pixels[c].a;
			bmpImg->SetPixel(i,j,p);
			c++;
		}
	}
	bmpImg->SetBitDepth(24);
	bmpImg->WriteToFile(fname);
}

void Image::AddNoise (double factor)
{
	int x,y;
	if (factor > 1 || factor < 0)
	{
		printf("factor out of range\n");
		return;
	}
	for (x = 0; x < Width(); x++)
	{
		for (y=0; y < Height(); y++)
		{
			Pixel p = GetPixel(x,y);
			Pixel random_p = p*(1-factor) + PixelRandom()*factor;
			GetPixel(x,y) = random_p;
		}
	}
}

void Image::Brighten (double factor)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel scaled_p = p*factor;
			GetPixel(x,y) = scaled_p;
		}
	}
}


void Image::ChangeContrast (double factor)
{
	int x,y;
	int count = 0;
	double sum = 0.0;
	for (x = 0 ; x < Width() ; x++)
			{
				for (y = 0 ; y < Height() ; y++)
				{
					count++;
					Pixel a = GetPixel(x,y);
					Component l = a.Luminance();
					sum += l;
				}
			}
	double avg_Lum = sum/count;
	Pixel g;
	g.Set(avg_Lum, avg_Lum,avg_Lum);

	for (x = 0 ; x < Width() ; x++)
		{
			for (y = 0 ; y < Height() ; y++)
			{
				Pixel p = GetPixel(x,y);
				Pixel q = PixelLerp(p,g,1-factor);
				GetPixel(x,y) = q;
			}
		}
}


void Image::ChangeSaturation(double factor)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++)
			{
				for (y = 0 ; y < Height() ; y++)
				{
					Pixel p = GetPixel(x,y);
					Component l = p.Luminance();
					Pixel g;
					g.Set(l,l,l);
					Pixel q = PixelLerp(p,g,1-factor);
					GetPixel(x,y) = q;
				}
			}
}


Image* Image::Crop(int x, int y, int w, int h)
{
	int i,j;
	int max_x = x + w/2;
	int min_x = x - w/2;
	int max_y = y + h/2;
	int min_y = y - h/2;
	Image* image = new Image(w,h);
	if (max_x > Width() || min_x < 0 || max_y > Height() || min_y < 0)
	{
		printf("out of image bounds\n");
		return NULL;
	}
	for (i = 0 ; i < Width() ; i++)
	{
		for (j = 0 ; j < Height() ; j++)
		{
			if (((i < max_x)&&(i >= min_x))&&(((j < max_y)&&(j >= min_y))))
			{
				Pixel src = GetPixel(i,j);
				image->GetPixel(i-min_x,j-min_y) = src;
			}
		}
	}

	return image;
}


void Image::ExtractChannel(int channel)
{
	int x,y;
		for (x = 0 ; x < Width() ; x++)
				{
					for (y = 0 ; y < Height() ; y++)
					{
						Pixel p = GetPixel(x,y);
						if (channel == 0)
						{
							p.b = 0;
							p.g = 0;
						}
						else if (channel == 1)
						{
							p.r = 0;
							p.b = 0;
						}
						else
						{
							p.r = 0;
							p.g = 0;
						}
						GetPixel(x,y) = p;
					}
				}
}


void Image::Quantize (int nbits)
{
	int x,y;
		for (x = 0 ; x < Width() ; x++)
				{
					for (y = 0 ; y < Height() ; y++)
					{
						Pixel p = GetPixel(x,y);
						Pixel new_p = PixelQuant(p,nbits);
						GetPixel(x,y) = new_p;
					}
				}
}

void Image::RandomDither (int nbits)
{
	int shift = 8-nbits;
	float mult = 255/float(255 >> shift);
	int new_r, new_g, new_b,x,y;
	double factor = shift/25.0;
		for (x = 0 ; x < Width() ; x++)
				{
					for (y = 0 ; y < Height() ; y++)
					{
						Pixel p = GetPixel(x,y);
						Pixel new_p;
						new_r = ((p.r + (Component)(ComponentRandom()*(factor))) >> shift);
						new_g = ((p.g + (Component)(ComponentRandom()*(factor))) >> shift);
						new_b = ((p.b + (Component)(ComponentRandom()*(factor))) >> shift);
						new_p.SetClamp(new_r*mult, new_g*mult, new_b*mult);
						GetPixel(x,y) = new_p;
					}
				}
}


static int Bayer4[4][4] =
{
    {15,  7, 13,  5},
    { 3, 11,  1,  9},
    {12,  4, 14,  6},
    { 0,  8,  2, 10}
};


void Image::OrderedDither(int nbits) //IS THIS RIGHT?
{
	int x,y;
	int shift = 8-nbits;
	float mult = 255/float(255 >> shift);
	for (y = 0 ; y < Height() ; y++)
	{
		for (x = 0 ; x < Width() ; x++)
		{
			int i = x%4, j = y%4;
			Pixel p = GetPixel(x,y);
			double e_r = GetPixel(x,y).r - PixelQuant(p,nbits).r;
			double e_g = GetPixel(x,y).g - PixelQuant(p,nbits).g;
			double e_b = GetPixel(x,y).b - PixelQuant(p,nbits).b;
			if (e_r > Bayer4[i][j])
			{
				p.r = ((GetPixel(x,y).r >> shift)+1)*mult;
			}
			else
			{
				p.r = PixelQuant(p,nbits).r;
			}
			if (e_g > Bayer4[i][j])
			{
				p.g = ((GetPixel(x,y).g >> shift)+1)*mult;
			}
			else
			{
				p.g = PixelQuant(p,nbits).g;
			}
			if (e_b > Bayer4[i][j])
			{
				p.b = ((GetPixel(x,y).b >> shift)+1)*mult;
			}
			else
			{
				p.b = PixelQuant(p,nbits).b;
			}
			GetPixel(x,y) = p;
			}
		 }
}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;


void Image::FloydSteinbergDither(int nbits)
{
	int x,y;
	for (y = 1 ; y < Height()-1 ; y++)
	{
		for (x = 1 ; x < Width()-1 ; x++)
		{
			Pixel p = GetPixel(x,y);
			Pixel new_p = PixelQuant(p,nbits);
			GetPixel(x,y) = new_p;
			double err_r, err_g, err_b;
			err_r = p.r - new_p.r;
			err_g = p.g - new_p.g;
			err_b = p.b - new_p.b;
			Pixel right = GetPixel(x+1,y);
			right.SetClamp(right.r+err_r*ALPHA,right.g+err_g*ALPHA,right.b+err_b*ALPHA);
			GetPixel(x+1,y) = right;
			Pixel left = GetPixel(x-1,y+1);
			left.SetClamp(left.r+err_r*BETA,left.g+err_g*BETA,left.b+err_b*BETA);
			GetPixel(x-1,y+1) = left;
			Pixel bottom = GetPixel(x,y+1);
			bottom.Set(bottom.r+err_r*GAMMA,bottom.g+err_g*GAMMA,bottom.b+err_b*GAMMA);
			GetPixel(x,y+1) = bottom;
			Pixel corner = GetPixel(x+1,y+1);
			corner.Set(corner.r+err_r*DELTA,corner.g+err_g*DELTA,corner.b+err_b*DELTA);
			GetPixel(x+1,y+1) = corner;
			}
		}
}

double Gaussian(int n, double s)
{
	double sigma =  s;
	//printf("%f\n", sigma);
	double x = exp((-1*pow(n,2))/(2*pow(sigma,2)));
	x = (1/(sqrt(2*M_PI*(pow(sigma,2)))))*x;
	return x;
}

void Image::Blur(int n)
{
int x,y;
double sum_r, sum_g, sum_b, sum_guass;
Image x_blur = Image(Width(), Height());
Image y_blur = Image(Width(), Height());
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			sum_r = 0, sum_g = 0, sum_b = 0, sum_guass = 0;
			for (int i = 0; i < n; i++)
			{
				double red, green, blue;
				int i_x = x - (n/2 - i);
				double g = Gaussian(n/2 - i,n+1/5.0);
				sum_guass += g;
				Pixel p;
				if (ValidCoord(i_x,y) == 0)
				{
					p.Set(0,0,0);
				}
				else p = GetPixel(i_x,y);
				red = p.r, green = p.g, blue = p.b;
				sum_r += red*g;
				sum_g += green*g;
				sum_b += blue*g;
			}
			Pixel new_p;
			//printf("%f\n", sum_guass);
			new_p.SetClamp(sum_r/sum_guass,sum_g/sum_guass,sum_b/sum_guass);
			x_blur.GetPixel(x,y) = new_p;
		}
	}
	for (x = 0 ; x < Width() ; x++)
		{
			for (y = 0 ; y < Height() ; y++)
			{
				sum_r = 0, sum_g = 0, sum_b = 0, sum_guass = 0;
				for (int j = 0; j < n; j++)
				{
					double red, green, blue;
					int i_y = y - (n/2 - j);
					double g = Gaussian(n/2 - j,n+1/5.0);
					sum_guass += g;
					Pixel p;
					if (ValidCoord(x,i_y) == 0)
					{
						p.Set(0,0,0);
					}
					else p = x_blur.GetPixel(x,i_y);
					red = p.r, green = p.g, blue = p.b;
					sum_r += red*g;
					sum_g += green*g;
					sum_b += blue*g;
				}
				Pixel new_p;
				new_p.SetClamp(sum_r/sum_guass,sum_g/sum_guass,sum_b/sum_guass);
				y_blur.GetPixel(x,y) = new_p;
			}
		}
	for (x = 0 ; x < Width() ; x++)
		{
			for (y = 0 ; y < Height() ; y++)
			{
				GetPixel(x,y) = y_blur.GetPixel(x,y);
			}
		}
}

void Image::Sharpen(int n)
{
	Image blurry = Image(Width(),Height());
	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			blurry.GetPixel(x,y) = GetPixel(x,y);
		}
	}
	blurry.Blur(5);
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p;
			if (ValidCoord(x,y) == 0)
			{
				p.Set(0,0,0);
			}
			else p = GetPixel(x,y);
			Pixel q = blurry.GetPixel(x,y);
			p = PixelLerp(p,q,-1*n);
			GetPixel(x,y) = p;
		}
	}

}

static double EdgeMatrix[3][3] =
{
	{-1, -1, -1},
	{-1,  8, -1},
	{-1, -1, -1}
};

void Image::EdgeDetect()
{
int x,y,i,j;
double sum_r,sum_b,sum_g;
Image new_image = Image(Width(), Height());
	for (x = 1 ; x < Width()-1 ; x++)
	{
		for (y = 1 ; y < Height()-1 ; y++)
		{
		sum_r = 0, sum_b = 0, sum_g = 0;
			for (i = 0; i < 3; i++)
			{
				for (j = 0; j < 3; j++)
				{
					int i_x = x-(1-i);
					int i_y = y-(1-j);
					Pixel p = GetPixel(i_x,i_y);

					double m = EdgeMatrix[i][j];
					double red = p.r, green = p.g, blue = p.b;
					sum_r += red*m;
					sum_g += green*m;
					sum_b += blue*m;
				}
			}
		Pixel new_p;
		new_p.SetClamp(sum_r,sum_g,sum_b);
		new_image.GetPixel(x,y) = new_p;
		}
	}
	for (x = 0 ; x < Width() ; x++)
		{
			for (y = 0 ; y < Height() ; y++)
			{
				GetPixel(x,y) = new_image.GetPixel(x,y);
			}
		}
}


Image* Image::Scale(double sx, double sy)
{
	int new_width = Width()*sx, new_height = Height()*sy;
	int x,y;
	Image* image = new Image(new_width,new_height);
	for (x = 0; x < new_width; x++)
	{
		for (y = 0; y < new_height; y++)
		{
			double old_x = x/sx, old_y = y/sy;
			Pixel sample = Sample(old_x,old_y);
			image->GetPixel(x,y) = sample;
		}
	}
	return image;
}

Image* Image::Rotate(double angle)
{
	int maxsize = sqrt(Width()*Width() + Height()*Height());
	Image* image = new Image(maxsize,maxsize);
	int x,y;

	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			if (image->ValidCoord (x + (maxsize/2 - Width()/2), y + (maxsize/2 - Height()/2)) == 1)
			{
				image->GetPixel(x + (maxsize/2 - Width()/2), y + (maxsize/2 - Height()/2)) = GetPixel(x,y);
			}
		}
	}
	Image* ret = new Image(maxsize,maxsize);
	for (x = 0; x < maxsize; x++)
		{
			for (y = 0; y < maxsize; y++)
			{
				double rot_x,rot_y;
				int i_x = x - (maxsize/2);
				int i_y = y - (maxsize/2);
				rot_x = i_x*cos(angle) - i_y*sin(angle) + maxsize/2;
				rot_y = i_x*sin(angle) + i_y*cos(angle) + maxsize/2;
				Pixel sample = image->Sample(rot_x, rot_y);
				ret->GetPixel(x,y) = sample;
			}
		}
	return ret;
}



Image* Image::Fun(double a)
{

	int maxsize = sqrt(Width()*Width() + Height()*Height());
		Image* image = new Image(maxsize,maxsize);
		int x,y;

		for (x = 0; x < Width(); x++)
		{
			for (y = 0; y < Height(); y++)
			{
				if (image->ValidCoord (x + (maxsize/2 - Width()/2), y + (maxsize/2 - Height()/2)) == 1)
				{
					image->GetPixel(x + (maxsize/2 - Width()/2), y + (maxsize/2 - Height()/2)) = GetPixel(x,y);
				}
			}
		}
		Image* ret = new Image(maxsize,maxsize);
		for (x = 0; x < maxsize; x++)
			{
				for (y = 0; y < maxsize; y++)
				{
					double rot_x,rot_y, angle;
					double area = .8*Height();
					double distance = pow((x-(Width()/2)),2) + pow((y-(Height()/2)),2);
					angle = a*exp(-1*distance/(area*area));
					int i_x = x - (maxsize/2);
					int i_y = y - (maxsize/2);
					rot_x = i_x*cos(angle) - i_y*sin(angle) + maxsize/2;
					rot_y = i_x*sin(angle) + i_y*cos(angle) + maxsize/2;
					Pixel sample = image->Sample(rot_x, rot_y);
					ret->GetPixel(x,y) = sample;
				}
			}
		return ret;
}

void Image::Nonphotorealism(int factor)
{
	int x,y;
	int count = 1;
	for (y = 0; y < Height(); y++)
		{
			for (x = factor; x < Width(); x++)
			{
				if (count%factor == 0)
				{
				count = 1;
				}
				else
				{
					GetPixel(x,y) = GetPixel(x - (count),y);
					count++;
				}
			}
		}
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method)
{
    assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
    sampling_method = method;
}


Pixel Image::Sample (double u, double v) //DO BINLINEAR
{
	Pixel p;
	int i,j;
	int new_u = trunc(u+.5);
	int new_v = trunc(v+.5);
	double sum_r = 0, sum_g = 0, sum_b = 0, sum_guass = 0;

	if (sampling_method == IMAGE_SAMPLING_POINT)
	{
		if (ValidCoord(new_u,new_v) == 0)
		{
			p.Set(255,255,255);
		}
		else p = GetPixel(new_u,new_v);
		return p;
	}
	else if (sampling_method == IMAGE_SAMPLING_GAUSSIAN)
	{
		Pixel p_x;
		for (i = 0; i < 3; i++)
		{
				double red, green, blue;
				int i_u = new_u - (1 - i);
				double g = Gaussian(1 - i,.75);
				sum_guass += g;
				if (ValidCoord(i_u,new_v) == 0)
				{
					p.Set(255,255,255);
				}
				else p = GetPixel(i_u,new_v);
				red = p.r, green = p.g, blue = p.b;
				sum_r += red*g;
				sum_g += green*g;
				sum_b += blue*g;
		}
		p_x.SetClamp(sum_r/sum_guass,sum_g/sum_guass,sum_b/sum_guass);
		sum_r = 0, sum_g = 0, sum_b = 0, sum_guass = 0;

		for (j = 0; j < 3; j++)
		{
				double red, green, blue;
				int i_v = new_v - (1 - j);
				double g = Gaussian(1 - j,.75);
				sum_guass += g;
				if (ValidCoord(new_u,i_v) == 0)
				{
					p_x.Set(255,255,255);
				}
				else p_x = GetPixel(new_u,i_v);
				red = p_x.r, green = p_x.g, blue = p_x.b;
				sum_r += red*g;
				sum_g += green*g;
				sum_b += blue*g;
		}
		p.SetClamp(sum_r/sum_guass,sum_g/sum_guass,sum_b/sum_guass);
		return p;
	}
	else if (sampling_method == IMAGE_SAMPLING_BILINEAR)
		{
			int Left = floor(u), Right = ceil(u);
			int Top = floor(v), Bottom = ceil(v);
			Pixel p, pTopLeft,pTopRight,pBottomLeft,pBottomRight;

			if (ValidCoord(Left,Top) == 1){
				 pTopLeft = GetPixel(Left,Top);
			}
			else {
				pTopLeft.Set(255,255,255);
			}
			if (ValidCoord(Left,Bottom) == 1){
				 pBottomLeft = GetPixel(Left,Bottom);
			}
			else {
				pBottomLeft.Set(255,255,255);
			}
			if (ValidCoord(Right,Top) == 1){
				 pTopRight = GetPixel(Right,Top);
			}
			else {
				pTopRight.Set(255,255,255);
			}
			if (ValidCoord(Right,Bottom) == 1){
				 pBottomRight = GetPixel(Right,Bottom);
			}
			else {
				pBottomRight.Set(255,255,255);
			}

			Pixel pTop = PixelLerp(pTopLeft,pTopRight,.5);
			Pixel pBottom = PixelLerp(pBottomLeft,pBottomRight,.5);
			p = PixelLerp(pTop,pBottom,.5);
			return p;
		}
	else return p;

}
