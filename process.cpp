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
void execute_command(char command, int firstNumber, int secondNumber, std::ofstream &output);
void recursive_delete(PCB process);
std::vector<PCB> Ready_Queue; //the 1st item on the ready queue is next to be added to Running
std::vector<PCB> Wait_Queue;
PCB current_process;
PCB init;
int quantum;

int main(int argc, char *argv[])
{
    //Create PID 0
    init.PID = 0;
    init.burst_time = 0;
    init.remaining_quantum = 0;
    current_process = init;
    //arguments need to be an input file and a quantum size
    quantum = atoi(argv[2]);
    std::string filename = argv[1];
    std::ifstream input;
    input.open(filename);               //I am reading from the file given in the commandline. In this case example.txt test1.dat test2.dat and test3.dat
    std::ofstream output("output.txt"); //output is written to output.txt
    std::string line;

    //read input
    while (!input.eof())
    {
        std::string ready_queue_list = "";
        std::string wait_queue_list = "";
        for (int i = 0; i < Ready_Queue.size(); i++)
        {
            ready_queue_list = ready_queue_list + ", " + std::to_string(Ready_Queue[i].PID);
        }
        for (int i = 0; i < Wait_Queue.size(); i++)
        {
            wait_queue_list = wait_queue_list + ", " + std::to_string(Wait_Queue[i].PID);
        }
        output << "PID" << current_process.PID << "running with " << current_process.remaining_quantum << " left" << std::endl;
        output << "Ready Queue:"
               << "PID" << ready_queue_list << std::endl;
        output << "Wait Queue:"
               << "PID" << wait_queue_list << std::endl;
        getline(input, line);
        output << line << std::endl;
        //parse input
        char parse[5];
        strcpy(parse, line.c_str());
        char command = parse[0];
        int firstNumber = parse[2]-48; //The -48 is to convert to an int
        int secondNumber = parse[4]-48;
        if (command == 'X')
        {
            input.close();
            output.close();
            return 0;
        }
        execute_command(command, firstNumber, secondNumber, output);
    }
    input.close();
    output.close();
    return 0;
}

void execute_command(char command, int firstNumber, int secondNumber, std::ofstream &output)
{

    if (command == 'C')
    { //create a PCB and add it to the Ready_Queue
        PCB temp;
        temp.burst_time = secondNumber;
        temp.PID = firstNumber;
        current_process.children.push_back(temp);
        Ready_Queue.push_back(temp);
        output << "PID" << temp.PID << " " << temp.burst_time << " placed on the READY QUEUE" << std::endl;
    }
    else if (command == 'D')
    {                                      //Destroy the process n and any children it has
        recursive_delete(current_process); //FIX!!!!!!!
        if (Ready_Queue.size() != 0)
        {
            current_process = Ready_Queue[0];
            Ready_Queue.erase(Ready_Queue.begin());
            current_process.remaining_quantum = quantum;
        }
        else
        {
            current_process = init;
        }
    }
    else if (command == 'I')
    { //Timer Interupt
        if (current_process.PID != 0)
        {
            current_process.remaining_quantum--;
            current_process.burst_time--;
        }

        if (current_process.remaining_quantum <= 0)
        {
            Ready_Queue.push_back(current_process);
            current_process = Ready_Queue[0];
            Ready_Queue.erase(Ready_Queue.begin());
            current_process.remaining_quantum = quantum;
        }
    }
    else if (command == 'W')
    { //Current running process is moved to waiting queue with event id (EID) n
        if (current_process.PID != 0)
        {
            current_process.event_id = firstNumber;
            Wait_Queue.push_back(current_process);
        }

        if (Ready_Queue.size() != 0)
        {
            current_process = Ready_Queue[0];
            Ready_Queue.erase(Ready_Queue.begin());
            current_process.remaining_quantum = quantum;
        }
        else
        {
            current_process = init;
        }
    }
    else if (command == 'E')
    { //(EID) n happens process moves to Ready Queue
        //run a loop through the wait queue
        PCB temp;
        for (int i = 0; i < Wait_Queue.size(); i++)
        {
            temp = Wait_Queue[i];
            if (temp.event_id == firstNumber)
            {
                Ready_Queue.push_back(Wait_Queue[i]);
                Wait_Queue.erase(Wait_Queue.begin() + i);
            }
        }
    }
}
void recursive_delete(PCB process)
{
    //base case is the PCB has no children
    if (process.children.size() == 0)
    {
        //read what queue the process is on, then remove it from that vector
        if (process.State == Ready)
        {
            for (int i = 0; i < Ready_Queue.size(); i++)
            {
                if (Ready_Queue[i].PID == process.PID)
                {
                    Ready_Queue.erase(Ready_Queue.begin() + i);
                }
            }
        }
        else if (process.State == Waiting)
        {
            for (int i = 0; i < Wait_Queue.size(); i++)
            {
                if (Wait_Queue[i].PID == process.PID)
                {
                    Wait_Queue.erase(Wait_Queue.begin() + i);
                }
            }
        }
    }
    //recursive case is the PCB has children
    else
    {
        //call the function on all of the children then remove the children
        for (int i = 0; i < process.children.size(); i++)
        {
            recursive_delete(process.children[i]);
        }
        process.children.clear();
    }
}