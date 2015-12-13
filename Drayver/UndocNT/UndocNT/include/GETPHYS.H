BOOL GetPhysicalAddressAndPageAttributes(void *VirtualAddress,
			unsigned int *PhysicalAddress, unsigned int *PageAttributes);
int CreateRing0CallGate();
int FreeRing0CallGate();
