#include <stdlib.h>

#include "Mrow.h"

inline void *Malloc(size_t Size) {
  void *Memory = malloc(Size);
  if (!Memory) {
    FatalCrash("Failed to allocate memory (%ld)", Size);
  }

  return Memory;
}

inline void Free(void *Pointer) { free(Pointer); }
