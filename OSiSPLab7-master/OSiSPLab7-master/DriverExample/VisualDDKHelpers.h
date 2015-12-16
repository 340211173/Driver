#pragma once

#ifdef _DEBUG

#include <excpt.h>
#include "ntdef.h"

typedef enum NTSTATUS_VisualDDK_Helper {} NTSTATUS_VisualDDK_Helper_t;
C_ASSERT(sizeof(NTSTATUS_VisualDDK_Helper_t) == sizeof(NTSTATUS));

#define NTSTATUS NTSTATUS_VisualDDK_Helper_t

typedef struct HANDLE_VisualDDK_Helper *HANDLE_VisualDDK_Helper_t, **PHANDLE_VisualDDK_Helper_t;
C_ASSERT(sizeof(HANDLE_VisualDDK_Helper_t) == sizeof(HANDLE));
C_ASSERT(sizeof(PHANDLE_VisualDDK_Helper_t) == sizeof(PHANDLE));

#define HANDLE HANDLE_VisualDDK_Helper_t
#define PHANDLE PHANDLE_VisualDDK_Helper_t 

#endif