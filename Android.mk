TOP_LOCAL_PATH := $(call my-dir)
include $(call all-subdir-makefiles)
LOCAL_PATH := $(TOP_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(LOCAL_PATH)/rapidjson/include $(LOCAL_PATH)/sdk/include 
LOCAL_CFLAGS +=  -DRAPIDJSON_HAS_STDSTRING=1

LOCAL_SRC_FILES := sdl2/src/main/android/SDL_android_main.c \
			sdlmain.cpp \
           SDLGameLoop.cpp \
           SDLMainWindow.cpp \
           TextureLoader.cpp \
            SDLSurfaceHelper.cpp \
            SDLTimer.cpp \
		AeroControlSurface.cpp \
		AeroControlSurfaceBoundary.cpp \
		AeroForceGenerator.cpp \
		AeroSectionElementForceGenerator.cpp \
		AeroSectionForceGenerator.cpp \
		AeroSectionSubElementForceGenerator.cpp \
		AeroWheelSpringForceGenerator.cpp \
		AircraftEngineForceGenerator.cpp \
		AirProperties.cpp \
		AoaClData.cpp \
		BA320RigidBody.cpp \
		Camera.cpp \
		ConstrainedSpringForceGenerator.cpp \
		CPPSourceCodeMeshModel.cpp \
		DataRecorder.cpp \
		FlightRecorder.cpp \
		ForceGenerator.cpp \
		GPSTileBoundary.cpp \
		GravityForceGenerator.cpp \
		GSForceGenerator.cpp \
		GSRigidBody.cpp \
		HeightTerrainData.cpp \
		Hydraulics.cpp \
		JSONRigidBody.cpp \
		JSONRigidBodyBuilder.cpp \
		JSONRigidBodyCollection.cpp \
		MeshModel.cpp \
		MipMapTerrainMeshModel.cpp \
		MipMapTerrainMeshModelCollection.cpp \
		OpenGLFrameBuffer.cpp \
		OpenGLRenderBuffer.cpp \
		OpenGLShaderProgram.cpp \
		PFDView.cpp \
		PivotObjects.cpp \
		RigidBody.cpp \
		RigidBodyMeshManager.cpp \
		ScaledHeightTerrainData.cpp \
		SceneryManager.cpp \
		SimpleSpringModel.cpp \
		SpringForceGenerator.cpp \
		SpringModel.cpp \
		SRTM30HeightTerrainData.cpp \
		SRTMFilename.cpp \
		stdafx.cpp \
		TerrainMeshModel.cpp \
		TextureManager.cpp \
		TileNode.cpp \
		TriangleFanQuadNode.cpp \
		Weather.cpp \
		WheelSpringForceGenerator.cpp \
		WorldSystem.cpp \
		OpenGLVertexBuffer.cpp \
		OpenGLPipeline.cpp \
		OpenGLTexture2D.cpp \
		WindTunnelForceGenerator.cpp \
		MeshGroupObject.cpp \
		OpenGLRenderer.cpp \
		GPSTerrainData.cpp \
		MeshObject.cpp \
		MeshData.cpp \
		MeshSurfaceObject.cpp \
		TerrainGroupObject.cpp \
		PerlinNoise.cpp \
		SkyDome.cpp \
		CameraView.cpp \
		CameraViewProvider.cpp \
		TurnDirection.cpp \
		OpenGLFontMeshBuilder.cpp \
		OpenGLFontRenderer2D.cpp \
		OpenGLFontTexture.cpp \
		OpenGLButtonTexture.cpp \
		OpenGLButtonTextureManager.cpp \
		OpenGLTextureRenderer2D.cpp \
		OpenGLSliderControl.cpp \
		SDLJoystickSystem.cpp \
		HeathrowRadar.cpp \
		OpenGLPainter.cpp
		
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image
LOCAL_LDLIBS := -lGLESv2 -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)