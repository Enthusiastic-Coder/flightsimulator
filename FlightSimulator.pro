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


CONFIG(release, debug|release) {
    CONFIGURATION = "Release"
}

CONFIG(debug, debug|release) {
    CONFIGURATION = "Debug"
}

win32{
DESTDIR = $$OUT_PWD/$$CONFIGURATION
OBJECTS_DIR = $$OUT_PWD/.obj
MOC_DIR = $$OUT_PWD/.moc
RCC_DIR = $$OUT_PWD/.rcc
UI_DIR = $$OUT_PWD/.ui


    DEPLOY_DIR = $$DESTDIR
}

android{
    DEPLOY_DIR = /assets
}

isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
}


CONFIG(release, debug|release) {
    contains(CONFIG, "qt"){

        win32 {
            DEPLOY_COMMAND = $(QTDIR)/bin/windeployqt
        }

        win32 {

        DEPLOY_TARGET =  $$shell_quote($$shell_path($${DESTDIR}/$${TARGET}$${TARGET_CUSTOM_EXT}))

        #message(  $${DEPLOY_COMMAND} $${DEPLOY_TARGET} )

        #  # Uncomment the following line to help debug the deploy command when running qmake
        #  warning($${DEPLOY_COMMAND} $${DEPLOY_TARGET})

            QMAKE_POST_LINK = $${DEPLOY_COMMAND} --qmldir $(QTDIR)\qml $${DEPLOY_TARGET}
        }
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
