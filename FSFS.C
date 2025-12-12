#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50
#define HASH_SIZE 1009
typedef struct ProcessControlBlock
{
    char name[NAME_LEN];
    int pid;

    int cpu_total_burst;
    int cpu_left;

    int io_trigger_time;
    int io_total_duration;
    int io_left;

    int io_in_progress;
    int cpu_executed;

    int completion_time;
    int state;

    struct ProcessControlBlock *next;
} ProcessControlBlock;
typedef struct QueueNode
{
    ProcessControlBlock *proc;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;
typedef struct KillEvent
{
    int target_pid;
    int trigger_time;
    int consumed;
    struct KillEvent *next;
} KillEvent;
unsigned computeHash(int pid)
{
    return pid % HASH_SIZE;
}

void hashInsert(ProcessControlBlock *hash_table[], ProcessControlBlock *proc)
{
    int h = computeHash(proc->pid);
    proc->next = hash_table[h];
    hash_table[h] = proc;
}

ProcessControlBlock *hashGet(ProcessControlBlock *hash_table[], int pid)
{
    int h = computeHash(pid);
    ProcessControlBlock *curr = hash_table[h];

    while(curr)
    {
        if(curr->pid == pid)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void hashRemove(ProcessControlBlock *hash_table[], int pid)
{
    int h = computeHash(pid);
    ProcessControlBlock *curr = hash_table[h];
    ProcessControlBlock *prev = NULL;

    while(curr)
    {
        if(curr->pid == pid)
        {
            if(prev)
                prev->next = curr->next;
            else
                hash_table[h] = curr->next;

            return;
        }

        prev = curr;
        curr = curr->next;
    }
}
void queueInit(Queue *q)
{
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

void enqueue(Queue *q, ProcessControlBlock *proc)
{
    QueueNode *n = (QueueNode *)malloc(sizeof(QueueNode));
    n->proc = proc;
    n->next = NULL;

    if(!q->rear)
    {
        q->front = q->rear = n;
    }
    else
    {
        q->rear->next = n;
        q->rear = n;
    }

    q->size++;
}

ProcessControlBlock *dequeue(Queue *q)
{
    if(!q->front)
        return NULL;

    QueueNode *n = q->front;
    ProcessControlBlock *p = n->proc;

    q->front = n->next;
    if(!q->front)
        q->rear = NULL;

    free(n);
    q->size--;

    return p;
}
int queueRemoveByPid(Queue *q, int pid)
{
    QueueNode *curr = q->front, *prev = NULL;

    while(curr)
    {
        if(curr->proc->pid == pid)
        {
            if(prev)
                prev->next = curr->next;
            else
                q->front = curr->next;

            if(curr == q->rear)
                q->rear = prev;

            free(curr);
            q->size--;

            return 1;
        }

        prev = curr;
        curr = curr->next;
    }

    return 0;
}
void addKillEvent(KillEvent **kill_event_list, int pid, int time)
{
    KillEvent *k = (KillEvent *)malloc(sizeof(KillEvent));

    k->target_pid = pid;
    k->trigger_time = time;
    k->consumed = 0;

    k->next = *kill_event_list;
    *kill_event_list = k;
}

int terminatedCount(Queue *terminated_queue)
{
    return terminated_queue->size;
}
void readInput(
    ProcessControlBlock *hash_table[],
    Queue *ready_queue,
    KillEvent **kill_events,
    int *total_processes
)
{
    char line[200];

    while(fgets(line, sizeof(line), stdin))
    {
        // KILL EVENT
        if(strncmp(line, "kill", 4) == 0)
        {
            int pid, time;
            if(sscanf(line + 4, "%d %d", &pid, &time) == 2)
                addKillEvent(kill_events, pid, time);
        }
        else
        {
            char pname[NAME_LEN], io_start_str[20], io_dur_str[20];
            int pid, cpu;
            int count = sscanf(line, "%s %d %d %s %s",
                               pname, &pid, &cpu,
                               io_start_str, io_dur_str);

            if(count >= 3)
            {
                ProcessControlBlock *proc = (ProcessControlBlock *)calloc(1, sizeof(ProcessControlBlock));

                strcpy(proc->name, pname);
                proc->pid = pid;

                proc->cpu_total_burst = cpu;
                proc->cpu_left = cpu;

                if(count >= 5 && strcmp(io_start_str, "-") != 0)
                {
                    proc->io_trigger_time = atoi(io_start_str);
                    proc->io_total_duration = atoi(io_dur_str);
                }
                else
                {
                    proc->io_trigger_time = -1;
                    proc->io_total_duration = 0;
                }

                proc->io_left = 0;
                proc->io_in_progress = 0;
                proc->cpu_executed = 0;

                hashInsert(hash_table, proc);
                enqueue(ready_queue, proc);
                (*total_processes)++;
            }
        }
    }
}
void applyKillEvents(
    int tick,
    KillEvent *kill_events,
    ProcessControlBlock *hash_table[],
    Queue *ready_queue,
    Queue *io_wait_queue,
    Queue *finished_queue,
    ProcessControlBlock **running_process
)
{
    KillEvent *k = kill_events;

    while(k)
    {
        if(!k->consumed && k->trigger_time == tick)
        {
            ProcessControlBlock *proc = hashGet(hash_table, k->target_pid);

            if(proc)
            {
               
                if(*running_process && (*running_process)->pid == proc->pid)
                {
                    (*running_process)->state = 2;
                    (*running_process)->completion_time = tick;

                    hashRemove(hash_table, proc->pid);
                    enqueue(finished_queue, *running_process);

                    *running_process = NULL;
                }
                else if(queueRemoveByPid(ready_queue, proc->pid))
                {
                    proc->state = 2;
                    proc->completion_time = tick;

                    hashRemove(hash_table, proc->pid);
                    enqueue(finished_queue, proc);
                }
                else if(queueRemoveByPid(io_wait_queue, proc->pid))
                {
                    proc->state = 2;
                    proc->completion_time = tick;

                    hashRemove(hash_table, proc->pid);
                    enqueue(finished_queue, proc);
                }
            }

            k->consumed = 1;
        }

        k = k->next;
    }
}
void simulate(
    ProcessControlBlock *hash_table[],
    Queue *ready_queue,
    Queue *io_wait_queue,
    Queue *finished_queue,
    KillEvent *kill_events,
    int total_processes
)
{
    int tick = 0;
    ProcessControlBlock *running_process = NULL;

    while(terminatedCount(finished_queue) < total_processes)
    {
        applyKillEvents(tick, kill_events,hash_table, ready_queue, io_wait_queue,finished_queue, &running_process);

        Queue new_io_queue;
        queueInit(&new_io_queue);

        QueueNode *cur = io_wait_queue->front;

        while(cur)
        {
            ProcessControlBlock *proc = cur->proc;
            QueueNode *next = cur->next;

            if(!proc->io_in_progress)
            {
                proc->io_in_progress = 1;
                enqueue(&new_io_queue, proc);
            }
            else
            {
                if(proc->io_left > 0)
                    proc->io_left--;

                if(proc->io_left == 0)
                {
                    proc->state = 0;
                    proc->io_in_progress = 0;
                    enqueue(ready_queue, proc);
                }
                else
                {
                    enqueue(&new_io_queue, proc);
                }
            }

            free(cur);
            cur = next;
        }

        *io_wait_queue = new_io_queue;
        if(!running_process)
        {
            running_process = dequeue(ready_queue);
            if(running_process)
                running_process->state = 1;
        }
        if(running_process)
        {
            running_process->cpu_executed++;
            running_process->cpu_left--;
        }

      
        if(running_process)
        {
            if(running_process->cpu_left == 0)
            {
                running_process->state = 3;
                running_process->completion_time = tick + 1;

                hashRemove(hash_table, running_process->pid);
                enqueue(finished_queue, running_process);

                running_process = NULL;
            }
            else if(running_process->io_trigger_time >= 0 &&
                    running_process->cpu_executed == running_process->io_trigger_time)
            {
                running_process->state = 4;
                running_process->io_left = running_process->io_total_duration;
                running_process->io_in_progress = 0;

                enqueue(io_wait_queue, running_process);
                running_process = NULL;
            }
        }

        tick++;
    }
}

void printResults(Queue *finished_queue)
{
    printf("PID\tName\tCPU\tIO\tTurnaround\tWaiting\n");

    QueueNode *cur = finished_queue->front;

    while(cur)
    {
        ProcessControlBlock *p = cur->proc;

        if(p->state == 2)  // killed
        {
            printf("%d\t%s\t%d\t%d\t-\t\t-\n",
                   p->pid, p->name, p->cpu_total_burst, p->io_total_duration);
        }
        else
        {
            int turnaround = p->completion_time;
            int waiting = turnaround - p->cpu_total_burst;

            if(waiting < 0) waiting = 0;

            printf("%d\t%s\t%d\t%d\t%d\t\t%d\n",
                   p->pid, p->name, p->cpu_total_burst, p->io_total_duration,
                   turnaround, waiting);
        }

        cur = cur->next;
    }
}

int main()
{
    ProcessControlBlock *hash_table[HASH_SIZE] = {0};

    Queue ready_queue, io_wait_queue, finished_queue;
    queueInit(&ready_queue);
    queueInit(&io_wait_queue);
    queueInit(&finished_queue);

    KillEvent *kill_events = NULL;
    int total_processes = 0;

    readInput(hash_table, &ready_queue, &kill_events, &total_processes);

    if(total_processes == 0)
    {
        printf("No processes provided.\n");
        return 0;
    }

    simulate(hash_table, &ready_queue, &io_wait_queue, &finished_queue,
             kill_events, total_processes);

    printResults(&finished_queue);

    return 0;
}
