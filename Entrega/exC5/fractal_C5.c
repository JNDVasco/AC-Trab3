#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_thread_num() 0
#endif

#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

//  These are the global variables.  That means they can be changed from
//  any of my functions.  This is usfull for my options function.  That
//  means that these variables are 'user' variables and can be changed
//  and the changes will be stored for the displaying part of the program

float conx = -0.74;	 // Real constant, horizontal axis (x)
float cony = 0.1;	 // Imaginary constant, verital axis (y)
float Maxx = 2;		 // Rightmost Real point of plane to be displayed
float Minx = -2;	 // Leftmost Real point
float Maxy = 1;		 // Uppermost Imaginary point
float Miny = -1;	 // Lowermost Imaginary point
float inIter = 1000; // # of times to repeat function

float pixcorx; // 1 pixel on screen = this many units on the
float pixcory; // plane for both x and y axis'

int scrsizex; // Horizontal screen size in pixels
int scrsizey; // Vertical screen size

int resx, resy, *img, difIter, *imgAux;
float alfa;

char path[10] = "./images/";
char filename[64];
char buffer[30];

int returnPixVal(int i, int j);
void difusion();
void putpixel(int x, int y, int color);
void julia(int xpt, int ypt);
void mandel(int xpt, int ypt);
void Generate(int frac);
void saveimg(int *img, int rx, int ry, char *fname);
void difusion(int select);

// =========================================================================
int main(int argc, char **argv)
{
	if (argc == 1)
	{
		resx = 3840;
		resy = 2160;
		difIter = 100;
		alfa = 0.5;
	}
	else if (argc == 5)
	{
		resx = atoi(argv[1]);
		resy = atoi(argv[2]);
		difIter = atoi(argv[3]);
		alfa = atof(argv[4]);
	}
	else
	{
		printf("Erro no número de argumentos\n");
		printf("Se não usar argumentos a imagem de saida terá dimensões 640x480\n");
		printf("Senão devera especificar o numero de colunas seguido do numero de linhas\n");
		printf("Seguido do numero de iteracoes da difusão e constante de difusao (entre 0 e 1)\n");
		printf("\nExemplo: %s 320 240 100 0.5\n", argv[0]);
		exit(1);
	}
	printf("Resolução: %d x %d\n", resx, resy);
	printf("Iterações da difusão: %d\n", difIter);
	printf("Constante de difusão: %f\n", alfa);

	img = (int *)malloc(resx * resy * sizeof(int));
	imgAux = (int *)malloc(resx * resy * sizeof(int));
	scrsizex = resx;
	scrsizey = resy;
	pixcorx = (Maxx - Minx) / scrsizex;
	pixcory = (Maxy - Miny) / scrsizey;

	Generate(0);
	printf("Mandelbrot Fractal generated.\n");
	saveimg(img, resx, resy, "mandel.pgm");
	difusion(0);

	Generate(1);
	printf("Julia Fractal generated.\n");
	saveimg(img, resx, resy, "julia.pgm");
	difusion(1);

	free(img);
	free(imgAux);
	return 0;
}

void putpixel(int x, int y, int color)
{
	img[y * resx + x] = color * 1111;
}

void julia(int xpt, int ypt)
{
	long double x = xpt * pixcorx + Minx;
	long double y = Maxy - ypt * pixcory; // converting from pixels to points
	long double xnew = 0;
	long double ynew = 0;
	int k;

	for (k = 0; k <= inIter; k++) // Each pixel loop
	{
		// The Julia Function Z=Z*Z+c (of complex numbers) into x and y parts
		xnew = x * x - y * y + conx;
		ynew = 2 * x * y + cony;
		x = xnew;
		y = ynew;
		if ((x * x + y * y) > 4)
			break; // Break condition Meaning the loop will go
				   // on to a value of infinity.
	}			   // End each pixel loop

	int color = k;
	if (color > 15)
		color = color % 15;
	if (k >= inIter)
		putpixel(xpt, ypt, 0);
	else
		putpixel(xpt, ypt, color);
}

