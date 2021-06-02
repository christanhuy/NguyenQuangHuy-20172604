#include "pintos_thread.h"

void make_water();

struct reaction {
	int h;
	struct lock lock;
	struct condition h_exist;
	struct condition o_exist;
};

void
reaction_init(struct reaction *reaction)
{
	reaction->h=0;
	lock_init(&reaction->lock);
	cond_init(&reaction->h_exist);
	cond_init(&reaction->o_exist);
}

void
reaction_h(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);
	reaction->h++;
	cond_signal(&reaction->h_exist,&reaction->lock);
	cond_wait(&reaction->o_exist,&reaction->lock);
	lock_release(&reaction->lock);
}

void
reaction_o(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);	
	while(reaction->h<2){
		cond_wait(&reaction->h_exist,&reaction->lock);
	}
	cond_signal(&reaction->o_exist,&reaction->lock);
	cond_signal(&reaction->o_exist,&reaction->lock);
	reaction->h -=2;
	make_water();
	lock_release(&reaction->lock);
}
