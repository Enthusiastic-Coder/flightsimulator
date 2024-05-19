// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STD__AFX_H__
#define __STD__AFX_H__
#define NOMINMAX
//#include <vld.h>

#include <mathsupport.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <list>
#include <cstdlib>
#include <fstream>
#include <include_gl.h>
#include <MassChannel.h>


//#define LOCATED_AT_LONDON
#define LOCATED_AT_GIBRALTER
#define FPS_RESOLUTION 20
#define SATELLITE_RUNWAY
#define FG_BLEND_VALUE 0.4f

#define arrayCount(arr)         (sizeof(arr)/sizeof(arr[0]))
#define indicesCount(arr,n)     (sizeof(arr)/sizeof(arr[0])/n)

enum __global_force_lines_debug
{
    force_lines_begin,
    force_lines_none,
    force_lines_aero_force,
    force_lines_aero_force_and_wind_tunnel,
    force_lines_aero_wind_tunnel,
    force_lines_end,
};

#ifndef __STDAFX_CPP
extern __global_force_lines_debug global_force_lines_debug;
extern bool global_fg_debug;
extern bool global_info;
#endif

#endif
