
#include <CGApp\CGApp.h>
#include <CGApp\CGAppDelegate.h>
#include <CGApp\CGEvent.h>

using namespace std;
using namespace CoreStructures;

// Static / class functions
LRESULT CALLBACK CGApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	if (message == WM_CREATE)
	{

		// associate host app (this) with window - this is so we can delegate subsequent events to the relevant application object.
        LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
        CGApp *app = (CGApp *)createStruct->lpCreateParams;

        ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(app));

        return 1;
    
	}
	else
	{

		CGApp *app = reinterpret_cast<CGApp *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
		
		if (app)
			return app->processMessage(hwnd, message, wParam, lParam);
		else
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
}


CGApp::CGApp() {

	appWindow = NULL;
	appDelegate = NULL;
	appClock = NULL;
	wicFactory = NULL;
}


CGApp::~CGApp() {

	if (appDelegate)
		appDelegate->release();

	if (appClock)
		appClock->release();

	SafeRelease(&wicFactory);
}


HRESULT CGApp::initApp(LPCWSTR appName, LPCWSTR windowTitle, int w, int h, DWORD windowStyle, CGAppDelegate *applicationDelegate) {

	module = GetModuleHandle(NULL);
	
	// setup default window class properties
	
	ZeroMemory(&wndclass, sizeof(wndclass));

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= CGApp::WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= module;
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL, IDC_CROSS);
	wndclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= appName;
	wndclass.hIconSm		= NULL;

	if (!RegisterClassEx(&wndclass))
		return E_FAIL;

	// setup the RECT describing the required size of the client area.  Call AdjustWindowRect() so the updated windowRect frames the entire window, including the title bar and window edges, while leaving the client area the required size

	RECT windowRect;

	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = w;
	windowRect.bottom = h;

	AdjustWindowRect(&windowRect, windowStyle, false);
	
	// give delegate a chance to create the application window - if it returns FALSE then CGApp creates a default window
	if (!applicationDelegate || !SUCCEEDED(applicationDelegate->createAppWindow(appName, windowTitle, windowRect, windowStyle, &appWindow))) {

		// delegate did not create app window so CGApp creates default window

		// create main window - pass last parameter as pointer to class so window associated with class and appropriate message handler method called
		appWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, appName, windowTitle, windowStyle|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, 0, w, h, NULL, NULL, module, this);

	}

	if (!appWindow)
		return E_FAIL;


	// initialise services (do this here in case delegate initialisation requires use of services provided by CGApp)

	// create WIC factory
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));

	// initialise app delegate
	if (applicationDelegate) {

		appDelegate = applicationDelegate; // implicit retain
		
		HDC hDC = GetDC(appWindow);
		appDelegate->initialise(appWindow, hDC);
		ReleaseDC(appWindow, hDC);
	}

	// show window
	ShowWindow(appWindow, SW_SHOWNORMAL);
	UpdateWindow(appWindow);
	SetFocus(appWindow);

	return S_OK;
}


HRESULT CGApp::startRunloop() {

	MSG						message;

	// initialise app clock
	appClock = new GUClock();

	if (appDelegate)
		appDelegate->runloopPreStart(appWindow);

	while(1)
	{
		
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			
			if (message.message == WM_QUIT)
				break;

			TranslateMessage(&message);
			DispatchMessage(&message);
		
		}
		else
		{

			// clock update
			appClock->tick();

			// if no events are pending, call the app delegate's onNullEvent method
			if (appDelegate)
				appDelegate->update(appWindow);
		}
	}

	// runloop ending (signals end of application)
	if (appDelegate)
		appDelegate->runloopEnd(appWindow);

	appClock->stop();
	appClock->reportTimingData();
	
	return S_OK;
}


