#-------------------------------------------------
#
# Project created by QtCreator 2016-01-30T14:46:20
#
#-------------------------------------------------

#QT       += core gui

TARGET = FlightSimulator

PROJECT = $(SDK_PROJECT)

include($$PROJECT/qMake/qImportNonQtApp.pri)
include($$PROJECT/qMake/qMakeDestination.pri)
include($$PROJECT/qMake/qImportSDL2.pri)
include($$PROJECT/qMake/qImportSDL2Image.pri)
include($$PROJECT/qMake/qImportGlee.pri)
include(FlightSimulator.pri)

INCLUDEPATH += $$PROJECT/include

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
    LIBS +=-L"C:/Program Files (x86)/OpenAL 1.1 SDK/libs/Win32"
    INCLUDEPATH += "C:/Program Files (x86)/OpenAL 1.1 SDK/include"

    LIBS += -lGdi32 -lopenal32
}

deploy_images.files += ./images/*
deploy_images.path  = $$DEPLOY_DIR/images
INSTALLS += deploy_images

#deploy_Models.files += ./Models/*
#deploy_Models.path  = $$DEPLOY_DIR/Models
#INSTALLS += deploy_Models

#deploy_RigidBodies.files += ./RigidBodies/*
#deploy_RigidBodies.path  = $$DEPLOY_DIR/RigidBodies
#INSTALLS += deploy_RigidBodies

deploy_shaders.files += ./shaders/*
deploy_shaders.path  = $$DEPLOY_DIR/shaders
INSTALLS += deploy_shaders

#deploy_Sounds.files += ./Sounds/*
#deploy_Sounds.path  = $$DEPLOY_DIR/Sounds
#INSTALLS += deploy_Sounds

#deploy_Terrain.files += ./Terrain/*
#deploy_Terrain.path  = $$DEPLOY_DIR/Terrain
#INSTALLS += deploy_Terrain

deploy_Textures.files += ./Textures/*
deploy_Textures.path  = $$DEPLOY_DIR/Textures
INSTALLS += deploy_Textures

deploy_Fonts.files += ./Fonts/*
deploy_Fonts.path  = $$DEPLOY_DIR/Fonts
INSTALLS += deploy_Fonts


DISTFILES += .\shaders\*
