#include <stdio.h>
#include <time.h>

#define FRAME_WIDTH 800
#define FRAME_HEIGHT 600
#define ITERATION_LIMIT 1000

// Function to calculate the Mandelbrot set
void calc_mandelbrot_set(int canvas[FRAME_HEIGHT][FRAME_WIDTH]) {
    for (int row = 0; row < FRAME_HEIGHT; row++) {
        for (int col = 0; col < FRAME_WIDTH; col++) {
            double real_coord = (col - FRAME_WIDTH / 2.0) * 4.0 / FRAME_WIDTH;
            double imag_coord = (row - FRAME_HEIGHT / 2.0) * 4.0 / FRAME_HEIGHT;

            double z_real = real_coord;
            double z_imag = imag_coord;

            int iterations;
            for (iterations = 0; iterations < ITERATION_LIMIT; iterations++) {
                double temp_z_real = z_real * z_real - z_imag * z_imag + real_coord;
                double temp_z_imag = 2 * z_real * z_imag + imag_coord;
                z_real = temp_z_real;
                z_imag = temp_z_imag;

                if (z_real * z_real + z_imag * z_imag > 4.0)
                    break;
            }

            canvas[row][col] = iterations;
        }
    }
}

// Function to output the Mandelbrot set to a PPM file
void output_to_ppm(const char *path, int canvas[FRAME_HEIGHT][FRAME_WIDTH]) {
    FILE *output_file = fopen(path, "wb");
    fprintf(output_file, "P6\n%d %d\n255\n", FRAME_WIDTH, FRAME_HEIGHT);

    for (int row = 0; row < FRAME_HEIGHT; row++) {
        for (int col = 0; col < FRAME_WIDTH; col++) {
            unsigned char pixel_color = (unsigned char)((canvas[row][col] % 256) * 255 / ITERATION_LIMIT);
            for (int i = 0; i < 3; i++) { // Write RGB components
                fwrite(&pixel_color, sizeof(unsigned char), 1, output_file);
            }
        }
    }

    fclose(output_file);
}

int main() {
    int render[FRAME_HEIGHT][FRAME_WIDTH];

    clock_t begin = clock(); // Start timing

    calc_mandelbrot_set(render);

    clock_t finish = clock(); // Stop timing

    double elapsed_time = (double)(finish - begin) / CLOCKS_PER_SEC;

    output_to_ppm("rendered_mandelbrot.ppm", render);

    printf("Time taken: %f seconds\n", elapsed_time);

    return 0;
}
