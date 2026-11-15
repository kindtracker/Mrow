#include <string.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

MrowProgram *MrowCreateProgram(void) {
  MrowProgram *Program = Malloc(sizeof(MrowProgram));
  memset(Program, 0, sizeof(MrowProgram));

  uc_engine *Unicorn;
  uc_err Error;
  Error = uc_open(UC_ARCH_ARM64, UC_MODE_64, &Unicorn);
  if (Error != UC_ERR_OK) {
    Crash("Failed to create unicorn engine: %s", uc_strerror(Error));
  }

  Program->Unicorn = Unicorn;

  return Program;
}
