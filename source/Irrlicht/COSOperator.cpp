// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COSOperator.h"
#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_
#include <windows.h>
#else
#include <string.h>
#endif

#ifdef MACOSX
#include "OSXClipboard.h"
#endif

namespace irr
{


// constructor
COSOperator::COSOperator(const c8* osVersion)
{
	OperationSystem = osVersion;
}


//! destructor
COSOperator::~COSOperator()
{
}


//! returns the current operation system version as string.
const wchar_t* COSOperator::getOperationSystemVersion()
{
	return OperationSystem.c_str();
}


//! copies text to the clipboard
void COSOperator::copyToClipboard(const c8* text)
{
	if (strlen(text)==0)
		return;

// Windows version
#if defined(_IRR_WINDOWS_)
	if (!OpenClipboard(0) || text == 0)
		return;

	EmptyClipboard();

	HGLOBAL clipbuffer;
	char * buffer;

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(text)+1);
	buffer = (char*)GlobalLock(clipbuffer);

	strcpy(buffer, text);

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

// MacOSX version
#elif defined(MACOSX)

	OSXCopyToClipboard(text);

// todo: Linux version
#endif
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
c8* COSOperator::getTextFromClipboard()
{
#if defined(_IRR_WINDOWS_)
	if (!OpenClipboard(NULL))
		return 0;
	
	char * buffer = 0;

	HANDLE hData = GetClipboardData( CF_TEXT );
	buffer = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	CloseClipboard();
	return buffer;

#elif defined(MACOSX)
	return (OSXCopyFromClipboard());
#else

// todo: Linux version

	return 0;
#endif
}


bool COSOperator::getProcessorSpeedMHz(irr::u32* MHz)
{
#if defined(_IRR_WINDOWS_)
	LONG Error;
	
	HKEY Key;
	Error = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
			0, KEY_READ, &Key);

	if(Error != ERROR_SUCCESS)
		return false;

	DWORD Speed = 0;
	DWORD Size = sizeof(Speed);
	Error = RegQueryValueEx(Key, "~MHz", NULL, NULL, (LPBYTE)&Speed, &Size);

	RegCloseKey(Key);

	if (Error != ERROR_SUCCESS)
		return false;
	else if (MHz)
		*MHz = Speed;
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return true;

#elif defined(LINUX)
	// could probably read from "/proc/cpuinfo" or "/proc/cpufreq" also
	struct clockinfo CpuClock;
	size_t Size = sizeof(clockinfo);

	if (!sysctlbyname("kern.clockrate", 2, &CpuClock, &Size, NULL, 0))
		return false;
	else if (MHz)
		*MHz = CpuClock.hz;
	return true;
#else

// todo: MacOSX version

	return false;
#endif
}

bool COSOperator::getSystemMemory(irr::u32* Total, irr::u32* Avail)
{
#if defined(_IRR_WINDOWS_)
	MEMORYSTATUS MemoryStatus;
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

	// cannot fail
	GlobalMemoryStatus(&MemoryStatus);

	if (Total)
		*Total = (irr::u32)MemoryStatus.dwTotalPhys;
	if (Avail)
		*Avail = (irr::u32)MemoryStatus.dwAvailPhys;
	
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return true;

#elif defined(LINUX)
	// could probably read from "/proc/meminfo"
	struct sysinfo SystemStatus;
	if (sysinfo(&SystemStatus) != 0)
		return false;

	if (Total)
		*Total = SystemStatus.totalram;
	if (Avail)
		*Avail = SystemStatus.freeram;
	return true;

#else

// todo: MacOSX version

	return false;
#endif
}


} // end namespace

