typedef unsigned int uint32_t;

static inline int Syscall(int Number, uint32_t Arg0, uint32_t Arg1,
                          uint32_t Arg2, uint32_t Arg3, uint32_t Arg4,
                          uint32_t Arg5) {
  register int R0 asm("r0") = Arg0;
  register uint32_t R1 asm("r1") = Arg1;
  register uint32_t R2 asm("r2") = Arg2;
  register uint32_t R3 asm("r3") = Arg3;
  register uint32_t R4 asm("r4") = Arg4;
  register uint32_t R5 asm("r5") = Arg5;
  register int R7 asm("r7") = Number;

  asm volatile("svc #0"
               : "+r"(R0)
               : "r"(R1), "r"(R2), "r"(R3), "r"(R4), "r"(R5), "r"(R7)
               : "memory", "cc");

  return R0;
}

static inline void SysExit(int Status) {
  Syscall(93, Status, 0, 0, 0, 0, 0);
  __builtin_unreachable();
}

static inline int SysWrite(int Fd, const char *Buffer, unsigned int Length) {
  return Syscall(64, Fd, (uint32_t)Buffer, Length, 0, 0, 0);
}

void _start(void) {
  static const char Message[] = "Meow\n";

  SysWrite(1, Message, sizeof(Message) - 1);
  SysExit(0);
}
