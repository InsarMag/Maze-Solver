#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "maze.h"

typedef struct _maze maze;

struct _maze {
  int width;
  int height;
  int *data;
  int *dataBackup;

  int solutionLength;
  int solutionCombIndex;

  int pathLength;
  
  FILE *fileRef;
};

/* ----------Configuring Maze Structure---------- */

int getWidth(FILE * filePtr) {
  // Check input
  if(filePtr == NULL)
    return 0;

  /* RESET */
  rewind(filePtr);

  // Get the width
  int size = -1;
  char length[20];
  fscanf(filePtr, "%s", length);
  while(length[size] != '\0') {
    size++;
  }

  if(size <= 0)
    return 0;

  //return
  return size;
}

int getHeight(FILE * filePtr) {
  // Check input
  if(filePtr == NULL)
    return 0;

  /* RESET */
  rewind(filePtr);
  
  // Get the height by deviding the total size by width
  int size = 0, buffer = 0;

  while(fscanf(filePtr, "%1d", &buffer) != EOF)
    size++;
  size /= getWidth(filePtr);

  if(size <= 0)
    return 0;

  //return
  return size;
}

maze * newMaze(int x, int y) {
  // Check inputs
  if(x <= 0 || y <= 0) {
    printf("negative or zero size\n");
    return NULL;
  } 

  // Allocate maze structure
  maze *M = malloc(sizeof(maze));
  if(!M) {
    printf("creation failed\n");
    return NULL;
  }

  // Set dimentions
  M->width = x;
  M->height = y;
  M->data = NULL;
  M->dataBackup = NULL;
  M->solutionLength = 1000;
  M->solutionCombIndex = 0;
  M->pathLength = 0;
  M->fileRef = NULL;

  

  // Set maze data - allocate an integer array of length height * rows
  M->data = (int *)malloc(x * y * sizeof(int));
  if(!M->data) {
    free(M);
    printf("data allocation failed\n");
    return NULL;
  }
  
  // set all data to 0
  for(int i = 0; i < (x*y); i++) {
    M->data[i] = 0;
  }

  //printf("returning maze adress");
  //return
  return M;
}

maze * createMazeFromFile(FILE * filePtr) {
  // Check inputs
  if(filePtr == NULL) {
    return NULL;
  }

  // Initialize variables
  int err;
  int x_Size = getWidth(filePtr);
  int y_Size = getHeight(filePtr);

  // Create new maze
  maze *M = newMaze(x_Size, y_Size);
  if(!M) return NULL;

  // Get maze data from the file
  err = transferMazeDataFromFile(M, filePtr);
  if(err) {
    free(M);
    return NULL;
  }

  // Set the file pointer to the file that has been accessed
  M->fileRef = filePtr;
  if(!M->fileRef) {
    free(M);
    return NULL;
  }

  // Backup dagta
  err = backupMazeData(M);
  if(err) {
    free(M);
    return NULL;
  }

  //return
  return M;
}

maze * duplicateMaze(maze * M) {
  // Check inputs
  if(!M) return NULL;

  // Create new maze
  maze * newM = newMaze(M->width, M->height);
  if(!newM) return NULL;

  // Copy all of the data from the old maze
  int err;
  err = copyMazeData(M, newM);
  if(err) {
    free(newM);
    return NULL;
  }  

  // Backup the data of the new maze
  err = backupMazeData(newM);
  if(err) {
    free(newM);
    return NULL;
  }

  //return
  return M;
}

void deleteMaze(maze * M) {
  // Check inputs
  if(M) {
    // free maze data
    free(M->data);
    // free maze backup if exists
    if(M->dataBackup)
      free(M->dataBackup);
    // free maze itself
    free(M);
  }
}

/* ----------Maze data manipulation---------- */

int backupMazeData(maze * M) {
  // Check input
  if(!M) return -1;

  // allocate memory to store the copy of the data
  M->dataBackup = (int *)malloc(M->width * M->height * sizeof(int));
  if(!M->dataBackup) {
    printf("data backup allocation failed\n");
    return -1;
  }
  
  // copy all data into backup
  for(int i = 0; i < (M->width*M->height); i++) {
    M->dataBackup[i] = M->data[i];
  }

  //return
  return 0;
}

int resetMazeDataFromFile(maze * M) {
  // Check inputs
  if(M == NULL || M->fileRef == NULL) return -1;

  // Get data
  int err;
  err = transferMazeDataFromFile(M, M->fileRef);
  if(err) return -1;

  return 0;  
}

int resetMazeDataFromBackup(maze * M) {
  // Check inputs
  if(M == NULL || M->data == NULL || M->dataBackup == NULL) {
    return -1;
  }

  // Copy data from backup
  for(int i = 0; i < (M->width*M->height); i++) {
    M->data[i] = M->dataBackup[i];
  }

  // Return
  return 0;
}

int transferMazeDataFromFile(maze * M, FILE * filePtr) {
  // Check inputs
  if(!M || !M->data || filePtr == NULL)
    return -1;

  /* RESET */
  rewind(filePtr); 

  // Read the file and assign maze data
  int input;
  for(int i = 0; i < (M->height*M->width); i++) {
    fscanf(filePtr, "%1d", &input);
    M->data[i] = input;
  }

  //return
  return 0;
}

