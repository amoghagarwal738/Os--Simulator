#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <algorithm>
#include <cmath>
using namespace std;


// ============================================================
// PROCESS STRUCTURE
// ============================================================

struct Process
{
    string name;

    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
    int startTime;
};


// ============================================================
// FCFS - FIRST COME FIRST SERVE
// ============================================================

void FCFS(vector<Process> &processes)
{
    int n = processes.size();

    // Current time of the CPU
    int currentTime = 0;


    // Process each process in the given order
    for (int i = 0; i < n; i++)
    {
        // If the process has not arrived yet,
        // CPU waits until its arrival time.
        if (currentTime < processes[i].arrivalTime)
        {
            currentTime = processes[i].arrivalTime;
        }


        // Time at which the process starts execution
        processes[i].startTime = currentTime;


        // Response Time = Start Time - Arrival Time
        processes[i].responseTime =
            processes[i].startTime -
            processes[i].arrivalTime;


        // Execute the complete process
        // FCFS is non-preemptive.
        currentTime += processes[i].burstTime;


        // Time at which the process completes
        processes[i].completionTime = currentTime;


        // Turnaround Time = Completion Time - Arrival Time
        processes[i].turnaroundTime =
            processes[i].completionTime -
            processes[i].arrivalTime;


        // Waiting Time = Turnaround Time - Burst Time
        processes[i].waitingTime =
            processes[i].turnaroundTime -
            processes[i].burstTime;
    }


    // --------------------------------------------------------
    // DISPLAY FCFS RESULTS
    // --------------------------------------------------------

    cout << "\n\n========== FCFS SCHEDULING ==========\n\n";

    cout << "Process\tAT\tBT\tST\tCT\tTAT\tWT\tRT\n";


    for (int i = 0; i < n; i++)
    {
        cout << processes[i].name << "\t"
             << processes[i].arrivalTime << "\t"
             << processes[i].burstTime << "\t"
             << processes[i].startTime << "\t"
             << processes[i].completionTime << "\t"
             << processes[i].turnaroundTime << "\t"
             << processes[i].waitingTime << "\t"
             << processes[i].responseTime << endl;
    }
}


// ============================================================
// SPN - SHORTEST PROCESS NEXT
// ============================================================
//
// SPN is also called Non-Preemptive SJF.
//
// At every point in time:
// 1. Find processes that have already arrived.
// 2. Ignore processes that are already completed.
// 3. Select the process with the smallest Burst Time.
// 4. Execute that process completely.
//
// SPN is NON-PREEMPTIVE.
// Once a process starts executing, it cannot be interrupted.
// ============================================================

void SPN(vector<Process> &processes)
{
    int n = processes.size();

    // Current time of the CPU
    int currentTime = 0;

    // Number of processes that have completed
    int completed = 0;


    // done[i] = true means process i has completed
    // Initially all processes are false.
    vector<bool> done(n, false);


    // Continue until every process has completed
    while (completed < n)
    {
        // Stores the index of the selected process
        int index = -1;


        // Start with the largest possible integer.
        // Any actual Burst Time should be smaller than this.
        int shortestBurst = INT_MAX;


        // ----------------------------------------------------
        // FIND SHORTEST AVAILABLE PROCESS
        // ----------------------------------------------------

        for (int i = 0; i < n; i++)
        {
            // A process can be selected only if:
            //
            // 1. It has not completed.
            // 2. It has already arrived.
            // 3. Its Burst Time is smaller than the
            //    currently selected shortest Burst Time.

            if (!done[i] &&
                processes[i].arrivalTime <= currentTime &&
                processes[i].burstTime < shortestBurst)
            {
                // Store the new shortest Burst Time
                shortestBurst = processes[i].burstTime;

                // Store the index of this process
                index = i;
            }
        }


        // ----------------------------------------------------
        // NO PROCESS HAS ARRIVED
        // ----------------------------------------------------

        if (index == -1)
        {
            // CPU is idle because no process is available.
            //
            // Move the current time forward by one unit
            // and check again.

            currentTime++;

            continue;
        }


        // ----------------------------------------------------
        // PROCESS STARTS EXECUTION
        // ----------------------------------------------------

        processes[index].startTime = currentTime;


        // Response Time = Start Time - Arrival Time
        processes[index].responseTime =
            processes[index].startTime -
            processes[index].arrivalTime;


        // ----------------------------------------------------
        // EXECUTE THE COMPLETE PROCESS
        // ----------------------------------------------------

        // SPN is non-preemptive.
        // Therefore the process executes for its entire
        // Burst Time without interruption.

        currentTime += processes[index].burstTime;


        // ----------------------------------------------------
        // CALCULATE PROCESS METRICS
        // ----------------------------------------------------

        // Completion Time
        processes[index].completionTime = currentTime;


        // Turnaround Time = Completion Time - Arrival Time
        processes[index].turnaroundTime =
            processes[index].completionTime -
            processes[index].arrivalTime;


        // Waiting Time = Turnaround Time - Burst Time
        processes[index].waitingTime =
            processes[index].turnaroundTime -
            processes[index].burstTime;


        // Mark process as completed
        done[index] = true;

        completed++;
    }


    // --------------------------------------------------------
    // DISPLAY SPN RESULTS
    // --------------------------------------------------------

    cout << "\n\n========== SPN SCHEDULING ==========\n\n";

    cout << "Process\tAT\tBT\tST\tCT\tTAT\tWT\tRT\n";


    for (int i = 0; i < n; i++)
    {
        cout << processes[i].name << "\t"
             << processes[i].arrivalTime << "\t"
             << processes[i].burstTime << "\t"
             << processes[i].startTime << "\t"
             << processes[i].completionTime << "\t"
             << processes[i].turnaroundTime << "\t"
             << processes[i].waitingTime << "\t"
             << processes[i].responseTime << endl;
    }
}
// ============================================================
// SRTF - SHORTEST REMAINING TIME FIRST
// ============================================================
//
// SRTF is the PREEMPTIVE version of SPN/SJF.
//
// At every unit of CPU time:
//
// 1. Look at all processes that have arrived.
// 2. Find the process with the smallest REMAINING TIME.
// 3. Execute it for 1 unit.
// 4. If another process arrives with a smaller remaining
//    time, the CPU switches to that process.
//
// This switching is called PREEMPTION.
//
// Example:
//
// P1 remaining time = 5
// P2 arrives with burst time = 2
//
// CPU stops P1 and executes P2.
//
// ============================================================

