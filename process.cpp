#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <vector>

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
    std::vector<PCB> children;
    int burst_time;
    int remaining_quantum;
    int event_id;
};
typedef struct PCB PCB;
void execute_command(char command, int firstNumber, int secondNumber);
void recursive_delete(PCB process);
std::vector<PCB> Ready_Queue; //the 1st item on the ready queue is next to be added to Running
std::vector<PCB> Wait_Queue;
PCB current_process;

int main(int argc, char *argv[])
{
    int quantum;
    //arguments need to be an input file and a quantum size
    quantum = atoi(argv[2]);
    std::string filename = argv[1];
    std::ifstream input;
    input.open(filename);               //I am reading from the file given in the commandline. In this case example.txt test1.dat test2.dat and test3.dat
    std::ofstream output("output.txt"); //output is written to output.txt
    std::string line;

    //read input
    getline(input, line);
    //parse input
    char parse[5];
    strcpy(parse, line.c_str());
    char command = parse[0];
    int firstNumber = parse[2];
    int secondNumber = parse[4];
    if (command == 'X')
    {
        input.close();
        output.close();
        return 0;
    }
    execute_command(command, firstNumber, secondNumber);
}

void execute_command(char command, int firstNumber, int secondNumber)
{

    if (command == 'C')
    { //create a PCB and add it to the Ready_Queue
        PCB temp;
        temp.burst_time = secondNumber;
        temp.PID = firstNumber;
        current_process.children.push_back(temp);
        Ready_Queue.push_back(temp);
    }
    else if (command == 'D')
    { //Destroy the process n and any children it has
    recursive_delete(current_process);
    current_process=Ready_Queue[0];
    Ready_Queue.erase(Ready_Queue.begin());
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
}
void recursive_delete(PCB process){
    //base case is the PCB has no children
    if(process.children.size()==0){
        //read what queue the process is on, then remove it from that vector
        if(process.State==Ready){
            for(int i=0;i<Ready_Queue.size();i++){
                if(Ready_Queue[i].PID==process.PID){
                    Ready_Queue.erase(Ready_Queue.begin()+i);
                }
            }
        }else if(process.State==Waiting){
            for(int i=0;i<Wait_Queue.size();i++){
                if(Wait_Queue[i].PID==process.PID){
                    Wait_Queue.erase(Wait_Queue.begin()+i);
                }
            }
        }
    }
    //recursive case is the PCB has children
    else{
        //call the function on all of the children then remove the children
        for(int i=0;i<process.children.size();i++){
            recursive_delete(process.children[i]);
        }
        process.children.clear();

    }
}