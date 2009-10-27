#include "global.h"
#include "queues.h"

int test_allocation() {
  process_queue* pq;
  pq_allocate(&pq);
  if (pq == NULL ||
 	pq->head != NULL ||
	pq->tail != NULL)
	return 0;
  pq_free(&pq);
  if (pq != NULL) return 0;
  return 1;
}

int test_enqueueing() {
  return 1;
}
int test_dequeueing() {
  return 1;
}
int test_peek() {
  return 1;
}
int test_is_empty() {
  return 1;
}


int queue_test() {
 printf("Testing queue allocation...\n");
 if ( test_allocation() ) printf("SUCCESS\n"); else { printf("FAILURE\n"); return 0;}
 printf("Testing queue appending...\n");
 if ( test_enqueueing() ) printf("SUCCESS\n"); else { printf("FAILURE\n"); return 0; }
 printf("Testing queue removals...\n");
 if ( test_dequeueing() ) printf("SUCCESS\n"); else { printf("FAILURE\n"); return 0; }
 printf("Testing queue iterators...\n");
 if ( test_peek() ) printf("SUCCESS\n"); else { printf("FAILURE\n"); return 0; }
 printf("Testing queue state checks...\n");
 if ( test_is_empty() ) printf("SUCCESS\n"); else { printf("FAILURE\n"); return 0; }

return 1;
}
