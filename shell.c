/******************************************************
* Course: CMPE 382, Assignment 1
* Author: Berke Öcal && Berkay Özek
*******************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#define TRUE 1
#define FALSE 0
#define LINE_SIZE 512

char *trim(char*, char []);
char *removeExtraChars(char*, char);
char *removeTrails(char* str);
void *call(void *);
void clearInputBuffer(FILE *fp);
int  numberOfChars(char *str, char delimeter);
pthread_t *resize(pthread_t *, int size);
pthread_t *resizeThreads(pthread_t *, int *, int *);

int main(int argc, char *argv[]) {

    int  numberOfThreads = 5, numberOfSemicolon = 0, rc, count = 0;
    char *str = (char *)malloc(LINE_SIZE * sizeof(char));
    pthread_t *threads = (pthread_t *)malloc(numberOfThreads * sizeof(pthread_t));
    char *verticalBarEnd, *semicolonEnd;  // End pointers for strtok_r().
    char *verticalBarToken, *semicolonToken;    // Token pointers for strtok_r().
    char delimeters[] = {' ', ';', '|'};        // Possible redundant chars in input.
    int  quit = FALSE;
    FILE *fp;

    if (str == NULL) {
        printf("Not enough memory for input string.\n");
        exit(-1);
    }

    if (threads == NULL) {
        printf("Not enough memory for threads.\n");
        exit(-1);
    }

    /*
     *  Determine input stream based on argc and argv[].
     */
    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            printf("Cannot open input file. Switching to interactive mode.\n");
            fp = stdin;
        }
    } else {
        fp = stdin;
    }

    // Main loop.
    while (!quit) {

        /*
        *  Check EOF for FILE *fp.
        *  Necessary for batch and interactive mod swapping.
        */
        if (fp != stdin && feof(fp)) {
            fclose(fp);
            fp = stdin;
        }

        if (fp == stdin) { printf("prompt >"); }

        // Get next input from input stream.
        if (fgets(str, LINE_SIZE, fp) == NULL) { continue; }

        /*
         *  Check for buffer overflow.
         *  Clear system and input buffer if there is an overflow.
         */
        if (str[strlen(str) - 1] != '\n' && strlen(str) >= LINE_SIZE - 1) {
            clearInputBuffer(fp);
            printf("Input line is too long.\n");
            str[0] = '\0';
            continue;
        }

        // Perform string manipulations to remove redundant chars.
        str = trim(str, delimeters);
        str = removeTrails(str);

        // Print input as ECHO in batch mode.
        if (fp != stdin) { printf("ECHO: %s\n", str); }

        // Split str by "|" delimeter.
        verticalBarToken = strtok_r(str, "|", &verticalBarEnd);

        while (verticalBarToken != NULL) {

            // Calculate number of threads needed for parallelism.
            numberOfSemicolon = numberOfChars(verticalBarToken, ';');
            // Resize number of threads.
            threads = resizeThreads(threads, &numberOfThreads, &numberOfSemicolon);
            if (threads == NULL) {
                verticalBarToken = strtok_r(NULL, "|", &verticalBarEnd);
                continue;
            }

            // Split verticalBarToken by ";" delimeter.
            semicolonToken = strtok_r(verticalBarToken, ";", &semicolonEnd);

            while (semicolonToken != NULL) {

                // Remove redundant chars.
                semicolonToken = removeTrails(semicolonToken);

                // Check if input is equal to "quit".
                if (strcmp(semicolonToken, "quit") == 0) {
                    quit = TRUE;
                }
                else if (strcmp(semicolonToken, "") != 0) {
                    rc = pthread_create(&threads[count++], NULL, call, (void *)semicolonToken);
                    if (rc) {
                        printf("ERROR at pthread_create(), return value: %d.\n", rc);
                        exit(-1);
                    }
                }
                semicolonToken = strtok_r(NULL, ";", &semicolonEnd);
            }

            // Wait for termination of the threads.
            for (int i = 0; i < count; i++) {
                pthread_join(threads[i], NULL);
            }

            // Reset count to 0. Used for tracking the threads count.
            count  = 0;
            verticalBarToken = strtok_r(NULL, "|", &verticalBarEnd);
        }
    }

    // Free dynamically allocated memory and return 0 from main function.
    if (threads != NULL) {
        free(threads);
    }
    free(str);
    return 0;
}

/*
 * Summary: Wrapper function.
 * Removes redundant chars (' ', ';', '|') in delimeter[] from *str.
 * Example: "ls    ;;;;; ls -l ||||| ls -la" becomes "ls ; ls -l | ls -la"
 * Param:   str is a char pointer. delimeters is a char array.
 * Return:  Input string, str is returned.
 */
