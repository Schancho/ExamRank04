#ifndef MICROSHELL_H
#define MICROSHELL_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
# include <sys/errno.h>

struct s_pipe
{
   int pipe[2];
   int pprev;
   int pnext; 
   int outpipe;
}t_p;


#endif