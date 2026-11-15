#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallOpenat(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t DirFd;
  uint64_t PathAddress;
  uint64_t Flags;
  uint64_t Mode;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &DirFd);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &PathAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &Flags);
  uc_reg_read(Unicorn, UC_ARM64_REG_X3, &Mode);

  char Path[4096];
  size_t Length;

  for (Length = 0; Length < sizeof(Path); Length++) {
    if (uc_mem_read(Unicorn, PathAddress + Length, &Path[Length], 1) !=
        UC_ERR_OK) {
      goto Fault;
    }

    if (Path[Length] == '\0') {
      break;
    }
  }

  if (Length == sizeof(Path)) {
    goto NameTooLong;
  }

  int HostFd = openat((int)(int64_t)DirFd, Path, (int)Flags, (mode_t)Mode);

  int Result = HostFd < 0 ? -errno : HostFd;
  goto Return;

Fault:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

NameTooLong:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-ENAMETOOLONG;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

Return:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)Result;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
  }

  // TODO: Allocate a guest file descriptor and map it to HostFd.
  // TODO: Restrict paths to the guest filesystem.
}
