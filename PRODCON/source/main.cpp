#include<iostream>
#include<fstream>
#include<thread>
#include<mutex>
#include<pthread.h>
#include<semaphore.h>
#include<string>
#include<chrono>
#include<cstdlib>
#include<sys/stat.h>
using namespace std;

int p;
int c;
int b;
int minSleep, maxSleep;
string currentOutputFile;

struct SalesRecord {
    string salesDate;
    int storeID;
    int registerNum;
    float saleAmount;
};

// shared buffer
SalesRecord buffer[100];
int head = 0, tail = 0;

// shared counter + flags
int totalProduced = 0;
bool finished = false;

// global stats
float storeSales[11] = {0.0};
float monthlySales[12] = {0.0};
float aggregateSales = 0.0;

// semaphores and mutexes
sem_t emptySem;
sem_t fullSem;
mutex bufferMutex;
mutex statsMutex;

// monitor thread, checks to make sure 1000 records are produced and that consumers can exit safely
void* monitor(void* arg) {
    while (totalProduced < 1000) {
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    finished = true;

    for (int i = 0; i < c; i++) {
        sem_post(&fullSem);
    }
    return nullptr;
}

void* producer(void* arg) {
    int storeID = *(int*)arg;

    while (true) {
        bufferMutex.lock();
        if (totalProduced >= 1000) {
            bufferMutex.unlock();
            break;
        }
        bufferMutex.unlock();

        string day = to_string(1 + rand() % 30);        
        string month = to_string(1 + rand() % 12);      
        string salesDate = day + "/" + month + "/16";    
        int registerNum = rand() % 6 + 1;               
        float saleAmount = 0.50f + static_cast<float>(rand() % 99950) / 100.0f;

        SalesRecord record = {salesDate, storeID, registerNum, saleAmount};

        sem_wait(&emptySem);
        bufferMutex.lock();

        if (totalProduced >= 1000) {
            bufferMutex.unlock();
            sem_post(&emptySem);
            break;
        }

        buffer[tail] = record;
        tail = (tail + 1) % b;
        totalProduced++;

        int sleepDuration = minSleep + rand() % (maxSleep - minSleep + 1);

        ofstream outFile(currentOutputFile, ios::app);
        if (outFile.is_open()) {
            outFile << "StoreID: " << storeID << " | Date: " << salesDate << " | Register: " << registerNum << " | Sale: $" << saleAmount << " | Total Produced: " << totalProduced << " | Sleep: " << sleepDuration << "ms\n";
            outFile.close();
        }

        bufferMutex.unlock();
        sem_post(&fullSem);

        this_thread::sleep_for(chrono::milliseconds(sleepDuration));
    }

    return nullptr;
}

void* consumer(void* arg) {
    int consumerID = *(int*)arg;

    float localAggregate = 0.0;
    float localMonthlySales[12] = {0.0};
    float localStoreSales[11] = {0.0};
    int localCount = 0;

    while (true) {
        if (finished) {
            if (sem_trywait(&fullSem) != 0) {
                break;
            }
        } else {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_nsec += 50000000; 
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }
            sem_timedwait(&fullSem, &ts);
        }

        bufferMutex.lock();

        if (head == tail) {
            bufferMutex.unlock();
            continue;
        }

        SalesRecord record = buffer[head];
        head = (head + 1) % b;

        bufferMutex.unlock();
        sem_post(&emptySem);

        localAggregate += record.saleAmount;
        localStoreSales[record.storeID] += record.saleAmount;
        int monthIndex = stoi(record.salesDate.substr(record.salesDate.find('/') + 1, 2)) - 1;
        localMonthlySales[monthIndex] += record.saleAmount;
        localCount++;
    }

    // print local stats
    cout << "\nConsumer " << consumerID << " Stats:\n";
    cout << "Records consumed: " << localCount << "\n";
    cout << "Local aggregate sales: $" << localAggregate << "\n";
    for (int i = 0; i < 12; i++)
        if (localMonthlySales[i] > 0)
            cout << "  Month " << i+1 << ": $" << localMonthlySales[i] << "\n";

    statsMutex.lock();
    aggregateSales += localAggregate;
    for (int i = 0; i < 12; i++)
        monthlySales[i] += localMonthlySales[i];
    for (int i = 1; i <= p; i++)
        storeSales[i] += localStoreSales[i];
    statsMutex.unlock();

    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: ./main <num_producers> <num_consumers>\n";
        return 1;
    }
    p = atoi(argv[1]);
    c = atoi(argv[2]);
    b = 2 * p;

    mkdir("output", 0777);

    srand(time(nullptr));

    ifstream inFile("input/sleep_times.txt");
    if (!inFile.is_open()) {
        cout << "Error: could not open input/sleep_times.txt\n";
        return 1;
    }
    inFile >> minSleep >> maxSleep;
    inFile.close();

    currentOutputFile = "output/output_p" + to_string(p) + "_c" + to_string(c) + ".txt";
    ofstream clearFile(currentOutputFile, ios::trunc);
    clearFile.close();

    // init semaphores and mutexes
    sem_init(&emptySem, 0, b);
    sem_init(&fullSem, 0, 0);
    pthread_t producers[p];
    pthread_t consumers[c];
    pthread_t monitorThread;

    auto startTime = chrono::high_resolution_clock::now();

    // create producer and consumer threads
    int storeIDs[p];
    for (int i = 0; i < p; i++) {
        storeIDs[i] = i + 1;
        pthread_create(&producers[i], nullptr, producer, &storeIDs[i]);
    }
    int consumerIDs[c];
    for (int i = 0; i < c; i++) {
        consumerIDs[i] = i + 1;
        pthread_create(&consumers[i], nullptr, consumer, &consumerIDs[i]);
    }
    pthread_create(&monitorThread, nullptr, monitor, nullptr);

    // join threads
    for (int i = 0; i < p; i++) {
        pthread_join(producers[i], nullptr);
    }
    for (int i = 0; i < c; i++) {
        pthread_join(consumers[i], nullptr);
    }
    pthread_join(monitorThread, nullptr);

    // record end time
    auto endTime = chrono::high_resolution_clock::now();

    // print global stats to output file
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    cout << "\nGlobal Stats: \n";
    cout << "Total simulation time: " << duration << "ms\n";
    cout << "Aggregate sales: $" << aggregateSales << "\n";
    cout << "Monthly sales:\n";
    for (int i = 0; i < 12; i++)
        if (monthlySales[i] > 0)
            cout << "  Month " << i+1 << ": $" << monthlySales[i] << "\n";
    cout << "Store-wide sales:\n";
    for (int i = 1; i <= p; i++)
        cout << "  Store " << i << ": $" << storeSales[i] << "\n";

    // cleanup semaphores and mutexes
    sem_destroy(&emptySem);
    sem_destroy(&fullSem);
    return 0;
}
