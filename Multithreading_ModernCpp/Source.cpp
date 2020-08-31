#include<iostream>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <Windows.h>//Sleep
#include <cerrno>

constexpr int BUFF_SIZE{ 128 };
static char buff[BUFF_SIZE]{};
constexpr int COUNT_START{ 5 };
constexpr int COUNT_LIMIT{ 10 };
static int count{};
int t_id[]{0,1,2};

pthread_mutex_t count_mutex{ PTHREAD_MUTEX_INITIALIZER };
pthread_cond_t count_cv{PTHREAD_COND_INITIALIZER};

void* increment_count(void* ptr) {
	int thread_id = *(static_cast<int*>(ptr));
	std::cout << __FUNCTION__ << ",thread: " << thread_id << std::endl;
	for (int i = 0; i < COUNT_START; i++) {
		pthread_mutex_lock(&count_mutex);
		count++;
		std::cout << __FUNCTION__ << ": "<< count << std::endl;
		if (count == COUNT_LIMIT) {
			pthread_cond_signal(&count_cv);
		}
		pthread_mutex_unlock(&count_mutex);
		Sleep(2000);
	}
	pthread_exit(0);
	return 0;
}

void* watch_count(void* ptr) {
	int thread_id = *(static_cast<int*>(ptr));
	pthread_mutex_lock(&count_mutex);
	std::cout << __FUNCTION__ << ",thread: " << thread_id << std::endl;
	if (count < COUNT_LIMIT) {
		pthread_cond_wait(&count_cv, &count_mutex);
	}
	std::cout << __FUNCTION__ << ", MAX count Reached"<< std::endl;
	pthread_mutex_unlock(&count_mutex);
	pthread_exit(0);
	return 0;
}

int main()
{
	pthread_t threads[3];
	pthread_attr_t thread_attr;

	pthread_mutex_init(&count_mutex, nullptr);
	pthread_cond_init(&count_cv, nullptr);
	pthread_attr_init(&thread_attr);

	if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE) != 0) {
		std::cout << "Error: while setdetach," << strerror_s(buff,errno) << std::endl;
	}

	pthread_create(&threads[0], &thread_attr, watch_count, (void*)&t_id[0]);
	pthread_create(&threads[1], &thread_attr, increment_count, (void*)&t_id[1]);
	pthread_create(&threads[2], &thread_attr, increment_count, (void*)&t_id[2]);

	for (int i = 0; i < 3;i++) {
		pthread_join(threads[i],nullptr);
	}

	pthread_attr_destroy(&thread_attr);
	pthread_mutex_destroy(&count_mutex);
	pthread_cond_destroy(&count_cv);
	return 0;
}