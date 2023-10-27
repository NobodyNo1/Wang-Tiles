#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "tiles.h"
#include "image_g.h"

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 320



const int cell_in_column  = 10;
const int cell_size       = IMAGE_HEIGHT / cell_in_column;
const int cell_in_row     = IMAGE_WIDTH  / cell_size;

Cell *cells;

int setup() {
    cells = (Cell*) calloc(cell_in_row*cell_in_column, sizeof(Cell));
    if(cells == NULL) return -1;
    // FOR color generation
    srand(time(NULL));
    // ? do we need image cells ?
    return 0;
}

int getRandomColor(){
    return (rand() % 4) + 1;
}

int generate() {
    if(cells == NULL) return -1;

    for(int i = 0; i < cell_in_column; i++) {
        for(int j = 0; j < cell_in_row; j++){
            int idx = i*cell_in_row + j;

            int bottomIdx = i+1 < cell_in_column ? (i+1)*cell_in_row + j : -1;
            if(bottomIdx != -1 && cells[bottomIdx].top != UNDEFINED_COLOR_IDX)
                cells[idx].bottom = cells[bottomIdx].top;
            else
                cells[idx].bottom = getRandomColor();

            int topIdx = i-1 >= 0 ? (i-1)*cell_in_row + j : -1;
            if(topIdx != -1 && cells[topIdx].bottom != UNDEFINED_COLOR_IDX)
                cells[idx].top = cells[topIdx].bottom;
            else
                cells[idx].top = getRandomColor();


            int leftIdx = j-1 >= 0 ? i*cell_in_row + j - 1 : -1;
            if(leftIdx != -1 && cells[leftIdx].right != UNDEFINED_COLOR_IDX)
                cells[idx].left = cells[leftIdx].right;
            else
                cells[idx].left = getRandomColor();


            int rightIdx = j+1 < cell_in_row ? i*cell_in_row + j + 1 : -1;
            if(rightIdx != -1 && cells[rightIdx].left != UNDEFINED_COLOR_IDX)
                cells[idx].right = cells[rightIdx].left;
            else
                cells[idx].right = getRandomColor();
        }
    }
}

void bake_image() {
    Tile *tiles = (Tile*) malloc(cell_in_row*cell_in_column*sizeof(Tile));
    for(int i = 0; i < cell_in_column; i++){
        for(int j = 0; j < cell_in_row; j++){
            int idx = i*cell_in_row + j;
            tiles[idx] = generate_tile(cell_size, cells[idx]);
            tiles->tile_size = cell_size;
        }
    }
    create_image(IMAGE_WIDTH, IMAGE_HEIGHT, tiles, cell_size);
}

void print_cells(){
    for(int i = 0; i < cell_in_column; i++) {
        for(int k = 0; k < 3; k++) {
            for(int j = 0; j < cell_in_row; j++) {
                int idx = i*cell_in_row + j;
                    if(k == 0)
                        printf("  %d  ", cells[idx].top);
                    if(k == 1)
                        printf(" %d-%d ", cells[idx].left, cells[idx].right);
                    if(k == 2)
                        printf("  %d  ", cells[idx].bottom);
            }
            printf("\n");
        }
    }
}