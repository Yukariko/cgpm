#ifndef CGPM_H
#define CGPM_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <memory.h>

#define CGPM_DEFAULT_UNIT 100
#define MAX_NAME_LEN 128
#define TRUE 1
#define FALSE 0
#define NULLPTR 0

#define CPU 1
#define GPU 2
#define NOCPU 3
#define NOGPU 4

static const int true = 1;
static const int false = 0;
static const int nullptr = 0;

static const char *cgpm_table_path = "cgpu_record.tbl";

typedef struct CGPM_RECORD {
  char name[MAX_NAME_LEN];
  int isTryCpu;
  int isTryGpu;
  int cpuVal;
  int gpuVal;
} CGPM_RECORD;

typedef struct CGPM {
  FILE *fp;
  CGPM_RECORD *records;
  int recordCnt;
  int recordSize;
  CGPM_RECORD *curRecord;
  int curSel;
  int curVal;
} CGPM;

CGPM *cgpm = nullptr;

void cgpm_init();
void cgpm_close();
void cgpm_create_records();
void cgpm_load_records();
void cgpm_store_records();
void cgpm_alloc_records(int size);
void cgpm_realloc_records();

CGPM_RECORD *cgpm_add_record(char *task);
CGPM_RECORD *cgpm_find_record(char *task);
int cgpm_compare_record(int cpuVal, int gpuVal);

void cgpm_select(int option);
void cgpm_update();
void cgpm_set_task(char *task);
int cgpm_choice();

#endif
