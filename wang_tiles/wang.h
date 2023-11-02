#define MEM_TRACK

#ifdef MEM_TRACK
#include "cmemcounter.h"
#endif

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>


#include "tile_config.h"
#include "log.h"
#include "benchmark_helper.h"


#define TILE_WIDTH      32
#define TILE_HEIGHT     32

#include "file_writer.h"

// #define USING_SKEWED_TILES

#ifdef USING_SKEWED_TILES
#include "skewed_tile/skewed_tile.h"
#else
#define TILE_FROM_RES
#include "default_tile/default_tile.h"
#endif

#define FILE_PATH "wang_tiles/wang.ppm"

TileConfig tileConfig;
CanvasConfig canvasConfig;


// TODO: MEMORY MANAGMENT
// TODO: OTHER TILES
CanvasConfig createCanvasConfig() {
    log_mes("createCanvasConfig");
#ifdef TILE_FROM_RES
    CanvasConfig config = {
        .image = { IMAGE_WIDTH, IMAGE_HEIGHT},
        .tile = { TILE_WIDTH, TILE_HEIGHT}
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
int setup();
void cleanUp();

int main() {
    setup();
    char userInput;
    while (true) {
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
#ifdef MEM_TRACK
        printf("[Memory] usage: %zu\n", malloced_memory_usage);
#endif
    }
    cleanUp();
#ifdef MEM_TRACK
    printf("[Memory] Finished with: %zu\n", malloced_memory_usage);
#endif
    return 0;
}

int setup() {
    log_mes("Program started!");
    srand(time(NULL));
    canvasConfig = createCanvasConfig();
#ifdef USING_SKEWED_TILES
    tileConfig  = createSkewedTileConfig();
#else
    tileConfig  = createDefaultTileConfig();
#endif
    return 0;
}

int make() {
    void** tileMap = generatePlane();

    if(tileMap == NULL) {
        log_mes("PANIC tileMap is NULL");
        return -1;
    }
    writeFilesToImage(FILE_PATH, &canvasConfig, &tileConfig, tileMap);

    free(tileMap);
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
clockStart();
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
clockEnd("Tile Map Generatation");
    
    return tileMap;
}

void cleanUp() {
    FileWriterCleanUp();
#ifdef USING_SKEWED_TILES
    //TODO: clear skewed
#else
    for(int i = 0; i < tileConfig.tileSetSize; ++i){
        free(tileConfig.tileSet[i]);
    }
    free(tileConfig.tileSet);
    freeConfig();
#endif
}