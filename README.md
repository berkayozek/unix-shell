# Authors
- Berke ÖCAL
- Berkay ÖZEK

# Design Overview
In this assignment, we have implemented a basic UNIX shell by using multiple threads. Shell prioritizes two characters "|" and ";". By decision, our implementation prioritizes "|" character over ";" character. As an example; ls | ls –l command will prioritize ls over ls –l. ls –l command will wait until previous commands termination. Then ls –l will be performed. In summary "|" character will prioritize input commands from left to right and will wait until previous commands termination. ";" character notates simultaneous execution. As an example; "ls ; ls –l". Both commands will run simultaneously without waiting for each other’s termination. Besides these design decisions, our shell uses threads and system() calls for executing a command. Additionally, shell uses necessary string manipulations to remove redundant chars and to avoid unnecessary operations. Shell has two modes. Batch and interactive. Interactive mode uses stdin as an input stream. Batch mode uses a file as an input stream. Batch mode will switch to interactive mode if there is no "quit" string in the file.

# Complete Specification
- Main loop checks exit condition("quit"). Determines input stream and gets input command with fgets() function. After these, input string is trimmed to remove redundant characters.

- First inner loop splits input string by "|" with strtok_r() function. Number of threads is resized if they are too low or too high for the task at hand.

- Second inner loop splits input string by "|" and creates a new thread for each token.

- We trimmed the input string which has repetitive symbols such as       "ls -la ||| ls". After the trim operation input string will be "ls -la | ls". Similarly, we removed redundant chars from left and right sides of the 
input string. An example; ";;;ls;;; " will be trimmed to ls. We aimed to increase performance and reduce the amount of unnecessary threads and system() calls. 

- In the batch mode, if the input file cannot be opened by any means. Shell prints "Cannot open input file." message and switches to interactive mode.

- If there is space between the two semicolons, a new thread will not be created.

- Shell uses several dynamic memory allocations for input string and threads. If a malloc fails, error message will be printed. 

- Thread size is determined by the number of semicolons after splitting input string by "|". If shell has 10 threads available and input only requires 1. Thread size will be reduced to 2. Similarly, if shell has 5 threads and requires 10. Thread size will be increased to 10.

- Shell uses a simple function to clear input buffer in the event of buffer overflow. As an example, if input string is 1024 characters and allocated memory for the string is 512. Fgets() function will run twice. This may cause unwanted separations in the input string. In order to prevent this, shell clears input buffer by reading char by char until "\n" or EOF.  

# Known Bugs and Problems
- Shell cannot run 2 nano commands simultaneously.
- Shell may not work with very large input string length. (We could not break the shell by this functionality but is a possibility.)
- Shell is only and only made for Linux and is not compatible with another OS. 
- Shell cannot run cd commands for changing current directory.
- Shell cannot be run by such input: ./shell < inp.txt. In order to run shell with a file use batch mode and execute as follows: ./shell inp.txt.


# How to Compile
`$ ./MakeFile`

# How to Run
Program is runnable in the interactive mode and batch mode:

#### Interactive Mode
  `$ ./shell`

### Shell Mode
  `$ ./shell FileName`
