#define FILE_DEVICE_HOOKINT  0x00008300
#define DRIVER_DEVICE_NAME   L"hookint"
#define HOOKINT_IOCTL_INDEX  0x830
#define IOCTL_HOOKINT_SYSTEM_SERVICE_USAGE     CTL_CODE(FILE_DEVICE_HOOKINT,  \
                                               HOOKINT_IOCTL_INDEX,  \
                                               METHOD_BUFFERED,       \
                                               FILE_ANY_ACCESS)