char* trim(char *str, char delimeters[]) {

    if (str == NULL) {
        return NULL;
    }

    for (int i = 0; delimeters[i]; i++) {
        str = removeExtraChars(str, delimeters[i]);
    }

    return str;
}

/*
 * Summary: Removes a singular redundant char(delimeter) from char *str.
 * Example: "ls ;;;;; ls -la" becomes "ls ; ls -la"
 * Param:   str is a char pointer. delimeter is a char.
 * Return:  Input string, str is returned.
 */
char* removeExtraChars(char *str, char delimeter) {

    if (str == NULL) {
        return NULL;
    }

    int x = 0;

    for (int i = 0; str[i]; i++) {
        if ((str[i]) != delimeter || (i > 0 && str[i - 1] != delimeter)) {
            str[x++] = str[i];
        }
    }

    str[x] = '\0';
    return str;
}

/*
 * Summary: Removes redundant chars from initial and final points of str.
 * Example: ";;||ls||;;" becomes "ls".
 * Param:   str is a char pointer.
 * Return:  Input string str is returned.
 */
char* removeTrails(char * str) {

    if (str == NULL) {
        return NULL;
    }

    int start = 0, x = 0;
    int end = strlen(str) - 1;

    if (str[end] == '\n') {
        str[end] = '\0';
    }

    for (; str[start]; start++) {
        if (str[start] != ' ' && str[start] != ';' && str[start] != '|' && str[start] != '\r') {
            break;
        }
    }

    for (; str[end]; end--) {
        if (str[end] != ' ' && str[end] != ';' && str[end] != '|' && str[end] != '\r') {
            break;
        }
    }

    for (; start <= end; start++) {
        str[x++] = str[start];
    }

    str[x] = '\0';
    return str;
}

/*
 * Summary: Performs system() call with "char *str" input.
 * Exits thread after this operation.
 * Param:   str is a char pointer.
 * Return:  None.
 */
void* call(void *str) {
    
    if (system(str) < 0) {
        printf("Error could not perform command: \"%s\".\n", (char *)str);
    }

    pthread_exit(NULL);
}

/*
 * Summary: Resolves buffer overflow issue of fgets() function.
 * Example: If input line length is 1024 and fgets() max size is 512,
 * other 512 chars are stored in system buffer. This method clears
 * system buffer (or file) to prevent unwanted operations.
 * Param:   fp is FILE pointer.
 * Return:  None.
 */
void clearInputBuffer(FILE *fp) {

    int c = fgetc(fp);

    while (c != '\n' && c != EOF) {
        c = fgetc(fp);
    }
}

/*
 * Summary: Counts number of occurrences of delimeter in str.
 * Param:   str is a char array. delimeter is a char.
 * Return:  Total number of occurrences of delimeter in str.
 */
int numberOfChars(char *str, char delimeter) {
    int count = 0;

    for (int i = 0; str[i]; i++) {
        if (str[i] == delimeter) { count++; }
    }

    return count;
}

/*
 * Summary: Performs dynamic memory allocation and resizing for *threads.
 * Param:   threads is a phtread_t pointer. size is int (number of threads needed).
 * Return:  Return threads.
 */
pthread_t* resize(pthread_t *threads, int size) {
    if (threads != NULL) {
        free(threads);
    }
    threads = (pthread_t *)malloc(size *  sizeof(pthread_t));
    if (threads ==  NULL) {
        printf("Cannot allocate memory for %d threads.\n", size);
        printf("Please shorten your input.\n");
    }
    return threads;
}

/*
 * Summary: Wrapper function for resizing *threads.
 * Param:   threads is a phtread_t pointer. numberOfThreads is the current number of threads.
 * numberOfSemicolon is the number of needed threads.
 * Return:  Return threads.
 */
pthread_t* resizeThreads(pthread_t * threads, int *numberOfThreads, int *numberOfSemicolon) {

    if (*numberOfThreads < *numberOfSemicolon + 1) {
        threads = resize(threads, *numberOfSemicolon + 1);
        if (threads == NULL) { 
            *numberOfThreads = 0;
            return NULL;
        }
        *numberOfThreads = *numberOfSemicolon + 1;
    }
    else if (*numberOfThreads / 4 >= *numberOfSemicolon + 1) {
        threads = resize(threads, (*numberOfSemicolon / 2) + 1);
        if (threads == NULL) { 
            *numberOfThreads = 0;
            return NULL; 
        }
        *numberOfThreads = (*numberOfSemicolon / 2) + 1;
    }

    return threads;
} 
