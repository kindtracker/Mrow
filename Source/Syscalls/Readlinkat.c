#include "Mrow.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define MROW_MAX_PATH_SIZE 4096
#define MROW_MAX_READLINK_SIZE (1024 * 1024)

void MrowSyscallReadlinkat(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t DirFd;
  uint64_t PathAddress;
  uint64_t BufferAddress;
  uint64_t BufferSize;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &DirFd);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &PathAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &BufferAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X3, &BufferSize);

  // TODO: Translate guest DirFd into a host file descriptor.
  // TODO: Resolve guest paths relative to Process's filesystem root.
  // TODO: Enforce filesystem isolation.
  // TODO: Validate guest memory permissions.
  // TODO: Handle guest pathname strings safely.
  // TODO: Return -EFAULT for invalid guest memory.
  // TODO: Return -ENAMETOOLONG for excessively long paths.

  (void)Process;

  if (BufferSize > MROW_MAX_READLINK_SIZE) {
    uint64_t Result = (uint64_t)-EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  char Path[MROW_MAX_PATH_SIZE];

  if (uc_mem_read(Unicorn, PathAddress, Path, 1) != UC_ERR_OK) {
    uint64_t Result = (uint64_t)-EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  size_t PathLength = 0;

  for (; PathLength < sizeof(Path); PathLength++) {
    if (uc_mem_read(Unicorn, PathAddress + PathLength, &Path[PathLength], 1) !=
        UC_ERR_OK) {
      uint64_t Result = (uint64_t)-EFAULT;
      uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
      return;
    }

    if (Path[PathLength] == '\0')
      break;
  }

  if (PathLength == sizeof(Path)) {
    uint64_t Result = (uint64_t)-ENAMETOOLONG;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  if (BufferSize == 0) {
    uint64_t Result = (uint64_t)-EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  char *Buffer = Malloc(BufferSize);

  if (!Buffer) {
    uint64_t Result = (uint64_t)-ENOMEM;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  ssize_t Read = readlinkat((int)DirFd, Path, Buffer, BufferSize);
  int ErrorNumber = errno;

  if (Read < 0) {
    Free(Buffer);

    uint64_t Result = (uint64_t)-ErrorNumber;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  uc_err Error = uc_mem_write(Unicorn, BufferAddress, Buffer, Read);

  Free(Buffer);

  if (Error != UC_ERR_OK) {
    uint64_t Result = (uint64_t)-EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  uint64_t Result = (uint64_t)Read;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}