void SRTF(vector<Process> &processes)
{
    int n = processes.size();

    // Current CPU time
    int currentTime = 0;

    // Number of completed processes
    int completed = 0;


    // --------------------------------------------------------
    // INITIALIZE REMAINING TIME
    // --------------------------------------------------------
    //
    // Initially, remaining time = burst time.
    //
    // Example:
    //
    // P1 BT = 8
    // P1 RT = 8
    //
    // After executing for 3 units:
    //
    // P1 RT = 5
    //

    for (int i = 0; i < n; i++)
    {
        processes[i].remainingTime =
            processes[i].burstTime;

        // We use -1 to indicate that the process
        // has not started yet.

        processes[i].startTime = -1;
    }


    // --------------------------------------------------------
    // RUN UNTIL ALL PROCESSES COMPLETE
    // --------------------------------------------------------

    while (completed < n)
    {
        // Index of process selected to run
        int index = -1;

        // Initially assume a very large remaining time
        int shortestRemaining = INT_MAX;


        // ----------------------------------------------------
        // FIND PROCESS WITH SHORTEST REMAINING TIME
        // ----------------------------------------------------

        for (int i = 0; i < n; i++)
        {
            // Process can run only if:
            //
            // 1. It has arrived.
            // 2. It still has some work remaining.
            // 3. Its remaining time is smaller than
            //    the current shortest remaining time.

            if (processes[i].arrivalTime <= currentTime &&
                processes[i].remainingTime > 0 &&
                processes[i].remainingTime < shortestRemaining)
            {
                shortestRemaining =
                    processes[i].remainingTime;

                index = i;
            }
        }


        // ----------------------------------------------------
        // CPU IS IDLE
        // ----------------------------------------------------

        if (index == -1)
        {
            // No process is available.
            // Move time forward.

            currentTime++;

            continue;
        }


        // ----------------------------------------------------
        // FIRST TIME PROCESS STARTS
        // ----------------------------------------------------

        // If startTime is -1, this is the first time
        // this process is getting CPU.

        if (processes[index].startTime == -1)
        {
            processes[index].startTime = currentTime;


            // Response Time is calculated only
            // the FIRST time the process gets CPU.

            processes[index].responseTime =
                processes[index].startTime -
                processes[index].arrivalTime;
        }


        // ----------------------------------------------------
        // EXECUTE FOR ONE TIME UNIT
        // ----------------------------------------------------
        //
        // This is the most important difference from SPN.
        //
        // SPN:
        //
        // currentTime += burstTime;
        //
        // SRTF:
        //
        // Execute ONLY 1 unit.
        //
        // Then check again whether another process
        // should run.

        processes[index].remainingTime--;

        currentTime++;


        // ----------------------------------------------------
        // CHECK WHETHER PROCESS HAS COMPLETED
        // ----------------------------------------------------

        if (processes[index].remainingTime == 0)
        {
            // Process has finished.

            processes[index].completionTime =
                currentTime;


            // Turnaround Time =
            // Completion Time - Arrival Time

            processes[index].turnaroundTime =
                processes[index].completionTime -
                processes[index].arrivalTime;


            // Waiting Time =
            // Turnaround Time - Burst Time

            processes[index].waitingTime =
                processes[index].turnaroundTime -
                processes[index].burstTime;


            // Increase completed process count

            completed++;
        }
    }


    // --------------------------------------------------------
    // DISPLAY SRTF RESULTS
    // --------------------------------------------------------

    cout << "\n\n========== SRTF SCHEDULING ==========\n\n";

    cout << "Process\tAT\tBT\tST\tCT\tTAT\tWT\tRT\n";


    for (int i = 0; i < n; i++)
    {
        cout << processes[i].name << "\t"
             << processes[i].arrivalTime << "\t"
             << processes[i].burstTime << "\t"
             << processes[i].startTime << "\t"
             << processes[i].completionTime << "\t"
             << processes[i].turnaroundTime << "\t"
             << processes[i].waitingTime << "\t"
             << processes[i].responseTime << endl;
    }
}
// ============================================================
// ROUND ROBIN (RR)
// ============================================================
//
// Round Robin is a PREEMPTIVE CPU scheduling algorithm.
//
// Each process gets a fixed amount of CPU time called
// TIME QUANTUM.
//
// If a process does not finish within the time quantum,
// it is preempted and moved to the back of the ready queue.
//
// Example:
//
// Time Quantum = 2
//
// P1 BT = 5
//
// P1 gets:
// 0 -> 2   remaining = 3
//
// Then another process gets CPU.
//
// Later P1 gets:
// 2 more units
//
// This continues until P1 finishes.
//
// ============================================================

void RoundRobin(vector<Process> &processes, int timeQuantum)
{
    int n = processes.size();

    // Current CPU time
    int currentTime = 0;

    // Number of completed processes
    int completed = 0;


    // --------------------------------------------------------
    // INITIALIZE REMAINING TIME
    // --------------------------------------------------------
    //
    // Initially:
    //
    // Remaining Time = Burst Time
    //
    // Example:
    //
    // P1 BT = 5
    // P1 Remaining Time = 5

    for (int i = 0; i < n; i++)
    {
        processes[i].remainingTime =
            processes[i].burstTime;

        // -1 means the process has not started yet
        processes[i].startTime = -1;
    }


    // --------------------------------------------------------
    // READY QUEUE
    // --------------------------------------------------------
    //
    // The queue stores the processes that are ready
    // to execute.
    //
    // Round Robin follows FIFO order in the ready queue.

    vector<int> readyQueue;

    // Keeps track of whether a process is already present
    // in the ready queue.
    vector<bool> inQueue(n, false);


    // --------------------------------------------------------
    // MAIN ROUND ROBIN LOOP
    // --------------------------------------------------------

    while (completed < n)
    {
        // ----------------------------------------------------
        // ADD ARRIVED PROCESSES TO READY QUEUE
        // ----------------------------------------------------

        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrivalTime <= currentTime &&
                processes[i].remainingTime > 0 &&
                !inQueue[i])
            {
                readyQueue.push_back(i);

                inQueue[i] = true;
            }
        }


        // ----------------------------------------------------
        // IF READY QUEUE IS EMPTY
        // ----------------------------------------------------

        if (readyQueue.empty())
        {
            // No process is currently ready.
            //
            // CPU is idle.
            //
            // Move time forward.

            currentTime++;

            continue;
        }


        // ----------------------------------------------------
        // TAKE FIRST PROCESS FROM QUEUE
        // ----------------------------------------------------

        int index = readyQueue.front();

        // Remove it from the front
        readyQueue.erase(readyQueue.begin());

        // It is no longer waiting inside the queue
        inQueue[index] = false;


        // ----------------------------------------------------
        // FIRST TIME PROCESS GETS CPU
        // ----------------------------------------------------

        if (processes[index].startTime == -1)
        {
            // Store first CPU start time
            processes[index].startTime = currentTime;


            // Response Time =
            // First Start Time - Arrival Time

            processes[index].responseTime =
                processes[index].startTime -
                processes[index].arrivalTime;
        }


        // ----------------------------------------------------
        // DETERMINE EXECUTION TIME
        // ----------------------------------------------------
        //
        // A process can execute for:
        //
        // 1. The complete time quantum
        //
        // OR
        //
        // 2. Its remaining time, if it is smaller
        //
        // Example:
        //
        // Time Quantum = 3
        // Remaining Time = 5
        //
        // Execute for 3 units.
        //
        // But:
        //
        // Time Quantum = 3
        // Remaining Time = 2
        //
        // Execute only 2 units and finish.

        int executionTime;

        if (processes[index].remainingTime < timeQuantum)
        {
            executionTime =
                processes[index].remainingTime;
        }
        else
        {
            executionTime = timeQuantum;
        }


        // ----------------------------------------------------
        // EXECUTE PROCESS
        // ----------------------------------------------------

        processes[index].remainingTime -= executionTime;

        currentTime += executionTime;


        // ----------------------------------------------------
        // ADD NEWLY ARRIVED PROCESSES
        // ----------------------------------------------------
        //
        // During the execution of the current process,
        // other processes may have arrived.
        //
        // Add them to the ready queue.

        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrivalTime <= currentTime &&
                processes[i].remainingTime > 0 &&
                !inQueue[i] &&
                i != index)
            {
                readyQueue.push_back(i);

                inQueue[i] = true;
            }
        }


        // ----------------------------------------------------
        // CHECK WHETHER CURRENT PROCESS FINISHED
        // ----------------------------------------------------

        if (processes[index].remainingTime == 0)
        {
            // Process has completed.

            processes[index].completionTime =
                currentTime;


            // Turnaround Time =
            // Completion Time - Arrival Time

            processes[index].turnaroundTime =
                processes[index].completionTime -
                processes[index].arrivalTime;


            // Waiting Time =
            // Turnaround Time - Burst Time

            processes[index].waitingTime =
                processes[index].turnaroundTime -
                processes[index].burstTime;


            // Increase completed process count

            completed++;
        }
        else
        {
            // ------------------------------------------------
            // PROCESS DID NOT FINISH
            // ------------------------------------------------
            //
            // Its time quantum has expired.
            //
            // Put it at the BACK of the ready queue.
            //
            // This is the key idea of Round Robin.

            readyQueue.push_back(index);

            inQueue[index] = true;
        }
    }


    // --------------------------------------------------------
    // DISPLAY RESULTS
    // --------------------------------------------------------

    cout << "\n\n========== ROUND ROBIN SCHEDULING ==========\n\n";

    cout << "Time Quantum: " << timeQuantum << "\n\n";

    cout << "Process\tAT\tBT\tST\tCT\tTAT\tWT\tRT\n";

    for (int i = 0; i < n; i++)
    {
        cout << processes[i].name << "\t"
             << processes[i].arrivalTime << "\t"
             << processes[i].burstTime << "\t"
             << processes[i].startTime << "\t"
             << processes[i].completionTime << "\t"
             << processes[i].turnaroundTime << "\t"
             << processes[i].waitingTime << "\t"
             << processes[i].responseTime << endl;
    }
}
// ============================================================
// HRRN - HIGHEST RESPONSE RATIO NEXT
// ============================================================
//
// HRRN is a NON-PREEMPTIVE CPU scheduling algorithm.
//
// Response Ratio:
//
//        Waiting Time + Burst Time
// RR =  --------------------------
//              Burst Time
//
// Or:
//
//        Waiting Time
// RR = 1 + ------------
//        Burst Time
//
// At every decision point:
// 1. Find all processes that have arrived.
// 2. Calculate their response ratio.
// 3. Select the process with the highest response ratio.
// 4. Execute it completely.
//
// HRRN helps reduce starvation because the response ratio
// increases as a process waits longer.
//
// ============================================================