LRESULT CGApp::processMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static BOOL mouseCaptured = FALSE;

	BOOL eventHandled = FALSE;

	switch(message)
	{
		case WM_CLOSE: // CGApp directly handles WM_CLOSE

			PostQuitMessage(0);
			return 0;


		case WM_PAINT: // CGApp directly handles WM_PAINT

			if (appDelegate) {

				HDC hDC = GetDC(hwnd);
				appDelegate->render(hwnd, hDC);
				ReleaseDC(hwnd, hDC);
			}

			RECT rect;
			GetClientRect(hwnd, &rect);
			ValidateRect(hwnd, &rect);
			
			return 0;


		case WM_SIZE: // delegate WM_SIZE to appDelegate if present, otherwise pass the event onto the default windows event handler

			if (appDelegate) {

				// get new client area dimensions
				short w = lParam & 0xFFFF;
				short h = (lParam & 0xFFFF0000) >> 16;

				HDC hDC = GetDC(hwnd);
				CGResizeEvent newEvent = CGResizeEvent(hwnd, hDC, w, h);

				appDelegate->onResize(&newEvent);

				eventHandled = newEvent.isEventHandled();

				ReleaseDC(hwnd, hDC);
			}

			if (eventHandled)
				return 0;
			else
				return DefWindowProc(hwnd, message, wParam, lParam);
		

		case WM_KEYDOWN:

			// check VK_ESCAPE
			if (wParam == VK_ESCAPE) {

				PostQuitMessage(0);
				return 0;
			
			} else {

				if (appDelegate) { // delegate WM_KEYDOWN to appDelegate if present, otherwise pass the event onto the default windows event handler

					HDC hDC = GetDC(hwnd);
					CGKeyEvent newEvent = CGKeyEvent(hwnd, hDC, wParam);

					appDelegate->onKeyDown(&newEvent);

					eventHandled = newEvent.isEventHandled();

					ReleaseDC(hwnd, hDC);
				}

				if (eventHandled)
					return 0;
				else
					return DefWindowProc(hwnd, message, wParam, lParam);
			}


		case WM_CHAR:

			// Handle WM_CHAR - Unicode Transformation Format (UTF) 16

			if (appDelegate) { // delegate WM_CHAR to appDelegate if present, otherwise pass the event onto the default windows event handler

				HDC hDC = GetDC(hwnd);
				CGCharEvent newEvent = CGCharEvent(hwnd, hDC, wParam);

				appDelegate->onChar(&newEvent);

				eventHandled = newEvent.isEventHandled();

				ReleaseDC(hwnd, hDC);
			}

			if (eventHandled)
				return 0;
			else
				return DefWindowProc(hwnd, message, wParam, lParam);


		case WM_KEYUP:

			if (appDelegate) { // delegate WM_KEYUP to appDelegate if present, otherwise pass the event onto the default windows event handler

				HDC hDC = GetDC(hwnd);
				CGKeyEvent newEvent = CGKeyEvent(hwnd, hDC, wParam);

				appDelegate->onKeyUp(&newEvent);

				eventHandled = newEvent.isEventHandled();

				ReleaseDC(hwnd, hDC);
			}

			if (eventHandled)
				return 0;
			else
				return DefWindowProc(hwnd, message, wParam, lParam);



		// Mouse input events

		// Mouse down events
		case WM_LBUTTONDOWN: // left button click
		case WM_LBUTTONDBLCLK: // left button double click
		case WM_RBUTTONDOWN: // right button click
		case WM_RBUTTONDBLCLK: // right button double click
		case WM_MBUTTONDOWN: // middle mouse button
		case WM_MBUTTONDBLCLK: // middle mouse doule click

			// CGApp directly handles mouse down events

			SetCapture(hwnd);
			mouseCaptured = TRUE;

			if (appDelegate) {

				HDC hDC = GetDC(hwnd);

				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam);

				CGMouseEvent newEvent = CGMouseEvent(hwnd, hDC, wParam, xPos, yPos);

				appDelegate->onMouseDown(&newEvent);
				
				ReleaseDC(hwnd, hDC);
			}
			
			return 0;
			


		case WM_MOUSEMOVE: // CGApp directly handles WM_MOUSEMOVE

			if (appDelegate) {

				HDC hDC = GetDC(hwnd);

				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam);

				CGMouseEvent newEvent = CGMouseEvent(hwnd, hDC, wParam, xPos, yPos);

				appDelegate->onMouseMove(&newEvent);

				ReleaseDC(hwnd, hDC);
			}

			return 0;


		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			
			// CGApp directly handles mouse up events

			if (appDelegate) {

				HDC hDC = GetDC(hwnd);

				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam);

				// Note: MSDN documentation does not list the wParam flag for the associated released button in the flags table.  It is assumed this is set to 0.  CHECK THIS!!!
				CGMouseEvent newEvent = CGMouseEvent(hwnd, hDC, wParam, xPos, yPos);

				appDelegate->onMouseUp(&newEvent);

				ReleaseDC(hwnd, hDC);
			}
			
			if (mouseCaptured) {

				ReleaseCapture();
				mouseCaptured = FALSE;
			}

			return 0;


		case WM_MOUSEWHEEL: // CGApp directly handles WM_MOUSEWHEEL

			if (appDelegate) {

				HDC hDC = GetDC(hwnd);

				short wheelZDelta = GET_WHEEL_DELTA_WPARAM(wParam);
				
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam);

				CGMouseEvent newEvent = CGMouseEvent(hwnd, hDC, wParam, xPos, yPos, wheelZDelta, 0);

				appDelegate->onMouseWheel(&newEvent);
				
				ReleaseDC(hwnd, hDC);
			}

			return 0;


		case WM_MOUSEHWHEEL: // CGApp directly handles WM_MOUSEHWHEEL (horizontal movement of mouse wheel)
			
			if (appDelegate) {

				HDC hDC = GetDC(hwnd);

				short wheelXDelta = GET_WHEEL_DELTA_WPARAM(wParam);
				
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam);

				CGMouseEvent newEvent = CGMouseEvent(hwnd, hDC, wParam, xPos, yPos, 0, wheelXDelta);

				appDelegate->onMouseWheel(&newEvent);

				ReleaseDC(hwnd, hDC);
			}

			return 0;

	
		case WM_MOUSELEAVE: // delegate WM_MOUSELEAVE to appDelegate if present, otherwise pass the event onto the default windows event handler
			
			if (appDelegate) {

				HDC hDC = GetDC(hwnd);

				CGMouseEvent newEvent = CGMouseEvent(hwnd, hDC, 0, 0, 0, 0, 0);

				appDelegate->onMouseLeave(&newEvent);
				
				eventHandled = newEvent.isEventHandled();

				ReleaseDC(hwnd, hDC);
			}

			if (eventHandled)
				return 0;
			else
				return DefWindowProc(hwnd, message, wParam, lParam);
				

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}


