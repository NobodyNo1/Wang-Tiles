
#ifndef IMAGE_H
#define IMAGE_H

typedef struct Color {
    int red;
    int green;
    int blue;
} Color;

typedef struct TileImage
{
    Color* cells;
} TileImage;

typedef struct Size
{
    int width;
    int height;
} Size;


#endif