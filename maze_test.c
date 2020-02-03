#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "maze.h"

int pathLength, pathShort, combIndex;

/* Main function */
int main(int argc, char *argv[]) {
  
  /* Read a provided file */

  FILE * filePtr; //pointer to a file
  
  if(argc == 1) {
    filePtr = fopen("mazeLib/maze1.txt", "r");
  }
  else if(argc == 3) {
    /* read input */
    if(strcmp(argv[1], "-f") == 0) {
      filePtr = fopen(argv[2], "r");
    }
    else {
      printf("ERROR, wrong arguments\n");
      printf("%s", argv[2]);
      usage();
      return -1;
    }
  }
  else {
    printf("ERROR, wrong amount of arguments\n");
    usage();
    return -1;
  }
    
  if(filePtr == NULL) {
    printf("ERROR, missing a maze file\n");
    usage();
    return -1;
  }

  /* maze manipulations */

  // Create a maze from file
  maze * mazePtr = createMazeFromFile(filePtr);

  // Check the creation
  if(mazePtr == NULL) {
    printf("maze is null\n");
    return 0;
  }

  // Print emty maze to the terminal
  printMaze(mazePtr);

  // Get longest path and print it
  findPathLong(mazePtr);
  printMaze(mazePtr);

  // Get shortest path and print it
  findPathShort(mazePtr);
  printMaze(mazePtr);

  // Delete maze and close the file
  deleteMaze(mazePtr);
  fclose(filePtr);

  /* Random maze */
  printf("\n===================\n");
  
  mazePtr = newMaze(100,200);
  assignRandomData(mazePtr);

  // Print emty maze to the terminal
  printMaze(mazePtr);

  // Get longest path and print it
  findPathLong(mazePtr);
  printMaze(mazePtr);

  // Get shortest path and print it
  findPathShort(mazePtr);
  printMaze(mazePtr);
  

  deleteMaze(mazePtr);

  //return
  return 0;
}
