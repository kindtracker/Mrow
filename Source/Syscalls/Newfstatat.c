#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

// AArch64 Linux kernel struct stat layout.
typedef struct {
  uint64_t Dev;
  uint64_t Ino;
  uint32_t Mode;
  uint32_t NLink;
  uint32_t Uid;
  uint32_t Gid;
  uint64_t RDev;
  uint64_t Pad1;
  int64_t Size;
  int32_t BlkSize;
  int32_t Pad2;
  int64_t Blocks;
  int64_t AtimeSec;
  uint64_t AtimeNsec;
  int64_t MtimeSec;
  uint64_t MtimeNsec;
  int64_t CtimeSec;
  uint64_t CtimeNsec;
  uint32_t Unused[2];
} MrowStat;

void MrowSyscallNewfstatat(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t DirFd;
  uint64_t PathAddress;
  uint64_t StatAddress;
  uint64_t Flags;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &DirFd);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &PathAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &StatAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X3, &Flags);

  (void)Process;

  char Path[4096];
  size_t PathLength = 0;

  for (; PathLength < sizeof(Path); PathLength++) {
    if (uc_mem_read(Unicorn, PathAddress + PathLength, &Path[PathLength], 1) !=
        UC_ERR_OK) {
      goto Fault;
    }

    if (Path[PathLength] == '\0')
      break;
  }

  if (PathLength == sizeof(Path)) {
    uint64_t Result = -ENAMETOOLONG;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  struct stat HostStat;

  // TODO: Translate DirFd through the guest file descriptor table.
  // TODO: Restrict filesystem access to Mrow's guest filesystem.
  // TODO: Validate supported flags against Linux AT_* flags.
  if (fstatat((int)DirFd, Path, &HostStat, (int)Flags) != 0) {
    uint64_t Result = -(int64_t)errno;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  MrowStat GuestStat = {.Dev = HostStat.st_dev,
                        .Ino = HostStat.st_ino,
                        .Mode = HostStat.st_mode,
                        .NLink = HostStat.st_nlink,
                        .Uid = HostStat.st_uid,
                        .Gid = HostStat.st_gid,
                        .RDev = HostStat.st_rdev,
                        .Size = HostStat.st_size,
                        .BlkSize = HostStat.st_blksize,
                        .Blocks = HostStat.st_blocks,
                        .AtimeSec = HostStat.st_atim.tv_sec,
                        .AtimeNsec = HostStat.st_atim.tv_nsec,
                        .MtimeSec = HostStat.st_mtim.tv_sec,
                        .MtimeNsec = HostStat.st_mtim.tv_nsec,
                        .CtimeSec = HostStat.st_ctim.tv_sec,
                        .CtimeNsec = HostStat.st_ctim.tv_nsec};

  UcError = uc_mem_write(Unicorn, StatAddress, &GuestStat, sizeof(GuestStat));

  {
    uint64_t Result = UcError == UC_ERR_OK ? 0 : -(int64_t)EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
  }

  // TODO: Use Process for guest filesystem and FD management.
  return;

Fault:
  {
    uint64_t Result = -EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
  }
}
