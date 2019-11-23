#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	/*
	Structura de pixel
	*/
	unsigned char r;
	unsigned char g;
	unsigned char b;
} pixel;

long long min2(long long l1, long long l2) {
	if (l1 < l2)
		return l1;
	return l2;
}

long long min3(long long l1, long long l2, long long l3) {
	return min2(l1, min2(l2, l3));
}

long long dual_gradient_energy(pixel p1x, pixel p2x, pixel p1y, pixel p2y) {
	/*
	Functie care calculeaza energia unui pixel in functie de vecinii sai:
	p1x, p2x - vecinii din stanga si din dreapta sa
	p1y, p2y - vecinii de deasupra si de dedesubtul sau
	*/
	pixel xdif;
	xdif.r = abs(p1x.r - p2x.r);
	xdif.g = abs(p1x.g - p2x.g);
	xdif.b = abs(p1x.b - p2x.b);

	pixel ydif;
	ydif.r = abs(p1y.r - p2y.r);
	ydif.g = abs(p1y.g - p2y.g);
	ydif.b = abs(p1y.b - p2y.b);

	long long xgrad = xdif.r * xdif.r + xdif.g * xdif.g + xdif.b * xdif.b;
	long long ygrad = ydif.r * ydif.r + ydif.g * ydif.g + ydif.b * ydif.b;

	return xgrad + ygrad;
}

