#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
using namespace std;
typedef enum State_t
{
    New = 1,
    Ready = 2,
    Running = 4,
    Waiting = 8,
    Terminated = 16
} State_t;
struct PCB
{
    State_t State;
    int PID;
    pid_t children[10]; //likely wont need more than 10 children
    int burst_time;
    int remaining_quantum;
};
PCB current_process;
typedef struct PCB PCB;
int quantum;
int main(int argc, char *argv[])
{
    //arguments need to be an input file and a quantum size
    quantum=atoi(argv[2]);
    string filename = argv[1];
    ifstream file_read;
    file_read.open(filename);      //I am reading from the file given in the commandline. In this case example.txt test1.dat test2.dat and test3.dat
    ofstream output("output.txt"); //output is written to output.txt

    //read input
    string line;
    getline(file_read, line);
    //parse input
    char parse[5];
    strcpy(parse, line.c_str());

    char command = parse[0];
    int firstNumber = parse[2];
    int secondNumber = parse[4];
    if (command == 'C')
    { //fork the process and assign values to pcb

        current_process.burst_time = secondNumber;
        current_process.PID = firstNumber;
    }
    else if (command == 'D')
    { //Destroy the process n and any children it has
    }
    else if (command == 'I')
    { //Timer Interupt
    }
    else if (command == 'W')
    { //Current running process is moved to waiting queue with event id (EID) n
    }
    else if (command == 'E')
    { //(EID) n happens process moves to Ready Queue
    }
    else if (command == 'X')
    { //Exit the program, this should show the current state of the simulation
    }

}