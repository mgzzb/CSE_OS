/*
fractal.c - Mandelbrot fractal generation
Starting code for CSE 30341 Project 3 - Spring 2023
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>
#include <stdbool.h>

#include "bitmap.h"
#include "fractal.h"

/*
Compute the number of iterations at point x, y
in the complex space, up to a maximum of maxiter.
Return the number of iterations at that point.

This example computes the Mandelbrot fractal:
z = z^2 + alpha

Where z is initially zero, and alpha is the location x + iy
in the complex plane.  Note that we are using the "complex"
numeric type in C, which has the special functions cabs()
and cpow() to compute the absolute values and powers of
complex values.
*/

static int compute_point( double x, double y, int max )
{
	double complex z = 0;
	double complex alpha = x + I*y;

	int iter = 0;

	while( cabs(z)<4 && iter < max ) {
		z = cpow(z,2) + alpha;
		iter++;
	}

	return iter;
}

// Function to show the help options
void helpShow(){
            printf("Usage: ./fractal [OPTIONS]\n\n");
            printf("Options:\n");
            printf("  -help           Display help information\n");
            printf("  -xmin X         New value for x min, current one is %.1f\n", DEFAULT_MIN_X);
            printf("  -xmax X         New value for x max, current one is %.1f\n", DEFAULT_MAX_X);
            printf("  -ymin Y         New value for y min, current one is %.1f\n", DEFAULT_MIN_Y);
            printf("  -ymax Y         New value for y max, current one is %.1f\n", DEFAULT_MAX_Y);
            printf("  -maxiter N      New value for the maximum number of iterations (must be an integer), current one is %d\n", DEFAULT_MAX_ITER);
            printf("  -width W        New width for the output image, current one is %d\n", DEFAULT_PIXEL_WIDTH);
            printf("  -height H       New height for the output image, current one is %d\n", DEFAULT_PIXEL_HEIGHT);
            printf("  -output F       New name for the output file, include .bmp at the end\n");
            printf("  -threads N      Number of threads to use for processing (default: 2)\n");
            printf("  -row            Run using a row-based approach\n");
            printf("  -task           Run using a thread-based approach\n");          
}

/*
Compute an entire image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax).

HINT: Generally, you will want to leave this code alone and write your threaded code separately

*/

// Function to compute the image using a single thread
void compute_image_singlethread ( struct FractalSettings * pSettings, struct bitmap * pBitmap)
{
	int i,j;

	// For every pixel i,j, in the image...

	for(j=0; j<pSettings->nPixelHeight; j++) {

		for(i=0; i<pSettings->nPixelWidth; i++) {

			// Scale from pixels i,j to coordinates x,y
			double x = pSettings->fMinX + i*(pSettings->fMaxX - pSettings->fMinX) / pSettings->nPixelWidth;
			double y = pSettings->fMinY + j*(pSettings->fMaxY - pSettings->fMinY) / pSettings->nPixelHeight;

			// Compute the iterations at x,y
			int iter = compute_point(x,y,pSettings->nMaxIter);

			// Convert a iteration number to an RGB color.
			// (Change this bit to get more interesting colors.)
			int gray = MAKE_RGBA(192, 0, 192, 127) * iter / pSettings->nMaxIter;;

            // Set the particular pixel to the specific value
			// Set the pixel in the bitmap.
			bitmap_set(pBitmap,i,j,gray);

		}
	}
}

/*
    Write your threaded code separately
*/

// Initialize a counter to be used by the lock
int counter;

// Initialize the lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Initializing a struct that contains the info for each thread
typedef struct {

  int row;
  pthread_t tid;
  struct FractalSettings * pSettings;
  struct bitmap * pBitmap;

} threadArgument;

// Initializing a struct that contains the info for each task
typedef struct {

  int startX, startY, stopX, stopY;
  pthread_t tid;
  struct FractalSettings * pSettings;
  struct bitmap * pBitmap;
  
} taskInfo;

// Initialize the array of tasks
taskInfo tasksArray[100000];

