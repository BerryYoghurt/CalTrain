#include <pthread.h>

struct station {
	pthread_mutex_t state_mutex; //mutex to protect state variables
	pthread_cond_t passenger_seated; //condition variable to signal that passenger has been seated
	pthread_cond_t train_arrival;//signals that train has arrived
	int train_capacity;//capacity of current train
	int seated_passengers;//passengers already seated in train
	int booked_passengers;//passengers who have booked, whether seated or not
	int waiting_passengers;//passengers waiting for a train
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);