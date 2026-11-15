#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallSetReuid(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t RealUid;
  uint64_t EffectiveUid;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &RealUid);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &EffectiveUid);

  int Result = setreuid((uid_t)(int64_t)RealUid, (uid_t)(int64_t)EffectiveUid);

  if (Result < 0) {
    Result = -errno;
  }

  uint64_t ReturnValue = (uint64_t)(int64_t)Result;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);

  // TODO: Apply the UID changes to the guest process instead of Mrow's host
  // process.
}
