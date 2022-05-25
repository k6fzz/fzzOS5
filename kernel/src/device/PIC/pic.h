#pragma once

#include <stddef.h>
#include <stdint.h>


void pic_remap();
void pic_disable();
void pic_EOI(uint8_t IRQ);
void pic_set_mask(uint8_t IRQ);
void pic_clear_mask(uint8_t IRQ);

void pit_init(uint64_t freq);