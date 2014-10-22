#include <stdio.h>
#include "cgpm.h"

void main(int argc, char *argv[])
{
	int i;
	
  cgpm_set_task("myFunction");
	i=cgpm_choice();
  if(i == CPU)
  {
    //do cpu function
    
  }
  else if (i == GPU)
  {
    //do gpu function
    
  }
  else
  {
    // cgpm_choice return NOCPU or NOGPU
    // if task set NOCPU and NOGPU, cgpm select CPU default.
    // if you want to change that, type CPU or GPU instead return value.  
    cgpm_select(i);
    // do i function
    
    cgpm_update();
  }
	printf("%d\n", i);
	
	cgpm_close();
}

