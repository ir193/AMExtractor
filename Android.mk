LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := main.c kmem.c kallsyms.c kernel_tcp.c kernel_pfn.c

LOCAL_MODULE := kmem
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -Os -fPIE
LOCAL_LDFLAGS += -Os -fPIE -pie

TOP_SRCDIR := $(abspath $(LOCAL_PATH))

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
