#ifndef MAZE_H_
#define MAZE_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/* Global variables and defenitions */

typedef struct _maze maze;

/* Function prototypes */

/* ----------Configuring Maze Structure---------- */

// Returns the width and the height of the maze stored in the File pointer
// Returns 0 if fails
int getWidth(FILE * filePtr);
int getHeight(FILE * filePtr);

// Allocates memory for a new maze of a given size and returns a pointer to it
// Returns NULL if fails
maze * newMaze(int xSize, int ySize);

// Allocates memory for a new maze from the provided file and returns a pointer to it
// File pointer is stored in the maze structure
// Returns NULL if fails
maze * createMazeFromFile(FILE * filePtr);

// Creates a duplicate of the maze passed, returns a pointer to the new maze
// Automatically backs up the data.
// Returns NULL if fails
maze * duplicateMaze(maze * M);

// Deletes the maze
void deleteMaze(maze * M);

/* ----------Maze data manipulation---------- */

// Stores a copy of maze data within within the maze structure
// Returns -1 if fails
int backupMazeData(maze * M);

// Resets the maze structure from the file referance
// Returns -1 if fails
int resetMazeDataFromFile(maze * M);

// Resets the maze from the backup
// Returns -1 if fails
int resetMazeDataFromBackup(maze * M);

// Reads from file and assignes it to the maze data
// Returns -1 if fails
int transferMazeDataFromFile(maze * M, FILE * filePtr);

// Copies the maze data from one maze to another
// Returns -1 if fails
int copyMazeData(maze const * fromM, maze * intoM);

// Assignes a random data to the maze
int assignRandomData(maze * M);

/* ----------Finding the path---------- */

// Runns through all of the combinations and stores the longest one in the data
int findPathLong(maze * M);

// Runns through all of the combinations and stores the shortest one in the data
int findPathShort(maze * M);

// Checkes the slots of the array until it finds a path
// returns -1 if the slot is not available
// returns 1 if find a finish and replacess slot under it with 4
int check(int row, int col, maze *M, int combNum);

// Checks one of the four sides possible and returns the result of the check.
// sideNum is 1-4 option, 1-Up, 2-Left, 3-Down, 4-Right
int side(int row, int col, maze *M, int combNum, int sideNum);

// Returns 1 of the 24 possible combinationss of the order in whcih to check sides.
int combination(int n);

/* ----------Displaying the maze---------- */

// Prints the maze data to the terminal
int printMaze(maze *M);

// Transforms the integer to a character for Maze visualization
// 0 or 5-' '-empty | 1-'S'-Start | 2-'X'-Wall | 3-'F'-Finish | 4-'.'-Path
int intToChar(int n);

/* ----------usage and error messages---------- */

// Prints the usage to the terminal
int usage();

#endif
