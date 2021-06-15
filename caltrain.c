#include <pthread.h>
#include "caltrain.h"
#include <stdio.h>



void
station_init(struct station *station)
{
    pthread_mutex_init(&(station->state_mutex),NULL);
    pthread_cond_init(&(station->train_arrival), NULL);
    pthread_cond_init(&(station->passenger_seated), NULL);
    station->waiting_passengers = 0;
    station->train_capacity = -1;
    station->booked_passengers = 0;
    station->seated_passengers = 0;
}

void
station_load_train(struct station *station, int count)
{
    //only one train!! no need to mutex protect train capacity
    station->train_capacity = count;
    pthread_cond_broadcast(&(station->train_arrival));

    pthread_mutex_lock(&(station->state_mutex));
    while((station->waiting_passengers > 0 /*there are people in the station*/
            && station->booked_passengers < station->train_capacity /*there is place in the train*/)
            || station->seated_passengers < station->booked_passengers /*not all booked have been seated*/){
        pthread_cond_wait(&(station->passenger_seated), &(station->state_mutex));
    }
    station->train_capacity = -1; //must be inside mutex because otherwise a passenger may read it as positive before resetting
    pthread_mutex_unlock(&(station->state_mutex));

    /*reset the state, doesn't have to be inside mutex because any +ve value > 0 > -1 anyway,
     * and no passenger can book or sit (i.e. modify these values) when there is no train*/
    station->booked_passengers = 0;
    station->seated_passengers = 0;
    /*station->waiting_passengers must remain as it is*/
}

void
station_wait_for_train(struct station *station)
{
	pthread_mutex_lock(&(station->state_mutex));
	//printf("passenger arrived\n");
	station->waiting_passengers++;
	while(station->booked_passengers >= station->train_capacity /*no train or there is a train but no place*/){
	    pthread_cond_wait(&(station->train_arrival), &(station->state_mutex));
	}
	station->waiting_passengers--;
	station->booked_passengers++;
    pthread_mutex_unlock(&(station->state_mutex));
}

void
station_on_board(struct station *station)
{
    pthread_mutex_lock(&(station->state_mutex));
    station->seated_passengers++;
    pthread_mutex_unlock(&(station->state_mutex));

    pthread_cond_signal(&(station->passenger_seated));
}
