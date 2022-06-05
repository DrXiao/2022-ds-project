#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "csv.h"
#define P1_FORMAT "%8s, %lf, %lf, %lf, %lf"
#define P1_FIELDS_IN(p1) p1.date, &p1.o_price, &p1.h_price, &p1.l_price, &p1.c_price
#define P1_FIELDS_OUT(p1) p1.date, p1.o_price, p1.h_price, p1.l_price, p1.c_price
#define P2_FORMAT_IN  "%8s , %3s , %lf , %6s , %c , %6s , %lf , %u , %*u"
#define P2_FORMAT_OUT "%s, %s, %lf, %s, %c, %s, %lf, %u"
#define P2_FIELDS_IN(p2) p2.date, p2.item_id, &p2.item_price, p2.deadline, &p2.auth, p2.deal_time, &p2.deal_price, &p2.dead_num
#define P2_FIELDS_OUT(p2) p2.date, p2.item_id, p2.item_price, p2.deadline, p2.auth, p2.deal_time, p2.deal_price, p2.dead_num
#define CSV_DELIMITER ","
#define BUF_SIZE 512

static void csv_read(csv *obj, char *filename);
static void csv_getrow(csv *obj, uint32_t idx);
static void csv_sort(csv *obj);
static void csv_destroy(csv *obj);
static void csv_read_p1(csv *obj, FILE *file);
static void csv_read_p2(csv *obj, FILE *file);


typedef struct p1_csv {
	char date[16];
	double o_price;
	double h_price;
	double l_price;
	double c_price;
} p1_csv;

typedef struct p2_csv {
	char date[16];
	char item_id[4];
	double item_price;
	char deadline[7];
	char auth;
	char deal_time[8];
	double deal_price;
	uint32_t dead_num;
} p2_csv;

csv csv_init(uint32_t idx) {
	static void (*csv_reads[])(csv *, FILE *) = {
		NULL, csv_read_p1, csv_read_p2
	};

	csv obj = {
		.readcsv = csv_read,
		.getrow = csv_getrow,
		.sort = csv_sort
		.destroy = csv_destroy,
		.__readcsv = csv_reads[idx],
		.__row = 0,
		.__col = 0,
		.__content = NULL
	};
	return obj;
}

static void csv_read(csv *obj, char *filename) {
	char buf[BUF_SIZE];
	char *token = NULL;
	FILE *file = fopen(filename, "r");
	int col = 0;
	fgets(buf, sizeof(buf), file);
	
	token = strtok(buf, CSV_DELIMITER);
	while(token) { 
		col++;
		token = strtok(NULL, CSV_DELIMITER);
	}
	obj->__col = col;

	obj->__readcsv(obj, file);

	fclose(file);
}

static void csv_sort(csv *obj) {

}

static void csv_destroy(csv *obj) {
	for (int i = 0; i < obj->__row; i++) {
		free(obj->__content[i]);
	}
	free(obj->__content);
}


#define MAX_ALLOC_SIZE 320000

static void csv_read_p1(csv *obj, FILE *file) {
	char buf[BUF_SIZE];
	char *token = NULL;
	
	p1_csv p1;
	obj->__content = malloc(sizeof(void *) * MAX_ALLOC_SIZE);
	while(fgets(buf, sizeof(buf), file)) {
		sscanf(buf, P1_FORMAT, P1_FIELDS_IN(p1));
		// printf(P1_FORMAT"\n", P1_FIELDS_OUT(p1));
		obj->__content[obj->__row] = malloc(sizeof(p1_csv));
		memcpy(obj->__content[obj->__row], &p1, sizeof(p1_csv));
		obj->__row++;
	}
	obj->__content = realloc(sizeof(void *) * obj->__row);
}

static void csv_read_p2(csv *obj, FILE *file) {
	char buf[BUF_SIZE];
	char *token = NULL;
	fgets(buf, sizeof(buf), file);
	p2_csv p2;
	obj->__content = malloc(sizeof(void *) * MAX_ALLOC_SIZE);
	while(fgets(buf, sizeof(buf), file)) {
		sscanf(buf, P2_FORMAT_IN, P2_FIELDS_IN(p2));
		// printf(P2_FORMAT_OUT"\n", P2_FIELDS_OUT(p2));
		obj->__content[obj->__row] = malloc(sizeof(p2_csv));
		memcpy(obj->__content[obj->__row], &p2, sizeof(p2_csv));
		obj->__row++;
	}
	obj->__content = realloc(sizeof(void *) * obj->__row);
}

