#pragma once

#include <al.h>
#include <alc.h>

bool loadWavFile(const std::string filename, ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format);
ALuint load_wave(const char * file_name);