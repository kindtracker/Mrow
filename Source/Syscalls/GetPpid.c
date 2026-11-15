#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallGetPpid(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t ReturnValue = (uint64_t)getppid();
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);

  // TODO: Return the guest parent process ID.
}
