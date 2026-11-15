#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallSetUid(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t Uid;
  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Uid);

  int Result = setuid((uid_t)Uid);

  if (Result < 0) {
    Result = -errno;
  }

  uint64_t ReturnValue = (uint64_t)(int64_t)Result;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);

  // TODO: Apply the UID to the guest process instead of changing Mrow's host
  // UID.
}
