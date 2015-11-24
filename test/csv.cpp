#include <stdio.h>
#include <math.h>

//���ַ��ⷽ��
double bisection(double Rbudget,double high,double low,double c[4], double k[4]) {
	double middle;
	double h;
	double l;
	double m;

	while ((high - low) >= 1e-6) {
		middle = (high + low) / 2;
		h = pow(k[0] * c[0] / high, 1 / (1 + k[0])) + pow(k[1] * c[1] / high, 1 / (1 + k[1])) + pow(k[2] * c[2] / high, 1 / (1 + k[2])) + pow(k[3] * c[3] / high, 1 / (1 + k[3])) - Rbudget;
		l = pow(k[0] * c[0] / low, 1 / (1 + k[0])) + pow(k[1] * c[1] / low, 1 / (1 + k[1])) + pow(k[2] * c[2] / low, 1 / (1 + k[2])) + pow(k[3] * c[3] / low, 1 / (1 + k[3])) - Rbudget;
		m = pow(k[0] * c[0] / middle, 1 / (1 + k[0])) + pow(k[1] * c[1] / middle, 1 / (1 + k[1])) + pow(k[2] * c[2] / middle, 1 / (1 + k[2])) + pow(k[3] * c[3] / middle, 1 / (1 + k[3])) - Rbudget;
		
		if (h*l > 0) {
			high *= 2;
		}
		if (m == 0) {
			return middle;
		}
		if (h*m < 0) {
			low = middle;
		}
		if (l*m < 0) {
			high = middle;
		}
	}
	return middle;
}

//����CSV�ļ�����ȡR��D��QP
void parseCSV(char *fileName, double (&s)[600][3], int framerate, int imageWidth, int imageHeight) {
	char content = 0;
	int commaNum = 0;

	FILE *fp = fopen(fileName, "r");

	while (content != '\n') {
		fread(&content, 1, 1, fp);
	}

	for (int i = 0; i < 600; i++) {
		while (commaNum != 3) {
			fread(&content, 1, 1, fp);
			if (content == ',') {
				commaNum++;
			}
		}
		commaNum = 0;
		for (int j = 0; j < 3; j++) {
			fscanf(fp, "%lf", &s[i][j]);
			fread(&content, 1, 1, fp);
		}
		if (i > 0) {
			s[i][1] += s[i - 1][1];	//s[i][1]=bits(1)+bits(2)+...+bits(i)
		}
		while (content != '\n') {
			fread(&content, 1, 1, fp);
		}
	}
	for (int i = 0; i < 600; i++) {
		s[i][1] = s[i][1] * framerate / ((i + 1) * 1000);	//s[i][1]=R(i)
		s[i][2] = 255 * 255 / pow(10, s[i][2] / 10);		//s[i][2]=MSE(i)
		s[i][2] = imageWidth*imageHeight *s[i][2];			//s[i][2]=D(i)
	}
	fclose(fp);
}

int main() {
	FILE *fp = fopen("BigShips_1280x720_60Hz_i420_part1_result.csv","w");
	double sheet1[600][3];	//���ڴ洢600��QP��Bits��Y PSNR����
	double sheet2[600][3];
	double sheet3[600][3];
	double sheet4[600][3];

	double QP[8];
	double R[8];
	double D[8];
	double lambda[8];
	double alpha[4];
	double beta[4];
	double c[4];
	double k[4];

	double Rbudget = 2000.0;	//2000kb/s
	double u;				//u=1/��
	double low = -10000.0;		//���ַ���ʽ��10���Ľ⣬��������
	double high = 20000.0;		//���ַ���ʽ��10���Ľ⣬��������
	int framerate = 60;
	int imageWidth = 1280;
	int imageHeight = 720;

	parseCSV("BigShips_1280x720_60Hz_i420_part1.csv", sheet1, framerate, imageWidth, imageHeight);	//����CSV�ļ�����600��R��D��QP����sheet����
	parseCSV("BigShips_1280x720_60Hz_i420_part2.csv", sheet2, framerate, imageWidth, imageHeight);
	parseCSV("City_corr_1280x720_60Hz_i420_part1.csv", sheet3, framerate, imageWidth, imageHeight);
	parseCSV("City_corr_1280x720_60Hz_i420_part2.csv", sheet4, framerate, imageWidth, imageHeight);

	for (int i = 0; i < 600; i ++) {
		for (int j = 0; j < 2; j++) {
			QP[j] = sheet1[i + j][0];
			R[j] = sheet1[i + j][1];
			D[j] = sheet1[i + j][2];

			QP[j+2] = sheet2[i + j][0];
			R[j+2] = sheet2[i + j][1];
			D[j+2] = sheet2[i + j][2];

			QP[j+4] = sheet3[i + j][0];
			R[j+4] = sheet3[i + j][1];
			D[j+4] = sheet3[i + j][2];

			QP[j+6] = sheet4[i + j][0];
			R[j+6] = sheet4[i + j][1];
			D[j+6] = sheet4[i + j][2];
		}
		for (int j = 0; j < 4; j++) {
			lambda[j * 2] = exp((QP[j * 2] - 13.7122) / 4.2005);			//������QP����Ӧ����Lamda���� ���������R����Ӧ����D��R��
			lambda[j * 2 + 1] = exp((QP[j * 2 + 1] - 13.7122) / 4.2005);	

			beta[j] = log(R[j * 2] / R[j * 2 + 1]) / log(lambda[j * 2] / lambda[j * 2 + 1]);	//�� �� �� �� ������
			alpha[j] = R[j * 2] / pow(lambda[j * 2], beta[j]);

			k[j] = -log(D[j * 2] / D[j * 2 + 1]) / log(R[j * 2] / R[j * 2 + 1]);	//��c k ������
			c[j] = D[j * 2] / pow(R[j * 2], -k[j]);
		}

		u = bisection(Rbudget, high, low, c, k);	//���ַ���ʽ10�Ľ�

		fprintf(fp, "R,lambda,QP\n");	//������������ݵı�ͷ��R	lambda	QP

		for (int j = 0; j < 4; j++) {
			R[j] = pow(k[j] * c[j] * u, 1 / (k[j] + 1));			//����Mu�Ѹ���c_i k_i �� R_i�����
			lambda[j] = pow(R[j] / alpha[j], 1 / beta[j]);			//����R_i, ����������_i
			QP[j] = 4.2005*log(lambda[j]) + 13.7122;				//���æ�_i��������QP_i
			fprintf(fp, "%lf,%lf,%lf\n", R[j], lambda[j], QP[j]);	//��������
		}
	}

	return 0;
}