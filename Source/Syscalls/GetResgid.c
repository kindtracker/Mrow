#define _GNU_SOURCE
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallGetResgid(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t RealGidAddress;
  uint64_t EffectiveGidAddress;
  uint64_t SavedGidAddress;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &RealGidAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &EffectiveGidAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &SavedGidAddress);

  gid_t RealGid;
  gid_t EffectiveGid;
  gid_t SavedGid;

  int Result = getresgid(&RealGid, &EffectiveGid, &SavedGid);

  if (Result < 0) {
    Result = -errno;
    goto Return;
  }

  uint32_t GuestRealGid = (uint32_t)RealGid;
  uint32_t GuestEffectiveGid = (uint32_t)EffectiveGid;
  uint32_t GuestSavedGid = (uint32_t)SavedGid;

  if (RealGidAddress && uc_mem_write(Unicorn, RealGidAddress, &GuestRealGid,
                                     sizeof(GuestRealGid)) != UC_ERR_OK) {
    Result = -EFAULT;
    goto Return;
  }

  if (EffectiveGidAddress &&
      uc_mem_write(Unicorn, EffectiveGidAddress, &GuestEffectiveGid,
                   sizeof(GuestEffectiveGid)) != UC_ERR_OK) {
    Result = -EFAULT;
    goto Return;
  }

  if (SavedGidAddress && uc_mem_write(Unicorn, SavedGidAddress, &GuestSavedGid,
                                      sizeof(GuestSavedGid)) != UC_ERR_OK) {
    Result = -EFAULT;
    goto Return;
  }

  Result = 0;

Return:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)Result;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
  }

  // TODO: Return the guest process's credentials instead of Mrow's host
  // credentials.
}
