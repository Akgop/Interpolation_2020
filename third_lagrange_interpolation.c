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
	// x_axis half_pel
	
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