// CGApp accessor methods
HMODULE& CGApp::appModule() {

	return module;
}


WNDCLASSEX& CGApp::appWndClass() {

	return wndclass;
}


const GUClock *CGApp::getAppClock() {

	return appClock;
}


//
// Services provided by CGApp
//


// WIC services provided by CGApp

HRESULT CGApp::getWICFormatConverter(IWICFormatConverter **formatConverter) {
	
	if (!formatConverter || !wicFactory)
		return E_FAIL;
	else
		return wicFactory->CreateFormatConverter(formatConverter);
}


// Load and return an IWICBitmap interface representing the image loaded from path.  No format conversion is done here - this is left to the caller so each delegate can apply the loaded image data as needed.
HRESULT CGApp::loadWICBitmap(LPCWSTR path, IWICBitmap **bitmap) {

	if (!bitmap || !wicFactory)
		return E_FAIL;

	IWICBitmapDecoder *bitmapDecoder = NULL;
	IWICBitmapFrameDecode *imageFrame = NULL;
	IWICFormatConverter *formatConverter = NULL;
	
	*bitmap = NULL;

	// create image decoder
	HRESULT hr = wicFactory->CreateDecoderFromFilename(path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapDecoder);
	
	// validate number of frames
	
	UINT numFrames = 0;

	if (SUCCEEDED(hr)) {

		hr = bitmapDecoder->GetFrameCount(&numFrames);
	}

	if (SUCCEEDED(hr) && numFrames>0) {

		// decode first image frame (default to first frame - for animated types add parameters to select frame later!)
		hr = bitmapDecoder->GetFrame(0, &imageFrame);
	}

	if (SUCCEEDED(hr)) {

		hr = wicFactory->CreateFormatConverter(&formatConverter);
	}

	WICPixelFormatGUID pixelFormat;

	if (SUCCEEDED(hr)) {

		// check we can convert to the required format GUID_WICPixelFormat32bppPBGRA			
		hr = imageFrame->GetPixelFormat(&pixelFormat);
	}

	BOOL canConvert = FALSE;

	if (SUCCEEDED(hr)) {

		hr = formatConverter->CanConvert(pixelFormat, GUID_WICPixelFormat32bppPBGRA, &canConvert);
	}

	if (SUCCEEDED(hr) && canConvert==TRUE) {
		
		hr = formatConverter->Initialize(
		imageFrame,						// Input source to convert
		GUID_WICPixelFormat32bppPBGRA,	// Destination pixel format
		WICBitmapDitherTypeNone,		// Specified dither pattern
		NULL,							// Specify a particular palette 
		0.f,							// Alpha threshold
		WICBitmapPaletteTypeCustom		// Palette translation type
		);
	}

	if (SUCCEEDED(hr)) {

		// convert and create bitmap from converter
		hr = wicFactory->CreateBitmapFromSource(formatConverter, WICBitmapCacheOnDemand, bitmap);
	}


	// cleanup
	SafeRelease(&formatConverter);
	SafeRelease(&imageFrame);
	SafeRelease(&bitmapDecoder);

	// return result
	return hr;
}