void HRRN(vector<Process> &processes)
{
    int n = processes.size();

    // Current CPU time
    int currentTime = 0;

    // Number of completed processes
    int completed = 0;

    // Keeps track of completed processes
    vector<bool> done(n, false);


    // Continue until all processes are completed
    while (completed < n)
    {
        // Index of selected process
        int index = -1;

        // Highest response ratio found so far
        double highestRatio = -1;


        // ----------------------------------------------------
        // FIND PROCESS WITH HIGHEST RESPONSE RATIO
        // ----------------------------------------------------

        for (int i = 0; i < n; i++)
        {
            // Process must:
            // 1. Not be completed
            // 2. Have already arrived

            if (!done[i] &&
                processes[i].arrivalTime <= currentTime)
            {
                // Calculate waiting time at this moment

                int waitingTime =
                    currentTime - processes[i].arrivalTime;


                // Calculate response ratio
                //
                // RR = (WT + BT) / BT

                double responseRatio =
                    (double)(waitingTime + processes[i].burstTime)
                    / processes[i].burstTime;


                // Select process with highest ratio

                if (responseRatio > highestRatio)
                {
                    highestRatio = responseRatio;

                    index = i;
                }
            }
        }


        // ----------------------------------------------------
        // NO PROCESS AVAILABLE
        // ----------------------------------------------------

        if (index == -1)
        {
            // CPU is idle because no process has arrived.

            currentTime++;

            continue;
        }


        // ----------------------------------------------------
        // PROCESS STARTS
        // ----------------------------------------------------

        processes[index].startTime = currentTime;


        // Response Time =
        // Start Time - Arrival Time

        processes[index].responseTime =
            processes[index].startTime -
            processes[index].arrivalTime;


        // ----------------------------------------------------
        // EXECUTE COMPLETE PROCESS
        // ----------------------------------------------------
        //
        // HRRN is NON-PREEMPTIVE.
        //
        // Therefore the selected process runs completely.

        currentTime += processes[index].burstTime;


        // ----------------------------------------------------
        // CALCULATE METRICS
        // ----------------------------------------------------

        // Completion Time
        processes[index].completionTime = currentTime;


        // Turnaround Time =
        // Completion Time - Arrival Time

        processes[index].turnaroundTime =
            processes[index].completionTime -
            processes[index].arrivalTime;


        // Waiting Time =
        // Turnaround Time - Burst Time

        processes[index].waitingTime =
            processes[index].turnaroundTime -
            processes[index].burstTime;


        // Mark process as completed

        done[index] = true;

        completed++;
    }


    // --------------------------------------------------------
    // DISPLAY RESULTS
    // --------------------------------------------------------

    cout << "\n\n========== HRRN SCHEDULING ==========\n\n";

    cout << "Process\tAT\tBT\tST\tCT\tTAT\tWT\tRT\n";


    for (int i = 0; i < n; i++)
    {
        cout << processes[i].name << "\t"
             << processes[i].arrivalTime << "\t"
             << processes[i].burstTime << "\t"
             << processes[i].startTime << "\t"
             << processes[i].completionTime << "\t"
             << processes[i].turnaroundTime << "\t"
             << processes[i].waitingTime << "\t"
             << processes[i].responseTime << endl;
    }
}
// ============================================================
// MULTILEVEL FEEDBACK QUEUE - MLFQ
// ============================================================
//
// MLFQ uses multiple priority queues.
//
// Queue 1 = Highest priority
// Queue 2 = Medium priority
// Queue 3 = Lowest priority
//
// New processes enter Queue 1.
//
// If a process uses its complete time quantum,
// it is moved to a lower-priority queue.
//
// This allows short/interactive processes to get
// faster CPU access while long processes gradually
// move down.
//
// ============================================================

