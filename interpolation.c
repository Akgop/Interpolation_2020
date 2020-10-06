#include <stdio.h>
# include <windows.h>
#pragma warning(disable: 4996)

void Bilinear(UCHAR img[512][512]);
void Third_Lagrange(UCHAR img[512][512]);
void Six_Tab(UCHAR img[512][512]);

int main() {
	FILE *fp_InputImg = fopen("lena(128x128).raw", "rb");

	UCHAR Input_imgbuf[128][128] = { 0, };
	UCHAR Bi_imgbuf[512][512] = { 0, };	// Bilinear Result buf
	UCHAR TL_imgbuf[512][512] = { 0, };	// Third Lagrange Result buf
	UCHAR ST_imgbuf[512][512] = { 0, };	// Six Tab Result buf

	if (!fp_InputImg) {
		printf("Cannot open file.\n");
		return 0;
	}
	// Step 1. Read lena(128x128).raw and Extend Quadraple
	fread(&Input_imgbuf[0][0], sizeof(UCHAR), 128*128, fp_InputImg);
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			Bi_imgbuf[i * 4][j * 4] = Input_imgbuf[i][j];
			TL_imgbuf[i * 4][j * 4] = Input_imgbuf[i][j];
			ST_imgbuf[i * 4][j * 4] = Input_imgbuf[i][j];
		}
	}
	fclose(fp_InputImg);
	
	// Step 2. Interpolation
	FILE *fp_Bi_OutputImg = fopen("Bi_lena(512x512).raw", "wb");
	FILE *fp_TL_OutputImg = fopen("TL_lena(512x512).raw", "wb");
	FILE *fp_ST_OutputImg = fopen("ST_lena(512x512).raw", "wb");
	// Step 2-1. Bilinear Algorithm
	Bilinear(Bi_imgbuf);
	fwrite(&Bi_imgbuf[0][0], sizeof(UCHAR), 512 * 512, fp_Bi_OutputImg);

	// Step 2-2. Third Lagrange Algorithm
	Third_Lagrange(TL_imgbuf);
	fwrite(&TL_imgbuf[0][0], sizeof(UCHAR), 512 * 512, fp_TL_OutputImg);

	// Step 2-3. Six Tab Algorithm
	Six_Tab(ST_imgbuf);
	fwrite(&ST_imgbuf[0][0], sizeof(UCHAR), 512 * 512, fp_ST_OutputImg);


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
	for (int i = 0; i < 512; i++)
	{
		for (int j = 1; j < 4; j++)
		{
			img[i][512 - j] = 2 * img[i][512 - 4] - img[i][512 - 2 * 4 + j];
			img[512 - j][i] = 2 * img[512 - 4][i] - img[512 - 2 * 4 + j][i];
		}
	}
}

void Third_Lagrange(UCHAR img[512][512]) {

}

void Six_Tab(UCHAR img[512][512]) {

}
