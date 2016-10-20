TOP_LOCAL_PATH := $(call my-dir)
include $(call all-subdir-makefiles)
LOCAL_PATH := $(TOP_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(LOCAL_PATH)/glm $(LOCAL_PATH)/sdk/include 
LOCAL_CFLAGS +=  -DRAPIDJSON_HAS_STDSTRING=1

LOCAL_SRC_FILES := sdl2/src/main/android/SDL_android_main.c \
		camera.cpp \
		matrices.cpp \
		mesh.cpp \
		meshLoader.cpp \
		opengl.cpp \
		shader.cpp \
		stdafx.cpp \
		vector3d.cpp \
		textureloader.cpp
		
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image
LOCAL_LDLIBS := -lGLESv2 -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)