#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"

void MrowSyscall(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Number;
  uc_reg_read(Unicorn, UC_ARM64_REG_X8, &Number);

  switch (Number) {
  case 64:
    MrowSyscallWrite(Unicorn, Process);
    break;

  case 93:
    MrowSyscallExit(Unicorn, Process);
    break;

  case 214:
    MrowSyscallBrk(Unicorn, Process);
    break;

  case 222:
    MrowSyscallMmap(Unicorn, Process);
    break;

  case 94:
    MrowSyscallExitGroup(Unicorn, Process);
    break;

  case 96:
    MrowSyscallSetTidAddress(Unicorn, Process);
    break;

  case 99:
    MrowSyscallSetRobustList(Unicorn, Process);
    break;

  case 293:
    MrowSyscallRseq(Unicorn, Process);
    break;

  case 261:
    MrowSyscallPrlimit64(Unicorn, Process);
    break;

  case 278:
    MrowSyscallGetRandom(Unicorn, Process);
    break;

  case 78:
    MrowSyscallReadlinkat(Unicorn, Process);
    break;

  case 226:
    MrowSyscallMprotect(Unicorn, Process);
    break;

  case 80:
    MrowSyscallFstat(Unicorn, Process);
    break;

  case 174:
    MrowSyscallGetRusage(Unicorn, Process);
    break;

  case 79:
    MrowSyscallNewfstatat(Unicorn, Process);
    break;

  case 29:
    MrowSyscallIoctl(Unicorn, Process);
    break;

  case 24:
    MrowSyscallSchedYield(Unicorn, Process);
    break;

  case 176:
    MrowSyscallGetResgid(Unicorn, Process);
    break;

  case 144:
    MrowSyscallSetUid(Unicorn, Process);
    break;

  case 146:
    MrowSyscallSetReuid(Unicorn, Process);
    break;

  case 175:
    MrowSyscallUmask(Unicorn, Process);
    break;

  case 56:
    MrowSyscallOpenat(Unicorn, Process);
    break;

  case 63:
    MrowSyscallRead(Unicorn, Process);
    break;

  case 57:
    MrowSyscallClose(Unicorn, Process);
    break;

  case 172:
    MrowSyscallGetPid(Unicorn, Process);
    break;

  case 134:
    MrowSyscallRtSigaction(Unicorn, Process);
    break;

  case 173:
    MrowSyscallGetPpid(Unicorn, Process);
    break;

  case 160:
    MrowSyscallUname(Unicorn, Process);
    break;

  case 17:
    MrowSyscallGetCwd(Unicorn, Process);
    break;

  default: {
    Crash("Unknown svc: Number=%lu", Number);

    // uint64_t Result = (uint64_t)-ENOSYS;
    // uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    break;
  }
  }
}