// Function to compute the image using multi-threading via rows
void compute_image_row( void * pData )
{

	int i,j;

    // Use the struct to get the thread settings
    struct threadSettings * data = pData;

    struct FractalSettings * pSettings = (struct FractalSettings *) data->threadsettings;
    struct bitmap * pBitmap = (struct bitmap *) data->threadbitmap;
    int min = (int) data->start;
    int max = (int) data->stop;


    // For every pixel i,j, in the image...

	for(j = min; j < max; j++) {

		for(i=0; i<pSettings->nPixelWidth; i++) {

			// Scale from pixels i,j to coordinates x,y
			double x = pSettings->fMinX + i*(pSettings->fMaxX - pSettings->fMinX) / pSettings->nPixelWidth;
			double y = pSettings->fMinY + j*(pSettings->fMaxY - pSettings->fMinY) / pSettings->nPixelHeight;

			// Compute the iterations at x,y
			int iter = compute_point(x,y,pSettings->nMaxIter);

			// Convert a iteration number to an RGB color.
			// (Change this bit to get more interesting colors.)
			int gray = MAKE_RGBA(192, 0, 192, 127) * iter / pSettings->nMaxIter;

            // Set the particular pixel to the specific value
			// Set the pixel in the bitmap.
			bitmap_set(pBitmap,i,j,gray);

		}
	} 

}

// Function to compute the image using task-based execution
void *compute_image_task ( void * pData )
{
    // Use the struct to get necessary task information
    taskInfo * args = (taskInfo *) pData;

	int i, j, tempVal;

    // While loop to lock the mutex and update the counter
    while (1) {

        // Lock the mutex
        pthread_mutex_lock(&lock);
        
        // Check if counter is greater than or equal to 0 to update tempVal and counter
        if (counter >= 0) {
            tempVal = counter;
            counter--;
        }
    // Else to unlock thread if counter is negative and return null
    else {

        pthread_mutex_unlock(&lock);
        return NULL;

    }

    // Unlock the mutex
    pthread_mutex_unlock(&lock);
      
  	// For every pixel i,j, in the image...
    
  	for (j = tasksArray[tempVal].startY; j < tasksArray[tempVal].stopY; j++) {
  		for (i = tasksArray[tempVal].startX; i < tasksArray[tempVal].stopX; i++) {
        
  			// Scale from pixels i,j to coordinates x,y
  			double x = args->pSettings->fMinX + i*(args->pSettings->fMaxX - args->pSettings->fMinX) / args->pSettings->nPixelWidth;
  			double y = args->pSettings->fMinY + j*(args->pSettings->fMaxY - args->pSettings->fMinY) / args->pSettings->nPixelHeight;
  
  			// Compute the iterations at x,y
  			int iter = compute_point(x,y,args->pSettings->nMaxIter);
  
  			// Convert a iteration number to an RGB color.
  			// (Change this bit to get more interesting colors.)
  			int gray = MAKE_RGBA(192, 0, 192, 127) * iter / args->pSettings->nMaxIter;
  
              // Set the particular pixel to the specific value
  			// Set the pixel in the bitmap.
  			bitmap_set(args->pBitmap,i,j,gray);

  		}
  	}
  }

  return NULL;

}

/* Process all of the arguments as provided as an input and appropriately modify the
   settings for the project 
   @returns 1 if successful, 0 if unsuccessful (bad arguments) */
char processArguments (int argc, char * argv[], struct FractalSettings * pSettings)
{
    /* If we don't process anything, it must be successful, right? */
    return 1;
}

