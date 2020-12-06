// hal_interman.h -- Interrupt Management Interface
#ifndef __hal_Interrupt_h
#define __hal_Interrupt_h
#include "_stdtype.h"

void Interrupt_Disable(void);
void Interrupt_Enable(void);

u16 Interrupt_SaveAndDisable(void);
void Interrupt_Restore(u16 flags);

void Interrupt_SetVector(u8 number, u32 handlerAddr);
u32 Interrupt_GetVector(u8 number);
#endif
