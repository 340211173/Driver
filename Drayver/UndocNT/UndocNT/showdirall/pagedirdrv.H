#define FILE_DEVICE_PAGEDIRDRV  0x00008300
#define DRIVER_DEVICE_NAME	L"pagedirdrv"

#define PAGEDIRDRV_IOCTL_INDEX  0x830

#define IOCTL_PAGEDIRDRV_DUMPPAGEDIR         CTL_CODE(FILE_DEVICE_PAGEDIRDRV,  \
                                               PAGEDIRDRV_IOCTL_INDEX,  \
                                               METHOD_BUFFERED,       \
                                               FILE_ANY_ACCESS)
