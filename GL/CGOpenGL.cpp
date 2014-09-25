// ---------------------------------------------------------------------
// INCLUDE STATEMENTS
// ---------------------------------------------------------------------

#include <iostream>
#include <wincodec.h>
#include <CGApp\CGEvent.h>
#include <GL\CGOpenGL.h>
#include <CGApp\CGApp.h>
#include <GL\GLSLFactory.h>
#include <GL\CGTextureLoader.h>
#include <GL\CGPrincipleAxesObject.h>
#include <GL\CGTexturedQuadObject.h>
#include <CGModel\CGModel.h>
#include <Importers\CGImporters.h>

using namespace std;
using namespace CoreStructures;

// ---------------------------------------------------------------------
// CONSTRUCTOR/DECONSTRUCTOR
// ---------------------------------------------------------------------

CGOpenGL::CGOpenGL(CGApp *mainAppHost) : CGAppDelegate(mainAppHost)
{
	openGLInitialised = false;
	openGLContext = NULL;
	mainCamera = NULL;
	principleAxesObj = NULL;

	// Set app host
	appHost = mainAppHost;
}

CGOpenGL::~CGOpenGL() 
{
	glUseProgram(0);
	glBindVertexArray(0);

	if (mainCamera)
		mainCamera->release();

	if (principleAxesObj)
		principleAxesObj->release();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(openGLContext);
}

// override window creation so we can setup OpenGL context and take advantage of more advanced wgl pixel formats
HRESULT CGOpenGL::createAppWindow(LPCWSTR appName, LPCWSTR windowTitle, RECT windowRect, DWORD windowStyle, HWND *appWindow) {

	CGApp *hostApp = getAppHost();

	if (!hostApp)
		return E_HANDLE;

	HWND glWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, appName, windowTitle, windowStyle|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hostApp->appModule(), hostApp);

	if (!glWindow)
		return E_HANDLE;

	HDC hDC = GetDC(glWindow);


	PIXELFORMATDESCRIPTOR		pixelFormatDesc;
	int							pixelFormatID;
	HGLRC						glContext = NULL;

	ZeroMemory(&pixelFormatDesc, sizeof(pixelFormatDesc));
	pixelFormatDesc.nSize = sizeof(pixelFormatDesc);
	pixelFormatDesc.nVersion = 1;
	pixelFormatDesc.dwFlags	= PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDesc.cColorBits = 24;
	pixelFormatDesc.cDepthBits = 24;
	pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

	if (!(pixelFormatID = ChoosePixelFormat(hDC, &pixelFormatDesc)))
		goto return_error;

	if (!SetPixelFormat(hDC, pixelFormatID, &pixelFormatDesc))
		goto return_error;
	
	if (!(glContext = wglCreateContext(hDC)))
		goto return_error;
				
	if (!wglMakeCurrent(hDC, glContext)) {

		wglDeleteContext(glContext);
		goto return_error;
	}

	GLenum err = glewInit();
	
	if (err!=GLEW_OK) {

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(glContext);
		goto return_error;
	}

	// glew setup so dispose of temp window
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(glContext);
	ReleaseDC(glWindow, hDC);
	DestroyWindow(glWindow); // note: we have to destroy the window since attached pixel format is immutable

	// ---------------

	// create actual window that will be returned and used for main rendering
	glWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, appName, windowTitle, windowStyle|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hostApp->appModule(), hostApp);

	if (!glWindow)
		return E_HANDLE;

	hDC = GetDC(glWindow);

	int pixelAttributes[] = {

		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB, GL_FALSE,
		//WGL_SAMPLES_ARB, 2,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		0
	};

	int pixelFormat;
	UINT numPixelFormats[1];

	if (!wglChoosePixelFormatARB(hDC, &(pixelAttributes[0]), NULL, 1, &pixelFormat, numPixelFormats))
		goto return_error; // did not create pixel format successfully

	if (!SetPixelFormat(hDC, pixelFormat, &pixelFormatDesc))
		goto return_error;

	if (!wglewIsSupported("WGL_ARB_create_context"))
		goto return_error; // cannot create context using wgl extensions 

	// SETUP OPENGL 3.3 RENDERING CONTEXT (try 4.0, 4.1 or 4.2 in the labs!)
	int actualContextAttribs[] = {

		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0
	};

	if (!(glContext = wglCreateContextAttribsARB(hDC, 0, actualContextAttribs)))
		goto return_error;

	if (!wglMakeCurrent(hDC, glContext)) {

		wglDeleteContext(glContext);
		goto return_error;
	}

	ReleaseDC(glWindow, hDC);

	openGLContext = glContext;
	*appWindow = glWindow;
	
	openGLInitialised = true;


	// report OpenGL version

	int majorVersion;
	int minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";

	
	// report OpenGL extensions
	/*
	const char *glExtensionString = (const char *)glGetString(GL_EXTENSIONS);

	cout << "Extensions supported...\n\n";

	char *strEnd = (char*)glExtensionString + strlen(glExtensionString);
	char *sptr = (char*)glExtensionString;

	while (sptr < strEnd) {

		int slen = (int)strcspn(sptr, " ");
		printf("%.*s\n", slen, sptr);
		sptr += slen + 1;
	}

	cout << "\n\n";*/

	return S_OK;

	// ---------------------

