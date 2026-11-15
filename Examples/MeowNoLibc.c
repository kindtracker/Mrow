typedef unsigned long uint64_t;

void _start(void) {
  static const char Message[] = "Meow\n";

  register uint64_t X0 asm("x0") = 1;
  register uint64_t X1 asm("x1") = (uint64_t)Message;
  register uint64_t X2 asm("x2") = sizeof(Message) - 1;
  register uint64_t X8 asm("x8") = 64;

  asm volatile("svc #0"
               : "+r"(X0)
               : "r"(X1), "r"(X2), "r"(X8)
               : "memory", "cc");

  X0 = 0;
  X8 = 93;

  asm volatile("svc #0" : : "r"(X0), "r"(X8) : "memory", "cc");

  __builtin_unreachable();
}
