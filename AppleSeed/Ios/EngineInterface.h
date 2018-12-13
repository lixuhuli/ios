#ifndef _ENGINE_INTERFACE_H_
#define _ENGINE_INTERFACE_H_

#include <stdint.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef OPENGLRENDER_EXPORTS
#	define OGLAPI __declspec (dllexport)
#else
#	define OGLAPI __declspec (dllimport)
#endif

#define GetCPUVTStatus					Export0000
#define RegisterEngineStatusCallback	Export0001
#define SetLicense						Export0002
#define EngineOn						Export0003
#define EngineOff						Export0004
#define	CapturePictureToFile			Export0005
#define RecordVideoToFile				Export0006
#define RecordVideoStop					Export0007
#define SetGPSCoord						Export0008
#define Shake							Export0009
#define Home							Export000a
#define SetSystemVolume					Export000b
#define MouseOpSync						Export000c
#define KeyInputSync					Export000d
#define InstallIPA						Export000e
#define UpdatePack						Export000f
#define GetEngineVersion				Export0010
#define GetInstalledAppCount			Export0011
#define GetInstalledAppInfo				Export0012
#define GetInputPosition				Export0013
#define InputString						Export0014
#define LoadKeyMap						Export0015
#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////

#define VT_STATUS_OK			0
#define VT_STATUS_NOT_SUPPPORTED	(-1)	// CPU does not have VT feature.
#define VT_STATUS_NEED_TURN_ON		(-2)	// CPU supports VT feature, but needs to be turned on in BIOS.

OGLAPI int GetCPUVTStatus();		// Use above status.

//////////////////////////////////////////////

typedef int(*FNENGINESTATUSCALLBACK)(/*void *ctx,*/ int status, uintptr_t param1, uintptr_t param2);

#define ENGINE_STATUS_SWITCHED_ON			0	// param1 not used. param2 not used. return 0. Received immediately after EngineOn()
#define ENGINE_STATUS_SWITCHED_OFF			1	// param1 not used. param2 not used. return 0. Received after the engine is successfully turn off.
#define ENGINE_STATUS_ON_PROGRESS			2	// param1 is percentage of loading, from 0-100. param2 not used. return 0.
#define ENGINE_STATUS_SCREEN_SIZE_CHANGE	3	// param1 is width. param2 is height. return 0. 
												// When the engine is ready to draw OpenGL on main screen, the message will be received for once.
												// After that, this message is received every time screen size is changed.
#define ENGINE_STATUS_FOREGROUND_APP_CHANGE	4	// Cast param1 to (const char *) to receive foreground app name, param2 not used. return 0.
#define ENGINE_STATUS_APPLICATION_ADDED		5	// Cast param1 to (const char *) to receive the installation complete notification after it has been installed.
#define ENGINE_STATUS_APPLICATION_MODIFIED	6	// Cast param1 to (const char *) to receive the re-installation complete notification after it has been re-installed.
#define ENGINE_STATUS_APPLICATION_REMOVED	7	// Cast param1 to (const char *) to receive the removal complete notification after it has been removed.

// WARNING:
// None of the callbacks are guaranteed to be called non-concurrently nor be on the same thread.
// Please use a lock of your own to make it safe.


OGLAPI void RegisterEngineStatusCallback(FNENGINESTATUSCALLBACK fnCallback);

//////////////////////////////////////////////

OGLAPI int SetLicense(const char *serial);

OGLAPI int EngineOn(HWND hDraw, const char *VMDKSystem, const char *VMDKSlave);			
									// Turn on the engine. hDraw is the HWND to the main view for OpenGL drawing. Returning 0 means OK, otherwise it fails.
OGLAPI int EngineOff();				// Turn off the engine. Returning 0 means OK, otherwise it fails.

//////////////////////////////////////////////

OGLAPI int CapturePictureToFile(const char *pathToFile);	// Screen capture to file. Returning 0 means OK, otherwise it fails.
OGLAPI int RecordVideoToFile(const char *pathToFile);		// Record video to file. Returning 0 means OK, otherwise it fails.
OGLAPI int RecordVideoStop();					// Stop recording video. Returning 0 means OK, otherwise it fails.
OGLAPI int SetGPSCoord(double latitude, double longitude);	// Set GPS Coordinate. Returning 0 means OK, otherwise it fails.
OGLAPI void Shake();						// Shake the device.
OGLAPI void Home();							// Press the home button.
OGLAPI void SetSystemVolume(int v);			// Get the volume amount, 0 - 10.
OGLAPI void MouseOpSync(const char op, const int x, const int y);
OGLAPI void KeyInputSync(const char op, WPARAM keyValue);


//////////////////////////////////////////////

OGLAPI int InstallIPA(const char *pathToFile);			// Install IPA file. Returning 0 means OK, otherwise it fails.
OGLAPI int UpdatePack(const char *pathToFile);			// Update file to update the engine. Returning 0 means OK, otherwise it fails.

OGLAPI void GetEngineVersion(int *major, int *minor, int *buildnumber);	// Get engine version.

typedef struct _IOSAPPINFO {
	char bundle_name[256];		// eg: com.tencent.smoba
	uint32_t version_number;	// A 32-bit number, the number is greater the version is newer
} IOSAPPINFO, *PIOSAPPINFO;

OGLAPI int GetInstalledAppCount();		// Get installed ios app count.
OGLAPI int GetInstalledAppInfo(PIOSAPPINFO pInfo, int buf_count);	// Use an array of IOSAPPINFO as pInfo to receive info, buf_count is the count of the array.
																	// Returning 0 means OK, otherwise it fails.

OGLAPI int GetInputPosition(uint32_t *x, uint32_t *y);				// Get the input position where the IME window should be. x, y receive the relative position
																	// to the left-top of the main window, in pixels.
																	// Returning 0 means OK, otherwise there is no current input in the current context.

OGLAPI int InputString(const char *string);							// Input the string. The string MUST be UTF-8 encoded. Returning 0 means OK.
OGLAPI int LoadKeyMap(const char *string);                          // Load key map file. The string MUST be UTF-8 encoded. Returning 0 means OK.
#ifdef __cplusplus
}
#endif

#endif