return_error:

	ReleaseDC(glWindow, hDC);
	DestroyWindow(glWindow);

	return E_HANDLE;
}

// ---------------------------------------------------------------------
// EVENT HANDLING
// ---------------------------------------------------------------------

void CGOpenGL::onMouseDown(CGMouseEvent *currentEvent)
{
	oldMouseX = currentEvent->mouseXPosition();
	oldMouseY = currentEvent->mouseYPosition();
}

void CGOpenGL::onMouseMove(CGMouseEvent *currentEvent)
{
	if (currentEvent->leftButtonDown()) {

		int dx = currentEvent->mouseXPosition() - oldMouseX;
		int dy = currentEvent->mouseYPosition() - oldMouseY;
		
		if (mainCamera)
			mainCamera->transformCamera((float)-dy, (float)-dx, 0.0f);

		oldMouseX = currentEvent->mouseXPosition();
		oldMouseY = currentEvent->mouseYPosition();
	}
}

void CGOpenGL::onMouseWheel(CGMouseEvent *currentEvent)
{
	if (mainCamera) {
		
		if (currentEvent->wheelDelta()<0)
			mainCamera->scaleCameraRadius(1.1f);
		else if (currentEvent->wheelDelta()>0)
			mainCamera->scaleCameraRadius(0.9f);
	}
}

void CGOpenGL::onKeyDown(CGKeyEvent *currentEvent)
{
	// put keydown handler code here.  Call currentEvent->setEventHandled() to tell CGApp that you've dealt with the onKeyDown event
}

void CGOpenGL::onKeyUp(CGKeyEvent *currentEvent)
{
	// put keyup handler code here.  Call currentEvent->setEventHandled() to tell CGApp that you've dealt with the onKeyUp event
	if(currentEvent->keyCode() == VK_SPACE)
	{
		controller.runAI();
		currentEvent->setEventHandled();
	}
	else if(currentEvent->keyCode() == 'N')
	{
		controller.refreshPop();
		currentEvent->setEventHandled();
	}
	else if(currentEvent->keyCode() == VK_LEFT || currentEvent->keyCode() == VK_RIGHT)
	{
		controller.swapAI();
		currentEvent->setEventHandled();
	}
}

// ---------------------------------------------------------------------
// Update Function - Calls main game loop to update and render objects
// ---------------------------------------------------------------------

void CGOpenGL::update(HWND hwnd)
{
	HDC hDC = GetDC(hwnd);

	// Update the timer (to get frame delta)
	delta = appHost->getAppClock()->gameTimeDelta();

	// Call update
	controller.Update(delta);

	// Render
	render(hwnd, hDC);

	// Release Device Context
	ReleaseDC(hwnd, hDC);
}

// ---------------------------------------------------------------------
// OPENGL INITIALISATION - Called once at app startup - put relevant setup code here.  Return S_OK if initialisation succeeds, E_FAIL otherwise
// ---------------------------------------------------------------------

HRESULT CGOpenGL::initialise(HWND hwnd, HDC hDC) {

	if (!openGLInitialised)
		return E_FAIL;

	CGApp *app = getAppHost();

	// Initialise texture loader - need to do this before we use the (static) public interface on this class
	CGTextureLoader::initialise(app);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Disable vertical sync (may introduce tearing - comment this out if you want to re-enable vertical sync)
	// BOOL swapIntervalSet = wglSwapIntervalEXT(0);

	// Setup main camera
	RECT viewportRect;
	GetClientRect(hwnd, &viewportRect);
	float viewportAspect = (float)(viewportRect.right - viewportRect.left) / (float)(viewportRect.bottom - viewportRect.top);
	mainCamera = new GUPivotCamera(-90.0f, 0.0f, 20.0f, 55.0f, viewportAspect, 0.1f);

	// ---------------------------------------------------------------------
	// OBJECT INITIALISATION
	// ---------------------------------------------------------------------

	principleAxesObj = new CGPrincipleAxesObject();
	controller.Initialise();

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	// Notify CGApp initialisation was successful
	return S_OK;
}

// ---------------------------------------------------------------------
// RENDER FUNCTION - Called in main app (render) loop
// ---------------------------------------------------------------------

void CGOpenGL::render(HWND hwnd, HDC hDC) {

	// Check for OpenGL init completion
	if (!openGLInitialised)
		return;

	// Clear the screen
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Get view-projection transform as a CGMatrix4_4
	// NOTE: These examples have no modelling transform - so they're not moved (translated), rotated or scaled from their modelling coorindates
	GUMatrix4 T = mainCamera->projectionTransform() * mainCamera->viewTransform();
	
	// ---------------------------------------------------------------------
	// RENDERING OF OBJECTS
	// ---------------------------------------------------------------------

	if (principleAxesObj)
		principleAxesObj->render(T);

	// Application render
	controller.Render(T);

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	// Set program to nothing
	glUseProgram(0);

	// Display the new frame from the backbuffer
	SwapBuffers(hDC);
}


