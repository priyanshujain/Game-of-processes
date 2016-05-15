//author Priyanshu jain
//priyanshudeveloper@gmail.com

/*
Problem: Write a C program to implement the following game. The parent program P first creates two pipes, and then spawns
two child processes C and D. One of the two pipes is meant for communications between P and C, and the other for
communications between P and D. Now, a loop runs as follows. In each iteration (also called round), P first randomly
chooses one of the two flags: MIN and MAX (the choice randomly varies from one iteration to another). Each of the
two child processes C and D generates a random positive integer and sends that to P via its pipe. P reads the two
integers; let these be c and d. If P has chosen MIN, then the child who sent the smaller of c and d gets one point. If P
has chosen MAX, then the sender of the larger of c and d gets one point. If c = d, then this round is ignored. The child
process who first obtains ten points wins the game. When the game ends, P sends a user-defined signal to both C and D,
and the child processes exit after handling the signal .
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<signal.h>

#define BUFSIZE 10
#define MIN 0
#define MAX 1

void childCsigHandler (int sig){//signal handler for process C
  if (sig == SIGUSR1) {
    printf("C has won\n");
  }else if(sig == SIGUSR2){
    printf("C has lost\n" );
  }
  exit(0);
}
void childDsigHandler (int sig){
  if (sig == SIGUSR1) {
    printf("D has won\n" );
  }else if(sig == SIGUSR2){
    printf("D has lost\n");
  }
  exit(0);
}

int main(int argc, char const *argv[]) {

  int pipe1[2];
  int pipe2[2];

  if (pipe(pipe1)==-1) {//create a pipe using pipe()
    printf("!!!!error : pipe not created\n");
    exit(0);
  }
  if (pipe(pipe2)==-1) {
    printf("!!!!error : pipe not created\n");
    exit(0);
  }

  int val1,val2,score1=0,score2=0;
  int flag,pid1,pid2;
  srand((unsigned int)time(NULL));

char line1[BUFSIZE];
char line2[BUFSIZE];

if((pid1=fork())==0){
  close(pipe1[0]);
  close(pipe2[0]);
  close(pipe2[1]);

  signal(SIGUSR1,childCsigHandler);//register signals
  signal(SIGUSR2,childCsigHandler);

  while(1){
    val1= rand();
    sprintf(line1,"%d",val1);
    write(pipe1[1],line1,BUFSIZE);
    printf("child C writes value %d\n", val1);
    sleep(1);
  }
}else if((pid2=fork())==0){
  close(pipe1[0]);
  close(pipe2[0]);
  close(pipe1[1]);

  signal(SIGUSR1,childDsigHandler);
  signal(SIGUSR2,childDsigHandler);

  while(1){
    val2= rand();
    sprintf(line2,"%d",val1);
    write(pipe2[1],line2,BUFSIZE);
    printf("Child D writes value %d\n",val2 );
    sleep(1);
  }
}else{
  close(pipe1[1]);
  close(pipe2[1]);
  int round=1;

  while((score1!=10) && (score2!=10)){
    flag = rand()%2;
    printf("\t\n" );
    printf("This is round number: %d\n",round );

    read(pipe1[0],line1,BUFSIZE);
    sscanf(line1,"%d",&val1);

    read(pipe2[0],line2,BUFSIZE);
    sscanf(line2,"%d",&val2);
    if (flag == MIN) {
      if (val1 > val2) {
        score2++;
        round++;
      }else if(score1==score2){
        round++;
      }
    }
    if (flag == MAX) {
      if (val1 > val2) {
        score1++;
        round++;
      }else if(score1==score2){
        round++;
      }
    }

    printf("scores for round %d are:\n",round-1 );
    printf("score of C: %d\n", score1);
    printf("score of D: %d\n", score2);
  }
  if (score1 ==10) {
    printf("C has won the game\n");
    kill(pid1,SIGUSR1);//sending signal to a process having pid and signal
    kill(pid2,SIGUSR2);
  }else if (score2 == 10) {
    printf("D has won the game\n");
    kill(pid1,SIGUSR2);
    kill(pid2,SIGUSR1);
  }
exit(0);
}
}
