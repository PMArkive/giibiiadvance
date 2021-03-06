// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2011-2015, 2019-2020, Antonio Niño Díaz
//
// GiiBiiAdvance - GBA/GB emulator

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "build_options.h"
#include "config.h"
#include "file_utils.h"
#include "general_utils.h"

#include "gui/win_main.h"

//------------------------------------------------------------------------------

static FILE *f_log;
static int log_file_opened = 0;

void Debug_End(void)
{
    if (log_file_opened)
        fclose(f_log);

    log_file_opened = 0;
}

void Debug_Init(void)
{
    log_file_opened = 0;
    atexit(Debug_End);

    // Remove previous log files
    char logpath[MAX_PATHLEN];
    snprintf(logpath, sizeof(logpath), "%slog.txt", DirGetRunningPath());
    if (FileExists(logpath))
        remove(logpath);
}

void Debug_LogMsgArg(const char *msg, ...)
{
    if (log_file_opened == 0)
    {
        char logpath[MAX_PATHLEN];
        snprintf(logpath, sizeof(logpath), "%slog.txt", DirGetRunningPath());
        f_log = fopen(logpath, "w");
        if (f_log)
            log_file_opened = 1;
    }

    if (log_file_opened)
    {
        va_list args;
        va_start(args, msg);
        vfprintf(f_log, msg, args);
        va_end(args);
        fputc('\n', f_log);
    }
}

void Debug_DebugMsgArg(const char *msg, ...)
{
    char dest[2000];

    va_list args;
    va_start(args, msg);
    vsnprintf(dest, sizeof(dest), msg, args);
    va_end(args);
    dest[sizeof(dest) - 1] = '\0';

    if (EmulatorConfig.debug_msg_enable == 0)
        return;

    //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
    //                         "GiiBiiAdvance - Debug", dest, NULL);
    Win_MainShowMessage(1, dest);
}

void Debug_ErrorMsgArg(const char *msg, ...)
{
    char dest[2000];

    va_list args;
    va_start(args, msg);
    vsnprintf(dest, sizeof(dest), msg, args);
    va_end(args);
    dest[sizeof(dest) - 1] = '\0';

    //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
    //                         "GiiBiiAdvance - Error", dest, NULL);
    Win_MainShowMessage(0, dest);
}

void Debug_DebugMsg(const char *msg)
{
    if (EmulatorConfig.debug_msg_enable == 0)
        return;

    //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
    //                         "GiiBiiAdvance - Debug", msg, NULL);
    Win_MainShowMessage(1, msg);
}

void Debug_ErrorMsg(const char *msg)
{
    //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
    //                         "GiiBiiAdvance - Error", msg, NULL);
    Win_MainShowMessage(0, msg);
}

//------------------------------------------------------------------------------

static char console_buffer[20 * 1024];

void ConsoleReset(void)
{
    console_buffer[0] = '\0';
}

void ConsolePrint(const char *msg, ...)
{
    va_list args;
    char buffer[1024];

    va_start(args, msg);
    vsnprintf(buffer, sizeof(buffer), msg, args);
    va_end(args);

    s_strncat(console_buffer, buffer, sizeof(console_buffer));
}

void ConsoleShow(void)
{
    Win_MainShowMessage(2, console_buffer);
    //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
    //                         "GiiBiiAdvance - Console", console_buffer, NULL);
}

//----------------------------------------------------------------------------------

static char sys_info_buffer[10000];

static void _sys_info_printf(const char *msg, ...)
{
    va_list args;
    char buffer[1000];

    va_start(args, msg);
    vsnprintf(buffer, sizeof(buffer), msg, args);
    va_end(args);

    s_strncat(sys_info_buffer, buffer, sizeof(sys_info_buffer));
}

static void _sys_info_reset(void)
{
    memset(sys_info_buffer, 0, sizeof(sys_info_buffer));

    _sys_info_printf("SDL information:\n"
                     "----------------\n"
                     "\n"
                     "SDL_GetPlatform(): %s\n\n"
                     "SDL_GetCPUCount(): %d (Number of logical CPU cores)\n"
#if SDL_VERSION_ATLEAST(2, 0, 1)
                     "SDL_GetSystemRAM(): %d MB\n"
#endif
                     "SDL_GetCPUCacheLineSize(): %d kB (Cache L1)\n\n",
                     SDL_GetPlatform(), SDL_GetCPUCount(),
#if SDL_VERSION_ATLEAST(2, 0, 1)
                     SDL_GetSystemRAM(),
#endif
                     SDL_GetCPUCacheLineSize());

    int total_secs, pct;
    SDL_PowerState st = SDL_GetPowerInfo(&total_secs, &pct);
    char *st_string;
    switch (st)
    {
        default:
        case SDL_POWERSTATE_UNKNOWN:
            st_string = "SDL_POWERSTATE_UNKNOWN (cannot determine power status)";
            break;
        case SDL_POWERSTATE_ON_BATTERY:
            st_string = "SDL_POWERSTATE_ON_BATTERY (not plugged in, running on battery)";
            break;
        case SDL_POWERSTATE_NO_BATTERY:
            st_string = "SDL_POWERSTATE_NO_BATTERY (plugged in, no battery available)";
            break;
        case SDL_POWERSTATE_CHARGING:
            st_string = "SDL_POWERSTATE_CHARGING (plugged in, charging battery)";
            break;
        case SDL_POWERSTATE_CHARGED:
            st_string = "SDL_POWERSTATE_CHARGED (plugged in, battery charged)";
            break;
    }

    unsigned int hours, min, secs;

    hours = ((unsigned int)total_secs) / 3600;
    min = (((unsigned int)total_secs) - (hours * 3600)) / 60;
    secs = (((unsigned int)total_secs) - (hours * 3600) - (min * 60));

    _sys_info_printf("SDL_GetPowerInfo():\n"
                     "  %s\n"
                     "  Time left: %d:%02d:%02d\n"
                     "  Percentage: %3d%%\n"
                     "\n",
                     st_string, hours, min, secs, pct);
#ifdef ENABLE_OPENGL
    _sys_info_printf("OpenGL information:\n"
                     "-------------------\n"
                     "\n"
                     "GL_RENDERER   = %s\n"
                     "GL_VERSION    = %s\n"
                     "GL_VENDOR     = %s\n"
                     "GL_EXTENSIONS = ",
                     (const char *)glGetString(GL_RENDERER),
                     (const char *)glGetString(GL_VERSION),
                     (const char *)glGetString(GL_VENDOR));
    _sys_info_printf("%s\n", (const char *)glGetString(GL_EXTENSIONS));
#endif
    _sys_info_printf("\nEND LOG\n");
}

void SysInfoShow(void)
{
    _sys_info_reset();

    Win_MainShowMessage(3, sys_info_buffer);
}
