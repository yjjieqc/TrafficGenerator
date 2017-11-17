#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include <pthread.h>
#include <bits/sigthread.h>
#include <asm/errno.h>
#include <signal.h>
#include "common.h"

/* buffer to use w/o rate limiting */
static char max_write_buf[TG_MAX_WRITE] = {0};
/* buffer to use with rate limiting */
static char min_write_buf[TG_MIN_WRITE] = {0};

bool common_verbose = 0; //by default, we do not need so much output

//struct timeval last_read_time, current_read_time;    /* last and current time of reading n bytes */
//the log file to log the throughput in reviever client
//char throughput_log_name[80] = "/home/hcy/tcp-data-analysis/throughput.txt";
//FILE *throughput_fd = NULL;


/*
 * This function attemps to read exactly count bytes from file descriptor fd
 * into buffer starting at buf. It repeatedly calls read() until either:
 * 1. count bytes have been read
 * 2. end of file is reached, or for a network socket, the connection is closed
 * 3. read() produces an error
 * Each internal call to read() is for at most max_per_read bytes. The return
 * value gives the number of bytes successfully read.
 * The dummy_buf flag can be set by the caller to indicate that the contents
 * of buf are irrelevant. In this case, all read() calls put their data at
 * location starting at buf, overwriting previous reads.
 * To avoid buffer overflow, the length of buf should be at least count when
 * dummy_buf = false, and at least min{count, max_per_read} when
 * dummy_buf = true.
 */
unsigned int read_exact(int fd, char *buf, size_t count, size_t max_per_read, bool dummy_buf)
{
    unsigned int bytes_total_read = 0;  /* total number of bytes that have been read */
    unsigned int bytes_to_read = 0; /* maximum number of bytes to read in next read() call */
    char *cur_buf = NULL;   /* current location */
    int n;  /* number of bytes read in current read() call */

    if (!buf)
        return 0;

    while (count > 0)
    {
        bytes_to_read = (count > max_per_read) ? max_per_read : count;
        cur_buf = (dummy_buf) ? buf : (buf + bytes_total_read);
        n = read(fd, cur_buf, bytes_to_read);

        if (n <= 0)
        {
            if (n < 0)
                printf("Error: read() in read_exact()");
            break;
        }
        else
        {
            bytes_total_read += n;
            count -= n;
        }
    }

    return bytes_total_read;
}


//This function extends the read_exact function to create a additional thread to
//output throughput in an fixed time interval, and can be used to replace the read_exact