void Feedback(vector<Process> &processes)
{
    int n = processes.size();

    int currentTime = 0;
    int completed = 0;

    // Three queues
    vector<int> q1;
    vector<int> q2;
    vector<int> q3;

    // Track whether a process is already inside a queue
    vector<bool> inQueue(n, false);

    // Remaining CPU time
    for (int i = 0; i < n; i++)
    {
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].startTime = -1;
    }

    // Time quantum for each queue
    int quantum1 = 2;
    int quantum2 = 4;
    int quantum3 = 8;

    while (completed < n)
    {
        // ----------------------------------------------------
        // ADD NEWLY ARRIVED PROCESSES TO QUEUE 1
        // ----------------------------------------------------

        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrivalTime <= currentTime &&
                processes[i].remainingTime > 0 &&
                !inQueue[i])
            {
                q1.push_back(i);
                inQueue[i] = true;
            }
        }

        // ----------------------------------------------------
        // CPU IDLE
        // ----------------------------------------------------

        if (q1.empty() && q2.empty() && q3.empty())
        {
            currentTime++;
            continue;
        }

        // ----------------------------------------------------
        // SELECT HIGHEST PRIORITY QUEUE
        // ----------------------------------------------------

        int index;
        int quantum;

        if (!q1.empty())
        {
            index = q1.front();
            q1.erase(q1.begin());

            quantum = quantum1;
        }
        else if (!q2.empty())
        {
            index = q2.front();
            q2.erase(q2.begin());

            quantum = quantum2;
        }
        else
        {
            index = q3.front();
            q3.erase(q3.begin());

            quantum = quantum3;
        }

        inQueue[index] = false;

        // ----------------------------------------------------
        // FIRST CPU ACCESS
        // ----------------------------------------------------

        if (processes[index].startTime == -1)
        {
            processes[index].startTime = currentTime;

            processes[index].responseTime =
                currentTime - processes[index].arrivalTime;
        }

        // ----------------------------------------------------
        // EXECUTION
        // ----------------------------------------------------

        int executionTime =
            min(quantum, processes[index].remainingTime);

        processes[index].remainingTime -= executionTime;

        currentTime += executionTime;

        // ----------------------------------------------------
        // ADD NEW ARRIVALS
        // ----------------------------------------------------

        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrivalTime <= currentTime &&
                processes[i].remainingTime > 0 &&
                !inQueue[i] &&
                i != index)
            {
                q1.push_back(i);
                inQueue[i] = true;
            }
        }

        // ----------------------------------------------------
        // PROCESS FINISHED
        // ----------------------------------------------------

        if (processes[index].remainingTime == 0)
        {
            processes[index].completionTime = currentTime;

            processes[index].turnaroundTime =
                processes[index].completionTime -
                processes[index].arrivalTime;

            processes[index].waitingTime =
                processes[index].turnaroundTime -
                processes[index].burstTime;

            completed++;
        }

        // ----------------------------------------------------
        // PROCESS DID NOT FINISH
        // ----------------------------------------------------

        else
        {
            // Move process to lower-priority queue.

            if (quantum == quantum1)
            {
                q2.push_back(index);
            }
            else
            {
                q3.push_back(index);
            }

            inQueue[index] = true;
        }
    }

    // --------------------------------------------------------
    // DISPLAY RESULTS
    // --------------------------------------------------------

    cout << "\n\n========== MULTILEVEL FEEDBACK QUEUE ==========\n\n";

    cout << "Q1 Quantum = " << quantum1 << endl;
    cout << "Q2 Quantum = " << quantum2 << endl;
    cout << "Q3 Quantum = " << quantum3 << endl;

    cout << "\nProcess\tAT\tBT\tST\tCT\tTAT\tWT\tRT\n";

    for (int i = 0; i < n; i++)
    {
        cout << processes[i].name << "\t"
             << processes[i].arrivalTime << "\t"
             << processes[i].burstTime << "\t"
             << processes[i].startTime << "\t"
             << processes[i].completionTime << "\t"
             << processes[i].turnaroundTime << "\t"
             << processes[i].waitingTime << "\t"
             << processes[i].responseTime
             << endl;
    }
}
// ============================================================
// AGING SCHEDULING
// ============================================================
//
// Aging is used to prevent starvation.
//
// A process that waits for a long time gradually gets
// higher priority.
//
// Lower priority number = higher priority.
//
// Example:
//
// P1 priority = 5
// P2 priority = 10
//
// If P2 waits for a long time:
//
// P2 effective priority becomes:
//
// 10 -> 9 -> 8 -> 7 -> ...
//
// Eventually P2 gets CPU.
//
// ============================================================

void Aging(vector<Process> &processes)
{
    int n = processes.size();

    int currentTime = 0;
    int completed = 0;

    vector<bool> done(n, false);

    // Priority values
    vector<int> priority(n);

    cout << "\nEnter priority for each process\n";
    cout << "Lower number = Higher priority\n\n";

    for (int i = 0; i < n; i++)
    {
        cout << "Priority for "
             << processes[i].name << ": ";

        cin >> priority[i];
    }

    while (completed < n)
    {
        int index = -1;
        int bestPriority = INT_MAX;

        // ----------------------------------------------------
        // FIND HIGHEST PRIORITY AVAILABLE PROCESS
        // ----------------------------------------------------

        for (int i = 0; i < n; i++)
        {
            if (!done[i] &&
                processes[i].arrivalTime <= currentTime)
            {
                // Calculate how long process has waited

                int waitingTime =
                    currentTime -
                    processes[i].arrivalTime;

                // Aging improves priority
                //
                // Every 3 units of waiting improves
                // priority by 1.

                int effectivePriority =
                    priority[i] - waitingTime / 3;

                if (effectivePriority < bestPriority)
                {
                    bestPriority = effectivePriority;
                    index = i;
                }
            }
        }

        // ----------------------------------------------------
        // CPU IDLE
        // ----------------------------------------------------

        if (index == -1)
        {
            currentTime++;
            continue;
        }

        // ----------------------------------------------------
        // PROCESS STARTS
        // ----------------------------------------------------

        processes[index].startTime = currentTime;

        processes[index].responseTime =
            currentTime -
            processes[index].arrivalTime;

        // ----------------------------------------------------
        // NON-PREEMPTIVE EXECUTION
        // ----------------------------------------------------

        currentTime +=
            processes[index].burstTime;

        // ----------------------------------------------------
        // METRICS
        // ----------------------------------------------------

        processes[index].completionTime =
            currentTime;

        processes[index].turnaroundTime =
            processes[index].completionTime -
            processes[index].arrivalTime;

        processes[index].waitingTime =
            processes[index].turnaroundTime -
            processes[index].burstTime;

        done[index] = true;

        completed++;
    }

    // --------------------------------------------------------
    // DISPLAY
    // --------------------------------------------------------

    cout << "\n\n========== AGING SCHEDULING ==========\n\n";

    cout << "Process\tAT\tBT\tST\tCT\tTAT\tWT\tRT\n";

    for (int i = 0; i < n; i++)
    {
        cout << processes[i].name << "\t"
             << processes[i].arrivalTime << "\t"
             << processes[i].burstTime << "\t"
             << processes[i].startTime << "\t"
             << processes[i].completionTime << "\t"
             << processes[i].turnaroundTime << "\t"
             << processes[i].waitingTime << "\t"
             << processes[i].responseTime
             << endl;
    }
}
// ============================================================
//                 PAGE REPLACEMENT ALGORITHMS
// ============================================================


// ============================================================
// FIFO PAGE REPLACEMENT
// ============================================================
//
// FIFO = First In First Out
//
// The page that entered memory first is removed first.
//
// Example:
//
// Frames = 3
//
// Pages:
// 1 2 3 4
//
// When 4 arrives and all frames are full:
//
// 1 is removed because it entered first.
//
// ============================================================

