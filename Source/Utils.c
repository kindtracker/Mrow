#include <stdlib.h>

#include "Mrow.h"

void *Malloc(size_t Size) {
  void *Memory = malloc(Size);
  if (!Memory) {
    FatalCrash("Failed to allocate memory (%ld)", Size);
  }

  return Memory;
}
