#include "cgpm.h"

#define tr(x...) { printf("[%d:%s:%d] ", getpid(), __func__, __LINE__); printf(x); printf("\n"); }

// initialize

void cgpm_init()
{
  cgpm = (CGPM *)malloc(sizeof(CGPM));
  cgpm->fp = fopen(cgpm_table_path,"r");
  if(cgpm->fp == 0)
    cgpm_create_records();
  else
    cgpm_load_records();
  fclose(cgpm->fp);
  
  cgpm->curRecord = nullptr;
  cgpm->curSel = 0;
  cgpm->curVal = 0;
}

void cgpm_close()
{
  cgpm_store_records();
  free(cgpm->records);
  free(cgpm);
}

void cgpm_create_records()
{
  cgpm->fp = fopen(cgpm_table_path,"w");
  cgpm->recordCnt = 0;
  cgpm_alloc_records(CGPM_DEFAULT_UNIT);
}

void cgpm_load_records()
{
  int i,j;
  int recordCnt;
  
  fscanf(cgpm->fp,"%d ",&cgpm->recordCnt);
  recordCnt = cgpm->recordCnt;

  cgpm_alloc_records(cgpm->recordCnt / CGPM_DEFAULT_UNIT * CGPM_DEFAULT_UNIT + CGPM_DEFAULT_UNIT);
  
  for(i=0;i<recordCnt;i++)
  {
    CGPM_RECORD *tmp = &cgpm->records[i];
    fgets(cgpm->fp,MAX_NAME_LEN,tmp->name);
    fscanf(cgpm->fp,"%d",&tmp->elemCnt);
    cgpm_alloc_elements(tmp);
    for(j=0;j<tmp->elemCnt;j++)
    {
      CGPM_ELEMENT *element = &tmp->elem[j];
      fscanf(cgpm->fp," %d %d %d %d %d",&element->ident,&element->isTryCpu,&element->isTryGpu,&element->cpuVal,&element->gpuVal);
    }
  }
}

void cgpm_store_records()
{
  int i,j;
  int recordCnt = cgpm->recordCnt;
  
  cgpm->fp = fopen(cgpm_table_path,"w");

  fprintf(cgpm->fp,"%d\n",recordCnt);
  
  for(i=0;i<recordCnt;i++)
  {
    CGPM_RECORD *tmp = &cgpm->records[i];
    fprintf(cgpm->fp,"%s\n",tmp->name);
    fprintf(cgpm->fp,"%d\n",tmp->elemCnt);
    for(j=0;j<tmp->elemCnt;j++)
    {
      CGPM_ELEMENt *element = &tmp->elem[j];
      fprintf(cgpm->fp,"%d %d %d %d %d\n",element->isTryCpu,element->isTryGpu,element->cpuVal,element->gpuVal);
    }
  }
  
  fclose(cgpm->fp);
}

void cgpm_alloc_records(int size)
{
  cgpm->recordSize = size;
  cgpm->records = (CGPM_RECORD *)malloc(sizeof(CGPM_RECORD) * size);
}

void cgpm_alloc_elements(CGPM_RECORD *record)
{
  record->elem = (CGPM_ELEMENT *)malloc(sizeof(CGPM_ELEMENT) * record->elemCnt); 
}

void cgpm_realloc_records()
{
  int size = cgpm->recordSize + CGPM_DEFAULT_UNIT;
  cgpm->recordSize = size;
  cgpm->records = (CGPM_RECORD *)realloc(cgpm->records, sizeof(CGPM_RECORD) * size);
}

// interface

CGPM_RECORD *cgpm_add_record(char *task)
{
  int recordCnt = cgpm->recordCnt;
  strcpy(cgpm->records[recordCnt].name,task);
  cgpm->records[recordCnt].isTryCpu = false;
  cgpm->records[recordCnt].isTryGpu = false;
  cgpm->records[recordCnt].cpuVal = 0;
  cgpm->records[recordCnt].gpuVal = 0;
  cgpm->recordCnt++;
  if(cgpm->recordCnt >= cgpm->recordSize)
    cgpm_realloc_records();
  return &cgpm->records[recordCnt];
}

CGPM_RECORD *cgpm_find_record(char *task)
{
  int i;
  int recordCnt = cgpm->recordCnt;
  for(i=0;i<recordCnt;i++)
  {
    if(strcmp(cgpm->records[i].name,task) == 0)
    {
      return &cgpm->records[i];
    }
  }
  return nullptr;
}

int cgpm_compare_record(int cpuVal, int gpuVal)
{
  return cpuVal < gpuVal ? CPU : GPU;
}

void cgpm_select(int option)
{
  if(option == CPU || option == NOCPU)
  {
    cgpm->curSel = CPU;
    cgpm->curVal = (int)time(0);
  }
  else if(option == GPU || option == NOGPU)
  {
    cgpm->curSel = GPU;
    cgpm->curVal = (int)time(0);
  }
}

void cgpm_update()
{
  if(cgpm->curRecord == nullptr)
    return;
  if(cgpm->curSel != CPU && cgpm->curSel != GPU)
    return;
  if(cgpm->curSel == CPU)
  {
    cgpm->curRecord->isTryCpu = true;
    cgpm->curRecord->cpuVal = (int)time(0) - cgpm->curVal;
  }
  else if (cgpm->curSel == GPU)
  {
    cgpm->curRecord->isTryGpu = true;
    cgpm->curRecord->gpuVal = (int)time(0) - cgpm->curVal;    
  }
}

void cgpm_set_task(char *task)
{
  if(cgpm == nullptr)
  {
    cgpm_init();
  }
  cgpm->curRecord = cgpm_find_record(task);
  if(cgpm->curRecord == nullptr)
    cgpm->curRecord = cgpm_add_record(task);
  cgpm->curSel = 0;
  cgpm->curVal = 0;
}

int cgpm_choice()
{
  CGPM_RECORD *record = cgpm->curRecord;
  if(record == nullptr)
    return false;
  if(record->isTryCpu == false)
    return NOCPU;
  if(record->isTryGpu == false)
    return NOGPU;
  return cgpm_compare_record(record->cpuVal,record->gpuVal);
}
