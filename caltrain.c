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
    //TODO only one train!! no need to mutex protect train capacity
    station->train_capacity = count;
    //printf("Hello I am a train with capacity %d\n", station->train_capacity);
    pthread_cond_broadcast(&(station->train_arrival));

    pthread_mutex_lock(&(station->state_mutex));
    while((station->waiting_passengers > 0 /*there are people in the station*/
            && station->booked_passengers < station->train_capacity /*there is place in the train*/)
            || station->seated_passengers < station->booked_passengers /*not all booked have been seated*/){
        /*printf("Train with capacity %d must wait because waiting = %d booked = %d seated = %d\n",
               station->train_capacity,station->waiting_passengers,
               station->booked_passengers, station->seated_passengers);*/
        pthread_cond_wait(&(station->passenger_seated), &(station->state_mutex));
    }
    station->train_capacity = -1; //must be inside mutex because otherwise a passenger may read it as positive before resetting
    pthread_mutex_unlock(&(station->state_mutex));

    /*reset the state, doesn't have to be inside mutex because any +ve value > 0 > -1 anyway*/
    station->booked_passengers = 0;
    station->seated_passengers = 0;
    //printf("Train with capacity %d leaving\n", count);
    /*waiting passengers must remain as it is*/
}

void
station_wait_for_train(struct station *station)
{
	pthread_mutex_lock(&(station->state_mutex));
	//printf("passenger arrived\n");
	station->waiting_passengers++;
	while(station->booked_passengers >= station->train_capacity /*no train or there is a train but no place*/){
        /*printf("passenger will sleep because capacity = %d booked = %d waiting = %d seated = %d\n",
               station->train_capacity, station->booked_passengers,
               station->waiting_passengers, station->seated_passengers);*/
	    pthread_cond_wait(&(station->train_arrival), &(station->state_mutex));
	    /*printf("passenger woke up to find capacity = %d booked = %d waiting = %d seated = %d\n",
               station->train_capacity, station->booked_passengers,
               station->waiting_passengers, station->seated_passengers);*/
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
