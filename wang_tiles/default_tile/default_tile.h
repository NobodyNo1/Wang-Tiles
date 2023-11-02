#include <stdlib.h>
#include <stdio.h>
#include "../log.h"
#include "stdbool.h"
#include "../tile_config.h"

#ifdef TILE_FROM_RES
// SOURCE of images http://www.cr31.co.uk/stagecast/wang/tiles_e.html
// #define IMAGE_PATH "wang_tiles/default_tile/res/wang2e.png"
#define IMAGE_PATH "wang_tiles/default_tile/res/pipe1.png"
#define TILE_COUNT 16
void freeImage();
#include "dt_image_get.h"

#else

#include "dt_image_gen.h"

#endif

#ifndef DEFAULT_TILE_H
#define DEFAULT_TILE_H

//TODO: FIX CYCLIC DEPENDENCY

typedef struct DefaultTile
{
    int top;
    int left;
    int bottom;
    int right;
} DefaultTile;

#include "../tile_config.h"
#include "../tile_image.h"

TileImage** cachedImages;

int getTileId(DefaultTile* tile){
    return (tile->top == 0? 0: 1)
            + (tile->right == 0? 0: 2) 
            + (tile->bottom == 0? 0: 4)
            + (tile->left == 0 ? 0 : 8);
}

DefaultTile** generateTileSet(int* size) {
    DefaultTile **tileSet = (DefaultTile**) malloc(TILE_COUNT*sizeof(DefaultTile*));
    int i = 0;
    for(int top = 0; top < 2; top++) {
        for(int right = 0; right < 2; right++) {
            for(int bottom = 0; bottom < 2; bottom++) {
                for(int left = 0; left < 2; left++) {
                    DefaultTile* item = (DefaultTile*)malloc(sizeof(DefaultTile));
                    item->top      = top   ;
                    item->right    = right ;
                    item->bottom   = bottom;
                    item->left     = left  ;
                    tileSet[i]     = item;
                    i++;
                }
            }
        }
    }
    *size = TILE_COUNT;
    return tileSet;
}

TileImage* importFromImage(
    const char* filePath,
    DefaultTile* tile,
    int tile_width,
    int tile_height
);

TileImage* generateTileImage(DefaultTile* tile, int tile_width, int tile_height);


TileImage* getTileImage(void* tile, int tile_width, int tile_height) {
    if(tile == NULL) return NULL;

    DefaultTile* defaultTile = (DefaultTile*) tile;
    TileImage* cached = cachedImages[getTileId(defaultTile)];
   
    if(cached != NULL){
        return cached;
    }
#ifdef TILE_FROM_RES
    TileImage* newTileImage = importFromImage(IMAGE_PATH, defaultTile, tile_width, tile_height);
#else
    TileImage* newTileImage = generateTileImage(defaultTile, tile_width, tile_height);
#endif
    cachedImages[getTileId(defaultTile)] = newTileImage;
    return newTileImage;
}

DefaultTile** defaultTileMapBuilder(int rows, int cols) {
    return (DefaultTile**) calloc(rows*cols, sizeof(DefaultTile*));
}

bool isDefaultTileMatching(
    int j, int i,
    DefaultTile* currentElement, 
    int rows, int cols,
    DefaultTile** tileMap 
){
    // log_mes("isDefaultTileMatching");

    bool isMatching = true;
    int idx = i*rows + j;

    int bottomIdx = i+1 < cols ? (i+1)*rows + j : -1;

    if(bottomIdx != -1 && tileMap[bottomIdx] != NULL)
        isMatching = isMatching && currentElement->bottom == tileMap[bottomIdx]->top;
    else 
        isMatching = true;

    if(!isMatching) return false;

    int topIdx = i-1 >= 0 ? (i-1)*rows + j : -1;
    if(topIdx != -1 && tileMap[topIdx] != NULL)
        isMatching = isMatching && currentElement->top == tileMap[topIdx]->bottom;
    else 
        isMatching = true;

    if(!isMatching) return false;

    int leftIdx = j-1 >= 0 ? i*rows + j - 1 : -1;
    
    if(leftIdx != -1 && tileMap[leftIdx] != NULL)
        isMatching = isMatching && currentElement->left == tileMap[leftIdx]->right;
    else 
        isMatching = true;

    if(!isMatching) return false;

    int rightIdx = j+1 < rows ? i*rows + j + 1 : -1;

    // log_mes("rightIdx : %d", rightIdx);
    if(rightIdx != -1 && tileMap[rightIdx] != NULL)
        isMatching = isMatching && currentElement->right == tileMap[rightIdx]->left;
    else 
        isMatching = true;

    return isMatching;
}

TileConfig createDefaultTileConfig(){
    log_mes("createDefaultTileConfig");
    int tileSetSize = 0;
    DefaultTile** tileSet = generateTileSet(&tileSetSize);
    cachedImages = (TileImage**) calloc(TILE_COUNT, sizeof(TileImage*));

    TileConfig config = {
        // some values needs to type casted (because of warnings)
        (void**)                                        tileSet,
                                                        tileSetSize,
        (void** (*)( int, int))                         &defaultTileMapBuilder,
        (bool   (*)(int, int, void*, int, int, void**)) &isDefaultTileMatching,
                                                        &getTileImage
    };
    return config;
}

void clearCache();

void freeConfig(){
#ifdef TILE_FROM_RES
    freeImage();
#endif
    clearCache();
}

void clearCache() {
    for(int i = 0; i < TILE_COUNT; ++i){
        free(cachedImages[i]->cells);
        free(cachedImages[i]);
    }
    free(cachedImages);
}
#endif