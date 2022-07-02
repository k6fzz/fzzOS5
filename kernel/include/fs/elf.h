#pragma once

#include <stdint.h>
#include <stddef.h>

uint64_t load_elf(void* ptr);

struct elf_header
{
    uint8_t magic[4];           //0x7F 0x45 0x4c 0x46
    uint8_t bitformat;          //32bit or 64bit
    uint8_t datatype;           //big or little endian
    uint8_t header_version;     //set to 1
    uint8_t OSABI;              //ABI Type (0x00 for SysV)
    uint8_t ABI_version;
    uint8_t unused[7];
    uint16_t type;              //Object type
    uint16_t instruction_set;   //Architecture
    uint32_t ELF_version;       //Set to 1
    uint64_t entry;             //Entry Point
    uint64_t ph_off;            //Offset to the Program Header Table
    uint64_t sh_off;            //Offset to the Section Header Table
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_ent_size;       //Size of a program header entry
    uint16_t ph_num;            //Number of program header entries
    uint16_t sh_ent_size;       //Size of a section header entry
    uint16_t sh_num;            //Number of section header entries
    uint16_t sh_str_idx;         //Index of the section table that contains section names
};

struct elf_program_header
{
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

struct elf_section_header
{
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
};