int main( int argc, char *argv[] )
{
    struct FractalSettings  theSettings;

	// The initial boundaries of the fractal image in x,y space.
    theSettings.fMinX = DEFAULT_MIN_X;
    theSettings.fMaxX = DEFAULT_MAX_X;
    theSettings.fMinY = DEFAULT_MIN_Y;
    theSettings.fMaxY = DEFAULT_MAX_Y;
    theSettings.nMaxIter = DEFAULT_MAX_ITER;

    theSettings.nPixelWidth = DEFAULT_PIXEL_WIDTH;
    theSettings.nPixelHeight = DEFAULT_PIXEL_HEIGHT;

    theSettings.nThreads = DEFAULT_THREADS;
    theSettings.theMode  = MODE_THREAD_SINGLE;
    
    strncpy(theSettings.szOutfile, DEFAULT_OUTPUT_FILE, MAX_OUTFILE_NAME_LEN);

    char * newFileName = NULL;
    bool nameChange = false;

    /* TODO: Adapt your code to use arguments where the arguments can be used to override 
             the default values 

        -help         Display the help information
        -xmin X       New value for x min
        -xmax X       New value for x max
        -ymin Y       New value for y min
        -ymax Y       New value for y max
        -maxiter N    New value for the maximum number of iterations (must be an integer)     
        -width W      New width for the output image
        -height H     New height for the output image
        -output F     New name for the output file
        -threads N    Number of threads to use for processing (default is 1) 
        -row          Run using a row-based approach        
        -task         Run using a thread-based approach

        Support for setting the number of threads is optional

        You may also appropriately apply reasonable minimum / maximum values (e.g. minimum image width, etc.)
    */

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {

        // Check if help argument is used to print out h
        if (strcmp(argv[i], "-help") == 0) {
            helpShow();
            return 0;
        // Else if to check other different flags and adjust values
        } else if (strcmp(argv[i], "-xmin") == 0) {
            if (i+1 >= argc) {
                printf("Error: -xmin requires a value\n");
                return 1;
            } 
            theSettings.fMinX = atof(argv[++i]);
        } else if (strcmp(argv[i], "-xmax") == 0) {
            if (i+1 >= argc) {
                printf("Error: -xmax requires a value\n");
                return 1;
            } 
            theSettings.fMaxX = atof(argv[++i]);
        } else if (strcmp(argv[i], "-ymin") == 0) {
            if (i+1 >= argc) {
                printf("Error: -ymin requires a value\n");
                return 1;
            } 
            theSettings.fMinY = atof(argv[++i]);
        } else if (strcmp(argv[i], "-ymax") == 0) {
            if (i+1 >= argc) {
                printf("Error: -ymax requires a value\n");
                return 1;
            } 
            theSettings.fMaxY = atof(argv[++i]);
        } else if (strcmp(argv[i], "-maxiter") == 0) {
            if (i+1 >= argc) {
                printf("Error: -maxiter requires a value\n");
                return 1;
            } 
            theSettings.nMaxIter = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-width") == 0) {
            if (i+1 >= argc) {
                printf("Error: -width requires a value\n");
                return 1;
            } 
            theSettings.nPixelWidth = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-height") == 0) {
            if (i+1 >= argc) {
                printf("Error: -height requires a value\n");
                return 1;
            } 
            theSettings.nPixelHeight = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-output") == 0) {
            if (i+1 >= argc) {
                printf("Error: -output requires a file name\n");
                return 1;
            } 
            newFileName = argv[++i];
            nameChange = true; 
        } else if (strcmp(argv[i], "-threads") == 0) {
            if (i+1 >= argc) {
                printf("Error: -threads requires a value\n");
                return 1;
            } 
            theSettings.nThreads = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-row") == 0) {
            theSettings.theMode = MODE_THREAD_ROW;
        } else if (strcmp(argv[i], "-task") == 0) {
            theSettings.theMode = MODE_THREAD_TASK;
        
        // Else to see if unknown option entered to exit program
        } else {
            printf("Unknown option: %s\n", argv[i]);
            return 1;
        }

    }

    // Validate command line arguments and if it fails, then exit
    if (theSettings.fMinX >= theSettings.fMaxX || theSettings.fMinY >= theSettings.fMaxY || theSettings.nMaxIter <= 0 || theSettings.nPixelWidth <= 0 || theSettings.nPixelHeight <= 0 || theSettings.nThreads <= 0) {
        fprintf(stderr, "Invalid argument(s)\n");
        return 1;
    }

   /* Are there any locks to set up? */

   if(processArguments(argc, argv, &theSettings))
   {
        /* Dispatch here based on what mode we might be in */
        if(theSettings.theMode == MODE_THREAD_SINGLE)
        {
            /* Create a bitmap of the appropriate size */
            struct bitmap * pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

            /* Fill the bitmap with dark blue */
            bitmap_reset(pBitmap,MAKE_RGBA(0, 0, 0, 255));

            /* Compute the image */
            compute_image_singlethread(&theSettings, pBitmap);

            // Save the image in the stated file.
            if(nameChange){
                if(!bitmap_save(pBitmap,newFileName)) {
                    fprintf(stderr,"fractal: couldn't write to %s: %s\n",newFileName,strerror(errno));
                    return 1;
                }
            }
            else{
                if(!bitmap_save(pBitmap,theSettings.szOutfile)) {
                    fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));
                    return 1;
                } 
            }
                       
        }
        else if(theSettings.theMode == MODE_THREAD_ROW)
        {
            /* A row-based approach will not require any concurrency protection */

            /* Could you send an argument and write a different version of compute_image that works off of a
               certain parameter setting for the rows to iterate upon? */

            /* Create a bitmap of the appropriate size */
            struct bitmap * pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

            /* Fill the bitmap with dark blue */
            bitmap_reset(pBitmap,MAKE_RGBA(0, 0, 0, 255));

            int numThreads = theSettings.nThreads;

            struct threadSettings threadArgs[numThreads];
            int end = theSettings.nPixelHeight / numThreads;
            threadArgs[0].start = 0;
            threadArgs[0].stop = end;

            for (int i = 0; i < numThreads; i++) {

                threadArgs[i].threadsettings = &theSettings;
                threadArgs[i].threadbitmap = pBitmap;

                /* Compute the image if error was not found */
                if (pthread_create(&threadArgs[i].ID, NULL, (void *) *compute_image_row, &threadArgs[i]) != 0) {
                    printf("Error with pthread_create().\n");
                    exit(1);
                }

                if (i+1 < numThreads) {
                    threadArgs[i+1].start = threadArgs[i].stop;
                    threadArgs[i+1].stop = end + threadArgs[i].stop;
                }
            }

            // Error Handling
            for (int i = 0; i < numThreads; i++) {
                if (pthread_join(threadArgs[i].ID, NULL) != 0) {
                    printf("Error with pthread_join().\n");
                    exit(1);
                }
            }

            /* Save the image in the stated file */
            if(nameChange){
                if(!bitmap_save(pBitmap,newFileName)) {
                    fprintf(stderr,"fractal: couldn't write to %s: %s\n",newFileName,strerror(errno));
                    return 1;
                }
            }
            else{
                if(!bitmap_save(pBitmap,theSettings.szOutfile)) {
                    fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));
                    return 1;
                } 
            }

        }
        else if(theSettings.theMode == MODE_THREAD_TASK)
        {
            /* For the task-based model, you will want to create some sort of a way that captures the instructions
               or task (perhaps say a startX, startY and stopX, stopY in a struct).  You can have a global array 
               of the particular tasks with each thread attempting to pop off the next task.  Feel free to tinker 
               on what the right size of the work unit is but 20x20 is a good starting point.  You are also welcome
               to modify the settings struct to help you out as well.  
               
               Generally, it will be good to create all of the tasks into that array and then to start your threads
               with them in turn attempting to pull off a task one at a time.  
               
               While we could do condition variables, there is not really an ongoing producer if we create all of
               the tasks at the outset. Hence, it is OK whenever a thread needs something to do to try to access
               that shared data structure with all of the respective tasks.  
               */

            /* Create a bitmap of the appropriate size */
            struct bitmap * pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

            /* Fill the bitmap with dark blue */
            bitmap_reset(pBitmap,MAKE_RGBA(0,0,255,0));

            /* Compute the image */
            int i, j, taskWidth = 20, taskHeight = 20, count = 0;
            threadArgument threadArguments[10];
          
            // Initialize the tasks array
            for (i = 0; i < theSettings.nPixelHeight; i += taskHeight) {
                for (j = 0; j < theSettings.nPixelWidth; j += taskWidth) {
                    tasksArray[count].startY = i;
                    tasksArray[count].startX = j;
                    tasksArray[count].stopY = i+(taskHeight);
                    tasksArray[count].stopX = j+(taskWidth);
                    tasksArray[count].pSettings = &theSettings;
                    tasksArray[count].pBitmap = pBitmap;
                    count++;
                }
            }

            // Update the counter
            counter = count;

            // For loop to create the threads for the tasks
            for (i = 0; i < theSettings.nThreads; i++) {
                pthread_create(&(threadArguments[i].tid), NULL, compute_image_task, (void *) &tasksArray[0]);
            }

            // For loop to join the task threads
            for (i = 0; i < theSettings.nThreads; i++) {
                pthread_join(threadArguments[i].tid, NULL);
            }
          
            // Save the image in the stated file.
            if (!bitmap_save(pBitmap,theSettings.szOutfile)) {

                fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));

                // Free malloc and return 1
                bitmap_delete(pBitmap);
                return 1;

            }

        }
        else 
        {
            /* Uh oh - how did we get here? */
            printf("fractal: ERROR with mode - Wanting to run using an unknown approach\n");
            return 1;
        }
   }
   else
   {
        /* Probably a great place to dump the help */
        helpShow();

        /* Probably a good place to bail out */
        exit(-1);
   }

    /* TODO: Do any cleanup as required */

	return 0;
    
}
