#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void sigchldHandler(){
    FILE* fptr = fopen("logFile.txt", "a");
    fprintf(fptr, "child process was terminated\n");
    fclose(fptr);
} //after closing child write in file


void forkChild(char** array, int background)
{
    pid_t childid;
    int status;
    
    signal(SIGCHLD, sigchldHandler); //handel signal not ignore
    
    childid = fork();
    
    if (childid >= 0)
    {
        if(childid == 0) //child
        {
            if(execvp(array[0], array) < 0) printf("error!\n"); // cutted words , if error then negative
            exit(0); // exit child
        }
        else
        {
            if(background == 0) //parent ,firefox only waiting close child
            {
                wait(&status); // if in background
                sleep(10); // not working without
            }
            return;
        }
    }
    else
    {
        printf("fork failed\n");
        return;
    }
    
}

int main()
{
    FILE* fptr = fopen("logFile.txt", "w");
    fclose(fptr);
    
    int max = 100; //create new file if one excist delete text in it
    
    while (1)  // loop to take commands
    {
        char* str = malloc(sizeof(char) * max);
        
        printf(">> ");
        fgets(str, max, stdin);
        
        if(str[strlen(str) - 1] == '\n'){
            str[strlen(str) - 1] = '\0'; //to remove enter to not consider it as argument
        }
        
        if(strlen(str) <= 0)
            continue; // out of loop ,start again
        
        if(strcmp(str,"exit") == 0) // break if exit
            break;
        
        int background = 0; //flag
        
        if(str[strlen(str) - 1] == '&') { // last character & background
            str = strsep(&str, "&"); // cut &
            background = 1;
        }
        
        char* array[max];
        
        int nwords = 0;
        
        while(nwords < max)
        {
            array[nwords] = strsep(&str, " "); // cut command at spaces
            
            if(array[nwords] != NULL) { // there is words
                if(strlen(array[nwords]) == 0) nwords--; // two spaces after eachother
            }
            else
                break; // if null
            
            nwords++;
        }
        
        if(strcmp(array[0], "cd")==0 && nwords==2){ // compare first word in array is zero and 2 words
            if(chdir(array[1]) < 0) printf("error!\n"); // then error is negative if directry not found
        }
        else
            forkChild(array, background); // to check in background or not
        
        free(str);
    }
    
    return 0;
}
