#include <stdio.h>
#include <math.h>
#include <windows.h>
#pragma warning(disable: 4996)

void Bilinear(UCHAR img[512][512]);
void Quality_Evaluation(UCHAR ori[512][512], UCHAR res[512][512]);

int main() {
	FILE *fp_InputImg = fopen("lena(128x128).raw", "rb");
	FILE * fp_originImg = fopen("lena(512x512).raw", "rb");
	UCHAR Input_imgbuf[128][128] = { 0, };	//Input Imagebuf
	UCHAR Or_imgbuf[512][512] = { 0, };	// Original Imagebuf
	UCHAR result_imgbuf[512][512] = { 0, };	// Bilinear Imagebuf

	if (!fp_InputImg) {
		printf("Cannot open file.\n");
		return 0;
	}
	// Step 0. Read Original lena(512x512).raw
	fread(&Or_imgbuf[0][0], sizeof(UCHAR), 512 * 512, fp_originImg);

	// Step 1. Read lena(128x128).raw and Extend Quadraple
	fread(&Input_imgbuf[0][0], sizeof(UCHAR), 128*128, fp_InputImg);
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			result_imgbuf[i * 4][j * 4] = Input_imgbuf[i][j];
		}
	}
	fclose(fp_InputImg);
	
	// Step 2. Bilinear Interpolation
	FILE *fp_outputImg = fopen("Bilinear_lena(512x512).raw", "wb");
	Bilinear(result_imgbuf);
	fwrite(&result_imgbuf[0][0], sizeof(UCHAR), 512 * 512, fp_outputImg);

	// Step 3. Evaluate
	Quality_Evaluation(Or_imgbuf, result_imgbuf);

	fclose(fp_outputImg);
	fclose(fp_originImg);
	return 0;
}

void Bilinear(UCHAR img[512][512]) {
	// x_axis half_pel
	for (int i = 0; i < 512; i += 4) {
		for (int j = 2; j < 512; j += 4) {
			img[i][j] = img[i][j - 2] / 2 + img[i][j + 2] / 2;
		}
	}
	// x-axis quater_pel
	for (int i = 0; i < 512; i += 4) {
		for (int j = 1; j < 512; j += 2) {
			img[i][j] = img[i][j - 1] / 2 + img[i][j + 1] / 2;
		}
	}
	// y-axis half_pel
	for (int i = 0; i < 512; i++) {
		for (int j = 2; j < 512; j += 4) {
			img[j][i] = img[j-2][i] / 2 + img[j+2][i] / 2;
		}
	}
	// y-axis quarter_pel
	for (int i = 0; i < 512; i++) {
		for (int j = 1; j < 512; j += 2) {
			img[j][i] = img[j - 1][i] / 2 + img[j + 1][i] / 2;
		}
	}
	// Outline: point symmetric
	for (int i = 0; i < 512; i++){
		for (int j = 1; j < 4; j++){
			img[i][512 - j] = 2 * img[i][508] - img[i][504 + j];
		}
	}
	for (int i = 0; i < 512; i++){
		for (int j = 1; j < 4; j++){
			img[512 - j][i] = 2 * img[508][i] - img[504 + j][i];
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
