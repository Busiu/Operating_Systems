#ifndef NAGLOWKOWY_H
#define NAGLOWKOWY_H

#define maxBufforSize 32
#define maxContentSize 256
#define maxLineSize 64
#define maxNumberOfClients 16
#define maxSizeOfQueue 10

#define serverQueue "/serverQueue"

struct order{
	long type;
	char value[maxContentSize];
	int pid;
	int where;
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
