
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
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
float initer = 1000; // # of times to repeat function

float pixcorx; // 1 pixel on screen = this many units on the
float pixcory; // plane for both x and y axis'

int scrsizex; // Horizontal screen size in pixels
int scrsizey; // Vertical screen size

int resx, resy, *img;

void putpixel(int x, int y, int color)
{
	img[y * resx + x] = color * 1111;
}

double time_between_timestamp(struct timespec begin, struct timespec end);

//	***************  JULIA FUNCTION ******************
/*  First, the pixel must be converted to its actual plane value (x,y).
	Then it is passed through the function the number of times in the
	gloabal variable initer(ations).  If the break condition is reached,
	which has been mathmaticaly determined so that if the value of Z is
	larger than that, the function will continue for ever to infinity.
	Depending on the number of times the function was initerated before the
	break value was reached, the color displayed on the screen changes. If
	the break value is never reached, the point is considered to be part of
	the set. The color for that is black (0).
	For the julia set, the Z is the point under consideration.  The
	constant can be changed by the user.
*/

void julia(int xpt, int ypt)
{
	long double x = xpt * pixcorx + Minx;
	long double y = Maxy - ypt * pixcory; // converting from pixels to points
	long double xnew = 0;
	long double ynew = 0;
	int k;

	for (k = 0; k <= initer; k++) // Each pixel loop
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
	if (k >= initer)
		putpixel(xpt, ypt, 0);
	else
		putpixel(xpt, ypt, color);
}

//	*************  MANDELBROT FUNCTION ***************
/*  The mandelbrot function is the same as the Julia function except
	that the Z is 0 and the constant is the point under consideration.
*/

void mandel(int xpt, int ypt)
{
	long double x = 0;
	long double y = 0; // converting from pixels to points
	long double xnew = 0;
	long double ynew = 0;
	int k;

	for (k = 0; k <= initer; k++) // Each pixel loop
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
	if (k >= initer)
		putpixel(xpt, ypt, 0);
	else
		putpixel(xpt, ypt, color);
}

void Generate(int frac)
{
	int j = 0;
	do // Start vertical loop
	{
		int i = 0;
		do // Start horizontal loop
		{
			if (frac)
			{
				julia(i, j);
			}
			else
			{
				mandel(i, j);
			}
			i++;
		} while ((i < scrsizex)); // End horizontal loop
		j++;
	} while ((j < scrsizey)); // End vertical loop
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

int main(int argc, char **argv)
{
	struct timespec t1, t2;
	if (argc == 1)
	{
		resx = 640;
		resy = 480;
	}
	else if (argc == 3)
	{
		resx = atoi(argv[1]);
		resy = atoi(argv[2]);
	}
	else
	{
		printf("Erro no número de argumentos\n");
		printf("Se não usar argumentos a imagem de saida terá dimensões 640x480\n");
		printf("Senão devera especificar o numero de colunas seguido do numero de linhas\n");
		printf("\nExemplo: %s 320 240\n", argv[0]);
		exit(1);
	}

	img = (int *)malloc(resx * resy * sizeof(int));
	scrsizex = resx;
	scrsizey = resy;
	pixcorx = (Maxx - Minx) / scrsizex;
	pixcory = (Maxy - Miny) / scrsizey;

	clock_gettime(CLOCK_REALTIME, &t1);
	Generate(0);
	clock_gettime(CLOCK_REALTIME, &t2);
	printf("Mandelbrot Fractal generated in %6.3f secs.\n", time_between_timestamp(t1, t2));
	saveimg(img, resx, resy, "mandel.pgm");

	clock_gettime(CLOCK_REALTIME, &t1);
	Generate(1);
	clock_gettime(CLOCK_REALTIME, &t2);
	printf("Julia Fractal generated in %6.3f secs.\n", time_between_timestamp(t1, t2));
	saveimg(img, resx, resy, "julia.pgm");
}

// Returns time in seconds
double time_between_timestamp(struct timespec begin, struct timespec end)
{
	struct timespec calc;
	calc.tv_sec = end.tv_sec - begin.tv_sec;
	calc.tv_nsec = end.tv_nsec - begin.tv_nsec;
	if (calc.tv_nsec < 0)
	{
		calc.tv_sec -= 1;
		calc.tv_nsec += 1e9;
	}
	return ((calc.tv_sec) + (calc.tv_nsec) / 1e9);
}