int copyMazeData(maze const * fromM, maze * intoM) {
  // Check inputs
  if(fromM == NULL || intoM == NULL || fromM->width != intoM->width
     || fromM->height != intoM->height) {
    return -1;
  }

  // Duplicate all data from one maze to the other
  for(int i = 0; i < (intoM->width*intoM->height); i++) {
    intoM->data[i] = fromM->data[i];
  }

  //return
  return 0;
}

int assignRandomData(maze * M) {
  // Check input
  if(!M) return -1;  

  // Variable incialization
  int i, n;

  // Create a random seed for the rand()
  srand(time(0));

  // Fill the maze with random data
  for(i = 0; i < (M->width * M->height); i++) {
    // Get a random number from 0 to 10
    n = rand() % 10;

    // Set 70% of space to empty
    if(n < 7)
      n = 0;
    else
      n = 2;

    // Assign data
    M->data[i] = n;
  }
  /*
  //TODO, randomly create start and finish in the maze
  while(n >= (M->width*M->height)) {
    n = rand();
    }*/
  
  // Set start point
  //n = rand() % 10;
  M->data[0] = 1;
  /*
  while(n >= (M->width*M->height)) {
    n = rand();
    }*/
  
  // Set finish
  //n = rand() % 10;
  M->data[i-1] = 3;

  // Backup data
  backupMazeData(M);

  //return
  return 0;
}

/* ----------Finding the path---------- */

int findPathLong(maze * M) {
  // Check inputs
  if(M == NULL || M->height <= 0 || M->width <= 0) return -1;

  /* RESET */
  resetMazeDataFromBackup(M);

  // Variable declaration
  int combNum, x, y, err;
  M->solutionLength = 0;

  // Run through all of the combinations
  for(y = 0; y < M->height; y++) {
    for(x = 0; x < M->width; x++) {
      // locates the Start position
      if(*(M->data + y*M->width + x) == 1) {

	// check all 24 combinations
	for(combNum = 1; combNum < 25; combNum++){
	  
	  // reset the maze
	  err = resetMazeDataFromBackup(M);
	  if(err) return -1;
	  
	  M->pathLength = 0;
	  check(y, x, M, combNum);
	  //printf("path = %d\n", M->pathLength);

	  // if the shorter path is found, assignes the legth;
	  if(M->pathLength > M->solutionLength) {
	    M->solutionLength = M->pathLength;
	    M->solutionCombIndex = combNum;
	    //printf("combNum = %d\n", combNum);
	    //printf("sol, length = %d\n", M->solutionLength);
	  }
	}
	
	break; // Once the Start is found, breaks out of the loop
      }
    }
  }

  /* RESET */
  resetMazeDataFromBackup(M);

  // Run the stored combination
  for(y = 0; y < M->height; y++) {
    for(x = 0; x < M->width; x++) {
      // locates the Start position
      if(*(M->data + y*M->width + x) == 1) {
	// Runs a spesific conviation
	check(y, x, M, M->solutionCombIndex);
	*(M->data + y*M->width + x) = 1; // Put start icon
	break; // Once the Start is found, breaks out of the loop
      }
    }
  }

  // print the resulted length
  if(M->solutionLength == 0)
    printf("\nNo solutin has been found\n");
  else
    printf("\nLongest path found is %d\n", M->solutionLength-1);

  //return
  return 0;
}

int findPathShort(maze * M) {
  // Check inputs
  if(M == NULL || M->height <= 0 || M->width <= 0) return -1;

  /* RESET */
  resetMazeDataFromBackup(M);

  // Variable declaration
  int combNum, x, y, err;
  M->solutionLength = 1000;
  
  for(y = 0; y < M->height; y++) {
    for(x = 0; x < M->width; x++) {
      // locates the Start position
      if(*(M->data + y*M->width + x) == 1) {

	// check all 24 combinations
	for(combNum = 1; combNum < 25; combNum++){
	  
	  // reset the maze
	  err = resetMazeDataFromBackup(M);
	  if(err) return -1;
	  
	  M->pathLength = 0;
	  check(y, x, M, combNum);
	  //printf("path = %d\n", M->pathLength);

	  // if the shorter path is found, assignes the legth;
	  if(M->pathLength < M->solutionLength && M->pathLength != 0) {
	    M->solutionLength = M->pathLength;
	    M->solutionCombIndex = combNum;
	    //printf("sol, length = %d\n", M->solutionLength);
	  }
	}
	
	break; // Once the Start is found, breaks out of the loop
      }
    }
  }

  /* RESET */
  resetMazeDataFromBackup(M);
  
  for(y = 0; y < M->height; y++) {
    for(x = 0; x < M->width; x++) {
      // locates the Start position
      if(*(M->data + y*M->width + x) == 1) {
	check(y, x, M, M->solutionCombIndex);

	*(M->data + y*M->width + x) = 1; // Put Start icon
	break; // Once the Start is found, breaks out of the loop
      }
    }
  }

  // print resulted length
  if(M->solutionLength == 1000)
    printf("\nNo solutin has been found\n");
  else
    printf("\nShortest path found is %d\n", M->solutionLength-1);

  //return
  return 0;
}

