#include "pintos_thread.h"

struct station {
	int empty_seats,pas_waiting,pas_boarding;
	struct lock lock;
	struct condition tr_wait;
	struct condition tr_ready;
};

void
station_init(struct station *station)
{
	station->empty_seats=0;
	station->pas_waiting=0;
	station->pas_boarding=0;
	lock_init(&station->lock);
	cond_init(&station->tr_wait);
	cond_init(&station->tr_ready);
}

void
station_load_train(struct station *station, int count)
{
	lock_acquire(&station->lock);
	station->empty_seats=count;
	station->pas_boarding=0;
	cond_broadcast(&station->tr_wait,&station->lock);
	while((station->empty_seats>0&&station->pas_waiting>0)||station->pas_boarding>0){
		cond_wait(&station->tr_ready,&station->lock);
	}
	station->empty_seats=0;
	station->pas_boarding=0;
	lock_release(&station->lock);
}

void
station_wait_for_train(struct station *station)
{
	lock_acquire(&station->lock);
	station->pas_waiting++;
	while(station->pas_boarding==station->empty_seats){
		cond_wait(&station->tr_wait,&station->lock);
	}
	station->pas_boarding++;
	station->pas_waiting--;
	lock_release(&station->lock);
}

void
station_on_board(struct station *station)
{
	lock_acquire(&station->lock);
	station->pas_boarding--;
	station->empty_seats--;
	if((station->empty_seats==0)||(station->pas_boarding==0&&station->pas_waiting==0)){
		cond_signal(&station->tr_ready,&station->lock);
	}
	lock_release(&station->lock);
}
