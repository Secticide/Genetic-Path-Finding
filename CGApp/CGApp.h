#pragma once

#include <CoreStructures\GUMemory.h>
#include <WinFoundation\WinFoundation.h>
#include <wincodec.h>
#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUClock.h>

class CGAppDelegate;

// Principle window / application class.  This is a singleton used to model a single windowed application

class CGApp : public CoreStructures::GUObject {

private:

	HMODULE						module; // app module
	WNDCLASSEX					wndclass; // default CGApp window class

	HWND						appWindow;
	CGAppDelegate				*appDelegate; // main rendering functionality (strong reference)
	CoreStructures::GUClock		*appClock;

	IWICImagingFactory			*wicFactory; // provide image loading services

public:

	CGApp();
	~CGApp();

	virtual HRESULT initApp(LPCWSTR appName, LPCWSTR windowTitle, int w, int h, DWORD windowStyle, CGAppDelegate *applicationDelegate); // setup main window and associate rendering delegate.  Implicit retain is made on the delegate object (we store the reference to the delegate but no explicit retain is called - this means the caller does not have to release the delegate object explicitly)

	virtual HRESULT startRunloop();
	
	// event handler for which all events except WM_CREATE are handled
	LRESULT processMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	// CGApp accessor methods
	HMODULE &appModule();
	WNDCLASSEX &appWndClass();
	const CoreStructures::GUClock *getAppClock(); // return const weak reference to contained application timer / clock

	// WIC services provided by CGApp
	HRESULT getWICFormatConverter(IWICFormatConverter **formatConverter);
	HRESULT loadWICBitmap(LPCWSTR filepath, IWICBitmap **bitmap);

private:

	// default message handler - delegate events to app delegate object
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

