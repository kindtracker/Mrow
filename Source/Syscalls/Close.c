#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallClose(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t Fd;
  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Fd);

  int Result = close((int)Fd);

  if (Result < 0) {
    Result = -errno;
  }

  uint64_t ReturnValue = (uint64_t)(int64_t)Result;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);

  // TODO: Translate the guest file descriptor to its host descriptor.
  // TODO: Remove the guest descriptor from the process's descriptor table.
}
