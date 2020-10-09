#include <stdio.h>
#include <math.h>
#include <windows.h>
#pragma warning(disable: 4996)

void Six_Tab_Filter(UCHAR img[512][512]);
void Quality_Evaluation(UCHAR ori[512][512], UCHAR res[512][512]);

int main() {
	FILE *fp_InputImg = fopen("lena(128x128).raw", "rb");
	FILE * fp_originImg = fopen("lena(512x512).raw", "rb");
	UCHAR Input_imgbuf[128][128] = { 0, };	//Input Imagebuf
	UCHAR Or_imgbuf[512][512] = { 0, };	// Original Imagebuf
	UCHAR result_imgbuf[512][512] = { 0, };	// Six_Tab_Filter Imagebuf

	if (!fp_InputImg) {
		printf("Cannot open file.\n");
		return 0;
	}
	// Step 0. Read Original lena(512x512).raw
	fread(&Or_imgbuf[0][0], sizeof(UCHAR), 512 * 512, fp_originImg);

	// Step 1. Read lena(128x128).raw and Extend Quadraple
	fread(&Input_imgbuf[0][0], sizeof(UCHAR), 128 * 128, fp_InputImg);
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			result_imgbuf[i * 4][j * 4] = Input_imgbuf[i][j];
		}
	}
	fclose(fp_InputImg);

	// Step 2. Six_Tab_Filter Interpolation
	FILE *fp_outputImg = fopen("Six_Tab_Filter_lena(512x512).raw", "wb");
	Six_Tab_Filter(result_imgbuf);
	fwrite(&result_imgbuf[0][0], sizeof(UCHAR), 512 * 512, fp_outputImg);

	// Step 3. Evaluate
	Quality_Evaluation(Or_imgbuf, result_imgbuf);

	fclose(fp_outputImg);
	fclose(fp_originImg);
	return 0;
}

