
#include "log.h"
#include "tile_config.h"
#include "benchmark_helper.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"

/*
    This implementation tries to write whole array at once
    seems the fastest way to write data to file
*/
int writeFilesToImage(
    const char* filePath,
    CanvasConfig* canvasConfig,
    TileConfig* tileConfig,
    void** tileMap
) {
    // double time_spent;
    // clock_t begin, end;

    log_mes("createImage");
    int tilesInRow = canvasConfig->image.width  / canvasConfig->tile.width;
    int tilesInCol = canvasConfig->image.height / canvasConfig->tile.height;

    const int dimx = canvasConfig->image.width;
    const int dimy = canvasConfig->image.height;

    unsigned char data[IMAGE_HEIGHT * IMAGE_WIDTH*3];
    int i, j;

clockStart();

    for(int i = 0; i < tilesInCol; i++) {
        for(int j = 0; j < tilesInRow; j++) {
            int idx = i*tilesInRow +j;
            void* tile = tileMap[idx];
            
            if(tile == NULL) {
                printf("PANIC tile is NULL\n");
                return -1;
            }
            TileImage* tileImage = tileConfig->getTileImage(
                tile, 
                canvasConfig->tile.width,
                canvasConfig->tile.height
            );
            if(tileImage == NULL){
                printf("PANIC tileImage is NULL\n");
                return -1;
            }

            int draw_x = j* canvasConfig->tile.width;
            int draw_y = i* canvasConfig->tile.height;

            for(int y = 0; y < canvasConfig->tile.height; y++){
                for(int x = 0; x < canvasConfig->tile.width; x++)
                {
                    int cellIdx = y*canvasConfig->tile.width + x; 

                    int red     = tileImage->cells[cellIdx].red;  
                    int green   = tileImage->cells[cellIdx].green;
                    int blue    = tileImage->cells[cellIdx].blue; 

                    data[(y+draw_y)*dimx*3 + (x+draw_x)*3     ] = tileImage->cells[cellIdx].red;    /* red */
                    data[(y+draw_y)*dimx*3 + (x+draw_x)*3  + 1] = tileImage->cells[cellIdx].green;  /* green */
                    data[(y+draw_y)*dimx*3 + (x+draw_x)*3  + 2] = tileImage->cells[cellIdx].blue;   /* blue */
                }
            }
        
        }
    }
clockEnd("Image data creation");

clockRestart();

    const int MaxColorValue = 255; // Maximum color value
    const char *comment = "# This is my new binary PPM file";
    /* Create a new file, give it a name, and open it in binary mode */
    FILE *fp = fopen(filePath, "wb");

    /* Write header to the file (P6 format) */
    fprintf(fp, "P6\n%s\n%d %d\n%d\n", comment, dimx, dimy, MaxColorValue);

    fwrite(data, 1, dimx*dimy*3, fp);
    fclose(fp);
    printf("OK - File %s saved\n", filePath);

clockEnd("Image file creation");

    return EXIT_SUCCESS;
}
