#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>
#include <string.h>
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
void recursive_delete(PCB process, std::ofstream &output);
PCB find_process(int PID);
void timer_tick(std::ofstream &output);
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
    //arguments need to be an input file, quantumsize, output file
    quantum = atoi(argv[2]);
    std::string filename = argv[1];
    std::ifstream input;
    input.open(filename);               //I am reading from the file given in the commandline. In this case example.txt test1.dat test2.dat and test3.dat
    std::ofstream output(argv[3]); //output is written to output.txt
    std::string line;

    //read input
    while (!input.eof())
    {
        if (Ready_Queue.size() > 0 && current_process.PID == 0)
        {
            init = current_process;
            current_process = Ready_Queue[0];
            Ready_Queue.erase(Ready_Queue.begin());
            current_process.remaining_quantum = quantum;
            current_process.State = Running;
        }
        std::string ready_queue_list = "";
        std::string wait_queue_list = "";
        for (int i = 0; i < Ready_Queue.size(); i++)
        {
            ready_queue_list = ready_queue_list + ", " + std::to_string(Ready_Queue[i].PID) + " " + std::to_string(Ready_Queue[i].burst_time);
        }
        for (int i = 0; i < Wait_Queue.size(); i++)
        {
            wait_queue_list = wait_queue_list + ", " + std::to_string(Wait_Queue[i].PID) + " " + std::to_string(Wait_Queue[i].burst_time) + " " + std::to_string(Wait_Queue[i].event_id);
        }
        output << "PID " << current_process.PID << " " << current_process.burst_time << " running with " << current_process.remaining_quantum << " left" << std::endl;
        output << "Ready Queue:"
               << "PID" << ready_queue_list << std::endl;
        output << "Wait Queue:"
               << "PID" << wait_queue_list << std::endl;
        getline(input, line);
        output << line << std::endl;

        //parse input
        char *parse;
        char *line_c_str = const_cast<char *>(line.c_str());
        int i = 0;
        char command;
        int firstNumber;
        int secondNumber;

        parse = strtok(line_c_str, " ");
        while (parse != NULL)
        {
            if (i == 0)
            {
                command = parse[0];
            }
            else if (i == 1)
            {
                firstNumber = atoi(parse);
            }
            else if (i == 2)
            {
                secondNumber = atoi(parse);
            }
            i++;
            parse = strtok(NULL, " ");
        }

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
        timer_tick(output);
        PCB temp;
        temp.burst_time = secondNumber;
        temp.PID = firstNumber;
        temp.State = Ready;
        current_process.children.push_back(temp);
        Ready_Queue.push_back(temp);
        output << "PID " << temp.PID << " " << temp.burst_time << " placed on the READY QUEUE" << std::endl;
    }
    else if (command == 'D')
    {
        if (current_process.PID != init.PID)
        {
            timer_tick(output);
            PCB temp = find_process(firstNumber);
            recursive_delete(temp, output); //FIX!!!!!!!
            if (current_process.PID == init.PID && Ready_Queue.size() > 0)
            {
                current_process = Ready_Queue[0];
                Ready_Queue.erase(Ready_Queue.begin());
                current_process.remaining_quantum = quantum;
            }
            else
            {
                current_process = init;
            }
        } //Destroy the process n and any children it has
    }
    else if (command == 'I')
    { //Timer Interupt
        timer_tick(output);
    }
    else if (command == 'W')
    { //Current running process is moved to waiting queue with event id (EID) n
        timer_tick(output);
        if (current_process.PID != 0)
        {
            current_process.event_id = firstNumber;
            current_process.State = Waiting;
            output<<"PID "<< current_process.PID<<" "<<current_process.burst_time<< " "<<current_process.event_id<< " placed on the WAIT_QUEUE"<<std::endl;
            Wait_Queue.push_back(current_process);
        }

        if (Ready_Queue.size() != 0)
        {
            current_process = Ready_Queue[0];
            Ready_Queue.erase(Ready_Queue.begin());
            current_process.remaining_quantum = quantum;
            current_process.State = Running;
        }
        else
        {
            current_process = init;
        }
    }
    else if (command == 'E')
    { //(EID) n happens process moves to Ready Queue
        //run a loop through the wait queue
        timer_tick(output);
        PCB temp;
        for (int i = 0; i < Wait_Queue.size(); i++)
        {
            temp = Wait_Queue[i];
            if (temp.event_id == firstNumber)
            {
                temp.State = Ready;
                Ready_Queue.push_back(temp);
                Wait_Queue.erase(Wait_Queue.begin() + i);
            }
        }
    }
}
void recursive_delete(PCB process, std::ofstream &output)
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
                    output << "PCB " << process.PID << " " << process.burst_time << " Terminated" << std::endl;
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
                    output << "PCB " << process.PID << " " << process.burst_time << " Terminated" << std::endl;
                }
            }
        }
        else
        {
            current_process = init;
            output << "PCB " << process.PID << " " << process.burst_time << " Terminated" << std::endl;
        }
    }
    //recursive case is the PCB has children
    else
    {
        //call the function on all of the children then remove the children
        for (int i = 0; i < process.children.size(); i++)
        {
            recursive_delete(process.children[i], output);
        }
        process.children.clear();
        recursive_delete(process, output);
    }
}
PCB find_process(int PID)
{
    if (current_process.PID == PID)
    {
        return current_process;
    }
    for (int i = 0; i < Ready_Queue.size(); i++)
    {
        if (Ready_Queue[i].PID == PID)
        {
            return Ready_Queue[i];
        }
    }
    for (int i = 0; i < Wait_Queue.size(); i++)
    {
        if (Wait_Queue[i].PID == PID)
        {
            return Wait_Queue[i];
        }
    }
    return init;
}
void timer_tick(std::ofstream &output)
{
    if (current_process.PID != 0)
    {
        current_process.remaining_quantum--;
        current_process.burst_time--;

        if (current_process.burst_time == 0)
        {
            recursive_delete(current_process, output);
            if (Ready_Queue.size() > 0)
            {
                current_process = Ready_Queue[0];
                Ready_Queue.erase(Ready_Queue.begin());
                current_process.remaining_quantum = quantum;
                current_process.State = Running;
            }
            else
            {
                current_process = init;
            }
        }
    }

    if (current_process.remaining_quantum <= 0) //switch out the current process
    {
        current_process.State = Ready;
        Ready_Queue.push_back(current_process);
        current_process = Ready_Queue[0];
        Ready_Queue.erase(Ready_Queue.begin());
        current_process.remaining_quantum = quantum;
        current_process.State = Running;
    }
}