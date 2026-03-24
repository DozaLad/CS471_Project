#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

struct Process {
    int arrival;
    int burst;
};

int main(int argc, char* argv[]) {
    // check for correct command-line arguments
    if (argc < 3) {
        cout << "Usage: ./cpusched <FIFO|SJF> <inputfile>" << endl;
        return 1;
    }

    string algorithm = argv[1];
    string filename = argv[2];

    // make sure algorithm is valid
    if (algorithm != "FIFO" && algorithm != "SJF") {
        cout << "Error: algorithm must be FIFO or SJF." << endl;
        return 1;
    }

    // open input file
    ifstream infile(filename);

    if (!infile) {
        cout << "Error opening file." << endl;
        return 1;
    }

    vector<Process> processes;

    int arrival, burst;
    string headerLine;

    getline(infile, headerLine);   // skip first line

    // read arrival and burst time pairs
    // only read the first 500 processes
    while (infile >> arrival >> burst && processes.size() < 500) {
        processes.push_back({arrival, burst});
    }

    // make sure at least one process was loaded
    if (processes.empty()) {
        cout << "Error: no processes were loaded from the input file." << endl;
        return 1;
    }

    // print number of processes loaded
    cout << "Loaded " << processes.size() << " processes." << endl;

    /*
    // temporary check: print first 5 processes
    for (int i = 0; i < processes.size() && i < 5; i++) {
        cout << "Process " << i + 1
             << ": Arrival = " << processes[i].arrival
             << ", Burst = " << processes[i].burst << endl;
    }
    */

    cout << fixed << setprecision(3);

    if (algorithm == "FIFO") {
        // FIFO scheduling simulation

        int current_time = 0;

        double total_waiting = 0;
        double total_turnaround = 0;
        double total_response = 0;
        double total_burst = 0;

        for (int i = 0; i < processes.size(); i++) {
            int arrival = processes[i].arrival;
            int burst = processes[i].burst;

            // if CPU is idle, jump to arrival time
            if (current_time < arrival) {
                current_time = arrival;
            }

            int waiting = current_time - arrival;
            int response = waiting; // same for non-preemptive
            int turnaround = waiting + burst;

            total_waiting += waiting;
            total_response += response;
            total_turnaround += turnaround;
            total_burst += burst;

            current_time += burst;
        }

        // calculate statistics

        double avg_waiting = total_waiting / processes.size();
        double avg_turnaround = total_turnaround / processes.size();
        double avg_response = total_response / processes.size();
        double throughput = total_burst / processes.size();
        double cpu_util = total_burst / current_time;

        // print results

        cout << "\n=== FIFO Results ===" << endl;
        cout << "Number of processes: " << processes.size() << endl;
        cout << "Total elapsed time: " << current_time << endl;
        cout << "Throughput: " << throughput << endl;
        cout << "CPU Utilization: " << cpu_util << endl;
        cout << "Average waiting time: " << avg_waiting << endl;
        cout << "Average turnaround time: " << avg_turnaround << endl;
        cout << "Average response time: " << avg_response << endl;
    }
    else if (algorithm == "SJF") {
        // SJF scheduling simulation (non-preemptive)

        int current_time = 0;
        int completed = 0;

        double total_waiting = 0;
        double total_turnaround = 0;
        double total_response = 0;
        double total_burst = 0;

        vector<bool> finished(processes.size(), false);

        while (completed < processes.size()) {
            int shortest_index = -1;

            // find the shortest job among processes that have arrived
            for (int i = 0; i < processes.size(); i++) {
                if (!finished[i] && processes[i].arrival <= current_time) {
                    if (shortest_index == -1 || processes[i].burst < processes[shortest_index].burst) {
                        shortest_index = i;
                    }
                }
            }

            // if no process is ready, jump to the next arriving process
            if (shortest_index == -1) {
                int next_arrival = -1;

                for (int i = 0; i < processes.size(); i++) {
                    if (!finished[i]) {
                        if (next_arrival == -1 || processes[i].arrival < next_arrival) {
                            next_arrival = processes[i].arrival;
                        }
                    }
                }

                current_time = next_arrival;
                continue;
            }

            int arrival = processes[shortest_index].arrival;
            int burst = processes[shortest_index].burst;

            int waiting = current_time - arrival;
            int response = waiting; // same for non-preemptive
            int turnaround = waiting + burst;

            total_waiting += waiting;
            total_response += response;
            total_turnaround += turnaround;
            total_burst += burst;

            current_time += burst;
            finished[shortest_index] = true;
            completed++;
        }

        // calculate statistics

        double avg_waiting = total_waiting / processes.size();
        double avg_turnaround = total_turnaround / processes.size();
        double avg_response = total_response / processes.size();
        double throughput = total_burst / processes.size();
        double cpu_util = total_burst / current_time;

        // print results

        cout << "\n=== SJF Results ===" << endl;
        cout << "Number of processes: " << processes.size() << endl;
        cout << "Total elapsed time: " << current_time << endl;
        cout << "Throughput: " << throughput << endl;
        cout << "CPU Utilization: " << cpu_util << endl;
        cout << "Average waiting time: " << avg_waiting << endl;
        cout << "Average turnaround time: " << avg_turnaround << endl;
        cout << "Average response time: " << avg_response << endl;
    }

    return 0;
}