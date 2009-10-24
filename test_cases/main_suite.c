#include "global.h"
#include "queues.h"
#include "queue_test.h"


int main() {
  if (queue_test()) { printf("QUEUE TESTS PASSED\n"); }
  else 
    printf("BAD!!\n");
  
return 0;
}
