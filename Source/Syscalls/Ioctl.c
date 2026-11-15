#define _GNU_SOURCE
#include <errno.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <unicorn/unicorn.h>

#include "Mrow.h"

typedef struct {
  uint16_t Rows;
  uint16_t Cols;
  uint16_t XPixel;
  uint16_t YPixel;
} MrowWinSize;

void MrowSyscallIoctl(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t Fd;
  uint64_t Request;
  uint64_t Address;

  if (uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Fd) != UC_ERR_OK ||
      uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Request) != UC_ERR_OK ||
      uc_reg_read(Unicorn, UC_ARM64_REG_X2, &Address) != UC_ERR_OK) {
    return;
  }

  int Result = -1;

  switch (Request) {
  case TCGETS: {
    struct termios Termios;

    if (ioctl((int)Fd, TCGETS, &Termios) == 0) {
      // TODO: Convert host termios to the AArch64 Linux termios layout.
      UcError = uc_mem_write(Unicorn, Address, &Termios, sizeof(Termios));

      if (UcError != UC_ERR_OK) {
        Result = -EFAULT;
        break;
      }

      Result = 0;
    }
    break;
  }

  case TCSETS:
  case TCSETSW:
  case TCSETSF: {
    struct termios Termios;

    if (uc_mem_read(Unicorn, Address, &Termios, sizeof(Termios)) != UC_ERR_OK) {
      Result = -EFAULT;
      break;
    }

    // TODO: Convert guest termios to the host termios layout.
    Result = ioctl((int)Fd, Request, &Termios);
    break;
  }

  case TIOCGWINSZ: {
    struct winsize HostSize;

    if (ioctl((int)Fd, TIOCGWINSZ, &HostSize) == 0) {
      MrowWinSize GuestSize = {.Rows = HostSize.ws_row,
                               .Cols = HostSize.ws_col,
                               .XPixel = HostSize.ws_xpixel,
                               .YPixel = HostSize.ws_ypixel};

      UcError = uc_mem_write(Unicorn, Address, &GuestSize, sizeof(GuestSize));

      Result = UcError == UC_ERR_OK ? 0 : -EFAULT;
    }
    break;
  }

  case TIOCSWINSZ: {
    MrowWinSize GuestSize;

    if (uc_mem_read(Unicorn, Address, &GuestSize, sizeof(GuestSize)) !=
        UC_ERR_OK) {
      Result = -EFAULT;
      break;
    }

    struct winsize HostSize = {.ws_row = GuestSize.Rows,
                               .ws_col = GuestSize.Cols,
                               .ws_xpixel = GuestSize.XPixel,
                               .ws_ypixel = GuestSize.YPixel};

    Result = ioctl((int)Fd, TIOCSWINSZ, &HostSize);
    break;
  }

  default:
    // TODO: Implement additional ioctl requests as needed.
    Result = -ENOTTY;
    break;
  }

  if (Result < 0 && Result != -EFAULT && Result != -ENOTTY) {
    Result = -errno;
  }

  uint64_t ReturnValue = (uint64_t)(int64_t)Result;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
}
