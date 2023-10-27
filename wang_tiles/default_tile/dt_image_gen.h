#include <math.h>


#ifndef DT_IMAGE_GEN_H
#define DT_IMAGE_GEN_H

#include "default_tile.h"
#include "../tile_image.h"

#define ENABLE_TILE_EDGE_HIGHLIGHT
#define ENABLE_TILE_DIAGONAL_HIGHLIGHT

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

TileImage* generateTileImage(DefaultTile* tile, int tile_width, int tile_height) {
    TileImage* tileImage = (TileImage*) malloc(sizeof(TileImage));
    tileImage->cells     = (Color*) calloc(tile_width*tile_height, sizeof(Color));

    float centerX = ((float)tile_width)/2.0;
    float centerY = ((float)tile_height)/2.0;
    float half_angle = (float)M_PI_2;
    float minus_half_angle = (-1)* (float)M_PI_2;
    float half_of_half = half_angle/2;
    float minus_half_of_half_angle = (-1)* half_of_half;

    float step = half_of_half;
    for(int i = 0; i < tile_height; i++){
        for(int j = 0; j < tile_width; j++){
            #ifdef ENABLE_TILE_DIAGONAL_HIGHLIGHT
            if(i == j || tile_height-i == j || tile_width-j == i) continue; //dialogals are black
            #endif
            #ifdef ENABLE_TILE_EDGE_HIGHLIGHT
            if(i == 0 || j == 0 || i == tile_height -1 || j == tile_width -1) continue; //edges are black
            #endif
            int idx = i*tile_width + j;

            double x_diff = j - centerX;
            double y_diff = i - centerY;

            float angle = atan2(y_diff,x_diff); 
            int side = UNDEFINED_COLOR_IDX;
            if(-step <= angle && angle <= step) {
                side = tile->right;
            } else if(step <= angle && angle <= 3*step){
                side = tile->bottom;
            } else if(
                (3*step <= angle || angle <= -M_PI + step)
            ) {
                side = tile->left;
            } else if(
                -M_PI + step <= angle && angle <= -step
            ) {
                side = tile->top;
            }

            
            // Assign the color based on the side
            tileImage->cells[idx] = getColorByIdx(side);
        }
    }    
    return tileImage;
}

#endif