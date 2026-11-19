cmake_minimum_required(VERSION 3.21)

project(FlightSimulator LANGUAGES C CXX)

if(NOT WIN32 OR NOT MSVC)
	message(FATAL_ERROR "FlightSimulator currently requires Windows and the MSVC toolchain.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(FATAL_ERROR "FlightSimulator dependencies are available only for x64.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(FLIGHTSIM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(FLIGHTSIM_SDL2_DIR "${FLIGHTSIM_ROOT}/sdl2/VisualC/x64")
set(FLIGHTSIM_SDL2_IMAGE_DIR "${FLIGHTSIM_ROOT}/sdl2_image/VisualC/x64")
set(FLIGHTSIM_OPENAL_DIR "${FLIGHTSIM_ROOT}/openal/libs/Win64")
set(FLIGHTSIM_ASSIMP_DIR "${FLIGHTSIM_ROOT}/assimp2")
set(FLIGHTSIM_SDL2_IMAGE_CODEC_DIR "${FLIGHTSIM_ROOT}/sdl2_image/VisualC/external/lib/x64")

set(FLIGHTSIM_SOURCES
	AeroControlSurface.cpp
	AeroControlSurfaceBoundary.cpp
	AeroForceGenerator.cpp
	AeroSectionElementForceGenerator.cpp
	AeroSectionForceGenerator.cpp
	AeroSectionSubElementForceGenerator.cpp
	AeroWheelSpringForceGenerator.cpp
	AircraftCarrierRigidBody.cpp
	AircraftEngineForceGenerator.cpp
	AircraftEngineNozzleBleeder.cpp
	AircraftEngineNozzleFeeder.cpp
	AirProperties.cpp
	AoaClData.cpp
	BA320RigidBody.cpp
	BA380RigidBody.cpp
	Camera.cpp
	CameraView.cpp
	CameraViewProvider.cpp
	CircularRunwayRigidBody.cpp
	ConstrainedSpringForceGenerator.cpp
	CPPSourceCodeMeshModel.cpp
	DataRecorder.cpp
	FlightRecorder.cpp
	ForceGenerator.cpp
	GPSTerrainData.cpp
	GPSTileBoundary.cpp
	GravityForceGenerator.cpp
	GSForceGenerator.cpp
	GSRigidBody.cpp
	HarrierRigidBody.cpp
	HeathrowRadar.cpp
	HeightTerrainData.cpp
	Hydraulics.cpp
	JSONRigidBody.cpp
	JSONRigidBodyBuilder.cpp
	JSONRigidBodyCollection.cpp
	loadWAV.cpp
	LondonGroundTile.cpp
	MeshData.cpp
	MeshGroupObject.cpp
	MeshModel.cpp
	MeshObject.cpp
	MeshSurfaceObject.cpp
	MipMapTerrainMeshModel.cpp
	MipMapTerrainMeshModelCollection.cpp
	OpenGLButtonTexture.cpp
	OpenGLButtonTextureManager.cpp
	OpenGLFontMeshBuilder.cpp
	OpenGLFontRenderer.cpp
	OpenGLFontTexture.cpp
	OpenGLFrameBuffer.cpp
	OpenGLPainter.cpp
	OpenGLPipeline.cpp
	OpenGLRenderBuffer.cpp
	OpenGLRenderer.cpp
	OpenGLShaderProgram.cpp
	OpenGLSliderControl.cpp
	OpenGLTextureRenderer2D.cpp
	OpenGLVertexBuffer.cpp
	OpenglTexture2d.cpp
	PFDView.cpp
	PerlinNoise.cpp
	PivotObjects.cpp
	ReferenceFrame.cpp
	RigidBody.cpp
	RigidBodyMeshManager.cpp
	ScaledHeightTerrainData.cpp
	SceneryManager.cpp
	SDLGameLoop.cpp
	SDLJoystickSystem.cpp
	SDLMainWindow.cpp
	SDLSurfaceHelper.cpp
	SDLTimer.cpp
	sdlmain.cpp
	SimpleCloud.cpp
	SimpleSpringModel.cpp
	SkyDome.cpp
	SpringForceGenerator.cpp
	SpringModel.cpp
	SRTM30HeightTerrainData.cpp
	SRTMFilename.cpp
	stdafx.cpp
	TerrainGroupObject.cpp
	TerrainMeshModel.cpp
	TextureLoader.cpp
	TextureManager.cpp
	TileNode.cpp
	TriangleFanQuadNode.cpp
	TurnDirection.cpp
	Weather.cpp
	WheelSpringForceGenerator.cpp
	WindTunnelForceGenerator.cpp
	WorldSystem.cpp
	sdk/include/GLee.c
)

set(FLIGHTSIM_REQUIRED_LIBRARIES
	"${FLIGHTSIM_SDL2_DIR}/Debug/SDL2main.lib"
	"${FLIGHTSIM_SDL2_DIR}/Debug/SDL2.lib"
	"${FLIGHTSIM_SDL2_DIR}/Release/SDL2main.lib"
	"${FLIGHTSIM_SDL2_DIR}/Release/SDL2.lib"
	"${FLIGHTSIM_SDL2_IMAGE_DIR}/Debug/SDL2_image.lib"
	"${FLIGHTSIM_SDL2_IMAGE_DIR}/Release/SDL2_image.lib"
	"${FLIGHTSIM_OPENAL_DIR}/OpenAL32.lib"
	"${FLIGHTSIM_ASSIMP_DIR}/Debug/Assimp32.lib"
	"${FLIGHTSIM_ASSIMP_DIR}/Release/Assimp32.lib"
	"${FLIGHTSIM_SDL2_DIR}/Debug/SDL2.dll"
	"${FLIGHTSIM_SDL2_DIR}/Release/SDL2.dll"
	"${FLIGHTSIM_SDL2_IMAGE_DIR}/Debug/SDL2_image.dll"
	"${FLIGHTSIM_SDL2_IMAGE_DIR}/Release/SDL2_image.dll"
	"${FLIGHTSIM_ROOT}/openal/Win32/x64/OpenAL32.dll"
	"${FLIGHTSIM_ASSIMP_DIR}/Debug/Assimp32.dll"
	"${FLIGHTSIM_ASSIMP_DIR}/Release/Assimp32.dll"
)

foreach(library IN LISTS FLIGHTSIM_REQUIRED_LIBRARIES)
	if(NOT EXISTS "${library}")
		message(FATAL_ERROR
			"Required dependency file not found: ${library}\n"
			"Build the x64 SDL2 and SDL2_image Debug/Release libraries first, or restore the dependency submodules and their prebuilt libraries.")
	endif()
endforeach()

file(GLOB FLIGHTSIM_SDL2_IMAGE_CODEC_DLLS CONFIGURE_DEPENDS
	"${FLIGHTSIM_SDL2_IMAGE_CODEC_DIR}/*.dll")
if(NOT FLIGHTSIM_SDL2_IMAGE_CODEC_DLLS)
	message(FATAL_ERROR
		"SDL_image codec DLLs were not found in ${FLIGHTSIM_SDL2_IMAGE_CODEC_DIR}.")
endif()

add_executable(FlightSimulator ${FLIGHTSIM_SOURCES})

target_include_directories(FlightSimulator PRIVATE
	"${FLIGHTSIM_ROOT}"
	"${FLIGHTSIM_ROOT}/sdl2/include"
	"${FLIGHTSIM_ROOT}/sdl2_image/include"
	"${FLIGHTSIM_ROOT}/openal/include"
	"${FLIGHTSIM_ROOT}/rapidjson/include"
	"${FLIGHTSIM_ROOT}/sdk/include"
	"${FLIGHTSIM_ROOT}/assimp2/include"
)

target_compile_definitions(FlightSimulator PRIVATE
	RAPIDJSON_HAS_STDSTRING=1
	WANT_SDL_SDL_SURFACE
)

target_link_libraries(FlightSimulator PRIVATE
	Qt6::Widgets
	"$<$<CONFIG:Debug>:${FLIGHTSIM_SDL2_DIR}/Debug/SDL2main.lib>"
	"$<$<CONFIG:Debug>:${FLIGHTSIM_SDL2_DIR}/Debug/SDL2.lib>"
	"$<$<NOT:$<CONFIG:Debug>>:${FLIGHTSIM_SDL2_DIR}/Release/SDL2main.lib>"
	"$<$<NOT:$<CONFIG:Debug>>:${FLIGHTSIM_SDL2_DIR}/Release/SDL2.lib>"
	"$<$<CONFIG:Debug>:${FLIGHTSIM_SDL2_IMAGE_DIR}/Debug/SDL2_image.lib>"
	"$<$<NOT:$<CONFIG:Debug>>:${FLIGHTSIM_SDL2_IMAGE_DIR}/Release/SDL2_image.lib>"
	"${FLIGHTSIM_OPENAL_DIR}/OpenAL32.lib"
	"$<$<CONFIG:Debug>:${FLIGHTSIM_ASSIMP_DIR}/Debug/Assimp32.lib>"
	"$<$<NOT:$<CONFIG:Debug>>:${FLIGHTSIM_ASSIMP_DIR}/Release/Assimp32.lib>"
	opengl32
	User32
)

set(FLIGHTSIM_RUNTIME_ASSETS
	images
	Models
	RigidBodies
	shaders
	Sounds
	Terrain
	Textures
	Fonts
)

foreach(asset_directory IN LISTS FLIGHTSIM_RUNTIME_ASSETS)
	if(EXISTS "${FLIGHTSIM_ROOT}/${asset_directory}")
		add_custom_command(TARGET FlightSimulator POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy_directory
				"${FLIGHTSIM_ROOT}/${asset_directory}"
				"$<TARGET_FILE_DIR:FlightSimulator>/${asset_directory}"
			VERBATIM
		)
	endif()
endforeach()

add_custom_command(TARGET FlightSimulator POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"$<IF:$<CONFIG:Debug>,${FLIGHTSIM_SDL2_DIR}/Debug/SDL2.dll,${FLIGHTSIM_SDL2_DIR}/Release/SDL2.dll>"
		"$<TARGET_FILE_DIR:FlightSimulator>"
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"$<IF:$<CONFIG:Debug>,${FLIGHTSIM_SDL2_IMAGE_DIR}/Debug/SDL2_image.dll,${FLIGHTSIM_SDL2_IMAGE_DIR}/Release/SDL2_image.dll>"
		"$<TARGET_FILE_DIR:FlightSimulator>"
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"$<IF:$<CONFIG:Debug>,${FLIGHTSIM_ASSIMP_DIR}/Debug/Assimp32.dll,${FLIGHTSIM_ASSIMP_DIR}/Release/Assimp32.dll>"
		"$<TARGET_FILE_DIR:FlightSimulator>"
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"${FLIGHTSIM_ROOT}/openal/Win32/x64/OpenAL32.dll"
		"$<TARGET_FILE_DIR:FlightSimulator>"
	VERBATIM
)

add_custom_command(TARGET FlightSimulator POST_BUILD
	COMMAND Qt6::windeployqt --no-translations --no-compiler-runtime
		"$<TARGET_FILE:FlightSimulator>"
	VERBATIM
)

foreach(codec_dll IN LISTS FLIGHTSIM_SDL2_IMAGE_CODEC_DLLS)
	add_custom_command(TARGET FlightSimulator POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different
			"${codec_dll}"
			"$<TARGET_FILE_DIR:FlightSimulator>"
		VERBATIM
	)
endforeach()
