#define FILE_DEVICE_CALLGATE  0x00008300
#define DRIVER_DEVICE_NAME    L"callgate"

#define CALLGATE_IOCTL_INDEX  0x830
#define CALLGATE_IOCTL_INDEX1 0x831


typedef struct CallGateInfo {
        void *FunctionLinearAddress;
        int NumberOfParameters;
        unsigned short CodeSelector;
        unsigned short CallGateSelector;
} CallGateInfo_t, *PCallGateInfo_t;


#define IOCTL_CALLGATE_CREATE          CTL_CODE(FILE_DEVICE_CALLGATE,  \
                                               CALLGATE_IOCTL_INDEX,  \
                                               METHOD_BUFFERED,       \
                                               FILE_ANY_ACCESS)

#define IOCTL_CALLGATE_RELEASE          CTL_CODE(FILE_DEVICE_CALLGATE,  \
                                               CALLGATE_IOCTL_INDEX1,  \
                                               METHOD_BUFFERED,       \
                                               FILE_ANY_ACCESS)
