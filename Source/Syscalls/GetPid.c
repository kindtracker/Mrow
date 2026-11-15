#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallGetPid(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t ReturnValue = (uint64_t)getpid();
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);

  // TODO: Return the guest process ID instead of Mrow's host process ID.
}
