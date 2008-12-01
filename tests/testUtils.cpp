// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

#define _CRT_SECURE_NO_WARNINGS

#include "testUtils.h"
#include <memory.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#if defined(_MSC_VER) && !defined(NDEBUG)
#include <windows.h>
#endif // #if defined(_MSC_VER) && !defined(NDEBUG)

bool binaryCompareFiles(const char * fileName1, const char * fileName2)
{
	assert(fileName1);
	assert(fileName2);
	if(!fileName1 || !fileName2)
		return false;

	FILE * file1 = fopen(fileName1, "rb");
	if(!file1)
	{
		(void)printf("binaryCompareFiles: File '%s' cannot be opened\n", fileName1);
		assert(file1);
		return false;
	}

	FILE * file2 = fopen(fileName2, "rb");
	if(!file2)
	{
		(void)printf("binaryCompareFiles: File '%s' cannot be opened\n", fileName2);
		(void)fclose(file1);
		assert(file2);
		return false;
	}


	(void)fseek(file1, 0, SEEK_END);
	(void)fseek(file2, 0, SEEK_END);
	if(ftell(file1) != ftell(file2))
	{
		(void)printf("binaryCompareFiles: Files are different sizes\n");
		(void)fclose(file1);
		(void)fclose(file2);
		return false;
	}

	(void)fseek(file1, 0, SEEK_SET);
	(void)fseek(file2, 0, SEEK_SET);

	char file1Buffer[8196];
	char file2Buffer[8196];

	while(!feof(file1))
	{
		if(feof(file2)
			||(fread(file1Buffer, sizeof(file1Buffer), 1, file1) !=
			fread(file2Buffer, sizeof(file2Buffer), 1, file2)))
		{
			(void)printf("binaryCompareFiles: Error during file reading\n");
			break;
		}

		if(memcmp(file1Buffer, file2Buffer, sizeof(file1Buffer)))
		{
			(void)printf("binaryCompareFiles: Error during file reading\n");
			break;
		}
	}

	bool filesAreIdentical = feof(file1) && feof(file2);
	(void)fclose(file1);
	(void)fclose(file2);

	return filesAreIdentical;
}

bool takeScreenshotAndCompareAgainstReference(irr::video::IVideoDriver * driver, const char * fileName)
{
	irr::video::IImage * screenshot = driver->createScreenShot();
	if(screenshot)
	{
		irr::core::stringc driverName = driver->getName();
		// For OpenGL (only), chop the version number out. Other drivers have more stable version numbers.
		if(driverName.find("OpenGL") > -1)
			driverName = "OpenGL";

		irr::core::stringc filename = "results/";
		filename += driverName;
		filename += fileName;
		bool written = driver->writeImageToFile(screenshot, filename.c_str());
		screenshot->drop();

		if(!written)
		{
			(void)printf("Failed to write screenshot to file '%s'\n", filename.c_str());
			return false;
		}

		irr::core::stringc referenceFilename = "media/";
		referenceFilename += driverName;
		referenceFilename += fileName;
		return binaryCompareFiles(filename.c_str(), referenceFilename.c_str());
	}
	else
	{
		(void)printf("Failed to take screenshot\n");
	}

	return false;
}

static FILE * logFile = 0;

bool openTestLog(const char * filename)
{
	closeTestLog();

	logFile = fopen(filename, "w");
	assert(logFile);
	if(!logFile)
		logTestString("\nWARNING: unable to open the test log file %s\n", filename);
	
	return (logFile != 0);
}

void closeTestLog(void)
{
	if(logFile)
		(void)fclose(logFile);
}


void logTestString(const char * format, ...)
{
	char logString[1024];

	va_list arguments;
	va_start(arguments, format);
	vsprintf(logString, format, arguments);
	va_end(arguments);

	(void)printf(logString);
	if(logFile)
	{
		(void)fprintf(logFile, logString);
		(void)fflush(logFile);
	}

#if defined(_MSC_VER) && !defined(NDEBUG)
	OutputDebugStringA(logString);
#endif // #if defined(_MSC_VER) && !defined(NDEBUG)
}