#ifndef NAGLOWKOWY_H
#define NAGLOWKOWY_H

#define maxBufforSize 32
#define maxContentSize 256
#define maxLineSize 64
#define maxNumberOfClients 16

struct order{
	long type;
	char value[maxContentSize];
	int qid;
	int pid;
};

typedef int bool;
enum {false, true};

typedef enum ilkOfOrder{
	NEW = 10,
	TIME = 11,
	END = 12,
	MIRROR = 13,
	CALC = 14
} ilkOfOrder;

#endif
