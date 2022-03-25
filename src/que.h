// Functions of que

#ifndef _QUE_H
#define _QUE_H

#include "airplane.h"

void que_init(void);
void que_add(airplane *plane, char *name);
void que_remove(char *name);
void que_pos(airplane *plane, char *name);
void que_print(airplane *plane, char *name);
void *que_next(airplane *plane, int index);
int que_size();
int que_index(char *name);


#endif  // _QUE_H