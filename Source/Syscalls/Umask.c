#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallUmask(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t Mask;
  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Mask);

  mode_t PreviousMask = umask((mode_t)Mask);

  uint64_t ReturnValue = (uint64_t)PreviousMask;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);

  // TODO: Store the umask in MrowProcess instead of changing Mrow's host umask.
}