unsigned int ex_read_exact(int fd, char *buf, size_t count, size_t max_per_read, bool dummy_buf, unsigned int time_interval, char *log_name)
{
    unsigned int bytes_total_read = 0;  /* total number of bytes that have been read */
    unsigned int bytes_to_read = 0; /* maximum number of bytes to read in next read() call */
    char *cur_buf = NULL;   /* current location */
    int n;  /* number of bytes read in current read() call */

    FILE *throughput_fd = NULL;
    pthread_t th_log_thread;
    struct stru_throughput throughput;
    int ret_val;
    int err;

    if (!buf)
        return 0;

    if (common_verbose)
        printf("thread %d try to open file %s \n", pthread_self(), log_name);

    throughput_fd = fopen(log_name, "w+");
    if (!throughput_fd)
        error("Error: open throughput file for the first time");
    fprintf(throughput_fd, "size %u \n", count);

    throughput.log_fd = throughput_fd;
    throughput.interval_us = time_interval;
    throughput.byte_total_read = 0;
    throughput.end_child_thread = 0;
    pthread_mutex_init(&(throughput.lock), NULL);

//TODO: fail to make the detached attr, and detached pthead makes multi thread easy to meet bugs!
//    pthread_attr_t  attr;
//    pthread_attr_init(&attr);
//    pthread_attr_setdetachstate(&attr,  PTHREAD_CREATE_DETACHED);
//    pthread_attr_destroy(&attr);

    err = pthread_create(&th_log_thread, NULL, log_throughput, &throughput);
    if (err != 0)
    {
        printf("error to create pthread \n");
        exit(EXIT_FAILURE);
    }

    if (common_verbose)
        printf("~~~ thread %d tries to create th_log_thread %d \n", pthread_self(), th_log_thread);

    while (count > 0)
    {
        bytes_to_read = (count > max_per_read) ? max_per_read : count;
        cur_buf = (dummy_buf) ? buf : (buf + bytes_total_read);
        n = read(fd, cur_buf, bytes_to_read);

        if (n <= 0)
        {
            if (n < 0)
                printf("Error: read() in extended read_exact()");
            break;
        }
        else
        {
            bytes_total_read += n;

            pthread_mutex_lock(&(throughput.lock));
            throughput.byte_total_read = bytes_total_read;
            pthread_mutex_unlock(&(throughput.lock));

            count -= n;
        }
    }

//TODO: should open or kill a thread safely !
//TODO: should fix the bug to make pthread_exit safely !
//TODO: may use a specific thread to open and write and close file
//    pthread_cancel can cause a segment fault problem
//      pthread_cancel implicityly send a signal to a thread, and can cause a pthread_exit() function
//      implicityly pthread_exit is easy to meet bugs!
//    pthread_cancel(th_log_thread);
//    printf("!!!!! now thread %d try to kill th_log_thread %d !!!\n", pthread_self(), th_log_thread);
//    ret_val = pthread_kill(th_log_thread, SIGTERM);

    throughput.end_child_thread = 1;
    pthread_join(th_log_thread, NULL);

//TODO: the following code to see the if the thread is killed, but that can cause SIGV
//    printf("==== to test the thread %d is dead ====\n", th_log_thread);
//    ret_val = pthread_kill(th_log_thread, 0);
//
//    if(!ret_val)
//    {
//        printf("the thread %d is still alive. loop to wait for thread end !\n",th_log_thread);
//        while (true)
//        {
//            ret_val = pthread_kill(th_log_thread, 0);
//            if (ret_val == ESRCH)
//            {
//                printf(" the thread %d has successfully been killed. \n", th_log_thread);
//                break;
//            }
//
//        }
//    }
//    else if(ret_val == ESRCH)
//    {
//        printf(" the thread %d has successfully been killed. \n", th_log_thread);
//    }
//    else if (ret_val == EINVAL)
//    {
//        printf(" the thread %d is illegal, and must find out why . \n", th_log_thread);
//    }

    if (common_verbose)
        printf("thread %d try to close file %s \n", pthread_self(), log_name);

    fclose(throughput_fd);
    return bytes_total_read;

}

//TODO: should bind the signal with this function
//TODO: find out how to cooperate with the pthread_cancel and pthread_kill function
//void on_signal_term(int sig)
//{
//    printf("on receiving the signal %d \n",sig);
//    printf("this is the thread %d , and I am about to exit\n", pthread_self());
//
////    pthread_exit(NULL);
//}

void *log_throughput(void *ptr)
{
    struct stru_throughput* th_ptr = (struct stru_throughput*)ptr;
    unsigned int t_inval = th_ptr->interval_us;
    FILE *log_fd = th_ptr->log_fd;

    unsigned long long interval_us;
    unsigned long long current_timestamp;
    double point_current_timestamp;

    struct timeval last_read_time;
    struct timeval current_read_time;

    unsigned int last_read_bytes = 1;
    unsigned int current_read_bytes = 0;
    unsigned long long tmp_bytes;

//    signal(SIGTERM, on_signal_term);
    gettimeofday(&last_read_time, NULL);
    while(true)
    {
        if (th_ptr->end_child_thread)
            break;
        gettimeofday(&current_read_time, NULL);
        interval_us = (current_read_time.tv_sec - last_read_time.tv_sec) * 1000000 + current_read_time.tv_usec-last_read_time.tv_usec;
        if(t_inval < interval_us)
        {
            last_read_time = current_read_time;
            current_timestamp = current_read_time.tv_sec *1000000 + current_read_time.tv_usec;
            point_current_timestamp = current_timestamp;
            point_current_timestamp /= 1000000;
            current_read_bytes = th_ptr->byte_total_read;
            tmp_bytes = current_read_bytes - last_read_bytes;
            tmp_bytes *= 8;
            pthread_testcancel();
            if (tmp_bytes > 0)
                fprintf(log_fd, "%f %llu\n",  point_current_timestamp, tmp_bytes/interval_us);
            pthread_testcancel();
            last_read_bytes = current_read_bytes;
        }
    }
    return NULL;
}

