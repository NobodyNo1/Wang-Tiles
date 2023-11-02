/*
    Improvemed version of old_file_writers
*/
#include "log.h"
#include "tile_config.h"
#include "benchmark_helper.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "pthread.h"

// enabling threads does not increase image creation time
#define THREAD_COUNT        1
#define PRE_CACHE_IMAGES    1

static bool isImagesCached = false;

static TileConfig*      mTileConfig;
static CanvasConfig*    mCanvasConfig;
static int              mTilesInRow;
static int              mTilesInCol;
static void**           mTileMap;
static unsigned char*   mData       = NULL;
static int              mImageWidth;
static int              mImageHeight;
static int              mTileWidth;
static int              mTileHeight;

int drawCellToImage(int idx);

void* writeCellThreaded(void* arg);

int writeFilesToImage(
    const char* filePath,
    CanvasConfig* canvasConfig,
    TileConfig* tileConfig,
    void** tileMap
) {

    log_mes("createImage");
    log_mes("   Status: preload-cache: %d, threads:%d", PRE_CACHE_IMAGES, THREAD_COUNT <= 1 ? 1 : THREAD_COUNT);

    mCanvasConfig = canvasConfig;
    mTileConfig = tileConfig;
    mTilesInRow = canvasConfig->image.width  / canvasConfig->tile.width;
    mTilesInCol = canvasConfig->image.height / canvasConfig->tile.height;

    mImageWidth = canvasConfig->image.width;
    mImageHeight = canvasConfig->image.height;

    // Bear in mind int overflowing
    //unsigned char data[IMAGE_HEIGHT * IMAGE_WIDTH*3];
    if(mData == NULL)
        mData = (unsigned char*) malloc(IMAGE_HEIGHT * IMAGE_WIDTH*3 * sizeof(unsigned char));
    mTileMap = tileMap;
    mTileWidth = canvasConfig->tile.width;
    mTileHeight = canvasConfig->tile.height;


#if PRE_CACHE_IMAGES
    if(!isImagesCached) {
        log_mes("cache load");
        for(int i = 0; i < mTileConfig->tileSetSize; i++){
            void* tile = mTileConfig->tileSet[i];
            if(tile == NULL){
                log_mes("TILE IS EMPTY");
            }
            mTileConfig->getTileImage(
                tile, 
                mTileWidth,
                mTileHeight
            );
        }
    }
    isImagesCached = true;
#endif
   
    int i, j;

clockStart();
#if THREAD_COUNT <= 1
    for(int i = 0; i < mTilesInCol; i++) {
        for(int j = 0; j < mTilesInRow; j++) {
            int idx = i*mTilesInRow +j;
            drawCellToImage(idx);
        }
    }
#else
    pthread_t threads[THREAD_COUNT];
    int i_values[THREAD_COUNT];
    for(int i = 0; i < THREAD_COUNT; i++) {
        i_values[i] = i;
        int result = pthread_create(
            &threads[i], 
            NULL,
            writeCellThreaded,
            &i_values[i]
        );
        if (result != 0) {
            printf("    [Error] thread creation is failed");
            // Handle thread creation error
        }
        
    }
    // Wait for all threads to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
   
#endif
clockEnd("Image data creation");

clockRestart();

    const int MaxColorValue = 255; // Maximum color value
    const char *comment = "# This is binary PPM file containing Wang Tiles";
    /* Create a new file, give it a name, and open it in binary mode */
    FILE *fp = fopen(filePath, "wb");

    /* Write header to the file (P6 format) */
    fprintf(fp, "P6\n%s\n%d %d\n%d\n", comment, mImageWidth, mImageHeight, MaxColorValue);

    fwrite(mData, 1, mImageWidth * mImageHeight*3, fp);
    fclose(fp);
    printf("OK - File %s saved\n", filePath);

clockEnd("Image file creation");

    return EXIT_SUCCESS;
}

void* writeCellThreaded(void* arg) {
    int startCol = *((int*)arg);
    
    int curColumn = startCol;
    while(curColumn < mTilesInCol) {
        for(int j = 0; j < mTilesInRow; j++) {
            int res = drawCellToImage(curColumn*mTilesInRow+j);
            if (res == -1) {
                printf("ERROR!");
            }
        }
        curColumn += THREAD_COUNT;
    }

    return NULL;
}

int drawCellToImage(int idx) {
    int j = idx%mTilesInRow;
    int i = idx/mTilesInRow;

    void* tile = mTileMap[idx];
    
    if(tile == NULL) {
        printf("PANIC tile is NULL\n");
        return -1;
    }
    TileImage* tileImage = mTileConfig->getTileImage(
        tile, 
        mTileWidth,
        mTileHeight
    );
    if(tileImage == NULL){
        printf("PANIC tileImage is NULL\n");
        return -1;
    }

    int draw_x = j* mTileWidth;
    int draw_y = i* mTileHeight;

    for(int y = 0; y < mTileHeight; y++){
        for(int x = 0; x < mTileWidth; x++)
        {
            int cellIdx = y*mTileWidth + x; 

            int red     = tileImage->cells[cellIdx].red;  
            int green   = tileImage->cells[cellIdx].green;
            int blue    = tileImage->cells[cellIdx].blue; 
            
            mData[(y+draw_y)*mImageWidth*3 + (x+draw_x)*3     ] = tileImage->cells[cellIdx].red;    /* red      */
            mData[(y+draw_y)*mImageWidth*3 + (x+draw_x)*3  + 1] = tileImage->cells[cellIdx].green;  /* green    */
            mData[(y+draw_y)*mImageWidth*3 + (x+draw_x)*3  + 2] = tileImage->cells[cellIdx].blue;   /* blue     */
        }
    }
    return 0;
}

void FileWriterCleanUp(){
    free(mData);
    mData = NULL;
}