void FIFO(vector<int> pages, int frames)
{
    vector<int> memory;

    int pageFaults = 0;
    int pageHits = 0;

    // Points to the frame that should be replaced next.
    int pointer = 0;


    for (int page : pages)
    {
        bool found = false;


        // ----------------------------------------------------
        // CHECK WHETHER PAGE IS ALREADY IN MEMORY
        // ----------------------------------------------------

        for (int x : memory)
        {
            if (x == page)
            {
                found = true;
                break;
            }
        }


        // ----------------------------------------------------
        // PAGE HIT
        // ----------------------------------------------------

        if (found)
        {
            pageHits++;
        }


        // ----------------------------------------------------
        // PAGE FAULT
        // ----------------------------------------------------

        else
        {
            pageFaults++;


            // If an empty frame is available,
            // simply put the page into memory.

            if (memory.size() < frames)
            {
                memory.push_back(page);
            }


            // If all frames are full,
            // replace the oldest page.

            else
            {
                memory[pointer] = page;

                // Move pointer to next frame.
                //
                // % frames makes the pointer circular.

                pointer = (pointer + 1) % frames;
            }
        }


        // Display current memory

        cout << "Page " << page << " : ";

        for (int x : memory)
        {
            cout << x << " ";
        }

        cout << endl;
    }


    cout << "\nPage Faults = " << pageFaults << endl;
    cout << "Page Hits   = " << pageHits << endl;
}


// ============================================================
// LRU PAGE REPLACEMENT
// ============================================================
//
// LRU = Least Recently Used
//
// When memory is full:
//
// Remove the page that has NOT been used for the longest time
// in the PAST.
//
// Example:
//
// Memory = 1 2 3
//
// If 1 was used most recently,
// and 2 was used longest ago,
// then remove 2.
//
// ============================================================

void LRU(vector<int> pages, int frames)
{
    vector<int> memory;

    int pageFaults = 0;
    int pageHits = 0;


    for (int i = 0; i < pages.size(); i++)
    {
        int page = pages[i];

        bool found = false;


        // ----------------------------------------------------
        // CHECK WHETHER PAGE IS ALREADY IN MEMORY
        // ----------------------------------------------------

        for (int x : memory)
        {
            if (x == page)
            {
                found = true;
                break;
            }
        }


        // ----------------------------------------------------
        // PAGE HIT
        // ----------------------------------------------------

        if (found)
        {
            pageHits++;
        }


        // ----------------------------------------------------
        // PAGE FAULT
        // ----------------------------------------------------

        else
        {
            pageFaults++;


            // Empty frame available

            if (memory.size() < frames)
            {
                memory.push_back(page);
            }


            // Frames are full

            else
            {
                int lruIndex = 0;

                int leastRecent = INT_MAX;


                // Check every page currently in memory

                for (int j = 0; j < memory.size(); j++)
                {
                    int lastUsed = -1;


                    // Search backwards from the current page.
                    //
                    // We are looking for the last time
                    // memory[j] was used.

                    for (int k = i - 1; k >= 0; k--)
                    {
                        if (pages[k] == memory[j])
                        {
                            lastUsed = k;
                            break;
                        }
                    }


                    // Smaller index means it was used
                    // longer ago.

                    if (lastUsed < leastRecent)
                    {
                        leastRecent = lastUsed;

                        lruIndex = j;
                    }
                }


                // Replace least recently used page

                memory[lruIndex] = page;
            }
        }


        // Display current frames

        cout << "Page " << page << " : ";

        for (int x : memory)
        {
            cout << x << " ";
        }

        cout << endl;
    }


    cout << "\nPage Faults = " << pageFaults << endl;
    cout << "Page Hits   = " << pageHits << endl;
}


// ============================================================
// OPTIMAL PAGE REPLACEMENT
// ============================================================
//
// Optimal replacement removes the page whose NEXT use is
// farthest in the FUTURE.
//
// If a page will NEVER be used again,
// it is the best page to remove.
//
// This algorithm is theoretically optimal,
// but an actual OS cannot normally know the future.
//
// ============================================================

void Optimal(vector<int> pages, int frames)
{
    vector<int> memory;

    int pageFaults = 0;
    int pageHits = 0;


    for (int i = 0; i < pages.size(); i++)
    {
        int page = pages[i];

        bool found = false;


        // Check whether page is already in memory

        for (int x : memory)
        {
            if (x == page)
            {
                found = true;
                break;
            }
        }


        // ----------------------------------------------------
        // PAGE HIT
        // ----------------------------------------------------

        if (found)
        {
            pageHits++;
        }


        // ----------------------------------------------------
        // PAGE FAULT
        // ----------------------------------------------------

        else
        {
            pageFaults++;


            // Empty frame available

            if (memory.size() < frames)
            {
                memory.push_back(page);
            }


            // Frames are full

            else
            {
                int replaceIndex = -1;

                int farthest = -1;


                // Check every page currently in memory

                for (int j = 0; j < memory.size(); j++)
                {
                    int nextUse = -1;


                    // Look into the FUTURE

                    for (int k = i + 1; k < pages.size(); k++)
                    {
                        if (pages[k] == memory[j])
                        {
                            nextUse = k;
                            break;
                        }
                    }


                    // If page will never be used again,
                    // immediately select it.

                    if (nextUse == -1)
                    {
                        replaceIndex = j;
                        break;
                    }


                    // Otherwise select the page whose
                    // next use is farthest away.

                    if (nextUse > farthest)
                    {
                        farthest = nextUse;

                        replaceIndex = j;
                    }
                }


                // Replace selected page

                memory[replaceIndex] = page;
            }
        }


        // Display current frames

        cout << "Page " << page << " : ";

        for (int x : memory)
        {
            cout << x << " ";
        }

        cout << endl;
    }


    cout << "\nPage Faults = " << pageFaults << endl;
    cout << "Page Hits   = " << pageHits << endl;
}


// ============================================================
//                    DISK SCHEDULING
// ============================================================


// ============================================================
// FCFS DISK SCHEDULING
// ============================================================
//
// FCFS = First Come First Serve
//
// Requests are serviced in exactly the order in which
// they were received.
//
// Example:
//
// Head = 50
//
// Requests:
// 82 170 43 140
//
// Service order:
//
// 50 -> 82 -> 170 -> 43 -> 140
//
// ============================================================

void DiskFCFS(vector<int> requests, int head)
{
    int totalMovement = 0;


    cout << "\nService Order: ";

    cout << head;


    // Process requests in their original order

    for (int request : requests)
    {
        // Calculate how far the head moves

        totalMovement += abs(head - request);


        // Move head to requested cylinder

        head = request;


        cout << " -> " << head;
    }


    cout << "\nTotal Head Movement = "
         << totalMovement << endl;
}


// ============================================================
// SSTF DISK SCHEDULING
// ============================================================
//
// SSTF = Shortest Seek Time First
//
// At every step:
//
// Choose the request closest to the current head position.
//
// Example:
//
// Head = 50
//
// Requests:
// 82 170 43 140
//
// Distances:
//
// 82  -> 32
// 170 -> 120
// 43  -> 7
// 140 -> 90
//
// So 43 is selected first.
//
// ============================================================