void Six_Tab_Filter(UCHAR img[512][512]) {
	// Six_Tab
	int half_pel[8] = { 0, };
	int center = 0;
	for (int i = 0; i < 492; i += 4) {	//y축 4칸씩 내려감
		for (int j = 0; j < 492; j += 4) {	//x축 window 4칸씩 움직임
			for (int k = 0; k < 6; k++) {
				half_pel[k] = img[i][j + (k * 4)] - 5 * img[i + 4][j + (k * 4)]
					+ 20 * img[i + 8][j + (k * 4)] + 20 * img[i + 12][j + (k * 4)]
					- 5 * img[i + 16][j + (k * 4)] + img[i + 20][j + (k * 4)];
			}
			half_pel[6] = img[i + 8][j] - 5 * img[i + 8][j + 4]
				+ 20 * img[i + 8][j + 8] + 20 * img[i + 8][j + 12]
				- 5 * img[i + 8][j + 16] + img[i + 8][j + 20];
			half_pel[7] = img[i + 12][j] - 5 * img[i + 12][j + 4]
				+ 20 * img[i + 12][j + 8] + 20 * img[i + 12][j + 12]
				- 5 * img[i + 12][j + 16] + img[i + 12][j + 20];

			img[i + 10][j + 8] = half_pel[2] / 32;
			img[i + 10][j + 12] = half_pel[3] / 32;
			img[i + 8][j + 10] = half_pel[6] / 32;
			img[i + 12][j + 10] = half_pel[7] / 32;

			// quarter_pel
			// boundary
			img[i + 8][j + 9] = (img[i + 8][j + 8] * 32 + half_pel[6]) / 64;
			img[i + 8][j + 11] = (img[i + 8][j + 12] * 32 + half_pel[6]) / 64;
			img[i + 9][j + 8] = (img[i + 8][j + 8] * 32 + half_pel[2]) / 64;
			img[i + 11][j + 8] = (img[i + 12][j + 8] * 32 + half_pel[2]) / 64;
			img[i + 9][j + 12] = (img[i + 8][j + 12] * 32 + half_pel[3]) / 64;
			img[i + 11][j + 12] = (img[i + 12][j + 12] * 32 + half_pel[3]) / 64;
			img[i + 12][j + 9] = (img[i + 12][j + 8] * 32 + half_pel[7]) / 64;
			img[i + 12][j + 11] = (img[i + 12][j + 12] * 32 + half_pel[7]) / 64;
			// center
			center = (half_pel[0] - 5 * half_pel[1] + 20 * half_pel[2]
				+ 20 * half_pel[3] - 5 * half_pel[4] + half_pel[6]);
			img[i + 10][j + 10] = center / 1024;
			// inner
			img[i + 9][j + 10] = (center + half_pel[6] * 32) / 2048;
			img[i + 11][j + 10] = (center + half_pel[7] * 32) / 2048;
			img[i + 10][j + 9] = (center + half_pel[2] * 32) / 2048;
			img[i + 10][j + 11] = (center + half_pel[3] * 32) / 2048;
			// diagonal
			img[i + 9][j + 9] = (half_pel[2] + half_pel[6]) / 64;
			img[i + 9][j + 11] = (half_pel[3] + half_pel[6]) / 64;
			img[i + 11][j + 9] = (half_pel[2] + half_pel[7]) / 64;
			img[i + 11][j + 11] = (half_pel[3] + half_pel[7]) / 64;
		}
	}

	// Boundary Top, Left
	for (int i = 0; i <= 8; i += 4) {
		for (int j = 0; j < 8; j += 4) {
			img[i][j + 1] = (3 * img[i][j] + img[i][j + 4]) / 4;
			img[i][j + 2] = (img[i][j] + img[i][j + 4]) / 2;
			img[i][j + 3] = (img[i][j] + 3 * img[i][j + 4]) / 4;
		}
	}
	for (int i = 0; i < 8; i += 4) {
		for (int j = 0; j < 8; j ++) {
			img[i+1][j] = (3 * img[i][j] + img[i+4][j]) / 4;
			img[i+2][j] = (img[i][j] + img[i+4][j]) / 2;
			img[i+3][j] = (img[i][j] + 3 * img[i+4][j]) / 4;
		}
	}

	for (int i = 0; i < 8; i += 4) {
		for (int j = 0; j < 492; j += 4) {
			// x-axis
			half_pel[0] = (img[i][j] - 5 * img[i][j + 4]
				+ 20 * img[i][j + 8] + 20 * img[i][j + 12]
				- 5 * img[i][j + 16] + img[i][j + 20]);
			half_pel[1] = (img[i + 4][j] - 5 * img[i + 4][j + 4]
				+ 20 * img[i + 4][j + 8] + 20 * img[i + 4][j + 12]
				- 5 * img[i + 4][j + 16] + img[i + 4][j + 20]);
			half_pel[2] = (16 * img[i][j + 8] + 20 * img[i + 4][j + 8]
				- 5 * img[i + 8][j + 8] + img[i + 12][j + 8]);
			half_pel[3] = (16 * img[i][j + 12] + 20 * img[i + 4][j + 12]
				- 5 * img[i + 8][j + 12] + img[i + 12][j + 12]);
			center = (16 * half_pel[0] + 20 * half_pel[1]
				+ ((-5)*(32 * img[i + 8][j + 10])) + (32 * img[i + 12][j + 10]));
			img[i + 2][j + 10] = center / 1024;

			// half_pel
			img[i][j + 10] = half_pel[0] / 32;
			img[i + 4][j + 10] = half_pel[1] / 32;
			img[i + 2][j + 8] = half_pel[2] / 32;
			img[i + 2][j + 12] = half_pel[3] / 32;
				
			//quarter_pel
			img[i][j + 9] = (32 * img[i][j + 8] + half_pel[0]) / 64;
			img[i][j + 11] = (32 * img[i][j + 12] + half_pel[0]) / 64;
			img[i + 4][j + 9] = (32 * img[i + 4][j + 8] + half_pel[1]) / 64;
			img[i + 4][j + 11] = (32 * img[i + 4][j + 12] + half_pel[1]) / 64;
			img[i + 1][j + 8] = (32 * img[i][j + 8] + half_pel[2]) / 64;
			img[i + 3][j + 8] = (32 * img[i + 4][j + 8] + half_pel[2]) / 64;
			img[i + 1][j + 12] = (32 * img[i][j + 12] + half_pel[3]) / 64;
			img[i + 3][j + 12] = (32 * img[i + 4][j + 12] + half_pel[3]) / 64;
			
			img[i + 1][j + 10] = (32 * half_pel[0] + center) / 2048;
			img[i + 3][j + 10] = (32 * half_pel[1] + center) / 2048;
			img[i + 2][j + 9] = (32 * half_pel[2] + center) / 2048;
			img[i + 2][j + 11] = (32 * half_pel[3] + center) / 2048;

			img[i + 1][j + 9] = (half_pel[0] + half_pel[2]) / 64;
			img[i + 1][j + 11] = (half_pel[0] + half_pel[3]) / 64;
			img[i + 3][j + 9] = (half_pel[1] + half_pel[2]) / 64;
			img[i + 3][j + 11] = (half_pel[1] + half_pel[3]) / 64;


			// y-axis
			half_pel[4] = (img[j][i] - 5 * img[j + 4][i]
				+ 20 * img[j + 8][i] + 20 * img[j + 12][i]
				- 5 * img[j + 16][i] + img[j + 20][i]);
			half_pel[5] = (img[j][i + 4] - 5 * img[j + 4][i + 4]
				+ 20 * img[j + 8][i + 4] + 20 * img[j + 12][i + 4]
				- 5 * img[j + 16][i + 4] + img[j + 20][i + 4]);
			half_pel[6] = (16 * img[j + 8][i] + 20 * img[j + 8][i + 4]
				- 5 * img[j + 8][i + 8] + img[j + 8][i + 12]);
			half_pel[7] = (16 * img[j + 12][i] + 20 * img[j + 12][i + 4]
				- 5 * img[j + 12][i + 8] + img[j + 12][i + 12]);
			center = (16 * half_pel[4] + 20 * half_pel[5]
				- (5 * 32 * img[j + 10][i + 8]) + img[j + 10][i + 12]);
			img[j + 10][i + 2] = center / 1024;

			// half_pel
			img[j + 10][i] = half_pel[4] / 32;
			img[j + 10][i + 4] = half_pel[5] / 32;
			img[j + 8][i + 2] = half_pel[6] / 32;
			img[j + 12][i + 2] = half_pel[7] / 32;

			//quarter_pel
			img[j + 8][i + 1] = (32 * img[j + 8][i] + half_pel[6]) / 64;
			img[j + 8][i + 3] = (32 * img[j + 8][i + 4] + half_pel[6]) / 64;
			img[j + 9][i] = (32 * img[j + 8][i] + half_pel[4]) / 64;
			img[j + 11][i] = (32 * img[j + 12][i] + half_pel[4]) / 64;
			img[j + 12][i + 1] = (32 * img[j + 12][i] + half_pel[7]) / 64;
			img[j + 12][i + 3] = (32 * img[j + 12][i + 4] + half_pel[7]) / 64;
			img[j + 9][i + 4] = (32 * img[j + 8][i + 4] + half_pel[5]) / 64;
			img[j + 11][i + 4] = (32 * img[j + 12][i + 4] + half_pel[5]) / 64;

			img[j + 9][i + 2] = (32 * half_pel[6] + center) / 2048;
			img[j + 11][i + 2] = (32 * half_pel[7] + center) / 2048;
			img[j + 10][i + 1] = (32 * half_pel[4] + center) / 2048;
			img[j + 10][i + 3] = (32 * half_pel[5] + center) / 2048;

			img[j + 9][i + 1] = (half_pel[6] + half_pel[4]) / 64;
			img[j + 9][i + 3] = (half_pel[6] + half_pel[5]) / 64;
			img[j + 11][i + 1] = (half_pel[7] + half_pel[4]) / 64;
			img[j + 11][i + 3] = (half_pel[7] + half_pel[5]) / 64;
		}
	}
	for (int i = 500; i <= 508; i += 4) {
		for (int j = 0; j <= 8; j += 4) {
			img[i][j + 1] = (3 * img[i][j] + img[i][j + 4]) / 4;
			img[i][j + 2] = (img[i][j] + img[i][j + 4]) / 2;
			img[i][j + 3] = (img[i][j] + 3 * img[i][j + 4]) / 4;
		}
	}
	for (int i = 500; i < 508; i += 4) {
		for (int j = 0; j <= 8; j++) {
			img[i + 1][j] = (3 * img[i][j] + img[i + 4][j]) / 4;
			img[i + 2][j] = (img[i][j] + img[i + 4][j]) / 2;
			img[i + 3][j] = (img[i][j] + 3 * img[i + 4][j]) / 4;
		}
	}
	
	// Boundary Bottom Right
	for (int i = 0; i <= 8; i += 4) {
		for (int j = 500; j < 508; j += 4) {
			img[i][j + 1] = (3 * img[i][j] + img[i][j + 4]) / 4;
			img[i][j + 2] = (img[i][j] + img[i][j + 4]) / 2;
			img[i][j + 3] = (img[i][j] + 3 * img[i][j + 4]) / 4;
		}
	}
	for (int i = 0; i < 8; i += 4) {
		for (int j = 500; j <= 508; j++) {
			img[i + 1][j] = (3 * img[i][j] + img[i + 4][j]) / 4;
			img[i + 2][j] = (img[i][j] + img[i + 4][j]) / 2;
			img[i + 3][j] = (img[i][j] + 3 * img[i + 4][j]) / 4;
		}
	}

	for (int i = 500; i < 508; i += 4) {
		for (int j = 0; j < 492; j += 4) {
			half_pel[0] = (img[i][j] - 5 * img[i][j + 4]
				+ 20 * img[i][j + 8] + 20 * img[i][j + 12]
				- 5 * img[i][j + 16] + img[i][j + 20]);
			half_pel[1] = (img[i + 4][j] - 5 * img[i + 4][j + 4]
				+ 20 * img[i + 4][j + 8] + 20 * img[i][j + 12]
				- 5 * img[i + 4][j + 16] + img[i + 4][j + 20]);
			half_pel[2] = (16 * img[i + 4][j + 8] + 20 * img[i][j + 8]
				- 5 * img[i - 4][j + 8] + img[i - 8][j + 8]);
			half_pel[3] = (16 * img[i + 4][j + 12] + 20 * img[i][j + 12]
				- 5 * img[i - 4][j + 12] + img[i - 8][j + 12]);
			center = (16 * half_pel[1] + 20 * half_pel[0]
				- 5 * 32 * img[i - 4][j + 10] + 32 * img[i - 8][j + 10]);

			//half_pel
			img[i][j + 10] = half_pel[0] / 32;
			img[i + 4][j + 10] = half_pel[1] / 32;
			img[i + 2][j + 8] = half_pel[2] / 32;
			img[i + 2][j + 12] = half_pel[3] / 32;
			img[i + 2][j + 10] = center / 1024;

			//quarter_pel
			img[i][j + 9] = (32 * img[i][j + 8] + half_pel[0]) / 64;
			img[i][j + 11] = (32 * img[i][j + 12] + half_pel[0]) / 64;
			img[i + 1][j + 8] = (32 * img[i][j + 8] + half_pel[2]) / 64;
			img[i + 3][j + 8] = (32 * img[i + 4][j + 8] + half_pel[2]) / 64;

			img[i + 1][j + 12] = (32 * img[i][j + 12] + half_pel[3]) / 64;
			img[i + 3][j + 12] = (32 * img[i + 4][j + 12] + half_pel[3]) / 64;
			img[i + 4][j + 9] = (32 * img[i + 4][j + 8] + half_pel[1]) / 64;
			img[i + 4][j + 11] = (32 * img[i + 4][j + 12] + half_pel[1]) / 64;

			img[i + 1][j + 10] = (32 * half_pel[0] + center) / 2048;
			img[i + 3][j + 10] = (32 * half_pel[1] + center) / 2048;
			img[i + 2][j + 9] = (32 * half_pel[2] + center) / 2048;
			img[i + 2][j + 11] = (32 * half_pel[3] + center) / 2048;

			img[i + 1][j + 9] = (half_pel[0] + half_pel[2]) / 64;
			img[i + 1][j + 11] = (half_pel[0] + half_pel[3]) / 64;
			img[i + 3][j + 9] = (half_pel[1] + half_pel[2]) / 64;
			img[i + 3][j + 11] = (half_pel[1] + half_pel[3]) / 64;

			half_pel[4] = (img[j][i] - 5 * img[j + 4][i]
				+ 20 * img[j + 8][i] + 20 * img[j + 12][i]
				- 5 * img[j + 16][i] + img[j + 20][i]);
			half_pel[5] = (img[j][i + 4] - 5 * img[j + 4][i + 4]
				+ 20 * img[j + 8][i + 4] + 20 * img[j + 12][i + 4]
				- 5 * img[j + 16][i + 4] + img[j + 20][i + 4]);
			half_pel[6] = (16 * img[j + 8][i + 4] + 20 * img[j + 8][i]
				- 5 * img[j + 8][i - 4] + img[j + 8][i - 8]);
			half_pel[7] = (16 * img[j + 12][i + 4] + 20 * img[j + 12][i]
				- 5 * img[j + 12][i - 4] + img[j + 12][i - 8]);
			center = (16 * half_pel[5] + 20 * half_pel[6]
				- 5 * 32 * img[j + 10][i - 4] + 32 * img[j + 10][i - 8]);

			//half_pel
			img[j + 10][i] = half_pel[4] / 32;
			img[j + 10][i + 4] = half_pel[5] / 32;
			img[j + 8][i + 2] = half_pel[6] / 32;
			img[j + 12][i + 2] = half_pel[7] / 32;
			img[j + 10][i + 2] = center / 1024;

			//quarter_pel
			img[j + 8][i + 1] = (32 * img[j + 8][i] + half_pel[6]) / 64;
			img[j + 8][i + 3] = (32 * img[j + 8][i + 4] + half_pel[6]) / 64;
			img[j + 9][i] = (32 * img[j + 8][i] + half_pel[4]) / 64;
			img[j + 11][i] = (32 * img[j + 12][i] + half_pel[4]) / 64;
			img[j + 9][i + 4] = (32 * img[j + 8][i + 4] + half_pel[5]) / 64;
			img[j + 11][i + 4] = (32 * img[j + 12][i + 4] + half_pel[5]) / 64;
			img[j + 12][i + 1] = (32 * img[j + 12][i] + half_pel[7]) / 64;
			img[j + 12][i + 3] = (32 * img[j + 12][i + 4] + half_pel[7]) / 64;

			img[j + 9][i + 2] = (32 * half_pel[6] + center) / 2048;
			img[j + 11][i + 2] = (32 * half_pel[7] + center) / 2048;
			img[j + 10][i + 1] = (32 * half_pel[4] + center) / 2048;
			img[j + 10][i + 3] = (32 * half_pel[5] + center) / 2048;

			img[j + 9][i + 1] = (half_pel[6] + half_pel[4]) / 64;
			img[j + 9][i + 3] = (half_pel[6] + half_pel[5]) / 64;
			img[j + 11][i + 1] = (half_pel[7] + half_pel[4]) / 64;
			img[j + 11][i + 3] = (half_pel[7] + half_pel[5]) / 64;
		}
	}

	for (int i = 500; i <= 508; i += 4) {
		for (int j = 500; j < 508; j += 4) {
			img[i][j + 1] = (3 * img[i][j] + img[i][j + 4]) / 4;
			img[i][j + 2] = (img[i][j] + img[i][j + 4]) / 2;
			img[i][j + 3] = (img[i][j] + 3 * img[i][j + 4]) / 4;
		}
	}
	for (int i = 500; i < 508; i += 4) {
		for (int j = 500; j <= 508; j++) {
			img[i + 1][j] = (3 * img[i][j] + img[i + 4][j]) / 4;
			img[i + 2][j] = (img[i][j] + img[i + 4][j]) / 2;
			img[i + 3][j] = (img[i][j] + 3 * img[i + 4][j]) / 4;
		}
	}

	// Boundary Outside
	for (int i = 0; i < 512; i++) {

	}
}

void Quality_Evaluation(UCHAR original[512][512], UCHAR result[512][512])
{
	int size_n = 512 * 512;
	int err = 0;
	double mse, psnr, sum = 0;
	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			err = original[i][j] - result[i][j];
			sum += err * err;
		}
	}
	mse = sum / size_n;
	psnr = 20 * log10(255 / sqrt(mse));
	printf("RMS: %f\n", sqrt(mse));
	printf("PSNR: %f\n", psnr);
}
