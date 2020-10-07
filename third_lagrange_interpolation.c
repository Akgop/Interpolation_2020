#include <stdio.h>
#include <math.h>
#include <windows.h>
#pragma warning(disable: 4996)

void Third_Lagrange(UCHAR img[512][512]);
void Quality_Evaluation(UCHAR ori[512][512], UCHAR res[512][512]);

int main() {
	FILE *fp_InputImg = fopen("lena(128x128).raw", "rb");
	FILE * fp_originImg = fopen("lena(512x512).raw", "rb");
	UCHAR Input_imgbuf[128][128] = { 0, };	//Input Imagebuf
	UCHAR Or_imgbuf[512][512] = { 0, };	// Original Imagebuf
	UCHAR result_imgbuf[512][512] = { 0, };	// Third_Lagrange Imagebuf

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

	// Step 2. Third_Lagrange Interpolation
	FILE *fp_outputImg = fopen("Third_Lagrange_lena(512x512).raw", "wb");
	Third_Lagrange(result_imgbuf);
	fwrite(&result_imgbuf[0][0], sizeof(UCHAR), 512 * 512, fp_outputImg);

	// Step 3. Evaluate
	Quality_Evaluation(Or_imgbuf, result_imgbuf);

	fclose(fp_outputImg);
	fclose(fp_originImg);
	return 0;
}

void Third_Lagrange(UCHAR img[512][512]) {
	// x_axis
	for (int i = 0; i < 512; i += 4) {
		for (int j = 1; j < 4; j++) {	// fill 1~3 pixel on the left
			img[i][j] = (((j - 4) * (j - 8) * (j - 12) * img[i][0]) / -384) +	// -384 = -4 * -8 * -12
				((j * (j - 8) * (j - 12) * img[i][4]) / 128) +					// 128 = 4 * -4 * -8
				((j * (j - 4) * (j - 12) * img[i][8]) / -128) +					// -128 = 8 * 4 * -4
				((j * (j - 4) * (j - 8) * img[i][12]) / 384);					// 384 = 12 * 8 * 4
		}

		for (int k = 0; k < 504; k += 4) {	// loop 126 times
			for (int j = 5; j < 8; j++) {	// fill middle space pixel 5~507
				img[i][k + j] = (((j - 4) * (j - 8) * (j - 12) * img[i][k]) / -384) +	// -384 = -4 * -8 * -12
					((j * (j - 8) * (j - 12) * img[i][k + 4]) / 128) +					// 128 = 4 * -4 * -8
					((j * (j - 4) * (j - 12) * img[i][k + 8]) / -128) +					// -128 = 8 * 4 * -4
					((j * (j - 4) * (j - 8) * img[i][k + 12]) / 384);					// 384 = 12 * 8 * 4
			}
		}
		//outline
		//509 ~ 511
		for (int j = 9; j < 12; j++) {
			img[i][j + 500] = (((j - 4) * (j - 8) * (j - 12) * img[i][496]) / -384) +	// -384 = -4 * -8 * -12
				((j * (j - 8) * (j - 12) * img[i][500]) / 128) +					// 128 = 4 * -4 * -8
				((j * (j - 4) * (j - 12) * img[i][504]) / -128) +					// -128 = 8 * 4 * -4
				((j * (j - 4) * (j - 8) * img[i][508]) / 384);					// 384 = 12 * 8 * 4
		}
	}
	// y_axis
	for (int i = 0; i < 512; i++) {
		for (int j = 1; j < 4; j++) {	// fill 1~3 pixel on the top
			img[j][i] = (((j - 4) * (j - 8) * (j - 12) * img[0][i]) / -384) +	// -384 = -4 * -8 * -12
				((j * (j - 8) * (j - 12) * img[4][i]) / 128) +					// 128 = 4 * -4 * -8
				((j * (j - 4) * (j - 12) * img[8][i]) / -128) +					// -128 = 8 * 4 * -4
				((j * (j - 4) * (j - 8) * img[12][i]) / 384);					// 384 = 12 * 8 * 4
		}

		for (int k = 0; k < 504; k += 4) {	// loop 126 times
			for (int j = 5; j < 8; j++) {	// fill middle space pixel 5~507
				img[k + j][i] = (((j - 4) * (j - 8) * (j - 12) * img[k][i]) / -384) +	// -384 = -4 * -8 * -12
					((j * (j - 8) * (j - 12) * img[k + 4][i]) / 128) +					// 128 = 4 * -4 * -8
					((j * (j - 4) * (j - 12) * img[k + 8][i]) / -128) +					// -128 = 8 * 4 * -4
					((j * (j - 4) * (j - 8) * img[k + 12][i]) / 384);					// 384 = 12 * 8 * 4
			}
		}
		//outline
		//509 ~ 511
		for (int j = 9; j < 12; j++) {
			img[j+500][i] = (((j - 4) * (j - 8) * (j - 12) * img[496][i]) / -384) +	// -384 = -4 * -8 * -12
				((j * (j - 8) * (j - 12) * img[500][i]) / 128) +					// 128 = 4 * -4 * -8
				((j * (j - 4) * (j - 12) * img[504][i]) / -128) +					// -128 = 8 * 4 * -4
				((j * (j - 4) * (j - 8) * img[508][i]) / 384);					// 384 = 12 * 8 * 4
		}		
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
