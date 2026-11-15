#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/utsname.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

typedef struct {
  char SysName[65];
  char NodeName[65];
  char Release[65];
  char Version[65];
  char Machine[65];
  char DomainName[65];
} MrowUtsname;

void MrowSyscallUname(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t BufferAddress;
  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &BufferAddress);

  struct utsname HostUname;

  if (uname(&HostUname) < 0) {
    uint64_t ReturnValue = (uint64_t)(int64_t)-errno;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

  MrowUtsname GuestUname = {0};

  // Copy fields with bounded lengths into the guest structure.
  snprintf(GuestUname.SysName, sizeof(GuestUname.SysName), "%s",
           HostUname.sysname);
  snprintf(GuestUname.NodeName, sizeof(GuestUname.NodeName), "%s",
           HostUname.nodename);
  snprintf(GuestUname.Release, sizeof(GuestUname.Release), "%s",
           HostUname.release);
  snprintf(GuestUname.Version, sizeof(GuestUname.Version), "%s",
           HostUname.version);
  snprintf(GuestUname.Machine, sizeof(GuestUname.Machine), "%s",
           HostUname.machine);
  snprintf(GuestUname.DomainName, sizeof(GuestUname.DomainName), "%s",
           HostUname.__domainname);

  if (uc_mem_write(Unicorn, BufferAddress, &GuestUname, sizeof(GuestUname)) !=
      UC_ERR_OK) {
    uint64_t ReturnValue = (uint64_t)(int64_t)-EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

  uint64_t ReturnValue = 0;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);

  // TODO: Report guest kernel/system information rather than Mrow's host
  // information.
}
