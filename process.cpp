#include <iostream>
#include <unistd.h>

using namespace std;
typedef enum State_t{
        New=1,
        Ready=2,
        Running=4,
        Waiting=8,
        Terminated=16
    }State_t;
struct PCB{
    State_t State;
    pid_t parent;
    pid_t children[10];//likely wont need more than 10 children
    int burst_time;
    int remaining_quantum;
    
};
typedef struct PCB PCB;

int main(int argc, char *argv[]){
//arguments need to be an input file
string filename=argv[1];

//parent
//read input

//child

//State=Ready

//state= waiting

//state = running

}