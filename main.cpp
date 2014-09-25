// ---------------------------------------------------------------------
// INCLUDE STATEMENTS
// ---------------------------------------------------------------------

#include <CGApp\CGApp.h>
#include <GL\CGOpenGL.h>
#include <iostream>

// Debug window
#include "PDebug.h"

using namespace std;

// ---------------------------------------------------------------------
// WIN MAIN
// ---------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int show_cmd)
{
	// Debug window instance
	PDebug debugWnd;

	// Tell Windows to terminate app if heap becomes corrupted
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Initialise COM (for Windows Imaging Component (WIC), Media Foundation and other COM features)
	HRESULT hr = CoInitialize(NULL);

	if (SUCCEEDED(hr))
	{
		// Redirect console output to the file "runlog.txt"
		FILE *consoleFile = NULL;
		freopen_s(&consoleFile, "runlog.txt", "w", stdout);

		// Initial memory report
		gu_memory_report();

		// Create main CGApp (singleton) object
		CGApp *app = new CGApp();

		if (app)
		{
			// Create main app delegate (contains OpenGL functionality)
			CGAppDelegate *appDelegate = new CGOpenGL(app);

			if (appDelegate)
			{
				// Init the debug window
				debugWnd.Initialise();

				// Initialise application with relevant delegate object. Ownership of appDelegate passed implicitly to app object so don't need to explicitly release here
				hr = app->initApp(TEXT("CGApplication"), TEXT("Jak Boulton - 10026037 - Genetic Programming"), 1000, 768, WS_OVERLAPPEDWINDOW, appDelegate);

				if (SUCCEEDED(hr))
					// Start app runloop if successfully initialised
					hr = app->startRunloop();
			}

			app->release();
		}

		// Final memory report
		gu_memory_report();

		// Redirect output back to the console
		freopen_s(&consoleFile,  "CON", "w", stdout);

		// Notify COM we're done
		CoUninitialize();
	}
}

// ---------------------------------------------------------------------