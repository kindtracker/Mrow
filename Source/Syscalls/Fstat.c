#include "Mrow.h"

#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

void MrowSyscallFstat(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Fd;
  uint64_t StatAddress;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Fd);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &StatAddress);

  // TODO: Translate the guest file descriptor into a host file descriptor.
  // TODO: Validate StatAddress and guest memory permissions.
  // TODO: Define an AArch64-compatible guest struct stat.
  // TODO: Convert the host struct stat into the guest struct stat.
  // TODO: Handle guest filesystem isolation.
  // TODO: Return -EBADF for invalid file descriptors.

  (void)Process;
  (void)Fd;
  (void)StatAddress;

  uint64_t Result = (uint64_t)-ENOSYS;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}
