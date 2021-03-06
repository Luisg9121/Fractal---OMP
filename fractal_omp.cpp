/*
Fractal code for CS 4380 / CS 5351

Copyright (c) 2016, Texas State University. All rights reserved.

Redistribution in source or binary form, with or without modification,
is not permitted. Use in source and binary forms, with or without
modification, is only permitted for academic use in CS 4380 or CS 5351
at Texas State University.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Author: Martin Burtscher and Luis Gomez
*/

#include <cstdlib>
#include <sys/time.h>
#include "cs43805351.h"
#include <math.h>

static const double Delta = 0.005491;
static const double xMid = 0.745796;
static const double yMid = 0.105089;

int main(int argc, char *argv[])
{
  printf("\n\nFractal v1.5 [OpenMP]\n");

  // check command line
  if (argc != 4) {
     fprintf(stderr, "usage: %s frame_width num_frames\n", argv[0]); 
     exit(-1);
     }

  int width = atoi(argv[1]);

  if (width < 10){
     fprintf(stderr, "error: frame_width must be at least 10\n"); 
     exit(-1);
     }
  int frames = atoi(argv[2]);

  if (frames < 1){
      fprintf(stderr, "error: num_frames must be at least 1\n"); 
      exit(-1);
      }

  printf("computing %d frames of %d by %d fractal\n", frames, width, width);

  int num_threads = atoi(argv[3]);

  if(num_threads < 1){
    printf("Number of threads must be at least 1\n"); 
    exit(-1);
    }
 
  printf("Suggested number of threads to run %d\n", num_threads);

  // allocate picture array
  unsigned char* pic = new unsigned char[frames * width * width];

  // start time
  struct timeval start, end;
  gettimeofday(&start, NULL);

  double delta = Delta;
  int frame, row, col, depth;
  double x, y, x2, y2;
 
  //compute frames
  #pragma omp parallel for num_threads(num_threads) \
  default(none) private(delta, frame, row, col, x, y, depth, x2, y2) \
  shared(frames, width, pic)
  for (frame = 0; frame < frames; frame++) {
    delta = Delta * pow(0.99, frame+1);
    const double xMin = xMid - delta;
    const double yMin = yMid - delta;
    const double dw = 2.0 * delta / width;
 
     for (row = 0; row < width; row++) {
      const double cy = -yMin - row * dw;

      for (col = 0; col < width; col++) {
        const double cx = -xMin - col * dw;
        x = cx;
        y = cy;
        depth = 256;

        do {
          x2 = x * x;
          y2 = y * y;
          y = 2 * x * y + cy;
          x = x2 - y2 + cx;
          depth--;

        } while ((depth > 0) && ((x2 + y2) < 5.0))
        pic[frame * width * width + row * width + col] = (unsigned char)depth;
      }
    }
  }
  // end time
  gettimeofday(&end, NULL);

  //display compute time
  double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
  printf("compute time: %.4f s\n", runtime);

  // verify result by writing frames to BMP files
  if ((width <= 400) && (frames <= 30)) {
    for (int frame = 0; frame < frames; frame++) {
         char name[32];
         sprintf(name, "fractal%d.bmp", frame + 1000);
         writeBMP(width, width, &pic[frame * width * width], name);
    }
  }

  delete [] pic;

  return 0;
}

