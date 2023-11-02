
#include "log.h"
#include "tile_config.h"
#include "benchmark_helper.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"


/*
    This implementation tried to write to file directly by editing it
    result it is the slowest of all options
*/

static bool is_file_open = false;
long header_length;
FILE *fp ;
int writeFilesToImage(
    const char* filePath,
    CanvasConfig* canvasConfig,
    TileConfig* tileConfig,
    void** tileMap
) {
    log_mes("createImage");

clockStart();
    int tilesInRow = canvasConfig->image.width / canvasConfig->tile.width;
    int tilesInCol = canvasConfig->image.height / canvasConfig->tile.height;

    const int dimx = canvasConfig->image.width;
    const int dimy = canvasConfig->image.height;

    const int MaxColorValue = 255; // Maximum color value
    if (!is_file_open) {
        const char *comment = "# This is my new binary PPM file";
        fp = fopen(filePath, "wb"); // Open the file for writing in binary mode

        if (fp == NULL) {
            printf("Error opening file for writing\n");
            return -1;
        }
        char *header_text = (char*)malloc(255 * sizeof(char));
        // We write text in to buffer (sprintf would add null termination)
        sprintf(header_text, "P6\n%s\n%d %d\n%d\n", comment, dimx, dimy, MaxColorValue);
        
        // for some reason first character is corrupted
        // now we have proper text with proper size
        header_length = strlen(header_text);
        fprintf(fp, header_text);
    }
    // Clearing the image
    unsigned char colorData[3] = { 254, 254, 254 };
    for (int y = 0; y < dimy; ++y) {
        for (int x = 0; x < dimx; ++x) {
            fwrite(colorData, 1, 3, fp);
        }
    }
    for (int i = 0; i < tilesInCol; i++) {
        for (int j = 0; j < tilesInRow; j++) {
            int idx = i * tilesInRow + j;
            void* tile = tileMap[idx];

            if (tile == NULL) {
                printf("PANIC tile is NULL\n");
                fclose(fp); // Close the file before returning
                return -1;
            }
            TileImage* tileImage = tileConfig->getTileImage(
                tile,
                canvasConfig->tile.width,
                canvasConfig->tile.height
            );

            if (tileImage == NULL) {
                printf("PANIC tileImage is NULL\n");
                fclose(fp); // Close the file before returning
                return -1;
            }

            int draw_x = j * canvasConfig->tile.width;
            int draw_y = i * canvasConfig->tile.height;

            for (int y = 0; y < canvasConfig->tile.height; y++) {
                for (int x = 0; x < canvasConfig->tile.width; x++) {
                    int cellIdx = y * canvasConfig->tile.width + x;
                    int image_y = y + draw_y;
                    int image_x = x + draw_x;
                    long position = (image_y * dimx * 3) + (image_x * 3) + header_length;

                    fseek(fp, position, SEEK_SET);
                    unsigned char colorData[3] = {
                        tileImage->cells[cellIdx].red   ,
                        tileImage->cells[cellIdx].green ,
                        tileImage->cells[cellIdx].blue
                    };
                    fwrite(colorData, 1, 3, fp);
                }
            }
        }
    }
    
    //fclose(fp);
    fflush(fp);
    printf("OK - File %s saved\n", filePath);
clockEnd("Image data creation");

    return 0;
}
