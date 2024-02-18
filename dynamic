#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>

// Define dimensions and iteration limit
#define IMG_WIDTH 800
#define IMG_HEIGHT 600
#define ITER_LIMIT 1000

// Function to create the Mandelbrot set
void create_mandelbrot(int start, int finish, int* segment) {
    int row, col, iter;
    int pos = 0;
    double x0, y0, x, y, xtemp;
    
    for (row = start; row < finish; row++) {
        for (col = 0; col < IMG_WIDTH; col++) {
            x0 = (col - IMG_WIDTH / 2.0) * 4.0 / IMG_WIDTH;
            y0 = (row - IMG_HEIGHT / 2.0) * 4.0 / IMG_HEIGHT;

            x = 0;
            y = 0;

            for (iter = 0; iter < ITER_LIMIT; iter++) {
                double xx = x * x, yy = y * y;
                if (xx + yy > 4.0) break;
                xtemp = xx - yy + x0;
                y = 2 * x * y + y0;
                x = xtemp;
            }

            segment[pos++] = iter;
        }
    }
}

// Function to save the image
void export_ppm(const char *file_name, int buffer[IMG_HEIGHT][IMG_WIDTH]) {
    FILE *fp = fopen(file_name, "wb");
    fprintf(fp, "P6\n%d %d\n255\n", IMG_WIDTH, IMG_HEIGHT);

    for (int i = 0; i < IMG_HEIGHT; i++) {
        for (int j = 0; j < IMG_WIDTH; j++) {
            unsigned char color = (unsigned char)((buffer[i][j] % 256) * 255 / ITER_LIMIT);
            for (int k = 0; k < 3; k++) { // Repeat color for RGB
                fwrite(&color, 1, 1, fp);
            }
        }
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, procs, segment_size, start, finish;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    int partition_height = IMG_HEIGHT / procs;
    start = rank * partition_height;
    finish = (rank + 1) * partition_height;

    int image[IMG_HEIGHT][IMG_WIDTH];
    segment_size = partition_height * IMG_WIDTH;
    int segment[segment_size];

    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    MPI_Request req_send, req_recv;
    MPI_Status status_send, status_recv;

    create_mandelbrot(start, finish, segment);

    MPI_Isend(segment, segment_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &req_send);
    MPI_Irecv(&image[start][0], segment_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &req_recv);

    MPI_Wait(&req_send, &status_send);
    MPI_Wait(&req_recv, &status_recv);

    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec) / 1e6;

    if (rank == 0) {
        export_ppm("result_mandelbrot.ppm", image);
        printf("Total time: %f seconds\n", time_taken);
    }

    MPI_Finalize();
    return 0;
}