void SSTF(vector<int> requests, int head)
{
    int totalMovement = 0;


    // visited[i] tells us whether request i
    // has already been serviced.

    vector<bool> visited(requests.size(), false);


    cout << "\nService Order: ";

    cout << head;


    // Every request must eventually be serviced

    for (int count = 0; count < requests.size(); count++)
    {
        int closestIndex = -1;

        int shortestDistance = INT_MAX;


        // Find closest unvisited request

        for (int i = 0; i < requests.size(); i++)
        {
            if (!visited[i])
            {
                int distance =
                    abs(head - requests[i]);


                if (distance < shortestDistance)
                {
                    shortestDistance = distance;

                    closestIndex = i;
                }
            }
        }


        // Mark selected request as completed

        visited[closestIndex] = true;


        // Move head

        totalMovement += shortestDistance;

        head = requests[closestIndex];


        cout << " -> " << head;
    }


    cout << "\nTotal Head Movement = "
         << totalMovement << endl;
}


// ============================================================
// SCAN DISK SCHEDULING
// ============================================================
//
// SCAN is also called the Elevator Algorithm.
//
// The disk head moves in one direction,
// services requests along the way,
// reaches the end of the disk,
// and then reverses direction.
//
// directionRight = true
// means the head initially moves towards larger numbers.
//
// directionRight = false
// means the head initially moves towards smaller numbers.
//
// ============================================================

void SCAN(vector<int> requests,
          int head,
          int diskSize,
          bool directionRight)
{
    int totalMovement = 0;


    // Sort requests so that we can process them
    // from left to right or right to left.

    sort(requests.begin(), requests.end());


    cout << "\nService Order: ";

    cout << head;


    // Find the first request that is >= head

    int index = 0;

    while (index < requests.size() &&
           requests[index] < head)
    {
        index++;
    }


    // --------------------------------------------------------
    // MOVE RIGHT
    // --------------------------------------------------------

    if (directionRight)
    {
        // Service requests on right side

        for (int i = index; i < requests.size(); i++)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }


        // SCAN continues all the way to the end
        // of the disk.

        if (head != diskSize - 1)
        {
            totalMovement +=
                abs(head - (diskSize - 1));

            head = diskSize - 1;

            cout << " -> " << head;
        }


        // Now reverse direction

        for (int i = index - 1; i >= 0; i--)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }
    }


    // --------------------------------------------------------
    // MOVE LEFT
    // --------------------------------------------------------

    else
    {
        // Service requests on left side

        for (int i = index - 1; i >= 0; i--)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }


        // Continue to beginning of disk

        if (head != 0)
        {
            totalMovement += head;

            head = 0;

            cout << " -> " << head;
        }


        // Reverse direction

        for (int i = index; i < requests.size(); i++)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }
    }


    cout << "\nTotal Head Movement = "
         << totalMovement << endl;
}


// ============================================================
// C-SCAN DISK SCHEDULING
// ============================================================
//
// C-SCAN = Circular SCAN
//
// The head moves in ONE direction only.
//
// When it reaches the end:
//
//     End -> Beginning
//
// Then it continues in the same direction.
//
// ============================================================

void CSCAN(vector<int> requests,
           int head,
           int diskSize)
{
    int totalMovement = 0;


    sort(requests.begin(), requests.end());


    cout << "\nService Order: ";

    cout << head;


    // Find first request >= head

    int index = 0;

    while (index < requests.size() &&
           requests[index] < head)
    {
        index++;
    }


    // Service requests towards the right

    for (int i = index; i < requests.size(); i++)
    {
        totalMovement +=
            abs(head - requests[i]);

        head = requests[i];

        cout << " -> " << head;
    }


    // Move to end of disk

    if (head != diskSize - 1)
    {
        totalMovement +=
            abs(head - (diskSize - 1));

        head = diskSize - 1;

        cout << " -> " << head;
    }


    // Circular jump:
    //
    // End -> Beginning

    totalMovement += diskSize - 1;

    head = 0;

    cout << " -> " << head;


    // Service remaining requests

    for (int i = 0; i < index; i++)
    {
        totalMovement +=
            abs(head - requests[i]);

        head = requests[i];

        cout << " -> " << head;
    }


    cout << "\nTotal Head Movement = "
         << totalMovement << endl;
}


// ============================================================
// LOOK DISK SCHEDULING
// ============================================================
//
// LOOK is similar to SCAN.
//
// Difference:
//
// SCAN goes all the way to the physical end of the disk.
//
// LOOK stops at the LAST REQUEST in that direction.
//
// Then it reverses.
//
// ============================================================

void LOOK(vector<int> requests,
          int head,
          bool directionRight)
{
    int totalMovement = 0;


    sort(requests.begin(), requests.end());


    cout << "\nService Order: ";

    cout << head;


    // Find first request >= head

    int index = 0;

    while (index < requests.size() &&
           requests[index] < head)
    {
        index++;
    }


    // --------------------------------------------------------
    // MOVE RIGHT
    // --------------------------------------------------------

    if (directionRight)
    {
        // Service right-side requests

        for (int i = index; i < requests.size(); i++)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }


        // Reverse at the last request.
        //
        // Unlike SCAN, we DON'T go to diskSize - 1.

        for (int i = index - 1; i >= 0; i--)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }
    }


    // --------------------------------------------------------
    // MOVE LEFT
    // --------------------------------------------------------

    else
    {
        // Service left-side requests

        for (int i = index - 1; i >= 0; i--)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }


        // Reverse at the last request.

        for (int i = index; i < requests.size(); i++)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }
    }


    cout << "\nTotal Head Movement = "
         << totalMovement << endl;
}


// ============================================================
// C-LOOK DISK SCHEDULING
// ============================================================
//
// C-LOOK is the circular version of LOOK.
//
// It moves in one direction.
//
// When it reaches the last request:
//
//     Last request -> First request
//
// It does NOT go to the physical end of the disk.
//
// ============================================================

void CLOOK(vector<int> requests, int head)
{
    int totalMovement = 0;


    sort(requests.begin(), requests.end());


    cout << "\nService Order: ";

    cout << head;


    // Find first request >= head

    int index = 0;

    while (index < requests.size() &&
           requests[index] < head)
    {
        index++;
    }


    // --------------------------------------------------------
    // SERVICE RIGHT-SIDE REQUESTS
    // --------------------------------------------------------

    for (int i = index; i < requests.size(); i++)
    {
        totalMovement +=
            abs(head - requests[i]);

        head = requests[i];

        cout << " -> " << head;
    }


    // --------------------------------------------------------
    // CIRCULAR JUMP
    // --------------------------------------------------------
    //
    // Jump from the largest request to the smallest request.
    //
    // We don't go to disk boundary.

    if (index > 0)
    {
        totalMovement +=
            abs(head - requests[0]);

        head = requests[0];

        cout << " -> " << head;


        // Continue servicing left-side requests

        for (int i = 1; i < index; i++)
        {
            totalMovement +=
                abs(head - requests[i]);

            head = requests[i];

            cout << " -> " << head;
        }
    }


    cout << "\nTotal Head Movement = "
         << totalMovement << endl;
}
// ============================================================
// BANKER'S ALGORITHM
// ============================================================
//
// Banker's Algorithm is used for DEADLOCK AVOIDANCE.
//
// It checks whether the system is in a SAFE STATE.
//
// Input:
//
// Allocation = resources currently allocated
// Max        = maximum resources required
// Available  = currently available resources
//
// Need:
//
// Need = Max - Allocation
//
// The algorithm tries to find a SAFE SEQUENCE.
//
// ============================================================

