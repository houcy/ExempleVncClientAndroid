LOCAL_PATH := $(call my-dir)

LIB_PATH := ../lib

#############################################
#########LIBRARIES###########################
#############################################
include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= c
LOCAL_SRC_FILES:= $(LIB_PATH)/libc.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= z
LOCAL_SRC_FILES:= $(LIB_PATH)/libz.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= nettle 
LOCAL_SRC_FILES:= $(LIB_PATH)/libnettle.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= hogweed 
LOCAL_SRC_FILES:= $(LIB_PATH)/libhogweed.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES :=
LOCAL_STATIC_LIBRARIES :=  gmp
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= gmp 
LOCAL_SRC_FILES:= $(LIB_PATH)/libgmp.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= tasn1
LOCAL_SRC_FILES:= $(LIB_PATH)/libtasn1.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= gnutls 
LOCAL_SRC_FILES:= $(LIB_PATH)/gnutls/libgnutls.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
LOCAL_STATIC_LIBRARIES := tasn1 nettle hogweed #gnutls_ext miniopencdk openpgp minitasn1 gnutls_auth gnutls_alg accelerated gnutlsxx crypto 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= gpg-error 
LOCAL_SRC_FILES:= $(LIB_PATH)/libgpg-error.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
LOCAL_STATIC_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= gcrypt 
LOCAL_SRC_FILES:= $(LIB_PATH)/libgcrypt.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
LOCAL_STATIC_LIBRARIES := gpg-error
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= turbojpeg 
LOCAL_SRC_FILES:= $(LIB_PATH)/libturbojpeg.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_LDLIBS :=
LOCAL_MODULE:= jpeg 
LOCAL_SRC_FILES:= $(LIB_PATH)/libjpeg.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_LDLIBS := 
LOCAL_MODULE:= vncclient 
LOCAL_SRC_FILES:= include/libvncclient/.libs/libvncclient.a 
LOCAL_EXPORT_C_INCLUDES := include \
		include/SDL \
		include/libvncclient \
		include/libvncserver \
		include/common \
		include/lib 
LOCAL_SHARED_LIBRARIES := 
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.cpp \
				main.cpp

LOCAL_SHARED_LIBRARIES := SDL2

APP_OPTIM:= debug


LOCAL_STATIC_LIBRARIES := gnustl_static  vncclient  gcrypt  z turbojpeg jpeg gnutls tasn1 nettle hogweed

LOCAL_LDLIBS := -lGLESv1_CM -llog -lgcc -landroid -lstdc++

include $(BUILD_SHARED_LIBRARY)
