
#pragma once

// Include
#include <GL\glew\glew.h>
#include <GL\glew\wglew.h>
#include <CGApp\CGAppDelegate.h>
#include <CoreStructures\GUMatrix4.h>
#include <CoreStructures\GUPivotCamera.h>

#include "PController.h"

class CGMouseEvent;
class CGPrincipleAxesObject;
class CGTexturedQuadObject;
class CGModel;

// ---------------------------------------------------------------------
// OPENGL Class - Derived from CGAppDelegate
// ---------------------------------------------------------------------

class CGOpenGL : public CGAppDelegate
{

private:

	bool							openGLInitialised; // flag to determine if OpenGL is setup okay (if not, we don't do any rendering)

	HGLRC							openGLContext; // main OpenGL rendering context

	CoreStructures::GUPivotCamera	*mainCamera;

	int								oldMouseX, oldMouseY; // event tracking attributes

	// App host (Used to update clock)
	CGApp*							appHost;

	// Delta
	float							delta;

	//
	// Objects
	//

	CGPrincipleAxesObject			*principleAxesObj;

	// PROJECT CONTROLLER
	PController						controller;

public:

	CGOpenGL(CGApp *mainAppHost);
	~CGOpenGL();

	// override window creation so we can access more advanced wgl features via glew
	HRESULT createAppWindow(LPCWSTR appName, LPCWSTR windowTitle, RECT windowRect, DWORD windowStyle, HWND *appWindow);

	// event handling
	void onMouseDown(CGMouseEvent *currentEvent);
	void onMouseMove(CGMouseEvent *currentEvent);
	void onMouseWheel(CGMouseEvent *currentEvent);
	void onKeyDown(CGKeyEvent *currentEvent);
	void onKeyUp(CGKeyEvent *currentEvent);

	void update(HWND hwnd);

	// OpenGL initialisation - called once at app startup - put relevant setup code here.  Return S_OK if initialisation succeeds, E_FAIL otherwise
	HRESULT initialise(HWND hwnd, HDC hDC);

	// rendering function - called in main app (render) loop
	void render(HWND hwnd, HDC hDC);

};
