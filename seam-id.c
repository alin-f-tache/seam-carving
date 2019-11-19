#include <stdio.h>

long min2(long l1, long l2) {
	if (l1 < l2)
		return l1;
	return l2;
}

long min3(long l1, long l2, long l3) {
	return min2(l1, min2(l2, l3));
}

int main() {
	/*
	MATRICEA DE ENERGII
	Calea minima ar trebui sa fie 3 4 3 2 2
	*/
	long mat[5][6] = {
		{57685, 50893, 91370, 25418, 33055, 37246},
		{15421, 56334, 22808, 54796, 11641, 25496},
		{12344, 19236, 52030, 17708, 44735, 20663},
		{17074, 23678, 30279, 80663, 37831, 45595},
		{32337, 30796, 4909, 73334, 40613, 36556}
	};

	int h = 5;
	int w = 6;
	int i, j;

	/*
	Calcul energie minima pt fiecare pixel incepand cu linia de sus
	*/
	long dp[h][w];
	for (j = 0; j < w; j++)
		dp[0][j] = mat[0][j];

	for (i = 1; i < h; i++)
		for (j = 0; j < w; j++)
			if (j == 0)
				dp[i][j] = mat[i][j] + min2(dp[i - 1][j], dp[i - 1][j + 1]);
			else if (j == w - 1)
				dp[i][j] = mat[i][j] + min2(dp[i - 1][j - 1], dp[i - 1][j]);
			else
				dp[i][j] = mat[i][j] + min3(dp[i - 1][j - 1], dp[i - 1][j], dp[i - 1][j + 1]);


	/*
	Identificare energia minima a unui seam din ultima linie a matricei dp
	*/
	int vertical_seam[h];
	long minenergy = 2000000;

	for (j = 0; j < w; j++)
		if (dp[h - 1][j] < minenergy) {
			minenergy = dp[h - 1][j];
			vertical_seam[h - 1] = j;
		}

	/*
	Reconstructia seam-ului cu energie minima pornind de jos in sus
	*/
	for (i = h - 2; i >= 0; i--) {
		long remaining = dp[i + 1][vertical_seam[i + 1]] - mat[i + 1][vertical_seam[i + 1]];
		if (vertical_seam[i + 1] == 0) {
			if (dp[i][vertical_seam[i + 1]] == remaining)
				vertical_seam[i] = vertical_seam[i + 1];
			else if (dp[i][vertical_seam[i + 1] + 1] == remaining)
				vertical_seam[i] = vertical_seam[i + 1] + 1;
		} else if (vertical_seam[i + 1] == w - 1) {
			if (dp[i][vertical_seam[i + 1] - 1] == remaining)
				vertical_seam[i] = vertical_seam[i + 1] - 1;
			else if (dp[i][vertical_seam[i + 1]] == remaining)
				vertical_seam[i] = vertical_seam[i + 1];
		} else {
			if (dp[i][vertical_seam[i + 1] - 1] == remaining)
				vertical_seam[i] = vertical_seam[i + 1] - 1;
			else if (dp[i][vertical_seam[i + 1]] == remaining)
				vertical_seam[i] = vertical_seam[i + 1];
			else if (dp[i][vertical_seam[i + 1] + 1] == remaining)
				vertical_seam[i] = vertical_seam[i + 1] + 1;
		}
	}

	for (i = 0; i < h; i++)
		printf("%d ", vertical_seam[i]);
	printf("\n");

	return 0;
}