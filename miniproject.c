#include <stdio.h>
#include <stdlib.h>

// Process Control Block
typedef struct {
    int id;
    int priority;
    int burst_time;
    int waiting_time; // Time spent waiting in the current queue
    int execution_order; // Order in which the process was executed
} Process;

// Queue Node
typedef struct Node {
    Process data;
    struct Node* next;
} Node;

// Queue
typedef struct {
    Node* front;
    Node* rear;
} Queue;

// Function to initialize a queue
void initializeQueue(Queue* q) {
    q->front = q->rear = NULL;
}

// Function to check if a queue is empty
int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

// Function to enqueue a process
void enqueue(Queue* q, Process process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = process;
    newNode->next = NULL;

    if (isQueueEmpty(q)) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// Function to dequeue a process
Process dequeue(Queue* q) {
    if (isQueueEmpty(q)) {
        // Handle underflow
        Process nullProcess = { -1, -1, -1, 0, -1 };
        return nullProcess;
    }

    Node* temp = q->front;
    Process process = temp->data;

    q->front = temp->next;
    free(temp);

    return process;
}

// Function to age processes in a queue
void ageProcesses(Queue* q) {
    Node* current = q->front;

    while (current != NULL) {
        // Decrement the counter for each process
        current->data.waiting_time--;

        // If waiting time reaches a threshold (0 in this case), increase priority
        if (current->data.waiting_time <= 0) {
            current->data.waiting_time = 0; // Reset waiting time

            if (current->data.priority < 2) { // Assuming 0, 1, 2 as priority levels
                current->data.priority++;
            }
        }

        current = current->next;
    }
}

// Function to display the processes in a queue
void displayQueue(Queue* q, const char* queueName) {
    printf("%s Queue: ", queueName);

    Node* current = q->front;
    while (current != NULL) {
        printf("P%d(%d) ", current->data.id, current->data.priority);
        current = current->next;
    }

    printf("\n");
}

// Function to simulate MLQ CPU Scheduling with aging
void simulateMLQWithAging(Process processes[], int numProcesses, int numQueues, int timeQuantum) {
    Queue* queues = (Queue*)malloc(numQueues * sizeof(Queue));

    for (int i = 0; i < numQueues; ++i) {
        initializeQueue(&queues[i]);
    }

    for (int i = 0; i < numProcesses; ++i) {
        enqueue(&queues[0], processes[i]); // Initially, enqueue processes in the lowest-priority queue
    }

    int currentTime = 0;
    int totalExecuted = 0;

    while (totalExecuted < numProcesses) {
        for (int i = 0; i < numQueues; ++i) {
            if (!isQueueEmpty(&queues[i])) {
                Process currentProcess = dequeue(&queues[i]);

                // Simulate execution for the current time quantum or until the process completes
                int executionTime = (currentProcess.burst_time < timeQuantum) ? currentProcess.burst_time : timeQuantum;

                printf("Time %d: Executing P%d from Queue %d for %d units\n", currentTime, currentProcess.id + 1, i, executionTime);
                currentProcess.burst_time -= executionTime;

                // Update waiting time and execution order
                currentProcess.waiting_time = 0;
                currentProcess.execution_order = currentTime;

                currentTime += executionTime;

                if (currentProcess.burst_time > 0) {
                    // Move the process to the next higher-priority queue
                    if (i < numQueues - 1) {
                        currentProcess.priority++;
                    }
                    enqueue(&queues[i], currentProcess);
                } else {
                    // Process has completed
                    printf("Process P%d completed.\n", currentProcess.id + 1);
                    totalExecuted++;
                }
            }
        }

        // Age processes in all queues
        for (int i = 0; i < numQueues; ++i) {
            ageProcesses(&queues[i]);
        }
    }

    free(queues);  // Don't forget to free the allocated memory when done
}


int main() {
    int numProcesses, timeQuantum;

    printf("Enter the number of processes: ");
    scanf("%d", &numProcesses);

    printf("Enter the time quantum for Round Robin scheduling: ");
    scanf("%d", &timeQuantum);

    Process* processes = (Process*)malloc(numProcesses * sizeof(Process));

    printf("Enter arrival time, burst time, and priority for each process:\n");
    for (int i = 0; i < numProcesses; ++i) {
        printf("Process %d: ", i + 1);
        scanf("%d %d %d", &processes[i].id, &processes[i].burst_time, &processes[i].priority);
        processes[i].waiting_time = 0;
        processes[i].execution_order = -1;
    }

    int numQueues = 3; // Assuming 3 priority levels

    simulateMLQWithAging(processes, numProcesses, numQueues, timeQuantum);

    free(processes);  // Don't forget to free the allocated memory when done

    return 0;
}
