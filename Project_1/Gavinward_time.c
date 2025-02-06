#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


/*
Name: Gavin Ward
CWID: 12368553
*/


int ftruncate(int fd, off_t length);

int main(int argc, char *argv[]){
    struct timeval start, end;

    //opens a spot of shared memory that can be read/write from
    int sm = shm_open("/shm_memory", O_CREAT | O_RDWR, 0666);
    if(sm == -1){
        printf("Failed to open shm");
        return 1;
    }
    
    //determines the necessary memory size
    int ftrun = ftruncate(sm, sizeof(struct timeval));
    if (ftrun == -1){
        printf("Failed to determine size of memory");
        return 2;
    }
    
    struct timeval *shared_mem_loc = mmap(NULL, sizeof(struct timeval), PROT_READ | PROT_WRITE, MAP_SHARED, sm, 0);
    pid_t pid;
    pid = fork();

    //parent process
    if (pid < 0){
        fprintf(stderr, "Fork Failed");
        exit(-1);
    //child process
    } else if (pid == 0){
        printf("Child PID: %d\n", getpid());
        //start timer for the command execution
        gettimeofday(&start, NULL);
        memcpy(shared_mem_loc, &start, sizeof(struct timeval));
        //runs the command
        int exec_check = execvp(argv[1], &argv[1]);
        if (exec_check == -1){
            perror("call to exec failed");
            fprintf(stderr, "Failed to execute: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        //get the end of command execution
        wait(NULL);
        printf("Parent PID: %d\n", getpid());
        gettimeofday(&end, NULL);
        //calculate time
        long double final_second = end.tv_sec - shared_mem_loc->tv_sec;
        final_second += (end.tv_usec - shared_mem_loc->tv_usec)/1000000.0;
        printf("Elapsed time: %Lf\n", final_second); 
    };

    //clear the memory 
    shm_unlink("/shm_memory");

    return 0;
}