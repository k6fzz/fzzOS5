#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct ustar_header
{
    uint8_t filename[100];
    uint64_t mode;
    uint64_t owner_id;
    uint64_t group_id;
    uint8_t file_size[12];
    uint8_t time[12];
    uint64_t checksum;
    uint8_t type;
    uint8_t linked_file[100];
    uint8_t indicator[6];
    uint16_t version;
    uint8_t owner_username[32];
    uint8_t owner_groupname[32];
    uint64_t device_majornumber;
    uint64_t device_minornumber;
    uint8_t filename_prefix[155];
};