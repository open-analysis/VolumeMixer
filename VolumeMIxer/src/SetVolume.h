#pragma once

/**

  Copyright (c) 2016, Dawid Ciecierski

  Based on and inspired by code shared by Larry Osterman of Microsoft at
  https://blogs.msdn.microsoft.com/larryosterman/2007/03/06/how-do-i-change-the-master-volume-in-windows-vista/

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#define WINVER       _WIN32_WINNT_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#define VERSION_MAJOR 2
#define VERSION_MINOR 0

#include <cstdio>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

void DisplayUsageAndExit();

void InitializeAudioEndpoint(IAudioEndpointVolume** audioEndpoint);

void DestroyAudioEndpoint(IAudioEndpointVolume* endpointVolume);

void SetVolume(IAudioEndpointVolume* endpoint, float volume);

float GetVolume(IAudioEndpointVolume* endpoint);

void SetMute(IAudioEndpointVolume* endpoint, BOOL newValue);

BOOL GetMute(IAudioEndpointVolume* endpoint);
