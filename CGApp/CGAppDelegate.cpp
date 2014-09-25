
#include <CGApp\CGApp.h>
#include <CGApp\CGAppDelegate.h>
#include <CGApp\CGEvent.h>

using namespace std;
using namespace CoreStructures;

CGAppDelegate::CGAppDelegate() : host(NULL), nodeDelegate(NULL) {}

CGAppDelegate::CGAppDelegate(CGApp *mainAppHost) {
	
	host = mainAppHost; // store weak reference to main app host

	nodeDelegate = NULL;
}

CGAppDelegate::~CGAppDelegate() {

	if (nodeDelegate)
		nodeDelegate->release();
}


// Return a reference to the host object (weak reference only)
CGApp* CGAppDelegate::getAppHost() {

	return host;
}


// initialise app window and return in *appWindow.  Return S_OK if app window created, E_HANDLE otherwise.  This gives CGAppDelegate a chance to create the application window - if it does not and fails (the default), CGApp will create the window
HRESULT CGAppDelegate::createAppWindow(LPCWSTR appName, LPCWSTR windowTitle, RECT windowRect, DWORD windowStyle, HWND *appWindow) {

	return E_HANDLE;
}


// event handlers
void CGAppDelegate::onResize(CGResizeEvent *currentEvent) {

	if (nodeDelegate)
		nodeDelegate->onResize(currentEvent); 
}

void CGAppDelegate::onKeyDown(CGKeyEvent *currentEvent) {
	
	if (nodeDelegate)
		nodeDelegate->onKeyDown(currentEvent); 
}

void CGAppDelegate::onChar(CGCharEvent *currentEvent) {
	
	if (nodeDelegate)
		nodeDelegate->onChar(currentEvent); 
}

void CGAppDelegate::onKeyUp(CGKeyEvent *currentEvent) {
	
	if (nodeDelegate)
		nodeDelegate->onKeyUp(currentEvent); 
}

void CGAppDelegate::onMouseDown(CGMouseEvent *currentEvent) {
	
	if (nodeDelegate)
		nodeDelegate->onMouseDown(currentEvent); 
}

void CGAppDelegate::onMouseMove(CGMouseEvent *currentEvent) {
	
	if (nodeDelegate)
		nodeDelegate->onMouseMove(currentEvent); 
}

void CGAppDelegate::onMouseUp(CGMouseEvent *currentEvent) {
	
	if (nodeDelegate)
		nodeDelegate->onMouseUp(currentEvent); 
}

void CGAppDelegate::onMouseWheel(CGMouseEvent *currentEvent) {

	if (nodeDelegate)
		nodeDelegate->onMouseWheel(currentEvent);
}

void CGAppDelegate::onMouseLeave(CGMouseEvent *currentEvent) {

	if (nodeDelegate)
		nodeDelegate->onMouseLeave(currentEvent);
}
