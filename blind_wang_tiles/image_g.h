#include <stdlib.h>
#include <stdio.h>
#include "tiles.h"

#define FILE_PATH "blind_wang_tiles/wang.ppm"

int create_image(int width, int height, Tile *tiles, int tileSize){
    const int dimx  = width, dimy = height;
    int tiles_in_col = dimy/tileSize; 
    int tiles_in_row = dimx/tileSize;
    int i, j;
    FILE *fp = fopen(FILE_PATH, "wb"); /* b - binary mode */
    (void) fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
    for (j = 0; j < dimy; ++j)
    {
        int jIdx = j/tileSize;
        for (i = 0; i < dimx; ++i)
        {
            int iIdx = i/tileSize;
            int tileIdx = jIdx*tiles_in_row + iIdx; 
            int cellIdx = (j%tileSize)* tileSize + (i%tileSize); 
            // printf("tiles_in_col:%d, tiles_in_row:%d\n", tiles_in_col, tiles_in_row);
            // printf("jIdx:%d, iIdx:%d, tile idx:%d, cellIdx:%d\n", jIdx,iIdx, tileIdx, cellIdx);
            // if(tileIdx >= tiles_in_col*tiles_in_row) {
            //     printf("skipping the tile\n");
            //     continue;
            // }

            static unsigned char color[3];
            color[0] = tiles[tileIdx].cells[cellIdx].red;  /* red */
            color[1] = tiles[tileIdx].cells[cellIdx].green;  /* green */
            color[2] = tiles[tileIdx].cells[cellIdx].blue;  /* blue */
            (void) fwrite(color, 1, 3, fp);
        }
    }
    (void) fclose(fp);
    return EXIT_SUCCESS;
}