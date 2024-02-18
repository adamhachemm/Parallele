#include <stdio.h>
#include <mpi.h>

#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600
#define ITERATIONS_MAX 1000

// Function to compute Mandelbrot set and fill the part of the canvas
void compute_mandelbrot(int part[CANVAS_HEIGHT][CANVAS_WIDTH], int lower_bound, int upper_bound) {
    int row, col;
    for (row = lower_bound; row < upper_bound; row++) {
        for (col = 0; col < CANVAS_WIDTH; col++) {
            double c_real = (col - CANVAS_WIDTH / 2.0) * 4.0 / CANVAS_WIDTH;
            double c_imag = (row - CANVAS_HEIGHT / 2.0) * 4.0 / CANVAS_HEIGHT;
            double x = 0, y = 0, x_temp;
            int n;
            for (n = 0; n < ITERATIONS_MAX; n++) {
                x_temp = x*x - y*y + c_real;
                y = 2*x*y + c_imag;
                x = x_temp;
                if (x*x + y*y > 4.0) break;
            }
            part[row][col] = n;
        }
    }
}

// Function to write the computed Mandelbrot set to a PPM file
void write_to_ppm(const char *path, int canvas[CANVAS_HEIGHT][CANVAS_WIDTH]) {
    FILE *output = fopen(path, "wb");
    fprintf(output, "P6\n%d %d\n255\n", CANVAS_WIDTH, CANVAS_HEIGHT);

    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            unsigned char colors[3];
            colors[0] = (unsigned char)((canvas[i][j] % 256) * 2);    // Modify color intensity
            colors[1] = (unsigned char)((canvas[i][j] % 256) * 1.5);  // for visual effect
            colors[2] = (unsigned char)((canvas[i][j] % 256) * 1);    // Different shades
            fwrite(colors, sizeof(unsigned char), 3, output);
        }
    }

    fclose(output);
}

// Main program to distribute work and gather results
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int process_id, total_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);

    int segment_height = CANVAS_HEIGHT / total_processes;
    int start = process_id * segment_height;
    int end = (process_id + 1) * segment_height;

    int segment[CANVAS_HEIGHT][CANVAS_WIDTH];

    double timing_start = MPI_Wtime();

    compute_mandelbrot(segment, start, end);

    double timing_end = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes reach this point

    // Collect and compile the segments of the Mandelbrot set
    if (process_id == 0) {
        for (int proc = 1; proc < total_processes; proc++) {
            MPI_Recv(&segment[proc * segment_height][0], segment_height * CANVAS_WIDTH, MPI_INT, proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        printf("Total computation time: %f seconds\n", (timing_end - timing_start));
        write_to_ppm("complete_mandelbrot.ppm", segment);
    } else {
        MPI_Send(&segment[start][0], segment_height * CANVAS_WIDTH, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
