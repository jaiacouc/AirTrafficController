// This will manage the takeoff que for the planes need to add thread

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "que.h"
#include "alist.h"
#include "airs_protocol.h"

static alist plane_que;

static pthread_rwlock_t quelock;
// Initialize que
void que_init(void){
    alist_init(&plane_que, free);
    pthread_rwlock_init(&quelock, NULL);
}

// Add plane name into the que
void que_add(airplane *plane, char *name){
    char * plane_name = (char *) malloc(sizeof(name));
    strcpy(plane_name, name);

    pthread_rwlock_wrlock(&quelock);
    alist_add(&plane_que, plane_name);
    pthread_rwlock_unlock(&quelock);

    if(alist_size(&plane_que) == 1){
        plane->state = 4;
        send_ok(plane);
        fprintf(plane->fp_send,"TAKEOFF\n");
        return;
    }
    send_ok(plane);
}

// Remove plane from que
void que_remove(char *name){
    pthread_rwlock_wrlock(&quelock);
    for (int i=0; i<alist_size(&plane_que); i++) {
        if (strcmp(alist_get(&plane_que, i), name) == 0) {
            alist_remove(&plane_que, i);
            pthread_rwlock_unlock(&quelock);
            return;
        }
    }
    pthread_rwlock_unlock(&quelock);
}


// Find position of given plane
void que_pos(airplane *plane, char *name){
    pthread_rwlock_wrlock(&quelock);
    for(int i=0; i<alist_size(&plane_que); i++){
        if(strcmp(name, alist_get(&plane_que, i)) == 0){
            fprintf(plane->fp_send,"OK: %d\n", i+1);
            pthread_rwlock_unlock(&quelock);
            return;
        }
    }
    pthread_rwlock_unlock(&quelock);
}

// Print the planes within the que
void que_print(airplane *plane, char *name){
    pthread_rwlock_wrlock(&quelock);
    for(int i=0; i<alist_size(&plane_que); i++){
        if(strcmp(name, alist_get(&plane_que, i)) != 0){
            fprintf(plane->fp_send,"%s, ", (char *)alist_get(&plane_que, i));
            fflush(plane->fp_send);
        } else{
            pthread_rwlock_unlock(&quelock);
            return;
        }
    }
    pthread_rwlock_unlock(&quelock);
}

// Find the next plane within the que
void *que_next(airplane *plane, int index){
    return(alist_get(&plane_que,index));
    }

// Get size of the que
int que_size(){
    return(alist_size(&plane_que));
}

// Find the index given a plane
int que_index(char *name){
    pthread_rwlock_wrlock(&quelock);
    for(int i=0; i<alist_size(&plane_que); i++){
        if(strcmp(name, alist_get(&plane_que, i)) == 0){
            pthread_rwlock_unlock(&quelock);
            return i;
        }
    }
    pthread_rwlock_unlock(&quelock);
    return -1;
}
