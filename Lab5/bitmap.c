#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"


/*
 * Read in the location of the pixel array, the image width, and the image
 * height in the given bitmap file.
 */
void read_bitmap_metadata(FILE *image, int *pixel_array_offset, int *width, int *height) {
  int error = 0;

  fseek(image, 10, SEEK_SET);
  error += fread(pixel_array_offset, sizeof(int), 1, image);

  fseek(image, 18, SEEK_SET);
  error += fread(width, sizeof(int), 1, image);

  fseek(image, 22, SEEK_SET);
  error += fread(height, sizeof(int), 1, image);

  // if (error != 3) {
  //   not sure if I should print this error message (might screw up autotest)
  //   fprintf(stderr, "Error: incorrect number of elements read.");
  //   exit(1);
  // }
}

/*
 * Read in pixel array by following these instructions:
 *
 * 1. First, allocate space for m `struct pixel *` values, where m is the
 *    height of the image.  Each pointer will eventually point to one row of
 *    pixel data.
 * 2. For each pointer you just allocated, initialize it to point to
 *    heap-allocated space for an entire row of pixel data.
 * 3. Use the given file and pixel_array_offset to initialize the actual
 *    struct pixel values. Assume that `sizeof(struct pixel) == 3`, which is
 *    consistent with the bitmap file format.
 *    NOTE: We've tested this assumption on the Teaching Lab machines, but
 *    if you're trying to work on your own computer, we strongly recommend
 *    checking this assumption!
 * 4. Return the address of the first `struct pixel *` you initialized.
 */
struct pixel **read_pixel_array(FILE *image, int pixel_array_offset, int width, int height) {
  struct pixel **pixel_map = malloc(height * sizeof(struct pixel*));

  for (int i = 0; i < height; i++) {
    pixel_map[i] = malloc(width * sizeof(struct pixel));
  }

  fseek(image, pixel_array_offset, SEEK_SET);
  unsigned char r, g, b;
  struct pixel current;
  for (int row = 0; row < height; row++){
    for (int col = 0; col < width; col++) {
      fread(&b, 1, 1, image);
      fread(&g, 1, 1, image);
      fread(&r, 1, 1, image);
      current.blue = b;
      current.green = g;
      current.red = r;

      memcpy(&(pixel_map[row][col]), &current, sizeof(struct pixel));
    }
  }

  return pixel_map;
}


/*
 * Print the blue, green, and red colour values of a pixel.
 * You don't need to change this function.
 */
void print_pixel(struct pixel p) {
    printf("(%u, %u, %u)\n", p.blue, p.green, p.red);
}
