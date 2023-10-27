#include <stdlib.h>
#include <stdio.h>
#include "../log.h"

/*
// PRINTS tile ids, to make sure that tile set generated correctly
    #define VALIDATE_TILE_IDS

*/ 

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
#include "dt_image_gen.h"


bool isDefaultTileMatch(DefaultTile *tile, DefaultTile *compare, int side) {
    
    if (compare == NULL) return true;
    
    switch (side)
    {
        case 0:
            return tile->top == compare->bottom;
        case 1:
            return tile->right == compare->left;
        case 2:
            return tile->bottom == compare->top;
        case 3:
            return tile->left == compare->right;
    }    

    return false;
}


DefaultTile** generateTileSet(int* size) {
    DefaultTile **tileSet = (DefaultTile**) malloc(16*sizeof(DefaultTile*));
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
    #ifdef VALIDATE_TILE_IDS
    for(int i = 0; i < 16; i++) {
        // int sum = (tileSet[i].top == 0? 0: 1)
        //         + (tileSet[i].right == 0? 0: 2) 
        //         + (tileSet[i].bottom == 0? 0: 4)
        //         + (tileSet[i].left == 0 ? 0 : 8);
        int sum = (tileSet[i]->top == 0? 0: 1)
            + (tileSet[i]->right == 0? 0: 2) 
            + (tileSet[i]->bottom == 0? 0: 4)
            + (tileSet[i]->left == 0 ? 0 : 8);
    }
    #endif
    *size = 16;
    return tileSet;
}

// TODO: Caching

TileImage* getTileImage(void* tile, int tile_width, int tile_height) {
    if(tile == NULL) return NULL;
    DefaultTile* defaultTile = (DefaultTile*) tile;
    return generateTileImage(defaultTile, tile_width, tile_height);
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

// TODO: TILECONFIG is shadowed import
TileConfig createDefaultTileConfig(){
    log_mes("createDefaultTileConfig");
    int tileSetSize = 0;
    DefaultTile** tileSet = generateTileSet(&tileSetSize);

    TileConfig config = {
        .tileSet = tileSet,
        .tileSetSize = tileSetSize,
        &defaultTileMapBuilder,
        &isDefaultTileMatching,
        &getTileImage
    };
    return config;
}

#endif