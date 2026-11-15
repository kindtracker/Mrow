#include <string.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

MrowProcess *MrowCreateProcess(void) {
  MrowProcess *Process = Malloc(sizeof(MrowProcess));
  memset(Process, 0, sizeof(MrowProcess));

  uc_engine *Unicorn;
  UcError = uc_open(UC_ARCH_ARM64, UC_MODE_ARM, &Unicorn);
  if (UcError != UC_ERR_OK) {
    Crash("Failed to create unicorn engine: %s", uc_strerror(UcError));
  }

  Process->Unicorn = Unicorn;

  return Process;
}
