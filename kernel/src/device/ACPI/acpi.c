#include "acpi.h"
#include "../../boot/boot.h"
#include "../../boot/stivale2.h"
#include "../../util/printf.h"
#include "../../libk/kstring.h"

#include <stddef.h>
#include <stdint.h>

const uint8_t* acpi_signatures[] = 
{
    "RSP PTR ",
    "APIC",
    "BGRT",
    "BERT",
    "CPEP",
    "DSDT",
    "ECDT",
    "EINJ",
    "ERST",
    "FACP",
    "FACS",
    "HEST",
    "MSCT",
    "MPST",
    "OEM",
    "PMTT",
    "PSDT",
    "RASF",
    "RSDT",
    "SBST",
    "SLIT",
    "SRAT",
    "SSDT",
    "XSDT",
};

struct RSDPDescriptor
{
    uint8_t signature[8];
    uint8_t checksum;
    uint8_t OEMID[6];
    uint8_t revision;
    uint32_t RSDTAddress;
}__attribute__((packed));

struct RSDP20Descriptor
{
    struct RSDPDescriptor header;
    uint32_t length;
    uint64_t XSDTAddress;
    uint8_t extChecksum;
    uint8_t reserved[3];
}__attribute__((packed));

struct ACPI_Info
{
    struct RSDP20Descriptor* rsdp;

}ACPI_Info;

void acpi_init()
{
    ACPI_Info.rsdp = (struct RSDP20Descriptor*)boot_info.tag_rsdp->rsdp;
    
    uint8_t test[8] = {'R', 'S', 'P', ' ', 'P', 'T', 'R', ' '};
    for(uint32_t i = 0; i < 8; i++)
    {
        if(test[i] == ACPI_Info.rsdp->header.signature[i]) 
            continue;
        else
            return;
    }

    printf("Valid RSDP Header. Revision %d\n", ACPI_Info.rsdp->header.revision);
    
}