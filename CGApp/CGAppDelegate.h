
#pragma once

#include <CoreStructures\GUMemory.h>
#include <WinFoundation\WinFoundation.h>
#include <CoreStructures\GUObject.h>

// Declare abstract base class on which to derive CGApp delegate objects

class CGApp;
class CGEvent;
class CGResizeEvent;
class CGKeyEvent;
class CGCharEvent;
class CGMouseEvent;

class CGAppDelegate : public CoreStructures::GUObject {

private:

	CGApp				*host; // weak reference to CGApp host
	CGAppDelegate		*nodeDelegate; // chain nodes CGAppDelegate nodes together to combine / composite functionality.  Each CGAppDelegate owns (retains) any 'child' delegate objects

public:

	CGAppDelegate();
	CGAppDelegate(CGApp *mainAppHost);
	~CGAppDelegate();

	CGApp *getAppHost(); // Return a (weak) reference to the host CGApp object

	// create app window and return in *appWindow.  Return S_OK if app window created, E_HANDLE otherwise.  This gives CGAppDelegate a chance to create the application window - if it does not and fails (the default), CGApp will create the window.  If S_OK is returned, *appWindow is a valid HWND
	virtual HRESULT createAppWindow(LPCWSTR appName, LPCWSTR windowTitle, RECT windowRect, DWORD windowStyle, HWND *appWindow);

	// window event handlers
	virtual void onResize(CGResizeEvent *currentEvent);
	virtual void onKeyDown(CGKeyEvent *currentEvent);
	virtual void onChar(CGCharEvent *currentEvent);
	virtual void onKeyUp(CGKeyEvent *currentEvent);
	virtual void onMouseDown(CGMouseEvent *currentEvent);
	virtual void onMouseMove(CGMouseEvent *currentEvent);
	virtual void onMouseUp(CGMouseEvent *currentEvent);
	virtual void onMouseWheel(CGMouseEvent *currentEvent);
	virtual void onMouseLeave(CGMouseEvent *currentEvent);

	// initialisation (load objects, setup resources etc.)
	virtual HRESULT initialise(HWND hwnd, HDC hDC) = 0;

	// render function - called during main app (render) loop
	virtual void render(HWND hwnd, HDC hDC) = 0;

	// runloop delegation
	virtual void update(HWND hwnd) {}; // called when no events are processed in a runloop iteration
	virtual void runloopPreStart(HWND hwnd) {}; // called just before the CGApp runloop starts
	virtual void runloopEnd(HWND hwnd) {}; // send when CGApp runloop ends - signals the end of the CGApp
};

