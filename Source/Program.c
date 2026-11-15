#include <string.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

MrowProgram *MrowCreateProgram(void) {
  MrowProgram *Program = Malloc(sizeof(MrowProgram));
  memset(Program, 0, sizeof(MrowProgram));

  uc_engine *Unicorn;
  UcError = uc_open(UC_ARCH_ARM64, UC_MODE_ARM, &Unicorn);
  if (UcError != UC_ERR_OK) {
    Crash("Failed to create unicorn engine: %s", uc_strerror(UcError));
  }

  Program->Unicorn = Unicorn;

  return Program;
}
