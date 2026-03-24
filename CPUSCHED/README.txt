CS471 Project – CPU Scheduling (CPUSCHED)

Authors: Joseph Jutton and Antonio Mendoza
Course: CS471
Project Part: CPU Scheduling (FIFO and Non-Preemptive SJF)

---

DESCRIPTION

This program simulates CPU scheduling using two algorithms:

1. First-In First-Out (FIFO)
2. Shortest Job First (SJF – non-preemptive)

The program reads a list of processes from an input file. Each process includes:

* Arrival Time
* CPU Burst Length

The simulation processes up to 500 jobs and calculates the following performance metrics:

* Total elapsed time
* Throughput
* CPU utilization
* Average waiting time
* Average turnaround time
* Average response time

The input file used for this project is:
Datafile1-txt.txt

---

DIRECTORY STRUCTURE

CPUSCHED/
│
├── source/        → Contains all C++ source files (.cpp)
├── executable/    → Contains compiled program (cpusched)
├── input/         → Contains input file (Datafile1-txt.txt)
├── output/        → Contains sample output files
└── README.txt     → This file

---

COMPILATION INSTRUCTIONS (WSL / Ubuntu)

Open a terminal in the CPUSCHED directory and run:

g++ source/*.cpp -o executable/cpusched

---

RUN INSTRUCTIONS

Run FIFO scheduling:

./executable/cpusched FIFO input/Datafile1-txt.txt

Run SJF scheduling:

./executable/cpusched SJF input/Datafile1-txt.txt

---

GENERATING SAMPLE OUTPUT FILES

The program prints results to the terminal. Sample output files can be generated using output redirection:

./executable/cpusched FIFO input/Datafile1-txt.txt > output/fifo_output.txt
./executable/cpusched SJF input/Datafile1-txt.txt > output/sjf_output.txt

Sample output files included:

* output/fifo_output.txt
* output/sjf_output.txt

---

INPUT FILE FORMAT

The input file contains two columns:

ArrivalTime    CPUBurstlength

Example:
10   22
68   12
98   34

The first line of the file contains column headers and is ignored by the program.

---

OUTPUT

The program prints scheduling results to the terminal.

The output includes:

* Scheduling algorithm used
* Number of processes
* Total elapsed time
* Throughput (calculated as total burst time divided by number of processes, per project specification)
* CPU utilization
* Average waiting time
* Average turnaround time
* Average response time

---

NOTES

* This program assumes non-preemptive scheduling.
* All processes are executed until completion.
* Time advances based on arrival times and CPU burst durations.
* The simulation handles idle CPU time if no process is available.
* The program validates command-line arguments and ensures a valid scheduling algorithm is provided.

---
