#include <elf.h>
#include <string.h>

#include "Mrow.h"

void MrowLoadProgram(char *FileContent, MrowProgram *Program) {
  Elf64_Ehdr *Header = (Elf64_Ehdr *)FileContent;

  if (strncmp("\127ELF", Header->e_ident, 4) != 0) {
    Crash("Invaild ELF (Magic number isn't ELF)");
  }

  if (Header->e_type != ET_EXEC) {
    Crash("Invaild ELF (ELF is not executable ELF)");
  }

  if (Header->e_machine != EM_AARCH64) {
    Crash("Invaild ELF (ELF is not for AArch64)");
  }

  if (Header->e_version != EV_CURRENT) {
    Crash("Invaild ELF (ELF's verison is not 1)");
  }

  if (Header->e_ehsize != 64) {
    Crash("Invaild ELF (ELF's e_ehsize is not 64)");
  }

  Program->Entry = Header->e_entry;

  Elf64_Phdr *Phdr = (Elf64_Phdr *)(FileContent + Header->e_phoff);
  Elf64_Shdr *Shdr = (Elf64_Shdr *)(FileContent + Header->e_shoff);
  Elf64_Half PhdrNum = Header->e_phnum;
  Elf64_Half ShdrNum = Header->e_shnum;
  Elf64_Half PhdrSize = Header->e_phentsize;
  Elf64_Half ShdrSize = Header->e_shentsize;
}
