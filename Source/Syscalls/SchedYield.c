#include <sched.h>
#include <stdint.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

void MrowSyscallSchedYield(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  sched_yield();

  uint64_t Result = 0;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);

  // TODO: Implement guest-thread scheduling.
}
