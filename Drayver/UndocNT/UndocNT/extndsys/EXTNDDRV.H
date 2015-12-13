#define FILE_DEVICE_EXTNDDRV  0x00008300
#define EXTNDDRV_IOCTL_INDEX  0x830
#define IOCTL_EXTNDDRV_GET_STARTING_SERVICEID      CTL_CODE(FILE_DEVICE_EXTNDDRV,  \
                                               EXTNDDRV_IOCTL_INDEX,  \
                                               METHOD_BUFFERED,       \
                                               FILE_ANY_ACCESS)
