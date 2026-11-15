#include <elf.h>
#include <string.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

#include <elf.h>
#include <stdint.h>
#include <string.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

uc_err UcError = {0};

void MrowLoadProgram(char *FileContent, size_t FileSize, MrowProgram *Program) {
  if (FileSize < sizeof(Elf64_Ehdr)) {
    Crash("Invalid ELF (File too small)");
  }

  Elf64_Ehdr *Header = (Elf64_Ehdr *)FileContent;

  if (memcmp(Header->e_ident, ELFMAG, SELFMAG) != 0) {
    Crash("Invalid ELF (Bad magic)");
  }

  if (Header->e_ident[EI_CLASS] != ELFCLASS64) {
    Crash("Invalid ELF (Not 64-bit)");
  }

  if (Header->e_ident[EI_DATA] != ELFDATA2LSB) {
    Crash("Invalid ELF (Not little-endian)");
  }

  if (Header->e_type != ET_EXEC) {
    Crash("Invalid ELF (Not executable)");
  }

  if (Header->e_machine != EM_AARCH64) {
    Crash("Invalid ELF (Not AArch64)");
  }

  if (Header->e_version != EV_CURRENT) {
    Crash("Invalid ELF (Unsupported version)");
  }

  if (Header->e_ehsize != sizeof(Elf64_Ehdr)) {
    Crash("Invalid ELF (Invalid header size)");
  }

  if (Header->e_phentsize != sizeof(Elf64_Phdr)) {
    Crash("Invalid ELF (Invalid program header size)");
  }

  if (Header->e_phnum == 0 || Header->e_phoff > FileSize ||
      Header->e_phnum > (FileSize - Header->e_phoff) / sizeof(Elf64_Phdr)) {
    Crash("Invalid ELF (Invalid program header table)");
  }

  Program->Entry = Header->e_entry;

  Elf64_Phdr *Phdr = (Elf64_Phdr *)(FileContent + Header->e_phoff);

  uint64_t MinAddress = UINT64_MAX;
  uint64_t MaxAddress = 0;

  for (int Idx = 0; Idx < Header->e_phnum; Idx++) {
    Elf64_Phdr *Ph = &Phdr[Idx];

    if (Ph->p_type != PT_LOAD || Ph->p_memsz == 0) {
      continue;
    }

    if (Ph->p_filesz > Ph->p_memsz || Ph->p_offset > FileSize ||
        Ph->p_filesz > FileSize - Ph->p_offset ||
        Ph->p_memsz > UINT64_MAX - Ph->p_vaddr) {
      Crash("Invalid ELF (Invalid load segment)");
    }

    uint64_t Address = Ph->p_vaddr & ~0xfffULL;
    uint64_t End = (Ph->p_vaddr + Ph->p_memsz + 0xfffULL) & ~0xfffULL;

    if (End < Ph->p_vaddr + Ph->p_memsz) {
      Crash("Invalid ELF (Address overflow)");
    }

    if (Address < MinAddress) {
      MinAddress = Address;
    }

    if (End > MaxAddress) {
      MaxAddress = End;
    }
  }

  if (MinAddress == UINT64_MAX || MaxAddress <= MinAddress) {
    Crash("Invalid ELF (No loadable segments)");
  }

  UcError = uc_mem_map(Program->Unicorn, MinAddress, MaxAddress - MinAddress,
                       UC_PROT_ALL);
  if (UcError != UC_ERR_OK) {
    Crash("Unicorn memory mapping failed: %s", uc_strerror(UcError));
  }

  uint8_t ZeroBuffer[4096] = {0};
  for (int Idx = 0; Idx < Header->e_phnum; Idx++) {
    Elf64_Phdr *Ph = &Phdr[Idx];

    if (Ph->p_type != PT_LOAD || Ph->p_memsz == 0) {
      continue;
    }

    if (Ph->p_filesz > 0) {
      UcError = uc_mem_write(Program->Unicorn, Ph->p_vaddr,
                             FileContent + Ph->p_offset, Ph->p_filesz);
      if (UcError != UC_ERR_OK) {
        Crash("Failed to write ELF segment: %s", uc_strerror(UcError));
      }
    }

    uint64_t BssAddress = Ph->p_vaddr + Ph->p_filesz;
    uint64_t BssSize = Ph->p_memsz - Ph->p_filesz;

    while (BssSize > 0) {
      size_t Chunk =
          BssSize < sizeof(ZeroBuffer) ? (size_t)BssSize : sizeof(ZeroBuffer);

      UcError = uc_mem_write(Program->Unicorn, BssAddress, ZeroBuffer, Chunk);
      if (UcError != UC_ERR_OK) {
        Crash("Failed to initialize BSS: %s", uc_strerror(UcError));
      }

      BssAddress += Chunk;
      BssSize -= Chunk;
    }
  }
}
