#include "Mrow.h"

#include <errno.h>
#include <stdint.h>

void MrowSyscallPrlimit64(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Pid;
  uint64_t Resource;
  uint64_t NewLimit;
  uint64_t OldLimit;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Pid);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Resource);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &NewLimit);
  uc_reg_read(Unicorn, UC_ARM64_REG_X3, &OldLimit);

  // TODO: Validate Pid.
  // TODO: Support Pid == 0 (current process).
  // TODO: Validate Resource.
  // TODO: Implement resource limit storage in MrowProcess.
  // TODO: Read NewLimit from guest memory if non-NULL.
  // TODO: Write the previous limit to OldLimit if non-NULL.
  // TODO: Validate guest memory addresses.
  // TODO: Return -EINVAL for invalid resources or limits.
  // TODO: Return -ESRCH if the requested process doesn't exist.
  // TODO: Return -EPERM when changing another process's limits isn't permitted.

  (void)Process;
  (void)Pid;
  (void)Resource;
  (void)NewLimit;
  (void)OldLimit;

  uint64_t Result = (uint64_t)-ENOSYS;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}
