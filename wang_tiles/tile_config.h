#include "tile_image.h"

#ifndef TILE_CONFIG_H
#define TILE_CONFIG_H

typedef struct TileConfig
{
    void** tileSet;
    int tileSetSize;
    void** (*tileMapBuilder)(/*elements in row*/ int , /*elements in col*/ int );
    bool (*isMatching)(
        int /*row*/, int /*col*/,
        void* /*current element*/, 
        int /*rows*/, int /*cols*/,
        void** /*whole map*/
    );
    TileImage* (*getTileImage) (
        void* /* tile */, 
        int  /*tile_width */, 
        int  /*tile_height */
    );
} TileConfig;


#endif