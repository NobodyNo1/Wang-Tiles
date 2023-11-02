#define ASYNC_WRITE 0

#if ASYNC_WRITE

#define THREADS_COUNT 4
#include <pthread.h> 

#endif


int writeOneRowOfData(
    int i, 
    int tilesInRow, 
    int tileWidth, 
    int tileHeight,
    TileConfig* tileConfig,
    void** tileMap,
    unsigned char data[IMAGE_HEIGHT][IMAGE_WIDTH][3]
);

void* writeColumn(void* arg);

unsigned char data[IMAGE_HEIGHT][IMAGE_WIDTH][3];

#if ASYNC_WRITE 
int mTilesInRow;
int mTilesInCol;
void** mTileMap;
TileConfig* mTileConfig;

void* writeColumn(void* arg) {
    struct ThreadArgs* threadArgs = (struct ThreadArgs*)arg;
 
    int i = *((int*)arg);

    int res = writeOneRowOfData(
        i,
        mTilesInRow,
        TILE_WIDTH,
        TILE_HEIGHT,
        mTileConfig,
        mTileMap,
        data
    );

    if (res == -1) {
        // Handle error if needed
    }

    return NULL;
}


void* writeColumnThreaded(void* arg) {
    int startCol = *((int*)arg);
    
    int curColumn = startCol;
    while(curColumn < mTilesInCol){
        int res = writeOneRowOfData(
            curColumn,
            mTilesInRow,
            TILE_WIDTH,
            TILE_HEIGHT,
            mTileConfig,
            mTileMap,
            data
        );
        if (res == -1) {
            // Handle error if needed
        }
        curColumn += THREADS_COUNT;
    }

    return NULL;
}
#endif

int writeFilesToImage(
    const char* filePath, 
    CanvasConfig* canvasConfig,
    TileConfig* tileConfig,
    void** tileMap
) {
    log_mes("createImage");
    int tileWidth = canvasConfig->tile.width;
    int tileHeight = canvasConfig->tile.height;
    int tilesInRow = canvasConfig->image.width  / tileWidth;
    int tilesInCol = canvasConfig->image.height / tileHeight;

    const int dimx = canvasConfig->image.width;
    const int dimy = canvasConfig->image.height;

    // unsigned char data[dimy][dimx][3];
    int i, j;

clockStart();

#if ASYNC_WRITE
    mTileMap = tileMap;
    mTilesInRow = tilesInRow;
    mTileConfig = tileConfig;
    mTilesInCol = tilesInCol;
    int threadCount = THREADS_COUNT == -1 ? tilesInCol : THREADS_COUNT;
    pthread_t threads[threadCount];
    int i_values[threadCount];

    #if THREADS_COUNT == -1
    for(int i = 0; i < tilesInCol; i++) {
        int threadIdx = i % threadCount;
        i_values[threadIdx] = i;
        int result = pthread_create(
            &threads[threadIdx], 
            NULL, writeColumn,
             &i_values[threadIdx]
        );
        if (result != 0) {
            printf("    [Error] thread creation is failed");
            // Handle thread creation error
        }
        
    }
    // Wait for all threads to finish
    for (int i = 0; i < tilesInCol; i++) {
        pthread_join(threads[i], NULL);
    }
    #else
    for(int i = 0; i < threadCount; i++) {
        i_values[i] = i;
        int result = pthread_create(
            &threads[i], 
            NULL,
            writeColumnThreaded,
            &i_values[i]
        );
        if (result != 0) {
            printf("    [Error] thread creation is failed");
            // Handle thread creation error
        }
        
    }
    // Wait for all threads to finish
    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }
    #endif
#else
    for(int i = 0; i < tilesInCol; i++) {
        int res = writeOneRowOfData(
            i, tilesInRow, tileWidth, tileHeight, tileConfig, tileMap, data
        );
        if(res == -1) return -1;
    }
#endif
clockEnd("Image data creation");

clockRestart();

    const int MaxColorValue = 255; // Maximum color value
    const char *comment = "# This is my new binary PPM file";
    /* Create a new file, give it a name, and open it in binary mode */
    FILE *fp = fopen(filePath, "wb");

    /* Write header to the file (P6 format) */
    fprintf(fp, "P6\n%s\n%d %d\n%d\n", comment, dimx, dimy, MaxColorValue);

    /* Write image data bytes to the file */
    for (int y = 0; y < dimy; ++y) {
        for (int x = 0; x < dimx; ++x) {
            for (int c = 0; c < 3; ++c) {
                // fputc(data[y][x][c], fp);
                fwrite(data[y][x], 1, 3, fp);
            }
        }
    }

    fclose(fp);
    printf("OK - File %s saved\n", filePath);

clockEnd("Image file creation");
    return 0;
}

int writeOneRowOfData(
    int i, 
    int tilesInRow, 
    int tileWidth, 
    int tileHeight,
    TileConfig* tileConfig,
    void** tileMap,
    unsigned char data[IMAGE_HEIGHT][IMAGE_WIDTH][3]
) {
    for(int j = 0; j < tilesInRow; j++) {
            int idx = i*tilesInRow +j;
            void* tile = tileMap[idx];
            
            if(tile == NULL) {
                printf("PANIC tile is NULL\n");
                return -1;
            }
            TileImage* tileImage = tileConfig->getTileImage(
                tile, 
                tileWidth,
                tileHeight
            );
            if(tileImage == NULL){
                printf("PANIC tileImage is NULL\n");
                return -1;
            }

            int draw_x = j* tileWidth;
            int draw_y = i* tileHeight;

            for(int y = 0; y < tileHeight; y++){
                for(int x = 0; x < tileWidth; x++)
                {
                    int cellIdx = y*tileWidth + x; 

                    int red     = tileImage->cells[cellIdx].red;  
                    int green   = tileImage->cells[cellIdx].green;
                    int blue    = tileImage->cells[cellIdx].blue; 
                    
                    data[y+draw_y][x+draw_x][0] = tileImage->cells[cellIdx].red;    /* red */
                    data[y+draw_y][x+draw_x][1] = tileImage->cells[cellIdx].green;  /* green */
                    data[y+draw_y][x+draw_x][2] = tileImage->cells[cellIdx].blue;   /* blue */
                }
            }
            for(int y = 0; y < tileHeight; y++){
                for(int x = 0; x < tileWidth; x++)
                {
                    int cellIdx = y*tileWidth + x; 

                    int red     = tileImage->cells[cellIdx].red;  
                    int green   = tileImage->cells[cellIdx].green;
                    int blue    = tileImage->cells[cellIdx].blue; 
                    
                    data[y+draw_y][x+draw_x][0] = tileImage->cells[cellIdx].red;    /* red */
                    data[y+draw_y][x+draw_x][1] = tileImage->cells[cellIdx].green;  /* green */
                    data[y+draw_y][x+draw_x][2] = tileImage->cells[cellIdx].blue;   /* blue */
                }
            }
        }
}