int check(int row, int col, maze *M, int combNum) {
  // Check inpts
  if(row >= M->height  || col >= M->width || row < 0 || col < 0) {
    return -1; // Slot does not exist
  }

  // if the slot is not empty (not 2-Wall, 5-already checked) and exists, check next slot
  // if returned -1 check next side
  // if no sides left, return -1;
  // else put (4-Path) into slot and return 1

  /* check contents of the slot */
  
  // if slot is not empty (2-Wall, 5-already checked), return -1
  if(*(M->data + row*M->width + col) == 2 || *(M->data + row*M->width + col) == 5)
    return -1;
  // if solution found (3-Finish) return 1;
  else if(*(M->data + row*M->width + col) == 3) {
    *(M->data + row*M->width + col) = 3;
    return 1;
  }
  // else replace contents with taken (5-already checked) and check the sides.
  else {
    *(M->data + row*M->width + col) = 5;
    
    // check all four sides
    // if check() does not return -1, then falls through, puts 4 and returns 1
    // Get combination. Call sides in order provided in the combination.
    
    int sideNum = 0;
    int comb = combination(combNum);
    sideNum = comb % 10;
    
    if(side(row, col, M, combNum, sideNum) == -1) {
      comb /= 10;
      sideNum = comb % 10;
      
      if(side(row, col, M, combNum, sideNum) == -1) {
	comb /= 10;
	sideNum = comb % 10;
	
	if(side(row, col, M, combNum, sideNum) == -1) {
	  comb /= 10;
	  sideNum = comb % 10;
	  
	  if(side(row, col, M, combNum, sideNum) == -1) {
	    return -1;
	  }
	}
      }
    }
  }
    
  // Put 4-Path into the current position
  *(M->data + row*M->width + col) = 4;
  M->pathLength++; // count the steps

  //return
  return 1;
}

int side(int row, int col, maze *M, int combNum, int sideNum) {
  //Switch to check one of the four sides.
  switch(sideNum) {
  case(1):
    return check(row - 1, col, M, combNum);
    break;
  case(2):
    return check(row, col - 1, M, combNum);
    break;
  case(3):
    return check(row + 1, col, M, combNum);
    break;
  case(4):
    return check(row, col + 1, M, combNum);
    break;
  default:
    printf("ERROR, COUNTING HAS FALLEN OUT OF THE 4 SIDES");
    return -5;
    break;
  }
}

int combination(int n) {
  // Set a default combination that will be return in the case of an error
  int comb = 1234;

  // Switch statement
  switch(n) {
  case(1): comb = 1234; break;
  case(2): comb = 1243; break;
  case(3): comb = 1324; break;
  case(4): comb = 1423; break;
  case(5): comb = 1342; break;
  case(6): comb = 1432; break;
    
  case(7): comb = 2134; break;
  case(8): comb = 2143; break;
  case(9): comb = 3124; break;
  case(10): comb = 4123; break;
  case(11): comb = 3142; break;
  case(12): comb = 4132; break;
    
  case(13): comb = 2314; break;
  case(14): comb = 2413; break;
  case(15): comb = 3214; break;
  case(16): comb = 4213; break;
  case(17): comb = 3412; break;
  case(18): comb = 4312; break;
	  
  case(19): comb = 2341; break;
  case(20): comb = 2431; break;
  case(21): comb = 3241; break;
  case(22): comb = 4231; break;
  case(23): comb = 3421; break;
  case(24): comb = 4321; break;
  }

  //return
  return comb;
}

/* ----------Displaying the maze---------- */

int printMaze(maze *M) {
  // Check inputs
  if(!M) return -1;
  
  /* Print the maze to the terminal with a barrier around it*/

  // Initialize variables
  int i, j;

  // Set top side
  printf("\n");
  for(i = 0; i < (M->width + 1); i++)
    printf("O");

  // Add before and after each line
  for(i = 0; i < M->height; i++) {
    printf("O\nO");
    for(j = 0; j < M->width; j++) {
      printf("%c", intToChar(*(M->data + i*M->width + j)));
    }
  }
  printf("O\n");

  // Set bottom side
  for(i = 0; i < (M->width + 2); i++)
    printf("O");
  printf("\n");
  
  //return
  return 0;
}

int intToChar(int n) {
  // Assign char depending on the input
  // if E is printed - error has occured
  char out = 'E';

  // Switch statement
  switch(n) {
  case(0):
    out = ' ';
    break;
  case(1):
    out = 'S';
    break;
  case(2):
    out = 'X';
    break;
  case(3):
    out = 'F';
    break;
  case(4):
    out = '.';
    break;
  case(5):
    out = ' ';
    break;
  }

  //return
  return out;
}

/* ----------usage and error messages---------- */

int usage() {
  // print usage
  printf("Code finds the path of a given maze\n");
  printf("Use: ./maze -f mazelib/maze1.txt\n");
  return 0;
}