void BankersAlgorithm()
{
    int n, m;

    cout << "\n========== BANKER'S ALGORITHM ==========\n\n";

    cout << "Enter number of processes: ";
    cin >> n;

    cout << "Enter number of resource types: ";
    cin >> m;

    vector<vector<int>> allocation(
        n, vector<int>(m));

    vector<vector<int>> maximum(
        n, vector<int>(m));

    vector<int> available(m);

    vector<vector<int>> need(
        n, vector<int>(m));

    // --------------------------------------------------------
    // ALLOCATION MATRIX
    // --------------------------------------------------------

    cout << "\nEnter Allocation Matrix:\n";

    for (int i = 0; i < n; i++)
    {
        cout << "P" << i << ": ";

        for (int j = 0; j < m; j++)
        {
            cin >> allocation[i][j];
        }
    }

    // --------------------------------------------------------
    // MAXIMUM MATRIX
    // --------------------------------------------------------

    cout << "\nEnter Maximum Matrix:\n";

    for (int i = 0; i < n; i++)
    {
        cout << "P" << i << ": ";

        for (int j = 0; j < m; j++)
        {
            cin >> maximum[i][j];
        }
    }

    // --------------------------------------------------------
    // AVAILABLE RESOURCES
    // --------------------------------------------------------

    cout << "\nEnter Available Resources:\n";

    for (int j = 0; j < m; j++)
    {
        cin >> available[j];
    }

    // --------------------------------------------------------
    // CALCULATE NEED
    // --------------------------------------------------------

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            need[i][j] =
                maximum[i][j] -
                allocation[i][j];
        }
    }

    // --------------------------------------------------------
    // DISPLAY NEED MATRIX
    // --------------------------------------------------------

    cout << "\nNeed Matrix:\n";

    for (int i = 0; i < n; i++)
    {
        cout << "P" << i << ": ";

        for (int j = 0; j < m; j++)
        {
            cout << need[i][j] << " ";
        }

        cout << endl;
    }

    // --------------------------------------------------------
    // SAFETY ALGORITHM
    // --------------------------------------------------------

    vector<bool> finished(n, false);

    vector<int> safeSequence;

    vector<int> work = available;

    int count = 0;

    while (count < n)
    {
        bool found = false;

        for (int i = 0; i < n; i++)
        {
            if (!finished[i])
            {
                bool canExecute = true;

                // Check:
                //
                // Need <= Work

                for (int j = 0; j < m; j++)
                {
                    if (need[i][j] > work[j])
                    {
                        canExecute = false;
                        break;
                    }
                }

                // ------------------------------------------------
                // PROCESS CAN FINISH
                // ------------------------------------------------

                if (canExecute)
                {
                    // Pretend process completes
                    //
                    // Its allocated resources are returned.

                    for (int j = 0; j < m; j++)
                    {
                        work[j] += allocation[i][j];
                    }

                    safeSequence.push_back(i);

                    finished[i] = true;

                    count++;

                    found = true;
                }
            }
        }

        // No process could execute
        if (!found)
        {
            break;
        }
    }

    // --------------------------------------------------------
    // RESULT
    // --------------------------------------------------------

    if (count == n)
    {
        cout << "\nSYSTEM IS IN SAFE STATE.\n";

        cout << "Safe Sequence: ";

        for (int i = 0; i < safeSequence.size(); i++)
        {
            cout << "P" << safeSequence[i];

            if (i != safeSequence.size() - 1)
            {
                cout << " -> ";
            }
        }

        cout << endl;
    }
    else
    {
        cout << "\nSYSTEM IS NOT IN A SAFE STATE.\n";
        cout << "Possible deadlock detected.\n";
    }
}
// ============================================================
// PERFORMANCE ANALYZER
// ============================================================
//
// Calculates:
//
// Average Waiting Time
// Average Turnaround Time
// Average Response Time
//
// ============================================================

void PerformanceAnalyzer(
    vector<Process> &processes,
    string algorithm)
{
    int n = processes.size();

    double totalWaiting = 0;
    double totalTurnaround = 0;
    double totalResponse = 0;

    for (int i = 0; i < n; i++)
    {
        totalWaiting +=
            processes[i].waitingTime;

        totalTurnaround +=
            processes[i].turnaroundTime;

        totalResponse +=
            processes[i].responseTime;
    }

    double averageWaiting =
        totalWaiting / n;

    double averageTurnaround =
        totalTurnaround / n;

    double averageResponse =
        totalResponse / n;

    cout << "\n========================================\n";
    cout << "       PERFORMANCE ANALYZER\n";
    cout << "========================================\n";

    cout << "Algorithm: "
         << algorithm << endl;

    cout << "\nAverage Waiting Time    : "
         << averageWaiting << endl;

    cout << "Average Turnaround Time : "
         << averageTurnaround << endl;

    cout << "Average Response Time   : "
         << averageResponse << endl;

    cout << "========================================\n";
}
// ============================================================
// GANTT CHART
// ============================================================
//
// Displays execution order using:
//
// Start Time
// Completion Time
//
// This version is mainly useful for non-preemptive
// scheduling algorithms such as:
//
// FCFS
// SPN
// HRRN
// Aging
//
// ============================================================

