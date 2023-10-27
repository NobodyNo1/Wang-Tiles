#include <math.h>


#ifndef ST_IMAGE_GEN_H
#define ST_IMAGE_GEN_H

#include "skewed_tile.h"
#include "../tile_image.h"

#define ENABLE_TILE_EDGE_HIGHLIGHT
#define ENABLE_TILE_CENTER_LINE_HIGHLIGHT

#define UNDEFINED_COLOR_IDX     -1

const Color red     = {255, 0,0};
const Color green   = {0, 255,0};
const Color blue    = {0, 0, 255};
const Color yellow  = {255, 255,0};
const Color black   = {0,0,0};

Color getColorByIdx(int idx) {
    if(idx == -1) return black;
    // DEFAULT tile have only 2 state either colored or not
    if(idx) return green;
    return blue;
}

TileImage* generateSkewedTileImage(SkewedTile* tile, int tile_width, int tile_height) {
    TileImage* tileImage = (TileImage*) malloc(sizeof(TileImage));
    tileImage->cells     = (Color*) calloc(tile_width*tile_height, sizeof(Color));

    float centerX = ((float)tile_width)/2.0;
    float centerY = ((float)tile_height)/2.0;
    float half_angle = (float)M_PI_2;
    float minus_half_angle = (-1)* (float)M_PI_2;
    float half_of_half = half_angle/2;
    float minus_half_of_half_angle = (-1)* half_of_half;

    float step = half_angle;
    for(int i = 0; i < tile_height; i++){
        for(int j = 0; j < tile_width; j++){
            #ifdef ENABLE_TILE_CENTER_LINE_HIGHLIGHT
            if(i == centerX || j == centerY) continue; //dialogals are black
            #endif
            #ifdef ENABLE_TILE_EDGE_HIGHLIGHT
            if(i == 0 || j == 0 || i == tile_height -1 || j == tile_width -1) continue; //edges are black
            #endif
            int idx = i*tile_width + j;

            double x_diff = j - centerX;
            double y_diff = i - centerY;

            int side = UNDEFINED_COLOR_IDX;
            if(x_diff == 0 && y_diff == 0){
                side = UNDEFINED_COLOR_IDX;
            }
            else if(x_diff< 0 && y_diff <0) {
                side = tile->topLeft;
                //top left
            }
            else if(x_diff > 0 && y_diff <0) {
                side = tile->topRight;
                // top right
            }
            else if(x_diff > 0 && y_diff > 0) {
                side = tile->bottomRight;
                // bottom right
            }
            else if(x_diff < 0 && y_diff > 0) {
                side = tile->bottomLeft;
                // bottom left
            }
            // Assign the color based on the side
            tileImage->cells[idx] = getColorByIdx(side);
        }
    }    
    return tileImage;
}

#endif