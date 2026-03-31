CS471 Project - Producer / Consumer Sales Program

Authors: Antonio Mendoza and Joseph Jutton
Course: CS471
Project Part: Producer / Consumer with Threads and Semaphores

---

DESCRIPTION

The program aims to simulate a multi-threaded producer consumer system that models retail sails data across different store locations and registers. Producers generate randomized sales records and place them in a shared circular buffer, while consumers retrieve and process the records to compute the sales statistics.

The program uses threads (pthreads), semaphores, and mutexes to manage concurrent access to the buffer. A dedicated thread labeled "monitor" manages production and signlas consumers to exit once all records have been completed.

Each sales record includes:
* Sale date (Formatted as DD/MM/16), with the year specified to be 16.
* Store ID
* Register number
* Sale amount

The simulation produces 1000 sales records and computes the following statistics:
* Aggregate total sales
* Store specific sales totals
* Month specific sales totals
* Consumer specific record counts and local sales breakdown
* Total simulation runtime

The sleep interval range for producer threads is read from the input:
    input/sleep_times.txt

---

DIRECTORY STRUCTURE

PRODCON/
│
|-- source/             > Contains all C++ source files (.cpp)
|-- executable/         > Contains compiled program (main)
|-- input/              > Contains input file (sleep_times.txt)
|-- output/             > Contains generated output files (auto-created at runtime)
|-- README.txt          > This file

---

COMPILATION AND RUN INSTRUCTIONS (WSL / Ubuntu / Linux)

Open a terminal in the project directory (PRODCON directory) and run the following command:

    g++ source/prodcon.cpp -o executable/main -lpthread

then:

    ./executable/prodcon <num_producers> <num_consumers>

Arguments:
<num_producers> = Number of producer threads, with each representing an individual store
<num_consumers> = Number of consumer threads

Examples:
./executable/main 2 2
./executable/main 5 4

---

INPUT FILE FORMAT

The file input/sleep_times.txt contains exactly two integers on a single line:
<minSleep> <maxSleep>
Thats it. These values define the minimum and maximum sleep duration in milliseconds for the producer threads between record generations. The rubric did not specify in detail the depth of the input file, so it is simplified to this.

---

OUTPUT

At runtime, the program does the following:
1. Creates the output/ directory if not already present
2. Writes a log of all produced sales records to a file named:
    output/output_p<num_producers>_c<num_consumers>.txt
    with each line as:
    StoreID | Date | Register # | Sale amounts | Total produced | Sleep duration
3. Prints consumer statistics to the terminal, which include:
    * Number of records consumed
    * Local aggregate sales
    * Local monthly sales
4. Prints global statistics to the terminal, which includes:
    * Total simulation time
    * Aggregate sales across all the consumers
    * Monthly sales totals
    * Individual store sales totals

---

NOTES
* Shared buffer size created to be 2 * num_producers
* Sale amounts limited to a range from $0.50 to $999.99
* Sale dates are randomly generated in the range of 1-30: day, 1-12: month, and with a fixed year of 2016
* Register numbers ranged from 1 to 6
* Program results will vary between runs due to the use of rand()

Thank you for your attention.

---
