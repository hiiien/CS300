#include <pthread.h>
#include <semaphore.h>
#include "mytime.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


int left, right, num_chairs, num_students, students_waiting = 0;
sem_t students, rec;
 
// +++++ pthread APIs: +++++ 
// pthread_mutex_t  lock1 = PTHREAD_MUTEX_INITIALIZER; 
// pthread_cond_t   cond1 = PTHREAD_COND_INITIALIZER; 
// pthread_cond_wait(&cond1, &lock1); 
// pthread_cond_signal(&cond1); 
// pthread_mutex_lock(&locl1); 
// pthread_mutex_unlock(&lock1); 


pthread_mutex_t  lock = PTHREAD_MUTEX_INITIALIZER; 


void get() {
    printf("Recruiter will call sem_wait;\n");
    printf("Recruiter will call mutex_lock;\n");
    sem_wait(&students);
    pthread_mutex_lock(&lock);
    students_waiting--;
    pthread_mutex_unlock(&lock);
    printf("Recruiter will call mutex_unlock;\n");
    sem_post(&rec);
    printf("Recruiter will call sem_post;\n");
    int t = mytime(left, right);
    printf("Recruiter to sleep %d sec;\n", t);
    sleep(t);
    printf("Recruiter wake up;\n");
    sleep(t);
}
 

void put(int i) { 	
    printf("Student %lu will call mutex_lock;\n", pthread_self());
    pthread_mutex_lock(&lock);
    if(students_waiting < num_chairs){
        students_waiting++;
        pthread_mutex_unlock(&lock);
        printf("Student %lu will call mutex_unlock;\n", pthread_self());
        printf("Stdent %d will call sem_post;\n", pthread_self());
        sem_post(&students);
        sem_wait(&rec);
        printf("Student %lu will call sem_wait;\n", pthread_self());
        printf("Student %lu will interview.\n", pthread_self());
    } else { 
        pthread_mutex_unlock(&lock);
        printf("No chairs open, Student %lu will leave;\n", pthread_self());
        printf("Student %lu will call mutex_unlock;\n", pthread_self());
    }
} 


void *student(void *arg) {
    int i = *((int*) arg);
    printf("Student %d thread started. ID: %lu\n", i, pthread_self());
    for (int j = 0; j < 2; j++){
        int t = mytime(left, right);
        printf("Student %lu to sleep %d sec;\n", pthread_self(), t);
        sleep(t);
        printf("Student Id: %lu wake up;\n", pthread_self());

        put(i);
    }
    printf("Student %lu completes their interviews now exiting.\n", pthread_self());
    free(arg);
	return NULL;
}

void *recruiter(void *arg) {
    printf("Recruiter thread started. ID: %lu.\n", pthread_self());
    while(1) {
        if(students_waiting == 0) {
            printf("Recruiter has no students, going back to work.\n");
        }
        get();
    }

}


 int main(int argc, char *argv[]) {
    if (argc != 5) {
		fprintf(stderr, "usage: %s <num_students> <num_chairs> <consumers>\n", argv[0]);
		exit(1);
    }
	num_students = atoi(argv[1]);
    num_chairs = atoi(argv[2]);
    left = atoi(argv[3]);
    right = atoi(argv[4]);
    srand(time(NULL));
    sem_init(&rec, 0, 0);
    sem_init(&students, 0, 0);

    pthread_t student_tids[num_students];
    pthread_t recruiter_tid;

    pthread_create(&recruiter_tid, NULL, recruiter, NULL);

    for (int i = 0; i < num_students; i++) {
		int *student_id = malloc(sizeof(int));
        *student_id = i + 1;
        pthread_create(&student_tids[i], NULL, student, student_id);
	}

    for (int i = 0; i < num_students; i++) {
        pthread_join(student_tids[i], NULL);
    }



   pthread_cancel(recruiter_tid);
   sem_destroy(&rec);
   sem_destroy(&students);
   pthread_mutex_destroy(&lock);

    return 0;
 }
