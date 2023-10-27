#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "tile_config.h"
#include "log.h"

#define USING_SKEWED_TILES

#ifdef USING_SKEWED_TILES
#include "skewed_tile/skewed_tile.h"
#else
#define TILE_FROM_RES
#include "default_tile/default_tile.h"
#endif

typedef struct CanvasConfig
{
    Size image;
    Size tile;
} CanvasConfig;

TileConfig tileConfig;
CanvasConfig canvasConfig;
// TODO: MEMORY MANAGMENT
// TODO: OTHER TILES
CanvasConfig createCanvasConfig() {
    log_mes("createCanvasConfig");
#ifdef TILE_FROM_RES //TODO: Everything that uses TILE_FROM_RES is disgusting
    CanvasConfig config = {
        .image = { 2*128, 2*128},
        .tile = { 32, 32}
    };
#else
    CanvasConfig config = {
        .image = { 360, 360},
        .tile = { 60, 60}
    };
#endif
    return config;
}
int createImage(void** tileMap);
void **generatePlane();

#ifdef STANDALONE
int make();
int main(){
    char userInput;

    while (1) {
        printf("Enter 'r' to perform a specific action or 'q' to quit: ");
        userInput = getchar();

        // Consume any remaining characters in the input buffer until a newline character
        while (getchar() != '\n')
            continue;

        if (userInput == 'r') {
            // Perform the specific action when 'r' is entered
            printf("Performing the action for 'r'.\n");
            make();
        } else if (userInput == 'q') {
            // Quit the loop when 'q' is entered
            printf("Quitting the program.\n");
            break;
        } else {
            printf("Invalid input. Please enter 'r' or 'q'.\n");
        }
    }
    return 0;
}

int make() {
    srand(time(NULL));
    log_mes("Program started!");
    canvasConfig = createCanvasConfig();
#ifdef USING_SKEWED_TILES
    tileConfig  = createSkewedTileConfig();
#else
    tileConfig  = createDefaultTileConfig();
#endif
    void** tileMap = generatePlane();

    if(tileMap == NULL) {
        printf("PANIC tileMap is NULL\n");
        return -1;
    }
    createImage(tileMap);
    return 0;
}
#endif


int getRandomTileIdx(int tileSetSize) {
    return (rand() % tileSetSize);
}

void **generatePlane() {
    log_mes("generatePlane");
    int tilesInRow = canvasConfig.image.width  / canvasConfig.tile.width;
    int tilesInCol = canvasConfig.image.height / canvasConfig.tile.height;

    void **tileMap = tileConfig.tileMapBuilder(tilesInRow, tilesInCol);
    if(tileMap == NULL){
        printf("TILE MAP IS NOT ALLOCETED\n");
        return 0;
    }
    for(int i = 0; i < tilesInCol; i++){
        for(int j = 0; j < tilesInRow; j++) {
            int idx = i*tilesInRow + j;
            bool isMatched = false;
            void* curTile = NULL;
            while(!isMatched){
                int curIdx = getRandomTileIdx(tileConfig.tileSetSize);

                curTile = tileConfig.tileSet[curIdx];

                isMatched = tileConfig.isMatching(
                    j,          i,          curTile, 
                    tilesInRow, tilesInCol, tileMap
                );
            }
            tileMap[idx] = curTile;
        }
    }
    return tileMap;
}

#include <stdlib.h>
#include <stdio.h>

#define FILE_PATH "wang_tiles/wang.ppm"

int createImage(void** tileMap) {
    log_mes("createImage");
    int tilesInRow = canvasConfig.image.width  / canvasConfig.tile.width;
    int tilesInCol = canvasConfig.image.height / canvasConfig.tile.height;

    const int dimx  = canvasConfig.image.width;
    const int dimy = canvasConfig.image.height;

    unsigned char data[dimy][dimx][3];
    int i, j;
    

    for(int i = 0; i < tilesInCol; i++){
        for(int j = 0; j < tilesInRow; j++) {
            int idx = i*tilesInRow +j;
            void* tile = tileMap[idx];
            
            if(tile == NULL) {
                printf("PANIC tile is NULL\n");
                return -1;
            }
            TileImage* tileImage = tileConfig.getTileImage(
                tile, 
                canvasConfig.tile.width,
                canvasConfig.tile.height
            );
            if(tileImage == NULL){
                printf("PANIC tileImage is NULL\n");
                return -1;
            }

            int draw_x = j* canvasConfig.tile.width;
            int draw_y = i* canvasConfig.tile.height;

            for(int y = 0; y < canvasConfig.tile.height; y++){
                for(int x = 0; x < canvasConfig.tile.width; x++)
                {
                    int cellIdx = y*canvasConfig.tile.width + x; 

                    int red = tileImage->cells[cellIdx].red;  
                    int green = tileImage->cells[cellIdx].green;
                    int blue = tileImage->cells[cellIdx].blue; 

                    data[y+draw_y][x+draw_x][0] = tileImage->cells[cellIdx].red;    /* red */
                    data[y+draw_y][x+draw_x][1] = tileImage->cells[cellIdx].green;  /* green */
                    data[y+draw_y][x+draw_x][2] = tileImage->cells[cellIdx].blue;   /* blue */
                }
            }
        
        }
    }


    const int MaxColorValue = 255; // Maximum color value
    const char *comment = "# This is my new binary PPM file";
    /* Create a new file, give it a name, and open it in binary mode */
    FILE *fp = fopen(FILE_PATH, "wb");

    /* Write header to the file (P6 format) */
    fprintf(fp, "P6\n%s\n%d %d\n%d\n", comment, dimx, dimy, MaxColorValue);

    /* Write image data bytes to the file */
    for (int y = 0; y < dimy; ++y) {
        for (int x = 0; x < dimx; ++x) {
            for (int c = 0; c < 3; ++c) {
                fputc(data[y][x][c], fp);
            }
        }
    }

    fclose(fp);
    printf("OK - File %s saved\n", FILE_PATH);

    return EXIT_SUCCESS;
}
