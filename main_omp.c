#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

typedef struct {
	/*
	===============================================================================================
	Structura de pixel
	*/
	unsigned char r;
	unsigned char g;
	unsigned char b;
} pixel;

long min2(long l1, long l2) {
	if (l1 < l2)
		return l1;
	return l2;
}

long min3(long l1, long l2, long l3) {
	return min2(l1, min2(l2, l3));
}

long dual_gradient_energy(pixel p1x, pixel p2x, pixel p1y, pixel p2y) {
	/*
	===============================================================================================
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

	long xgrad = xdif.r * xdif.r + xdif.g * xdif.g + xdif.b * xdif.b;
	long ygrad = ydif.r * ydif.r + ydif.g * ydif.g + ydif.b * ydif.b;

	return xgrad + ygrad;
}

long **generate_energy_matrix(int h, int w, pixel **color_mat) {
	/*
	===============================================================================================
	Functie care genereaza matricea cu energiile fiecarui pixel
	*/
	int i, j;
	long **energy_matrix = malloc(h * sizeof(long *));
	long energy;
	for (i = 0; i < h; i++)
		energy_matrix[i] = malloc(w * sizeof(long));
	#pragma omp parallel for shared(energy_matrix,color_mat,h,w) private(i,j,energy)
	
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++) {
			
			/* 
			Colturi
			*/
			if (i == 0 && j == 0) {
				energy = dual_gradient_energy(color_mat[i][w - 1], color_mat[i][j + 1], color_mat[h - 1][j], color_mat[i + 1][j]);
			}
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
	return energy_matrix;
}

long **generate_seam_energies(int h, int w, long **energy_matrix) {
	/*
	===============================================================================================
	Functie care genereaza energiile tuturor seam-urilor prin programare dinamica
	*/
	int i, j;
	long **dp = malloc(h * sizeof(long *));
	for (i = 0; i < h; i++)
		dp[i] = malloc(w * sizeof(long));
	#pragma omp parallel for shared(energy_matrix,dp,h,w) private(i,j)
	
	for (i = 1; i < h; i++)
		for (j = 0; j < w; j++)
			if (j == 0)
				dp[i][j] = energy_matrix[i][j] + min2(dp[i - 1][j], dp[i - 1][j + 1]);
			else if (j == w - 1)
				dp[i][j] = energy_matrix[i][j] + min2(dp[i - 1][j - 1], dp[i - 1][j]);
			else
				dp[i][j] = energy_matrix[i][j] + min3(dp[i - 1][j - 1], dp[i - 1][j], dp[i - 1][j + 1]);

	return dp;
}

int *determine_min_seam(int h, int w, long **dp) {
	/*
	===============================================================================================
	Functie care determina seam-ul minim ce trebuie sters din imagine
	*/
	int i, j;
	int *vertical_seam = malloc(h * sizeof(int));
	long min_energy = 2000000;

	min_energy = dp[h - 1][0];
	for (j = 0; j < w; j++)
		if (dp[h - 1][j] < min_energy) {
			min_energy = dp[h - 1][j];
			vertical_seam[h - 1] = j;
		}

	for (i = h - 2; i >= 0; i--) {
		if (vertical_seam[i + 1] == 0) {
			if (dp[i][vertical_seam[i + 1]] < dp[i][vertical_seam[i + 1] + 1]) {
				vertical_seam[i] = vertical_seam[i + 1];
			} else {
				vertical_seam[i] = vertical_seam[i + 1] + 1;
			}
		} else if (vertical_seam[i + 1] == w - 1) {
			if (dp[i][vertical_seam[i + 1] - 1] < dp[i][vertical_seam[i + 1]]) {
				vertical_seam[i] = vertical_seam[i + 1] - 1;
			} else {
				vertical_seam[i] = vertical_seam[i + 1];
			}
		} else {
			if (dp[i][vertical_seam[i + 1] - 1] < dp[i][vertical_seam[i + 1]] && dp[i][vertical_seam[i + 1] - 1] < dp[i][vertical_seam[i + 1] + 1]) {
				vertical_seam[i] = vertical_seam[i + 1] - 1;
			} else if (dp[i][vertical_seam[i + 1]] < dp[i][vertical_seam[i + 1] + 1]) {
				vertical_seam[i] = vertical_seam[i + 1];
			} else {
				vertical_seam[i] = vertical_seam[i + 1] + 1;
			}
		}
	}

	return vertical_seam;
}

int main(int argc, char* argv[]) {
	/*
	===============================================================================================
	Citire fisier de intrare
	*/
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
			long **energy_matrix = generate_energy_matrix(h, w, color_mat);

			/*
			=======================================================================================
			Determinam energiile seam-urilor prin programare dinamica
			*/
			long **dp = generate_seam_energies(h, w, energy_matrix);

			/*
			=======================================================================================
			Determinam seam-ul vertical ce trebuie eliminat
			*/
			int *vertical_seam = determine_min_seam(h, w, dp);

			/*
			=======================================================================================
			Eliminam seam-ul vertical si scadem latimea imaginii
			*/
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

			/*
			=======================================================================================
			Clean-up
			*/
			free(vertical_seam);
			for (i = 0; i < h; i++) {
				free(energy_matrix[i]);
				free(dp[i]);
			}
			free(energy_matrix);
			free(dp);
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