#define IOCTL_PRINT_DEBUG_MESS CTL_CODE( \
	FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_CHANGE_IRQL CTL_CODE(\
	FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MAKE_SYSTEM_CRASH CTL_CODE( \
	FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_TOUCH_PORT_378H CTL_CODE( \
	FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SEND_BYTE_TO_USER CTL_CODE( \
	FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)