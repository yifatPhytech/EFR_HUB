#ifndef SLOT_HANDLE
#define SLOT_HANDLE

uint8_t RestoreSlot(uint8_t index);
uint8_t GetHashFreeSlot(uint32_t id);
uint8_t SwapSlot(uint8_t index);

extern uint64_t  g_lMySlots;


#endif //SLOT_HANDLE