void mandel(int xpt, int ypt)
{
	long double x = 0;
	long double y = 0; // converting from pixels to points
	long double xnew = 0;
	long double ynew = 0;
	int k;

	for (k = 0; k <= inIter; k++) // Each pixel loop
	{
		// The Mandelbrot Function Z=Z*Z+c into x and y parts
		xnew = x * x - y * y + xpt * pixcorx + Minx;
		ynew = 2 * x * y + Maxy - ypt * pixcory;
		x = xnew;
		y = ynew;
		if ((x * x + y * y) > 4)
			break; // Break condition
	}			   // End each pixel loop

	int color = k;
	if (color > 15)
		color = color % 15;
	if (k >= inIter)
		putpixel(xpt, ypt, 0);
	else
		putpixel(xpt, ypt, color);
}

void Generate(int frac)
{
	int j = 0, i = 0;
#pragma omp parallel
	{
#pragma omp for
		for (int j = 0; j < scrsizey; j++)
		{
			for (int i = 0; i < scrsizex; i++)
			{
				// Start horizontal loop
				if (frac)
				{
					julia(i, j);
				}
				else
				{
					mandel(i, j);
				}
				// End horizontal loop
			}
			// End vertical loop
		}
	}
}

void saveimg(int *img, int rx, int ry, char *fname)
{
	FILE *fp;
	int color, i, j;
	fp = fopen(fname, "w");
	/* header for PPM output */
	fprintf(fp, "P6\n# CREATOR: AC Course, DEEC-UC\n");
	fprintf(fp, "%d %d\n255\n", rx, ry);

	for (i = 0; i < ry; i++)
	{
		for (j = 0; j < rx; j++)
		{

			color = img[i * rx + j];
			//	printf(" %d %d \n",img[i*rx+j],color);
			fputc((char)(color & 0x00ff), fp);
			fputc((char)(color >> 4 & 0x00ff), fp);
			fputc((char)((color >> 6 & 0x00ff)), fp);
		}
	}
	fclose(fp);
}

int returnPixVal(int i, int j)
{
	if (i < 0 || i >= resy || j < 0 || j >= resx)
	{
		return 0;
	}
	else
	{
		return img[i * resx + j];
	}
}

void difusion(int select)
{
	int j = 0, i = 0, x = 0;
	int currentPixel = 0;
	int neighbourPixels = 0;

	for (x = 0; x < difIter; x++)
	{
#pragma omp parallel private(j, i, currentPixel, neighbourPixels)
		{
#pragma omp for
			for (i = 0; i < resy; i++)
			{
				for (j = 0; j < resx; j++)
				{
					// imgAux[i * resx + j] = 0;
					neighbourPixels = 0;
					
					currentPixel = ((1 - alfa) * returnPixVal(i, j));
					neighbourPixels += returnPixVal(i - 1, j - 1);
					neighbourPixels += returnPixVal(i - 1, j);
					neighbourPixels += returnPixVal(i - 1, j + 1);
					neighbourPixels += returnPixVal(i, j - 1);
					neighbourPixels += returnPixVal(i, j + 1);
					neighbourPixels += returnPixVal(i + 1, j - 1);
					neighbourPixels += returnPixVal(i + 1, j);
					neighbourPixels += returnPixVal(i + 1, j + 1);
					neighbourPixels *= (alfa * 0.125);
					imgAux[i * resx + j] = currentPixel + neighbourPixels;
				}
			}
		}

		strcpy(filename, path);

		if (select)
			snprintf(buffer, 26, "Julia_diff-%04d.ppm", x);
		else
			snprintf(buffer, 27, "Mandel_diff-%04d.ppm", x);

		strcat(filename, buffer);
		printf("[INFO]  %s\n", filename);
		saveimg(imgAux, resx, resy, filename);

		memcpy(img, imgAux, resx * resy * sizeof(int));
	}
}