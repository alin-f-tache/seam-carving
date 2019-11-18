#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} pixel;

int main(int argc, char* argv[]) {
	unsigned char type;
	unsigned int w, h, maxval;
	int i;

	FILE *f = fopen(argv[1], "r");
	fscanf(f, "P%c\n", &type);
	fscanf(f, "%d %d\n", &w, &h);
	fscanf(f, "%d\n", &maxval);
	printf("%d %d %d\n", w, h, maxval);
	fflush(stdout);

	pixel color_mat[h][w];
	pixel new_color_mat[h][w];
	unsigned char gs_mat[h][w];
	unsigned char new_gs_mat[h][w];

	if (type == '6')
		for (i = 0; i < h; i++)
			fread(color_mat[i], 3 * w, 1, f);
	else
		for (i = 0; i < h; i++)
			fread(gs_mat[i], w, 1, f);
	fclose(f);


	f = fopen(argv[2], "w");

	fprintf(f, "P%c\n%d %d\n%d\n", type, w, h, maxval);
	if (type == '6')
		for (i = 0; i < h; i++)
			fwrite(color_mat[i], 3 * w, 1, f);
	else
		for (i = 0; i < h; i++)
			fwrite(gs_mat[i], w, 1, f);
	fclose(f);

	return 0;
}