#include <highgui.h>

int X[20][2] = {	28,131,
					7,58,
					154,201,
					-44,118,
					-81,21,
					152,316,
					220,242,
					91,194,
					65,193,
					-26,82,
					136,217,
					141,145,
					122,99,
					246,219,
					68,79,
					251,322,
					60,244,
					64,13,
					85,58,
					66,51 };

int size_x = 350;
int size_y = 350;

inline double sq2(double x, double y)//square function
{
	return x * x + y * y;
}

int nearest_site(double x, double y) { //get the nearest site id
	int k, ret = 0;
	double d, dist = 0;
	for (k = 0; k < 20; k++) {
		d = sq2(x - X[k][0], y - X[k][1]);
		if (!k || d < dist) { //get the nearest distance
			dist = d;
			ret = k;
		}
	}
	return ret;
}

/* see if a pixel is different from any neighboring ones */
int at_edge(int *color, int y, int x){
	int i, j, c = color[y * size_x + x];
	for (i = y - 1; i <= y + 1; i++){
		if (i < 0 || i >= size_y)
			continue;

		for (j = x - 1; j <= x + 1; j++) {
			if (j < 0 || j >= size_x) 
				continue;
			if (color[i * size_x + j] != c)
				return 1;
		}
	}
	return 0;
}

void gen_map(IplImage *image){
	int i, j, k;
	int *nearest = (int*)malloc(sizeof(int) * size_y * size_x);

	for (i = 0; i < size_y; i++) {
		for (j = 0; j < size_x; j++) {
			nearest[i * size_x + j] = nearest_site(j, i);
		}
	}

	for (i = 0; i < size_y; i++){
		for (j = 0; j < size_x; j++) {
			if (at_edge(nearest, i, j)) {
				cvSet2D(image, i, j, cvScalar(255, 0, 0));
			}
		}
	}

	/* draw sites */
	for (k = 0; k < 20; k++) {
		cvSet2D(image, X[k][1]+1, X[k][0], cvScalar(0, 0, 255));
		cvSet2D(image, X[k][1], X[k][0]+1, cvScalar(0, 0, 255));
		cvSet2D(image, X[k][1]-1, X[k][0], cvScalar(0, 0, 255));
		cvSet2D(image, X[k][1], X[k][0]-1, cvScalar(0, 0, 255));
		cvSet2D(image, X[k][1], X[k][0], cvScalar(0, 0, 255));
	}

	image->origin = 1;
	cvShowImage("Voronoi", image);
	cvWaitKey();
}

int main(){
	IplImage *showVoronoi = cvCreateImage(cvSize(size_x, size_y), 8, 3);
	cvSet(showVoronoi, cvScalar(255, 255, 255));

	for (int i = 0; i < 20;i++){
		X[i][0] += 91;
		X[i][1] += 10;
	}

	gen_map(showVoronoi);
	return 0;
}