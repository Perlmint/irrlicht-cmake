// This is the entry point for the Irrlicht test suite.
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>

// This is an MSVC pragma to link against the Irrlicht library.
// Other builds must link against it in the project files.
#if defined(_MSC_VER)
#pragma comment(lib, "Irrlicht.lib")
#endif // _MSC_VER

/* Each test must have the same signature.  Test should (but are not
 * required to) live in a .cpp file of the same name.  There is no 
 * need to #include anything since the test entry points can be
 * declared as extern before calling them.
 */
#define RUN_TEST(testEntryPoint)\
	extern bool testEntryPoint(void);\
	if(!testEntryPoint()) fails++;

//! This is the main entry point for the Irrlicht test suite.
/** \return The number of test that failed, i.e. 0 is success. */
int main()
{
	int fails = 0;

	RUN_TEST(planeMatrix);
	RUN_TEST(screenshots);
	RUN_TEST(disambiguateTextures);
	RUN_TEST(drawPixel);
	RUN_TEST(fast_atof);
	RUN_TEST(line2dIntersectWith);

	(void)printf("\nTests finished. %d test failed.\n", fails);
	
	if(0 == fails)
	{
		time_t rawtime;
		struct tm * timeinfo;
		(void)time(&rawtime);
		timeinfo = localtime(&rawtime);
		(void)printf("\nTest suite pass at %s\n", asctime(timeinfo));
	}

	return fails;
}