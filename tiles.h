#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef TILES_H
#define TILES_H

#define UNDEFINED_COLOR_IDX     0
#define COLOR_IDX_RED           1
#define COLOR_IDX_BLUE          2
#define COLOR_IDX_GREEN         3
#define COLOR_IDX_YELLOW        4

typedef struct Cell {
    int top;
    int left;
    int bottom;
    int right;
} Cell;

typedef struct ImageCell {
    int red;
    int green;
    int blue;
} ImageCell;


const ImageCell red     = {255, 0,0};
const ImageCell green   = {0, 255,0};
const ImageCell blue    = {0, 0, 255};
const ImageCell yellow  = {255, 255,0};
const ImageCell black   = {0,0,0};


ImageCell getColorByIdx(int idx) {
    switch (idx)
    {
        case 0:
            return black;
        case 1:
            return red;
        case 2:
            return green;
        case 3:
            return blue;
        case 4:
            return yellow;
    }

    return black;
}

typedef struct Tile
{
    //always square
    int tile_size;
    ImageCell* cells;
} Tile;

Tile generate_tile(int tile_size, Cell cell) {
    Tile tile = {
        .tile_size = tile_size,
        .cells     = (ImageCell*) calloc(tile_size*tile_size, sizeof(ImageCell))
    };
    float center = tile_size/2;
    float half_angle = (float)M_PI_2;
    float minus_half_angle = (-1)* (float)M_PI_2;
    float half_of_half = half_angle/2;
    float minus_half_of_half_angle = (-1)* half_of_half;
    for(int i = 0; i < tile_size; i++){
        for(int j = 0; j < tile_size; j++){
            if(i == j || tile_size-i == j || tile_size-j == i) continue; //dialogals are black
            if(i == 0 || j == 0 || i == tile_size -1 || j == tile_size -1) continue; //edges are black
            int idx = i*tile_size + j;

            double x_diff = j - center;
            double y_diff = i - center;
            float angle =  atan2(y_diff,x_diff); 
            int side = UNDEFINED_COLOR_IDX;
            if(minus_half_of_half_angle < angle && angle < half_of_half){
                side = cell.right;
            } else if(half_of_half < angle && angle < 3*half_of_half){
                side = cell.bottom;
            } else if(
                (3*half_of_half <= angle || angle <= -M_PI + half_of_half)
                ){
                side = cell.left;
            } else if(
                -M_PI + half_of_half < angle && angle < minus_half_of_half_angle
            ){
                side = cell.top;
            }

            // Assign the color based on the side
            tile.cells[idx] = getColorByIdx(side);
        }
    }    
    return tile;
}


#endif