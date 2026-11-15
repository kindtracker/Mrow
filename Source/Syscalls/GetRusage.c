#define _GNU_SOURCE
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>

#include "Mrow.h"

typedef struct {
  int64_t TvSec;
  int64_t TvUsec;
} MrowTimeval;

typedef struct {
  MrowTimeval UTime;
  MrowTimeval STime;
  int64_t MaxRss;
  int64_t IxRss;
  int64_t IdRss;
  int64_t IsRss;
  int64_t MinFlt;
  int64_t MajFlt;
  int64_t NSwap;
  int64_t InBlock;
  int64_t OuBlock;
  int64_t MsgSnd;
  int64_t MsgRcv;
  int64_t NSignals;
  int64_t NVCSw;
  int64_t NIVCSw;
} MrowRusage;

void MrowSyscallGetRusage(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Who;
  uint64_t Address;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Who);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Address);

  (void)Process;

  struct rusage Usage = {0};

  if ((int64_t)Who != RUSAGE_SELF && (int64_t)Who != RUSAGE_CHILDREN &&
      (int64_t)Who != RUSAGE_THREAD) {
    uint64_t Result = -EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  if (getrusage((int)Who, &Usage) != 0) {
    uint64_t Result = -(int64_t)errno;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  MrowRusage GuestUsage = {.UTime = {.TvSec = Usage.ru_utime.tv_sec,
                                     .TvUsec = Usage.ru_utime.tv_usec},
                           .STime = {.TvSec = Usage.ru_stime.tv_sec,
                                     .TvUsec = Usage.ru_stime.tv_usec},
                           .MaxRss = Usage.ru_maxrss,
                           .IxRss = Usage.ru_ixrss,
                           .IdRss = Usage.ru_idrss,
                           .IsRss = Usage.ru_isrss,
                           .MinFlt = Usage.ru_minflt,
                           .MajFlt = Usage.ru_majflt,
                           .NSwap = Usage.ru_nswap,
                           .InBlock = Usage.ru_inblock,
                           .OuBlock = Usage.ru_oublock,
                           .MsgSnd = Usage.ru_msgsnd,
                           .MsgRcv = Usage.ru_msgrcv,
                           .NSignals = Usage.ru_nsignals,
                           .NVCSw = Usage.ru_nvcsw,
                           .NIVCSw = Usage.ru_nivcsw};

  UcError = uc_mem_write(Unicorn, Address, &GuestUsage, sizeof(GuestUsage));

  uint64_t Result = UcError == UC_ERR_OK ? 0 : -(int64_t)EFAULT;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);

  // TODO: Track resource usage per emulated process.
  // TODO: Track resource usage per emulated thread for RUSAGE_THREAD.
  // TODO: Check uc_reg_read and uc_reg_write errors.
}
