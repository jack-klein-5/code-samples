/**************************************************************************

This is a code sample from an assignment I did where I used multithreading
to multiply a matrix. This sample includes the functions from two files.
The first is the main thread and beginning of the program, and the second
is the code used for the threads that are created. It was written in C++ 
and is documented using Doxygen file and function headers. This project 
taught me the basics of multithreading and made me more familiar with
Linux as an operating system.

**************************************************************************/

/*!*************************************************************************
\file   main-thread.c
\author Jack Klein
\par    email: jack.klein\@digipen.edu
\par    DigiPen login: jack.klein
\par    Course: CS180
\par    Section: A
\par    Assignment #4
\date   3/15/2018
\brief  
    This file contains the implementation of the following functions for 
    the matrix thread assignment.
      
    Functions include:
    
       . main
       . get_matrix
       . print_matrix
  
*/
/**************************************************************************/

/*!*************************************************************************
  \brief Squares a matrix from a file by creating multiple threads
  
  \param argc
           The number of command line arguments given

  \param argv
           The command line arguments supplied

  \return
    Error code of whether the program returned successful
*/
/**************************************************************************/
int main(int argc, char **argv) 
{
  int width;              /* width of the matrix      */
  int *input_matrix;      /* the matrix read in       */
  int *output_matrix;     /* The resulting matrix     */
  int i;                  /* Loop variable            */
  int matsize;            /* Total size of the matrix */
  pthread_t *threadArray; /* Array of thread IDs      */
  data *dataArray;        /* Array of data structs    */

  if (argc < 2)
  {
    printf("Insufficient parameters supplied\n");
    return ERROR;
  }

  /* Read in matrix values from file */ 
  input_matrix = get_matrix(argv[1], &width);
  /* Initialize output matrix to anything */
  output_matrix = get_matrix(argv[1], &width);
  
  /* Set matrix size with found width */
  matsize = width * width;
  
  /* print the matrix */
  print_matrix(input_matrix, width);
  
  /* Allocate memory for arrays */
  threadArray = (pthread_t *)malloc(matsize * sizeof(pthread_t));
  if (!threadArray)
  {
    printf("Can't malloc thread ID array \n");
    return ERROR;
  }
  dataArray = (data *)malloc(matsize * sizeof(data));
  if (!dataArray)
  {
    printf("Can't malloc data array \n");
    return ERROR;
  }
  
  /* Creating all of the other threads and supplying them with */
  /* their parameters                                          */
  for (i = 0; i < matsize; i++)
  {
    /* Set data struct variables */
    dataArray[i].threadNumber = i;
    dataArray[i].input_matrix = input_matrix;
    dataArray[i].output_matrix = output_matrix;
    dataArray[i].row = (i / width);
    dataArray[i].column = (i % width);
    dataArray[i].width = width;
    /* Create thread */
    pthread_create(&threadArray[i], NULL, matrixThread, &dataArray[i]);
  }

  /* Waiting for all of the threads to finish. */
  for (i = 0; i < matsize; i++)
  {
    pthread_join(threadArray[i], NULL);
  }
    
  /* Print resulting matrix */
  print_matrix(output_matrix, width);

  /* Free matrix memory */
  free(input_matrix);
  free(output_matrix);
  /* Free array memory */
  free(threadArray);
  free(dataArray);
  
  return 0;
}


/*!*************************************************************************
\file   matrix-thread.c
\author Jack Klein
\par    email: jack.klein\@digipen.edu
\par    DigiPen login: jack.klein
\par    Course: CS180
\par    Section: A
\par    Assignment #4
\date   3/15/2018
\brief  
    This file contains the implementation of the following functions for 
    the matrix thread assignment.
      
    Functions include:
    
       . matrixThread
  
*/
/**************************************************************************/

#include "matrix-thread.h" /* Data struct for thread */


/*!*************************************************************************
  \brief Start of program, squares one part of the matrix
  
  \param matrixData
    The data struct with matrix information for thread
  
  \return 
    A null pointer
*/
/**************************************************************************/
void *matrixThread(void *matrixData)
{
  int threadNumber; /* Thread number                */
  int row;          /* Row number to square         */
  int column;       /* Column number to square      */
  int width;        /* Width of matrix              */
  int numberToMult; /* The lhs number to multiply   */
  int sum = 0;      /* Sum of matrix multiplication */
  int i;            /* Loop variable                */
  
  /* Cast pointer to data struct */
  data *threadData = (data *)matrixData;
  
  /* Set variables to those of thread struct */
  row = threadData->row;
  column = threadData->column;
  threadNumber = threadData->threadNumber;
  width = threadData->width;
  
  for (i = 0; i < width; i++)
  {
    /* Square the correct part of the matrix */
    numberToMult = threadData->input_matrix[(width * row) + i];
    /* Multiply row by column */
    sum += numberToMult * threadData->input_matrix[(width * (i)) + column];
  }
  /* Set the coordinate of new matrix to sum */
  threadData->output_matrix[threadNumber] = sum;

  /* Return nothing for pthread */
  return 0;
}
