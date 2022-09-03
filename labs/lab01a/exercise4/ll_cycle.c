#include "ll_cycle.h"
#include <stddef.h>

int ll_has_cycle(node *head) {
  node *fast = head, *slow = head;
  do {
    if (!fast || !fast->next)
      break;
    fast = fast->next->next;
    slow = slow->next;

    if (fast == slow)
      return 1;
  } while (fast != slow);
  return 0;
}
