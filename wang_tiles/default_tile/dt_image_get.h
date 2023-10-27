
#ifndef DT_IMAGE_GET_H
#define DT_IMAGE_GET_H

#include "default_tile.h"
#include "../tile_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


int width, height, channels;
unsigned char* imageData = NULL;

int loadImage(const char* imagePath);
// TODO: tile width might not match with image width
TileImage* importFromImage(
    const char* filePath,
    DefaultTile* tile,
    int tile_width,
    int tile_height
) {
    // TODO: CACHING
    TileImage* tileImage = (TileImage*) malloc(sizeof(TileImage));
    tileImage->cells     = (Color*) calloc(tile_width*tile_height, sizeof(Color));

    if(imageData == NULL){
        loadImage(filePath);
    }
    int idToPositionMap[] = {
        4, 6, 14, 12,
        5, 7, 15, 13,
        1, 3, 11, 9,
        0, 2, 10, 8
    };

    int tileIdx = getTileId(tile);
    int i;
    for(i = 0; i < 16; i++)   
      if (tileIdx == idToPositionMap[i]) break; else continue;
    
    int orderByX = i%4;
    int orderByY = i/4;
    
    int x = orderByX*tile_width;
    int y = orderByY*tile_height;

    int dataCount = width * height;
    for (int i = y; i < y+tile_height; i++) {
        int tileY = i - y;
        for (int j = x; j < x+tile_width; j++) {
            int idx = i * width + j;
            int tileX = j - x;
            int tileIdx = tileY * tile_width + tileX;
            tileImage->cells[tileIdx].red = imageData[idx * 3];
            tileImage->cells[tileIdx].green = imageData[idx * 3 + 1];
            tileImage->cells[tileIdx].blue = imageData[idx * 3 + 2];
            // tileImage->cells[idx].red   = imageData[idx * 3];
            // tileImage->cells[idx].green = imageData[idx * 3 + 1];
            // tileImage->cells[idx].blue  = imageData[idx * 3 + 2];
        }
    }
    return tileImage;
}

int loadImage(const char* imagePath) {
    //TODO: needs to be freed after app finishes
    imageData = stbi_load(imagePath, &width, &height, &channels, 3);
    // TODO: scale down
    if (imageData==NULL) {
        printf("Failed to load image.\n");
        return 1;
    }
}


#endif