#-------------------------------------------------
#
# Project created by QtCreator 2016-01-30T14:46:20
#
#-------------------------------------------------

TARGET = FlightSimulator

QT += core gui network
QT += opengl

CONFIG += c++17

DEFINES += "RAPIDJSON_HAS_STDSTRING=1" WANT_SDL_SDL_SURFACE

# Organise generated build files
OBJECTS_DIR = .obj
MOC_DIR     = .moc
RCC_DIR     = .rcc
UI_DIR      = .ui


win32:CONFIG(release, debug|release) {
    DEPLOY_EXE = $$shell_path($$OUT_PWD/release/$${TARGET}.exe)
    DEPLOY_TOOL = $$shell_path($$[QT_INSTALL_BINS]/windeployqt.exe)

    QMAKE_POST_LINK = $$quote($$DEPLOY_TOOL) $$quote($$DEPLOY_EXE) $$escape_expand(\n\t)
}

android{
    DEPLOY_DIR = /assets
}

win32 {
    CONFIG(debug, debug|release) {
        DEPLOY_DIR = $$OUT_PWD/Debug
    }

    CONFIG(release, debug|release) {
        DEPLOY_DIR = $$OUT_PWD/release
    }
}

isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
}


include(assimp2/assimp.pri)
include(sdl2_image/sdl2image.pri)
include(sdl2/sdl2.pri)
include(openal/openal.pri)

INCLUDEPATH += rapidjson/include

INCLUDEPATH += sdk/include

win32 {
    HEADERS += sdk/include/*
}

include(FlightSimulator.pri)

win32 {
    SOURCES += sdk/include/GLee.c
    HEADERS += sdk/include/GLee.h
}


CONFIG += INSTALL_TERRAIN
CONFIG += INSTALL_MODELS
CONFIG += INSTALL_BODIES

SOURCES += sdlmain.cpp \
           SDLGameLoop.cpp \
           SDLMainWindow.cpp \
           TextureLoader.cpp \
            SDLSurfaceHelper.cpp

HEADERS += SDLGameLoop.h \
           SDLMainWindow.h \
           TextureLoader.h \
            SDLSurfaceHelper.h

win32 {
    LIBS += -lUser32
}

deploy_images.files += ./images/*
deploy_images.path  = $$DEPLOY_DIR/images
INSTALLS += deploy_images

deploy_Models.files += ./Models/*
deploy_Models.path  = $$DEPLOY_DIR/Models
INSTALLS += deploy_Models

deploy_RigidBodies.files += ./RigidBodies/*
deploy_RigidBodies.path  = $$DEPLOY_DIR/RigidBodies
INSTALLS += deploy_RigidBodies

deploy_shaders.files += ./shaders/*
deploy_shaders.path  = $$DEPLOY_DIR/shaders
INSTALLS += deploy_shaders

deploy_Sounds.files += ./Sounds/*
deploy_Sounds.path  = $$DEPLOY_DIR/Sounds
INSTALLS += deploy_Sounds

deploy_Terrain.files += ./Terrain/*
deploy_Terrain.path  = $$DEPLOY_DIR/Terrain
INSTALLS += deploy_Terrain

deploy_Textures.files += ./Textures/*
deploy_Textures.path  = $$DEPLOY_DIR/Textures
INSTALLS += deploy_Textures

deploy_Fonts.files += ./Fonts/*
deploy_Fonts.path  = $$DEPLOY_DIR/Fonts
INSTALLS += deploy_Fonts


DISTFILES += .\shaders\*
