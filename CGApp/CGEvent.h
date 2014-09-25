
#pragma once

#include <CoreStructures\GUMemory.h>
#include <WinFoundation\WinFoundation.h>
#include <CoreStructures\GUObject.h>

//
// Implement different event models
//

// Base event model

class CGEvent : public CoreStructures::GUObject {

private:

	HWND			hwnd;
	HDC				hDC;

	BOOL			eventHandled;

public:

	CGEvent(HWND _hwnd, HDC _hDC) : hwnd(_hwnd), hDC(_hDC), eventHandled(FALSE) {}

	// Accessor methods

	// set event handled.  Note:  Once set, we cannot unset the event handled status
	void setEventHandled() {

		eventHandled = TRUE;
	}

	BOOL isEventHandled() {

		return eventHandled;
	}


	// read-only accessors
	
	HWND windowHandle() {

		return hwnd;
	}

	HDC deviceContext() {

		return hDC;
	}

};



class CGResizeEvent : public CGEvent {

private:

	short w, h; // new client area after resize

public:

	CGResizeEvent(HWND _hwnd, HDC _hDC, short newWidth, short newHeight) : CGEvent(_hwnd, _hDC), w(newWidth), h(newHeight) {}

	// Accessor methods

	short width() {

		return w;
	}

	short height() {

		return h;
	}
};


class CGKeyEvent : public CGEvent {

private:

	WPARAM			key;

public:

	CGKeyEvent(HWND _hwnd, HDC _hDC, WPARAM _keyCode) : CGEvent(_hwnd, _hDC), key(_keyCode) {}

	// Accessor methods

	WPARAM keyCode() {

		return key;
	}
};


// Event to handle WM_CHAR - Unicode Transformation Format (UTF) 16
class CGCharEvent : public CGEvent {

private:

	WPARAM			charCode;

public:

	CGCharEvent(HWND _hwnd, HDC _hDC, WPARAM _charCode) : CGEvent(_hwnd, _hDC), charCode(_charCode) {}

	// Accessor methods

	WPARAM characterCode() {

		return charCode;
	}
};



// CGMouseEvent contains the current state of the mouse
class CGMouseEvent : public CGEvent {

private:

	WPARAM			buttonState; // store button and modifier key state
	int				mouseX, mouseY;
	short			wheelZDelta, wheelXDelta;

public:

	// initialButtonState set from wParam directly. This contains the relevant button and modifier key status - see MSDN WM_LBUTTONDOWN page for table of flags on which CGMouseEvent is based.

	CGMouseEvent(HWND _hwnd, HDC _hDC, WPARAM initialButtonState, int _x, int _y) : CGEvent(_hwnd, _hDC), buttonState(initialButtonState), mouseX(_x), mouseY(_y), wheelZDelta(0), wheelXDelta(0) {}


	CGMouseEvent(HWND _hwnd, HDC _hDC, WPARAM initialButtonState, int _x, int _y, short _wheelZDelta, short _wheelXDelta) : CGEvent(_hwnd, _hDC), buttonState(initialButtonState), mouseX(_x), mouseY(_y), wheelZDelta(_wheelZDelta), wheelXDelta(_wheelXDelta) {}


	// Accessor methods

	BOOL leftButtonDown() {

		return (buttonState & MK_LBUTTON) == MK_LBUTTON;
	}

	BOOL rightButtonDown() {

		return (buttonState & MK_RBUTTON) == MK_RBUTTON;
	}

	BOOL middleButtonDown() {

		return (buttonState & MK_MBUTTON) == MK_MBUTTON;
	}

	BOOL ctrlKeyPressed() {

		return (buttonState & MK_CONTROL) == MK_CONTROL;
	}

	BOOL shiftKeyPressed() {

		return (buttonState & MK_SHIFT) == MK_SHIFT;
	}

	int mouseXPosition() {

		return mouseX;
	}

	int mouseYPosition() {

		return mouseY;
	}

	short wheelDelta() {

		return wheelZDelta;
	}

	short wheelHorizontalDelta() {

		return wheelXDelta;
	}

};