void GanttChart(vector<Process> processes)
{
    // Sort according to start time
    sort(processes.begin(), processes.end(),
         [](const Process &a, const Process &b)
         {
             return a.startTime < b.startTime;
         });

    cout << "\n\n========== GANTT CHART ==========\n\n";

    if (processes.empty())
        return;

    // Top line
    cout << " ";

    for (int i = 0; i < processes.size(); i++)
    {
        cout << "--------";
    }

    cout << "\n|";

    // Process names
    for (int i = 0; i < processes.size(); i++)
    {
        cout << "  " << processes[i].name << "   |";
    }

    cout << "\n ";

    // Bottom line
    for (int i = 0; i < processes.size(); i++)
    {
        cout << "--------";
    }

    cout << "\n";

    // Time values
    cout << processes[0].startTime;

    for (int i = 0; i < processes.size(); i++)
    {
        cout << "\t" << processes[i].completionTime;
    }

    cout << "\n";
}
vector<Process> inputProcesses()
{
    int n;

    cout << "\nEnter number of processes: ";
    cin >> n;

    vector<Process> processes(n);

    for (int i = 0; i < n; i++)
    {
        processes[i].name = "P" + to_string(i + 1);

        cout << "\nEnter Arrival Time for "
             << processes[i].name << ": ";
        cin >> processes[i].arrivalTime;

        cout << "Enter Burst Time for "
             << processes[i].name << ": ";
        cin >> processes[i].burstTime;

        // Initialize values
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].completionTime = 0;
        processes[i].turnaroundTime = 0;
        processes[i].waitingTime = 0;
        processes[i].responseTime = 0;
        processes[i].startTime = -1;
    }

    return processes;
}
void CompareScheduling(vector<Process> original)
{
    vector<Process> fcfs = original;
    vector<Process> spn = original;
    vector<Process> srtf = original;
    vector<Process> hrrn = original;

    FCFS(fcfs);
    SPN(spn);
    SRTF(srtf);
    HRRN(hrrn);

    double fcfsWT = 0, spnWT = 0;
    double srtfWT = 0, hrrnWT = 0;

    double fcfsTAT = 0, spnTAT = 0;
    double srtfTAT = 0, hrrnTAT = 0;

    int n = original.size();

    for (int i = 0; i < n; i++)
    {
        fcfsWT += fcfs[i].waitingTime;
        spnWT += spn[i].waitingTime;
        srtfWT += srtf[i].waitingTime;
        hrrnWT += hrrn[i].waitingTime;

        fcfsTAT += fcfs[i].turnaroundTime;
        spnTAT += spn[i].turnaroundTime;
        srtfTAT += srtf[i].turnaroundTime;
        hrrnTAT += hrrn[i].turnaroundTime;
    }

    cout << "\n\n========== ALGORITHM COMPARISON ==========\n\n";

    cout << "Algorithm\tAvg WT\t\tAvg TAT\n";

    cout << "FCFS\t\t"
         << fcfsWT / n << "\t\t"
         << fcfsTAT / n << endl;

    cout << "SPN\t\t"
         << spnWT / n << "\t\t"
         << spnTAT / n << endl;

    cout << "SRTF\t\t"
         << srtfWT / n << "\t\t"
         << srtfTAT / n << endl;

    cout << "HRRN\t\t"
         << hrrnWT / n << "\t\t"
         << hrrnTAT / n << endl;
}
void SchedulingMenu()
{
    vector<Process> original = inputProcesses();

    while (true)
    {
        cout << "\n\n========== CPU SCHEDULING ==========\n";

        cout << "1. FCFS\n";
        cout << "2. SPN / SJF\n";
        cout << "3. SRTF\n";
        cout << "4. Round Robin\n";
        cout << "5. HRRN\n";
        cout << "6. MLFQ\n";
        cout << "7. Aging\n";
        cout << "8. Compare All Algorithms\n";
        cout << "9. Back to Main Menu\n";

        int choice;
        cout << "\nEnter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            vector<Process> p = original;

            FCFS(p);
            PerformanceAnalyzer(p, "FCFS");
            GanttChart(p);
        }

        else if (choice == 2)
        {
            vector<Process> p = original;

            SPN(p);
            PerformanceAnalyzer(p, "SPN / SJF");
            GanttChart(p);
        }

        else if (choice == 3)
        {
            vector<Process> p = original;

            SRTF(p);
            PerformanceAnalyzer(p, "SRTF");
            GanttChart(p);
        }

        else if (choice == 4)
        {
            int quantum;

            cout << "\nEnter Time Quantum: ";
            cin >> quantum;

            vector<Process> p = original;

            RoundRobin(p, quantum);
            PerformanceAnalyzer(p, "Round Robin");
            GanttChart(p);
        }

        else if (choice == 5)
        {
            vector<Process> p = original;

            HRRN(p);
            PerformanceAnalyzer(p, "HRRN");
            GanttChart(p);
        }

        else if (choice == 6)
        {
            vector<Process> p = original;

            Feedback(p);
            PerformanceAnalyzer(p, "MLFQ");
            GanttChart(p);
        }

        else if (choice == 7)
        {
            vector<Process> p = original;

            Aging(p);
            PerformanceAnalyzer(p, "Aging");
            GanttChart(p);
        }

        else if (choice == 8)
        {
            CompareScheduling(original);
        }

        else if (choice == 9)
        {
            break;
        }

        else
        {
            cout << "\nInvalid choice!\n";
        }
    }
}
void PageReplacementMenu()
{
    int n, frames;

    cout << "\n========== PAGE REPLACEMENT ==========\n";

    cout << "Enter number of pages: ";
    cin >> n;

    vector<int> pages(n);

    cout << "Enter page reference string:\n";

    for (int i = 0; i < n; i++)
    {
        cin >> pages[i];
    }

    cout << "Enter number of frames: ";
    cin >> frames;

    while (true)
    {
        cout << "\n\n1. FIFO\n";
        cout << "2. LRU\n";
        cout << "3. Optimal\n";
        cout << "4. Back\n";

        int choice;

        cout << "\nEnter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            cout << "\n========== FIFO ==========\n";
            FIFO(pages, frames);
        }

        else if (choice == 2)
        {
            cout << "\n========== LRU ==========\n";
            LRU(pages, frames);
        }

        else if (choice == 3)
        {
            cout << "\n========== OPTIMAL ==========\n";
            Optimal(pages, frames);
        }

        else if (choice == 4)
        {
            break;
        }

        else
        {
            cout << "Invalid choice!\n";
        }
    }
}
void DiskSchedulingMenu()
{
    int n;

    cout << "\n========== DISK SCHEDULING ==========\n";

    cout << "Enter number of requests: ";
    cin >> n;

    vector<int> requests(n);

    cout << "Enter disk requests:\n";

    for (int i = 0; i < n; i++)
    {
        cin >> requests[i];
    }

    int head;

    cout << "Enter initial head position: ";
    cin >> head;

    int diskSize;

    cout << "Enter disk size: ";
    cin >> diskSize;

    while (true)
    {
        cout << "\n\n1. FCFS\n";
        cout << "2. SSTF\n";
        cout << "3. SCAN\n";
        cout << "4. C-SCAN\n";
        cout << "5. LOOK\n";
        cout << "6. C-LOOK\n";
        cout << "7. Back\n";

        int choice;

        cout << "\nEnter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            DiskFCFS(requests, head);
        }

        else if (choice == 2)
        {
            SSTF(requests, head);
        }

        else if (choice == 3)
        {
            char direction;

            cout << "Move right or left? (R/L): ";
            cin >> direction;

            bool right = (direction == 'R' ||
                          direction == 'r');

            SCAN(requests, head, diskSize, right);
        }

        else if (choice == 4)
        {
            CSCAN(requests, head, diskSize);
        }

        else if (choice == 5)
        {
            char direction;

            cout << "Move right or left? (R/L): ";
            cin >> direction;

            bool right = (direction == 'R' ||
                          direction == 'r');

            LOOK(requests, head, right);
        }

        else if (choice == 6)
        {
            CLOOK(requests, head);
        }

        else if (choice == 7)
        {
            break;
        }

        else
        {
            cout << "Invalid choice!\n";
        }
    }
}
int main()
{
    while (true)
    {
        cout << "\n\n";
        cout << "============================================\n";
        cout << "       OPERATING SYSTEM SIMULATOR\n";
        cout << "============================================\n";

        cout << "1. CPU Scheduling\n";
        cout << "2. Page Replacement\n";
        cout << "3. Disk Scheduling\n";
        cout << "4. Banker's Algorithm\n";
        cout << "5. Exit\n";

        int choice;

        cout << "\nEnter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            SchedulingMenu();
        }

        else if (choice == 2)
        {
            PageReplacementMenu();
        }

        else if (choice == 3)
        {
            DiskSchedulingMenu();
        }

        else if (choice == 4)
        {
            BankersAlgorithm();
        }

        else if (choice == 5)
        {
            cout << "\nExiting simulator...\n";
            break;
        }

        else
        {
            cout << "\nInvalid choice!\n";
        }
    }

    return 0;
}