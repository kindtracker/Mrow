#include "Mrow.h"

void MrowSyscall(uc_engine *Unicorn) {
  uint64_t Number;
  uint64_t R0;
  uint64_t R1;
  uint64_t R2;
  uint64_t R3;

  uc_reg_read(uc, UC_ARM64_REG_X8, &Number);
  uc_reg_read(uc, UC_ARM64_REG_X0, &R0);
  uc_reg_read(uc, UC_ARM64_REG_X1, &R1);
  uc_reg_read(uc, UC_ARM64_REG_X2, &R2);
  uc_reg_read(uc, UC_ARM64_REG_X3, &R3);

  switch (Number) {}
}
