#include <highgui.h>

int rgb2yuv() {
	CvCapture *capture = cvCreateFileCapture("D:\\测试文件\\The_Magic_of_Flight_1440x1080.wmv");
	IplImage *frame;
	int size;
	int width;
	int height;
	int count = 1000;
	unsigned char *yuv=new unsigned char[size];
	unsigned char *y;
	unsigned char *u;
	unsigned char *v;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	FILE *fp = fopen("D:\\测试文件\\The_Magic_of_Flight_1080.yuv", "wb");
	frame = cvQueryFrame(capture);
	width = frame->width;
	height = frame->height;
	size = width*height * 3 / 2;
	while (frame && count--) {
		y = yuv;
		u = yuv + width * height;
		v = yuv + width * height * 5 / 4;
		for (int i = 0; i < frame->height;i++) {
			for (int j = 0; j < frame->width; j++) {
				b = (unsigned char)frame->imageData[i*frame->widthStep + j * 3];
				g = (unsigned char)frame->imageData[i*frame->widthStep + j * 3 + 1];
				r = (unsigned char)frame->imageData[i*frame->widthStep + j * 3 + 2];
				*y = 0.299*r + 0.587*g + 0.114*b;
				y++;
				if ((i % 2 == 1) && (j % 2 == 1)) {
					*u = -0.169*r - 0.331*g + 0.499*b + 128;
					*v = 0.499*r - 0.418*g - 0.0813*b + 128;
					u++;
					v++;
				}
			}
		}
		printf("%d\n", count);
		printf("%d\n", fwrite(yuv, 1, size, fp));
		frame = cvQueryFrame(capture);
	}
	return 0;
}