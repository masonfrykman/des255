#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <cairo.h>
#include <cairo-svg.h>

// Program Parameters
#define ROWS 36 // 36 in * 72 = 2592 pt
#define COLS 24 // 24 in * 72 = 1728 pt
#define MINIMUM_LEN 300

#define ORIGIN_ROW 11
#define ORIGIN_COL 10

// Debug options
#define DEBUG_PRINT_CHOICES false
#define DEBUG_PRINT_RESTART_MSG true
#define DEBUG_PRINT_FILL_GRID false

// Global variables
int setCounter = 0;
cairo_t* cr;
cairo_surface_t* surface;

void cleanup(int** fillGrid) {
    for(int r = 0; r < ROWS; r++) {
        free(fillGrid[r]);
    }
    
    cairo_set_source_rgb(cr, 200, 200, 200);
    cairo_set_line_width(cr, 0.1);
    cairo_stroke(cr);
    cairo_surface_finish(surface);
    cairo_surface_flush(surface);
}

void set(int newValue, int idxRow, int idxCol, int** grid, int rows, int cols) {
    if(idxRow < 0 || idxCol < 0 || idxCol >= cols || idxRow >= rows) return; // bounds check
    grid[idxRow][idxCol] = newValue;
}

int* getSpot(int idxRow, int idxCol, int** grid) {
    return grid[idxRow] + idxCol;
}

// travelDirection 0: travelled columns
// travelDirection 1: travelled rows
void setWithDirectionalBorder(int fillRow, int fillCol, int travelDirection, int** grid) {
    if(travelDirection > 1 || travelDirection < 0) return;

    // set the spot to 1.
    grid[fillRow][fillCol] = 1;
    setCounter++;

    // set the border
    if(travelDirection == 0) { // row - 1,
        if(fillRow != 0 && grid[fillRow - 1][fillCol] != 1) {
            grid[fillRow - 1][fillCol] = 2;
        }
        if(fillRow != ROWS - 1 && grid[fillRow + 1][fillCol] != 1) {
            grid[fillRow + 1][fillCol] = 2;
        }
    } else {
        if(fillCol != 0 && grid[fillRow][fillCol - 1] != 1) {
            grid[fillRow][fillCol - 1] = 2;
        }
        if(fillCol != COLS - 1 && grid[fillRow][fillCol + 1] != 1) {
            grid[fillRow][fillCol + 1] = 2;
        }
    }

    // draw the line
    cairo_line_to(cr, fillCol, fillRow);
}

int main() {
#ifndef CAIRO_HAS_SVG_SURFACE
    printf("ERROR: cannot create svg surface.\n");
    return 1;
#endif

    srand(time((0)));

    // Initialize grid
    if(ROWS * COLS <= MINIMUM_LEN) {
        printf("Bad configured preprocessor definition MIN_LENGTH, minimum length is impossible to reach.\n");
        return 1;
    }

    // 0 - no fill
    // 1 - fill
    // 2 - border
    int* fillGrid[ROWS]; // 20x30 row-major grid

restartLbl:
    for(int i = 0; i < ROWS; i++) {
        fillGrid[i] = (int*)malloc(sizeof(int) * COLS);
        for(int j = 0; j < COLS; j++) {
            fillGrid[i][j] = 0;
        }
    }
    fillGrid[ORIGIN_ROW][ORIGIN_COL] = 1;

    // setup cairo
    surface = cairo_svg_surface_create("out.svg", COLS, ROWS);
    cr = cairo_create(surface);
    cairo_svg_surface_set_document_unit(surface, CAIRO_SVG_UNIT_IN);

    // draw loop
    int currentRow = ORIGIN_ROW; // y axis
    int currentCol = ORIGIN_COL; // x axis
    cairo_move_to(cr, currentCol, currentRow);

    while(true) {
        // 0: up
        // 1: right
        // 2: down
        // 3: left
        bool validSpots[4];
        for(int i = 0; i < 4; i++) {validSpots[i] = false;}
        int validSpotCounter = 0;

        // up
        if(currentRow != 0) {
            int up = *getSpot(currentRow - 1, currentCol, fillGrid);
            if(up != 1) {
                validSpots[0] = true;
                validSpotCounter++;
            } else {
                validSpots[0] = false;
            }
        }

        // right
        if(currentCol != COLS - 1) {
            int right = *getSpot(currentRow, currentCol + 1, fillGrid);
            if(right != 1) {
                validSpots[1] = true;
                validSpotCounter++;
            } else {
                validSpots[1] = false;
            }
        }

        // down
        if(currentRow != ROWS - 1) {
            int down = *getSpot(currentRow + 1, currentCol, fillGrid);
            if(down != 1) {
                validSpots[2] = true;
                validSpotCounter++;
            } else {
                validSpots[2] = false;
            }
        }

        // left
        if(currentCol != 0) {
            int left = *getSpot(currentRow, currentCol - 1, fillGrid);
            if(left != 1) {
                validSpots[3] = true;
                validSpotCounter++;
            } else {
                validSpots[3] = false;
            }
        }

        // print choices table
        if(DEBUG_PRINT_CHOICES) {
            printf("CHOICES: ");
            for(int i = 0; i < 4; i++) {
                printf("%d ", validSpots[i]);
            }
            printf("\n");
        }

        // done if we can't go anywhere else
        if(validSpotCounter == 0) break;

        // if 1 valid spot, then just iterate there.
        if(validSpotCounter == 1) {
            if(validSpots[0] == true) { // up
                setWithDirectionalBorder(currentRow - 1, currentCol, 1, fillGrid);
                currentRow--;
            } else if(validSpots[1] == true) { // right
                setWithDirectionalBorder(currentRow, currentCol + 1, 0, fillGrid);
                currentCol++;
            } else if(validSpots[2] == true) { // down
                setWithDirectionalBorder(currentRow + 1, currentCol, 1, fillGrid);
                currentRow++;
            } else if(validSpots[3] == true) { // left
                setWithDirectionalBorder(currentRow, currentCol - 1, 0, fillGrid);
                currentCol--;
            }
            continue;
        }

        // if more than one valid spot, randomly choose.
        int r;
        while(true) {
            r = rand() % 4;
            if(validSpots[r] == true) break;
        }

        if(r == 0) { // up
            setWithDirectionalBorder(currentRow - 1, currentCol, 1, fillGrid);
            currentRow--;
        } else if(r == 1) { // right
            setWithDirectionalBorder(currentRow, currentCol + 1, 0, fillGrid);
            currentCol++;
        } else if(r == 2) { // down
            setWithDirectionalBorder(currentRow + 1, currentCol, 1, fillGrid);
            currentRow++;
        } else if(r == 3) { // left
            setWithDirectionalBorder(currentRow, currentCol - 1, 0, fillGrid);
            currentCol--;
        }
    }

    if(setCounter < MINIMUM_LEN) {
        if(DEBUG_PRINT_RESTART_MSG) {
            printf("Bad length generated (%d), starting over.\n", setCounter);
        }
        setCounter = 0;
        cleanup(fillGrid);
        goto restartLbl;
    }
    // print table
    if(DEBUG_PRINT_FILL_GRID) {
        for(int i = 0; i < ROWS; i++) {
            for(int j = 0; j < COLS; j++) {
                printf("%d ", fillGrid[i][j]);
            }
            printf("\n");
        }
    }
    printf("len = %d\n", setCounter);

    // clean up
    cleanup(fillGrid);
}