/*
 * This function attemps to write exactly count bytes from the buffer starting
 * at buf to file referred to by file descriptor fd. It repeatedly calls
 * write() until either:
 * 1. count bytes have been written
 * 2. write() produces an error
 * Each internal call to write() is for at most max_per_write bytes. The return
 * value gives the number of bytes successfully written.
 * The dummy_buf flag can be set by the caller to indicate that the contents
 * of buf are irrelevant. In this case, all write() calls get their data from
 * starting location buf.
 * To avoid buffer overflow, the length of buf should be at least count when
 * dummy_buf = false, and at least min{count, max_per_write} when
 * dummy_buf = true.
 * Users can rate-limit the sending of traffic. If rate_mbps is equal to 0, it indicates no rate-limiting.
 * Users can also set ToS value for traffic.
 */
unsigned int write_exact(int fd, char *buf, size_t count, size_t max_per_write,
    unsigned int rate_mbps, unsigned int tos, unsigned int sleep_overhead_us, bool dummy_buf)
{
    unsigned int bytes_total_write = 0; /* total number of bytes that have been written */
    unsigned int bytes_to_write = 0;    /* maximum number of bytes to write in next send() call */
    char *cur_buf = NULL;   /* current location */
    int n;  /* number of bytes read in current read() call */
    struct timeval tv_start, tv_end;    /* start and end time of write */
    long sleep_us = 0;  /* sleep time (us) */
    long write_us = 0;  /* time used for write() */

    if (setsockopt(fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0)
        printf("Error: set IP_TOS option in write_exact()");

    while (count > 0)
    {
        bytes_to_write = (count > max_per_write) ? max_per_write : count;
        cur_buf = (dummy_buf) ? buf : (buf + bytes_total_write);
        gettimeofday(&tv_start, NULL);
        n = write(fd, cur_buf, bytes_to_write);
        gettimeofday(&tv_end, NULL);
        write_us = (tv_end.tv_sec - tv_start.tv_sec) * 1000000 + tv_end.tv_usec - tv_start.tv_usec;
        sleep_us += (rate_mbps) ? n * 8 / rate_mbps - write_us : 0;

        if (n <= 0)
        {
            if (n < 0)
                printf("Error: write() in write_exact()");
            break;
        }
        else
        {
            bytes_total_write += n;
            count -= n;
            if (sleep_overhead_us < sleep_us)
            {
                usleep(sleep_us - sleep_overhead_us);
                sleep_us = 0;
            }
        }
    }

    return bytes_total_write;
}

/* read the metadata of a flow and return true if it succeeds. */
bool read_flow_metadata(int fd, struct flow_metadata *f)
{
    char buf[TG_METADATA_SIZE] = {0};

    if (!f)
        return false;

    if (read_exact(fd, buf, TG_METADATA_SIZE, TG_METADATA_SIZE, false) != TG_METADATA_SIZE)
        return false;

    /* extract metadata */
    memcpy(&(f->id), buf + offsetof(struct flow_metadata, id), sizeof(f->id));
    memcpy(&(f->size), buf + offsetof(struct flow_metadata, size), sizeof(f->size));
    memcpy(&(f->tos), buf + offsetof(struct flow_metadata, tos), sizeof(f->tos));
    memcpy(&(f->rate), buf + offsetof(struct flow_metadata, rate), sizeof(f->rate));

    return true;
}

/* write a flow request into a socket and return true if it succeeds */
bool write_flow_req(int fd, struct flow_metadata *f)
{
    char buf[TG_METADATA_SIZE] = {0};   /* buffer to hold metadata */

    if (!f)
        return false;

    /* fill in metadata */
    memcpy(buf + offsetof(struct flow_metadata, id), &(f->id), sizeof(f->id));
    memcpy(buf + offsetof(struct flow_metadata, size), &(f->size), sizeof(f->size));
    memcpy(buf + offsetof(struct flow_metadata, tos),  &(f->tos), sizeof(f->tos));
    memcpy(buf + offsetof(struct flow_metadata, rate), &(f->rate), sizeof(f->rate));

    /* write the request into the socket */
    if (write_exact(fd, buf, TG_METADATA_SIZE, TG_METADATA_SIZE, 0, f->tos, 0, false) == TG_METADATA_SIZE)
        return true;
    else
        return false;
}

/* write a flow (response) into a socket and return true if it succeeds */
bool write_flow(int fd, struct flow_metadata *f, unsigned int sleep_overhead_us)
{
    char *write_buf = NULL;  /* buffer to hold the real content of the flow */
    unsigned int max_per_write = 0;
    unsigned int result = 0;

    if (!f)
        return false;

    /* echo back metadata */
    if (!write_flow_req(fd, f))
    {
        printf("Error: write_flow_req() in write_flow()\n");
        return false;
    }

    /* use min_write_buf with rate limiting */
    if (f->rate > 0)
    {
        write_buf = min_write_buf;
        max_per_write = TG_MIN_WRITE;
    }
    /* use max_write_buf w/o rate limiting */
    else
    {
        write_buf = max_write_buf;
        max_per_write = TG_MAX_WRITE;
    }

    /* generate the flow response */
    result = write_exact(fd, write_buf, f->size, max_per_write, f->rate, f->tos, sleep_overhead_us, true);
    if (result == f->size)
        return true;
    else
    {
        printf("Error: write_exact() in write_flow() only successfully writes %u of %u bytes.\n", result, f->size);
        return false;
    }
}

/* print error information */
void error(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/* remove '\r' and '\n' from a string */
void remove_newline(char *str)
{
    int i = 0;

    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\r' || str[i] == '\n')
            str[i] = '\0';
    }
}

/* generate poission process arrival interval */
double poission_gen_interval(double avg_rate)
{
    if (avg_rate > 0)
        return -logf(1.0 - (double)(rand() % RAND_MAX) / RAND_MAX) / avg_rate;
    else
        return 0;
}

//generate uniform process arrival interval
double uniform_gen_interval(double period_us)
{
    return period_us;
}

/* calculate usleep overhead */
unsigned int get_usleep_overhead(int iter_num)
{
    int i=0;
    unsigned int tot_sleep_us = 0;
    struct timeval tv_start, tv_end;

    if (iter_num <= 0)
        return 0;

    gettimeofday(&tv_start, NULL);
    for(i = 0; i < iter_num; i ++)
        usleep(0);
    gettimeofday(&tv_end, NULL);
    tot_sleep_us = (tv_end.tv_sec - tv_start.tv_sec) * 1000000 + tv_end.tv_usec - tv_start.tv_usec;

    return tot_sleep_us/iter_num;
}

/* mark dscp for each flow according to flow size*/
unsigned int gen_dscp_weight(double flowsize, unsigned int len) {
	if (len == 3) {
		if (flowsize < 20000)
			return 6;
		else if (flowsize < 200000)
			return 5;
		else
			return 4;
	}
	if (len == 6) {
		if (flowsize < 16000)
			return 6;
		else if (flowsize < 20000)
			return 5;
		else if (flowsize < 50000)
			return 4;
		else if (flowsize < 200000)
			return 3;
		else if (flowsize < 2000000)
			return 2;
		else
			return 1;
	}
}

/* randomly generate value based on weights */
unsigned int gen_value_weight(unsigned int *vals, unsigned int *weights, unsigned int len, unsigned int weight_total)
{
    unsigned int i = 0;
    unsigned int val = rand() % weight_total;

    for (i = 0; i < len; i++)
    {
        if (val < weights[i])
            return vals[i];
        else
            val -= weights[i];
    }

    return vals[len - 1];
}

/* display progress */
void display_progress(unsigned int num_finished, unsigned int num_total)
{
    if (num_total == 0)
        return;

    printf("Generate %u / %u (%.1f%%) requests\r", num_finished, num_total, (num_finished * 100.0) / num_total);
    fflush(stdout);
}
