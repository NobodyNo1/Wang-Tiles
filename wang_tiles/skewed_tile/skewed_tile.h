#include <stdlib.h>
#include <stdio.h>
#include "../log.h"

#ifndef SKEWED_TILE_H
#define SKEWED_TILE_H

//TODO: keep up with default tile impl

//TODO: FIX CYCLIC DEPENDENCY

typedef struct SkewedTile
{
    int topLeft;
    int topRight;
    int bottomLeft;
    int bottomRight;
} SkewedTile;

#include "../tile_config.h"
#include "../tile_image.h"
#include "st_image_gen.h"



SkewedTile** generateTileSet(int* size) {
    SkewedTile **tileSet = (SkewedTile**) malloc(16*sizeof(SkewedTile*));
    int i = 0;
    for(int topLeft = 0; topLeft < 2; topLeft++) {
        for(int topRight = 0; topRight < 2; topRight++) {
            for(int bottomLeft = 0; bottomLeft < 2; bottomLeft++) {
                for(int bottomRight = 0; bottomRight < 2; bottomRight++) {
                    SkewedTile* item = (SkewedTile*)malloc(sizeof(SkewedTile));
                    item->topLeft       = topLeft       ;
                    item->topRight      = topRight      ;
                    item->bottomLeft    = bottomLeft    ;
                    item->bottomRight   = bottomRight   ;
                    tileSet[i]          = item          ;
                    i++;
                }
            }
        }
    }

    *size = 16;
    return tileSet;
}

TileImage* getTileImage(void* tile, int tile_width, int tile_height) {
    if(tile == NULL) return NULL;
    SkewedTile* skewedTile = (SkewedTile*) tile;
    return generateSkewedTileImage(skewedTile, tile_width, tile_height);
}

SkewedTile** SkewedTileMapBuilder(int rows, int cols) {
    return (SkewedTile**) calloc(rows*cols, sizeof(SkewedTile*));
}

bool isSkewedTileMatching(
    int j, int i,
    SkewedTile* currentElement, 
    int rows, int cols,
    SkewedTile** tileMap 
){
    // log_mes("isSkewedTileMatching");

    bool isMatching = true;
    int idx = i*rows + j;

    int bottomIdx = i+1 < cols ? (i+1)*rows + j : -1;

    if(bottomIdx != -1 && tileMap[bottomIdx] != NULL)
        isMatching = isMatching && currentElement->bottomRight == tileMap[bottomIdx]->topRight && currentElement->bottomLeft == tileMap[bottomIdx]->topLeft;
    else 
        isMatching = true;

    if(!isMatching) return false;

    int topIdx = i-1 >= 0 ? (i-1)*rows + j : -1;
    if(topIdx != -1 && tileMap[topIdx] != NULL)
        isMatching = isMatching && currentElement->topLeft == tileMap[topIdx]->bottomLeft && currentElement->topRight == tileMap[topIdx]->bottomRight;
    else 
        isMatching = true;

    if(!isMatching) return false;

    int leftIdx = j-1 >= 0 ? i*rows + j - 1 : -1;
    
    if(leftIdx != -1 && tileMap[leftIdx] != NULL)
        isMatching = isMatching && currentElement->topLeft == tileMap[leftIdx]->topRight && currentElement->bottomLeft == tileMap[leftIdx]->bottomRight;
    else 
        isMatching = true;

    if(!isMatching) return false;

    int rightIdx = j+1 < rows ? i*rows + j + 1 : -1;

    // log_mes("rightIdx : %d", rightIdx);
    if(rightIdx != -1 && tileMap[rightIdx] != NULL)
        isMatching = isMatching && currentElement->topRight == tileMap[rightIdx]->topLeft && currentElement->bottomRight == tileMap[leftIdx]->bottomLeft;
    else 
        isMatching = true;

    return isMatching;
}

// TODO: TILECONFIG is shadowed import
TileConfig createSkewedTileConfig(){
    log_mes("createSkewedTileConfig");
    int tileSetSize = 0;
    SkewedTile** tileSet = generateTileSet(&tileSetSize);

    TileConfig config = {
        .tileSet = tileSet,
        .tileSetSize = tileSetSize,
        &SkewedTileMapBuilder,
        &isSkewedTileMatching,
        &generateSkewedTileImage
    };
    return config;
}

#endif