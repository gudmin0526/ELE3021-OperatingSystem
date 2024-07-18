#include <stdio.h>
#include <pthread.h>

int shared_resource = 0;

#define NUM_ITERS 5000
#define NUM_THREADS 5000

void lock();
void unlock();

int n = NUM_THREADS;

int choice[NUM_THREADS] = { 0 };
int ticket[NUM_THREADS] = { 0 };
int S = 1;

int max(void)
{
    int rv = ticket[0];
    for (int i = 1; i < NUM_THREADS; i++) {
        if (ticket[i] > rv)
            rv = ticket[i];
    }
    return rv;
}

void lock(int tid)
{   
    long cnt;

    choice[tid] = 1;
    ticket[tid] = 1 + max();
    choice[tid] = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (i == tid) continue;

        cnt = 0;
        while (choice[i]) {
            if (++cnt > NUM_ITERS * NUM_THREADS) {
                printf("Deadlock occured.\n");
                pthread_exit(NULL);
            }
        }

        cnt = 0;
		while (
            (ticket[i] != 0) &&
            (ticket[i] < ticket[tid] || (ticket[i] == ticket[tid] && i < tid))
        ) {
            if (++cnt > NUM_ITERS * NUM_THREADS) {
                printf("Deadlock occured.\n");
                pthread_exit(NULL);
            }
        }
    }

    cnt = 0;
    while (S <= 0) {
        if (++cnt > NUM_ITERS * NUM_THREADS) {
                printf("Deadlock occured.\n");
                pthread_exit(NULL);
        }
    }
    S--;
}

void unlock(int tid)
{
	S++;
    ticket[tid] = 0;
}

void* thread_func(void* arg) {
    int tid = *(int*)arg;
    
    lock(tid);
    
    for(int i = 0; i < NUM_ITERS; i++)    shared_resource++;
   
	unlock(tid);
    
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[n];
    int tids[n];
    
    for (int i = 0; i < n; i++) {
        tids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &tids[i]);
    }
    
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("shared: %d\n", shared_resource);
    
    return 0;
}
