/* 
 * File:   genericThreadClass.cpp
 * Author: Kritpal
 *
 * Created on 27 April, 2017, 10:16 AM
 * g++ genericThreadClass.cpp -lpthread -w  -o /tftpboot/tcp
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

class GenericThread {
public:
    GenericThread();
    static void *thread_call(void *data); //In this you can call your function which requires timeout
    int ping(int timeOut);
private:
    int status; //0 success 1 fail
    pthread_mutex_t calculating; // = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t done; // = PTHREAD_COND_INITIALIZER;
    int do_or_timeout( struct timespec *max_wait);


};

void *GenericThread::thread_call(void *context) {
    int oldtype;
    GenericThread *cI = (GenericThread*) context;
    /* allow the thread to be killed at any time */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
    //printf("Host=%s\n", cI->host);
    /* ... calculations and expensive io here, for example:
     * infinitely loop
     */

    sleep(3);
    cI->status = 0;
    /* wake up the caller if we've completed in time */
    pthread_cond_signal(&cI->done);
    pthread_exit((void*) 1);
    return NULL;
}

/* note: this is not thread safe as it uses a global condition/mutex */
int GenericThread::do_or_timeout( struct timespec *max_wait) {
    struct timespec abs_time;
    pthread_t tid = NULL;
    int err;

    pthread_mutex_init(&calculating, NULL);
    pthread_cond_init(&done, NULL);
    pthread_mutex_lock(&calculating);

    /* pthread cond_timedwait expects an absolute time to wait until */
    clock_gettime(CLOCK_REALTIME, &abs_time);
    abs_time.tv_sec += max_wait->tv_sec;
    abs_time.tv_nsec += max_wait->tv_nsec;
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);

    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    pthread_create(&tid, &attrs, thread_call, (void*) this);

    /* pthread_cond_timedwait can return spuriously: this should
     * be in a loop for production code
     */
    err = pthread_cond_timedwait(&done, &calculating, &abs_time);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    if (err == ETIMEDOUT) {
        fprintf(stderr, "%s: calculation timed out\n", __func__);
        status = 1;
    }
    //if (!err)
    pthread_mutex_unlock(&calculating);
    pthread_mutex_destroy(&calculating);
    pthread_cond_destroy(&done);
    pthread_attr_destroy(&attrs);

    return err;
}

GenericThread::GenericThread() {
    status = -1;

}

int GenericThread::ping(int timeOut) {
    struct timespec max_wait;
    memset(&max_wait, 0, sizeof (max_wait));
    // system("date");
    /* wait at most 2 seconds */
    max_wait.tv_sec = timeOut;
    do_or_timeout( &max_wait);
    return status;
}

int main() {

    while (1) {
        //system("date");
        GenericThread cI;

        if (cI.ping(5)) {
            printf("Timeout\n");
        } else {
            printf("Success\n");
            sleep(5);
        }


    }
    return 0;
}
