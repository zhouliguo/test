#include <highgui.h>
#include <math.h>

int main() {
	double *colorPixels;
	double *depthPixels;
	double *w2;
	double *depth_adj;
	double *Itmo;
	double b;
	double c;
	double *Tlog;
	double Imax[3] = { 255.0,255.0,255.0 };
	double temp;
	double Texp;

	IplImage *color = cvLoadImage("color100.bmp", CV_LOAD_IMAGE_UNCHANGED);
	IplImage *depth = cvLoadImage("depth100.bmp", CV_LOAD_IMAGE_UNCHANGED);
	cvSmooth(depth, depth, CV_MEDIAN);
	cvShowImage("color", color);
	cvShowImage("depth", depth);
	colorPixels = new double[color->width*color->height * 3];
	depthPixels = new double[depth->width*depth->height];
	w2 = new double[depth->width*depth->height];
	depth_adj = new double[depth->width*depth->height];
	double maxOut = 1;
	double minOut = 0;
	double maxA = (unsigned char)depth->imageData[0];
	double minA = (unsigned char)depth->imageData[0];

	for (int i = 0; i < depth->height;i++) {
		for (int j = 0; j < depth->width; j++) {
			colorPixels[i*color->width * 3 + j * 3] = (unsigned char)color->imageData[i*color->widthStep + j * 3];
			colorPixels[i*color->width * 3 + j * 3 + 1] = (unsigned char)color->imageData[i*color->widthStep + j * 3 + 1];
			colorPixels[i*color->width * 3 + j * 3 + 2] = (unsigned char)color->imageData[i*color->widthStep + j * 3 + 2];
			depthPixels[i*depth->width + j] = (unsigned char)depth->imageData[i*depth->widthStep + j];
			if (maxA < depthPixels[i*depth->width + j]) {
				maxA = depthPixels[i*depth->width + j];
			}
			if (minA > depthPixels[i*depth->width + j]) {
				minA = depthPixels[i*depth->width + j];
			}
		}
	}

	for (int i = 0; i < depth->height; i++) {
		for (int j = 0; j < depth->width; j++) {
			w2[i*depth->width + j] = (depthPixels[i*depth->width + j] - minA) / (maxA - minA)*(maxOut - minOut) + minOut;
			depth_adj[i*depth->width + j] = 2 * exp(-w2[i*depth->width + j]);
		}
	}
	

	b = 0.8;
	c = log(b) / log(0.5);
	Tlog = new double[color->width*color->height * 3];
	Itmo = new double[color->width*color->height * 3];
	
	for (int i = 0; i < color->height; i++) {
		for (int j = 0; j < color->width; j++) {
			Tlog[i*color->width * 3 + j * 3] = log(colorPixels[i*color->width * 3 + j * 3] + 1) / log(Imax[0] + 1);
			Tlog[i*color->width * 3 + j * 3 + 1] = log(colorPixels[i*color->width * 3 + j * 3 + 1] + 1) / log(Imax[1] + 1);
			Tlog[i*color->width * 3 + j * 3 + 2] = log(colorPixels[i*color->width * 3 + j * 3 + 2] + 1) / log(Imax[2] + 1);
			
			temp = log(5 + pow(colorPixels[i*color->width * 3 + j * 3] / Imax[0], c) * 5);
			Texp = log(10) / temp;
			Itmo[i*color->width * 3 + j * 3] = Tlog[i*color->width * 3 + j * 3] * Texp*Imax[0];
			color->imageData[i*color->widthStep + j * 3] = Itmo[i*color->width * 3 + j * 3];
			temp = log(5 + pow(colorPixels[i*color->width * 3 + j * 3+1] / Imax[1], c) * 5);
			Texp = log(10) / temp;
			Itmo[i*color->width * 3 + j * 3 + 1] = Tlog[i*color->width * 3 + j * 3 + 1] * Texp*Imax[1];
			color->imageData[i*color->widthStep + j * 3 + 1] = Itmo[i*color->width * 3 + j * 3 + 1];
			temp = log(5 + pow(colorPixels[i*color->width * 3 + j * 3+2] / Imax[2], c) * 5);
			Texp = log(10) / temp;
			Itmo[i*color->width * 3 + j * 3+2] = Tlog[i*color->width * 3 + j * 3+2] * Texp*Imax[2];
			color->imageData[i*color->widthStep + j * 3 + 2] = Itmo[i*color->width * 3 + j * 3 + 2];
		}
	}
	cvShowImage("anchor_final_GD", color);

	for (int i = 0; i < color->height; i++) {
		for (int j = 0; j < color->width; j++) {
			Tlog[i*color->width * 3 + j * 3] = log(colorPixels[i*color->width * 3 + j * 3] + 1) / log(Imax[0] + 1);
			Tlog[i*color->width * 3 + j * 3 + 1] = log(colorPixels[i*color->width * 3 + j * 3 + 1] + 1) / log(Imax[1] + 1);
			Tlog[i*color->width * 3 + j * 3 + 2] = log(colorPixels[i*color->width * 3 + j * 3 + 2] + 1) / log(Imax[2] + 1);

			temp = log(5 + pow(colorPixels[i*color->width * 3 + j * 3] / Imax[0], log(depth_adj[i*depth->width + j]) / log(0.5)) * 5);
			Texp = log(10) / temp;
			Itmo[i*color->width * 3 + j * 3] = Tlog[i*color->width * 3 + j * 3] * Texp*Imax[0];
			color->imageData[i*color->widthStep + j * 3] = Itmo[i*color->width * 3 + j * 3];
			temp = log(5 + pow(colorPixels[i*color->width * 3 + j * 3 + 1] / Imax[1], log(depth_adj[i*depth->width + j])/log(0.5)) * 5);
			Texp = log(10) / temp;
			Itmo[i*color->width * 3 + j * 3 + 1] = Tlog[i*color->width * 3 + j * 3 + 1] * Texp*Imax[1];
			color->imageData[i*color->widthStep + j * 3 + 1] = Itmo[i*color->width * 3 + j * 3 + 1];
			temp = log(5 + pow(colorPixels[i*color->width * 3 + j * 3 + 2] / Imax[2], log(depth_adj[i*depth->width + j]) / log(0.5)) * 5);
			Texp = log(10) / temp;
			Itmo[i*color->width * 3 + j * 3 + 2] = Tlog[i*color->width * 3 + j * 3 + 2] * Texp*Imax[2];
			color->imageData[i*color->widthStep + j * 3 + 2] = Itmo[i*color->width * 3 + j * 3 + 2];
		}
	}

	cvShowImage("our_final_GD", color);
	cvWaitKey();

	return 0;
}