int main(int argc, char* argv[]) {
	/*
	===============================================================================================
	Citire fisier de intrare
	*/

	unsigned char type;
	unsigned int w, h, maxval;
	int i, j, k;

	FILE *f = fopen(argv[1], "r");
	fscanf(f, "P6\n");
	fscanf(f, "%d %d\n", &w, &h);
	fscanf(f, "%d\n", &maxval);

	pixel **color_mat = malloc(h * sizeof(pixel *));
	for (i = 0; i < h; i++)
		color_mat[i] = malloc(w * sizeof(pixel));

	for (i = 0; i < h; i++)
		fread(color_mat[i], 3 * w, 1, f);
	fclose(f);

	/*
	===============================================================================================
	Reducem latimea sau inaltimea imaginii
	*/
	if (strcmp(argv[3], "width") == 0) {
		int to_be_removed = atoi(argv[4]);

		for (k = 0; k < to_be_removed; k++) {

			/*
			=======================================================================================
			Calculam energia fiecarui pixel
			*/
			long long energy_matrix[h][w];

			for (i = 0; i < h; i++)
				for (j = 0; j < w; j++) {
					long energy;
					/* 
					Colturi
					*/
					if (i == 0 && j == 0)
						energy = dual_gradient_energy(color_mat[i][w - 1], color_mat[i][j + 1], color_mat[h - 1][j], color_mat[i + 1][j]);
					else if (i == 0 && j == w - 1)
						energy = dual_gradient_energy(color_mat[i][j - 1], color_mat[i][0], color_mat[h - 1][j], color_mat[i + 1][j]);
					else if (i == h - 1 && j == 0)
						energy = dual_gradient_energy(color_mat[i][w - 1], color_mat[i][j + 1], color_mat[i - 1][j], color_mat[0][j]);
					else if (i == h - 1 && j == w - 1)
						energy = dual_gradient_energy(color_mat[i][j - 1], color_mat[i][0], color_mat[i - 1][j], color_mat[0][j]);
					/*
					Margini
					*/
					else if (i == 0)
						energy = dual_gradient_energy(color_mat[i][j - 1], color_mat[i][j + 1], color_mat[h - 1][j], color_mat[i + 1][j]);
					else if (i == h - 1)
						energy = dual_gradient_energy(color_mat[i][j - 1], color_mat[i][j + 1], color_mat[i - 1][j], color_mat[0][j]);
					else if (j == 0)
						energy = dual_gradient_energy(color_mat[i][w - 1], color_mat[i][j + 1], color_mat[i - 1][j], color_mat[i + 1][j]);
					else if (j == w - 1)
						energy = dual_gradient_energy(color_mat[i][j - 1], color_mat[i][0], color_mat[i - 1][j], color_mat[i + 1][j]);
					/*
					Interior
					*/
					else
						energy = dual_gradient_energy(color_mat[i][j - 1], color_mat[i][j + 1], color_mat[i - 1][j], color_mat[i + 1][j]);
					energy_matrix[i][j] = energy;
				}


			/*
			=======================================================================================
			Determinam energiile seam-urilor prin programare dinamica
			*/
			long long dp[h][w];
			int vertical_seam[h];
			long long min_energy;

			for (i = 1; i < h; i++)
				for (j = 0; j < w; j++)
					if (j == 0)
						dp[i][j] = energy_matrix[i][j] + min2(dp[i - 1][j], dp[i - 1][j + 1]);
					else if (j == w - 1)
						dp[i][j] = energy_matrix[i][j] + min2(dp[i - 1][j - 1], dp[i - 1][j]);
					else
						dp[i][j] = energy_matrix[i][j] + min3(dp[i - 1][j - 1], dp[i - 1][j], dp[i - 1][j + 1]);

			/*
			=======================================================================================
			Determinam seam-ul vertical ce trebuie eliminat
			*/
			min_energy = dp[h - 1][0];
			for (j = 0; j < w; j++)
				if (dp[h - 1][j] < min_energy) {
					min_energy = dp[h - 1][j];
					vertical_seam[h - 1] = j;
				}

			for (i = h - 2; i >= 0; i--) {
				long long remaining = dp[i + 1][vertical_seam[i + 1]] - energy_matrix[i + 1][vertical_seam[i + 1]];
				if (vertical_seam[i + 1] == 0) {
					if (dp[i][vertical_seam[i + 1]] == remaining) {
						vertical_seam[i] = vertical_seam[i + 1];
					} else if (dp[i][vertical_seam[i + 1] + 1] == remaining) {
						vertical_seam[i] = vertical_seam[i + 1] + 1;
					}
				} else if (vertical_seam[i + 1] == w - 1) {
					if (dp[i][vertical_seam[i + 1] - 1] == remaining) {
						vertical_seam[i] = vertical_seam[i + 1] - 1;
					} else if (dp[i][vertical_seam[i + 1]] == remaining) {
						vertical_seam[i] = vertical_seam[i + 1];
					}
				} else {
					if (dp[i][vertical_seam[i + 1] - 1] == remaining) {
						vertical_seam[i] = vertical_seam[i + 1] - 1;
					} else if (dp[i][vertical_seam[i + 1]] == remaining) {
						vertical_seam[i] = vertical_seam[i + 1];
					} else if (dp[i][vertical_seam[i + 1] + 1] == remaining) {
						vertical_seam[i] = vertical_seam[i + 1] + 1;
					}
				}
			}

			/*
			=======================================================================================
			Eliminam seam-ul vertical si scadem latimea imaginii
			*/
			/*for (i = 0; i < h; i++)
				printf("%d ", vertical_seam[i]);
			printf("\n");*/

			for (i = 0; i < h; i++) {
				pixel *new_line = malloc((w - 1) * sizeof(pixel));
				int skipped = 0;
				for (j = 0; j < w; j++) {
					if (vertical_seam[i] == j)
						skipped = 1;
					else
						new_line[j - skipped] = color_mat[i][j];
				}
				free(color_mat[i]);
				color_mat[i] = new_line;
			}
			w -= 1;
		}
	}

	/*
	===============================================================================================
	Scriere fisier de output
	*/
	f = fopen(argv[2], "w");

	fprintf(f, "P6\n%d %d\n%d\n", w, h, maxval);
	for (i = 0; i < h; i++)
		fwrite(color_mat[i], 3 * w, 1, f);
	fclose(f);

	for (i = 0; i < h; i++)
		free(color_mat[i]);
	free(color_mat);

	return 0;
}