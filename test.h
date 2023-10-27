#include "tiles.h"
#include <stdlib.h>
#include <stdio.h>

int create_image(Tile tile)
{
  const int dimx = tile.tile_size, dimy =  tile.tile_size;
  int i, j;
  FILE *fp = fopen("first.ppm", "wb"); /* b - binary mode */
  (void) fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
  for (j = 0; j < dimy; ++j)
  {
    for (i = 0; i < dimx; ++i)
    {
      static unsigned char color[3];
      int idx = j*dimy + i;
      color[0] =  tile.cells[idx].red ;  /* red */
      color[1] =  tile.cells[idx].green;  /* green */
      color[2] = tile.cells[idx].blue;  /* blue */
      (void) fwrite(color, 1, 3, fp);
    }
  }
  (void) fclose(fp);
  return EXIT_SUCCESS;
}

void pring(Tile tile)
{
    const int dimx = tile.tile_size, dimy =  tile.tile_size;
    int i, j;
    for (j = 0; j < dimy; ++j)
    {
        for (i = 0; i < dimx; ++i)
        {
            printf("color %d,%d,%d  | ",
                tile.cells[i].red ,  /* red */
                tile.cells[i].green,  /* green */
                tile.cells[i].blue  /* blue */
            );
        }
        printf("\n");
    }
}


int generate_single_tile(){
    Cell cell = { .top = COLOR_IDX_RED, .bottom =COLOR_IDX_BLUE , .left=COLOR_IDX_GREEN, .right=COLOR_IDX_YELLOW };
    Tile tile = generate_tile(100, cell);

    create_image(tile);
    return 0;
}