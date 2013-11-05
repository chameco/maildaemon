#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cuttle/debug.h>
#include "worldgen.h"

typedef struct noise {
	double data[WORLD_DIM][WORLD_DIM];
} noise;

double interplolate(double x0, double x1, double alpha)
{
	return x0 * (1 - alpha) + alpha * x1;
}

noise *make_white_noise()
{
	noise *ret = (noise *) malloc(sizeof(noise));
	memset(ret->data, 0, sizeof(ret->data));
	int x, y;
	for (x = 0; x < WORLD_DIM; ++x) {
		for (y = 0; y < WORLD_DIM; ++y) {
			ret->data[x][y] = ((double) rand())/RAND_MAX;
		}
	}
	return ret;
}

noise *make_smooth_noise(noise *basenoise, int octave)
{
	noise *ret = (noise *) malloc(sizeof(noise));
	memset(ret->data, 0, sizeof(ret->data));
	int sampleperiod = 1 << octave;
	double samplefrequency = 1.0 / sampleperiod;
	int x, y;
	for (x = 0; x < WORLD_DIM; ++x) {
		int sample_x0 = (x / sampleperiod) * sampleperiod;
		int sample_x1 = (sample_x0 + sampleperiod) % WORLD_DIM;
		double horizontal_blend = (x - sample_x0) * samplefrequency;
		for (y = 0; y < WORLD_DIM; ++y) {
			int sample_y0 = (y / sampleperiod) * sampleperiod;
			int sample_y1 = (sample_y0 + sampleperiod) % WORLD_DIM;
			double vertical_blend = (y - sample_y0) * samplefrequency;
			double top = interplolate(basenoise->data[sample_x0][sample_y0],
					basenoise->data[sample_x1][sample_y0], horizontal_blend);
			double bottom = interplolate(basenoise->data[sample_x0][sample_y1],
					basenoise->data[sample_x1][sample_y1], vertical_blend);
			ret->data[x][y] = interplolate(top, bottom, vertical_blend);
		}
	}
	return ret;
}

noise *make_cloudy_noise(noise *basenoise)
{
	noise *ret = (noise *) malloc(sizeof(noise));
	memset(ret->data, 0, sizeof(ret->data));
	noise *smoothnoise[6];
	for (int i = 0; i < 6; ++i) {
		smoothnoise[i] = make_smooth_noise(basenoise, i);
	}
	double amplitude = 1.0;
	double total_amplitude = 0.0;
	for (int octave = 5; octave >= 0; octave--) {
		amplitude /= 1.5;
		total_amplitude += amplitude;
		for (int x = 0; x < WORLD_DIM; ++x) {
			for (int y = 0; y < WORLD_DIM; ++y) {
				ret->data[x][y] += smoothnoise[octave]->data[x][y] * amplitude;
			}
		}
	}
	for (int x = 0; x < WORLD_DIM; ++x) {
		for (int y = 0; y < WORLD_DIM; ++y) {
			ret->data[x][y] /= total_amplitude;
		}
	}
	return ret;
}

void spit_world(world *world)
{
	FILE *f = fopen("readable_world.txt", "w");
	int x, y;
	for (y = 0; y < WORLD_DIM; ++y) {
		for (x = 0; x < WORLD_DIM; ++x) {
			fprintf(f, "%x ", world->regions[x][y]->terrain_type);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

void make_world(int seed)
{
	srand(seed);
	noise *heightmap = make_cloudy_noise(make_white_noise());
	register_standard_handlers();
	world w;
	w.player_x = 0;
	w.player_y = 0;
	char sprintf_fodder[100];
	int x, y;
	for (x = 0; x < WORLD_DIM; ++x) {
		for (y = 0; y < WORLD_DIM; ++y) {
			int temperature = rand() % 100;
			sprintf(sprintf_fodder, "%d %f", temperature, heightmap->data[x][y]);
			w.regions[x][y] = generate_region(sprintf_fodder, 10, 10,
					temperature, heightmap->data[x][y]);
		}
	}
	save_world(&w);
	spit_world(&w);
}

int main(int argc, char *argv[])
{
	int seed;
	if (argc > 1) {
		seed = atoi(argv[1]);
	} else {
		seed = time(NULL);
	}
	make_world(seed);
	return 0;
}
