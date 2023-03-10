// ---------------------------------------------------------------------------
#pragma comment (lib, "SOIL.lib")       // x32 - SOIL.lib, x53 - SOIL.a
#include <tchar.h>
#include <vcl.h>
#include <windows.h>    // Header file for windows
#include <math.h>	// For the Sin() function
#include <stdio.h>      // Header file for standard Input/Output
#include <gl\gl.h>      // Header file for the OpenGL32 library
#include <gl\glu.h>     // Header file for the GLu32 library
#include "SOIL.h"
#pragma hdrstop
// ---------------------------------------------------------------------------
#pragma argsused

HGLRC hRC = NULL; // Permanent rendering context
HDC hDC = NULL; // Private GDI device context
HWND hWnd = NULL; // Holds our window handle
HINSTANCE hInstance = NULL; // Holds the instance of the application

bool keys[256]; // Array used for the keyboard routine
bool active = true; // Window active flag set to TRUE by default
bool fullscreen = false; // Fullscreen flag set to fullscreen mode by default

GLfloat xrot; // X rotation
GLfloat yrot; // Y rotation
GLfloat zrot; // Z rotation

GLuint texture[1]; // Storage for one texture

float points[45][45][3]; // The array for the points on the grid of our "Wave"
int wiggle_count = 0; // Counter used to control how fast flag waves
GLfloat hold; // Temporarily holds a floating point value

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Declaration for WndProc

int LoadGLTextures() // Load Bitmaps And Convert To Textures
{
	/* load an image file directly as a new OpenGL texture */
	texture[0] = SOIL_load_OGL_texture("../../Data/Tim.bmp", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	if (texture[0] == 0)
		return false;

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true; // Return Success
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
	// Resize and initialize the GL window
{
	if (height == 0) // Prevent A Divide By Zero By
	{
		height = 1; // Making height equal One
	}

	glViewport(0, 0, width, height); // Reset the current viewport

	glMatrixMode(GL_PROJECTION); // Select the projection matrix
	glLoadIdentity(); // Reset the projection matrix

	// Calculate the aspect ratio of the window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW); // Select the modelview matrix
	glLoadIdentity(); // Reset the modelview matrix
}

int InitGL(void) // All setup for OpenGL goes here
{
	if (!LoadGLTextures()) // Jump to texture loading routine
	{
		return false; // If texture didn't load return FALSE
	}

	glEnable(GL_TEXTURE_2D); // Enable texture mapping

	glShadeModel(GL_SMOOTH); // Enable smooth shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Black background
	glClearDepth(1.0f); // Depth buffer setup
	glEnable(GL_DEPTH_TEST); // Enables depth testing
	glDepthFunc(GL_LEQUAL); // The type of depth testing to do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// Really nice perspective calculations

	glPolygonMode(GL_BACK, GL_FILL); // Back face is filled in
	glPolygonMode(GL_FRONT, GL_LINE); // Front face is drawn with lines

	// Loop through the X plane
	for (int x = 0; x < 45; x++) {
		// Loop through the Y plane
		for (int y = 0; y < 45; y++) {
			// Apply the wave to our mesh
			points[x][y][0] = float((x / 5.0f) - 4.5f);
			points[x][y][1] = float((y / 5.0f) - 4.5f);
			points[x][y][2] =
				float(sin((((x / 5.0f) * 40.0f) / 360.0f)
				* 3.141592654 * 2.0f));
		}
	}

	return true; // Initialization went OK
}

int DrawGLScene(void) // Here's where we do all the drawing
{
	int x, y; // Loop variables
	float float_x, float_y, float_xb, float_yb;
	// Used to break the flag into tiny quads

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Clear screen and depth buffer
	glLoadIdentity(); // Reset the current modelview matrix

	glTranslatef(0.0f, 0.0f, -12.0f); // Translate 17 units into the screen

	glRotatef(xrot, 1.0f, 0.0f, 0.0f); // Rotate on The X axis
	glRotatef(yrot, 0.0f, 1.0f, 0.0f); // Rotate on The Y axis
	glRotatef(zrot, 0.0f, 0.0f, 1.0f); // Rotate on The Z axis

	glBindTexture(GL_TEXTURE_2D, texture[0]); // Select Our Texture

	glBegin(GL_QUADS); // Start drawing our quads
	for (x = 0; x < 44; x++) // Loop through the X plane 0-44 (45 Points)
	{
		for (y = 0; y < 44; y++) // Loop through the Y plane 0-44 (45 Points)
		{
			float_x = float(x) / 44.0f; // Create a floating point X value
			float_y = float(y) / 44.0f; // Create a floating point Y value
			float_xb = float(x + 1) / 44.0f;
			// Create a floating point Y value+0.0227f
			float_yb = float(y + 1) / 44.0f;
			// Create a floating point Y value+0.0227f

			glTexCoord2f(float_x, float_y);
			// First texture coordinate (bottom left)
			glVertex3f(points[x][y][0], points[x][y][1], points[x][y][2]);

			glTexCoord2f(float_x, float_yb);
			// Second texture coordinate (top left)
			glVertex3f(points[x][y + 1][0], points[x][y + 1][1],
				points[x][y + 1][2]);

			glTexCoord2f(float_xb, float_yb);
			// Third texture coordinate (top right)
			glVertex3f(points[x + 1][y + 1][0], points[x + 1][y + 1][1],
				points[x + 1][y + 1][2]);

			glTexCoord2f(float_xb, float_y);
			// Fourth texture coordinate (bottom right)
			glVertex3f(points[x + 1][y][0], points[x + 1][y][1],
				points[x + 1][y][2]);
		}
	}
	glEnd(); // Done drawing Our Quads

	if (wiggle_count == 2) // Used to slow down the wave (every 2nd frame only)
	{
		for (y = 0; y < 45; y++) // Loop through The Y plane
		{
			hold = points[0][y][2]; // Store current value one left side of wave
			for (x = 0; x < 44; x++) // Loop through the X plane
			{
				// Current wave value equals value to the right
				points[x][y][2] = points[x + 1][y][2];
			}
			points[44][y][2] = hold;
			// Last value becomes the far left dtored value
		}
		wiggle_count = 0; // Set vounter back to zero
	}
	wiggle_count++; // Increase the counter

	xrot += 0.3f; // Increase the X rotation variable
	yrot += 0.2f; // Increase the Y rotation variable
	zrot += 0.4f; // Increase the Z rotation variable

	return true; // Jump back
}

GLvoid KillGLWindow(void) // Properly kill the window
{
	if (fullscreen) // Are we in fullscreen mode?
	{
		ChangeDisplaySettings(NULL, 0); // If so switch back to the desktop
		ShowCursor(true); // Show mouse pointer
	}

	if (hRC) // Do we have a rendering context?
	{
		if (!wglMakeCurrent(NULL, NULL))
			// Are we able to release the DC and RC contexts?
		{
			MessageBox(NULL, "Release of DC and RC failed.", "SHUTDOWN ERROR",
				MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC)) // Are we able to delete the RC?
		{
			MessageBox(NULL, "Release rendering context failed.",
				"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL; // Set RC to NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC)) // Are we able to release the DC
	{
		MessageBox(NULL, "Release device context failed.", "SHUTDOWN ERROR",
			MB_OK | MB_ICONINFORMATION);
		hDC = NULL; // Set DC to NULL
	}

	if (hWnd && !DestroyWindow(hWnd)) // Are we able to destroy the window?
	{
		MessageBox(NULL, "Could not release hWnd.", "SHUTDOWN ERROR",
			MB_OK | MB_ICONINFORMATION);
		hWnd = NULL; // Set hWnd to NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))
		// Are we able to unregister class
	{
		MessageBox(NULL, "Could not unregister class.", "SHUTDOWN ERROR",
			MB_OK | MB_ICONINFORMATION);
		hInstance = NULL; // Set hInstance to NULL
	}
}

/* This Code Creates Our OpenGL Window.  Parameters Are:
 *	title			- Title To Appear At The Top Of The Window
 *	width			- Width Of The GL Window Or Fullscreen Mode
 *	height			- Height Of The GL Window Or Fullscreen Mode
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE) */

BOOL CreateGLWindow(char* title, int width, int height, byte bits,
	bool fullscreenflag) {
	GLuint PixelFormat; // Holds the results after searching for a match
	WNDCLASS wc; // Windows class structure
	DWORD dwExStyle; // Window extended style
	DWORD dwStyle; // Window style
	RECT WindowRect; // Grabs rctangle upper left / lower right values
	WindowRect.left = (long)0; // Set left value to 0
	WindowRect.right = (long)width; // Set right value to requested width
	WindowRect.top = (long)0; // Set top value to 0
	WindowRect.bottom = (long)height; // Set bottom value to requested height

	fullscreen = fullscreenflag; // Set the global fullscreen flag

	hInstance = GetModuleHandle(NULL); // Grab an instance for our window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	// Redraw on size, and own DC for window
	wc.lpfnWndProc = (WNDPROC) WndProc; // WndProc handles messages
	wc.cbClsExtra = 0; // No extra window data
	wc.cbWndExtra = 0; // No extra window data
	wc.hInstance = hInstance; // Set the Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); // Load the default icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Load the arrow pointer
	wc.hbrBackground = NULL; // No background required for GL
	wc.lpszMenuName = NULL; // We don't want a menu
	wc.lpszClassName = "OpenGL"; // Set the class name

	if (!RegisterClass(&wc)) // Attempt to register the window class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR",
			MB_OK | MB_ICONEXCLAMATION);

		return false; // Return FALSE
	}

	if (fullscreen) // Attempt fullscreen mode?
	{
		DEVMODE dmScreenSettings; // Device mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		// Makes sure memory's cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		// Size of the devmode structure
		dmScreenSettings.dmPelsWidth = width; // Selected screen width
		dmScreenSettings.dmPelsHeight = height; // Selected screen height
		dmScreenSettings.dmBitsPerPel = bits; // Selected bits per pixel
		dmScreenSettings.dmFields =
			DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try to set selected mode and get results. NOTE: CDS_FULLSCREEN gets rid of start bar.
		if (ChangeDisplaySettings(&dmScreenSettings,
			CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			// If the mode fails, offer two options. Quit or use windowed mode.
			if (MessageBox(NULL,
				"The requested fullscreen mode is not supported by\nyour video card. Use windowed mode instead?",
				"NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
				fullscreen = false;
				// Windowed mode selected. Fullscreen = FALSE
			}
			else {
				// Pop up a message box letting user know the program is closing.
				MessageBox(NULL, "Program will now close.", "ERROR",
					MB_OK | MB_ICONSTOP);
				return false; // Return FALSE
			}
		}
	}

	if (fullscreen) // Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW; // Window extended style
		dwStyle = WS_POPUP; // Windows style
		ShowCursor(false); // Hide mouse pointer
	}
	else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window extended style
		dwStyle = WS_OVERLAPPEDWINDOW; // Windows style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);
	// Adjust window to true requested size

	// Create the window
	if (!(hWnd = CreateWindowEx(dwExStyle, // Extended Style For The Window
		"OpenGL", // Class name
		title, // Window title
		dwStyle | // Defined window style
		WS_CLIPSIBLINGS | // Required window style
		WS_CLIPCHILDREN, // Required window style
		0, 0, // Window position
		WindowRect.right - WindowRect.left, // Calculate window width
		WindowRect.bottom - WindowRect.top, // Calculate window height
		NULL, // No parent window
		NULL, // No menu
		hInstance, // Instance
		NULL))) // Dont pass anything to WM_CREATE
	{
		KillGLWindow(); // Reset the display
		MessageBox(NULL, "Window Creation Error.", "ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return false; // Return FALSE
	}

	static PIXELFORMATDESCRIPTOR pfd =
		// pfd tells windows how we want things to be
	{sizeof(PIXELFORMATDESCRIPTOR), // Size of this pixel format descriptor
		1, // Version number
		PFD_DRAW_TO_WINDOW | // Format must support window
			PFD_SUPPORT_OPENGL | // Format must support OpenGL
			PFD_DOUBLEBUFFER, // Must support double buffering
		PFD_TYPE_RGBA, // Request an RGBA format
		bits, // Select our color depth
		0, 0, 0, 0, 0, 0, // Color bits ignored
		0, // No alpha buffer
		0, // Shift bit ignored
		0, // No accumulation buffer
		0, 0, 0, 0, // Accumulation bits ignored
		16, // 16Bit Z-Buffer (Depth buffer)
		0, // No stencil buffer
		0, // No auxiliary buffer
		PFD_MAIN_PLANE, // Main drawing layer
		0, // Reserved
		0, 0, 0 // Layer masks ignored
	};

	if (!(hDC = GetDC(hWnd))) // Did we get a device context?
	{
		KillGLWindow(); // Reset the display
		MessageBox(NULL, "Can't create a GL device context.", "ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return false; // Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
		// Did windows find a matching pixel format?
	{
		KillGLWindow(); // Reset the display
		MessageBox(NULL, "Can't find a suitable pixelformat.", "ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return false; // Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))
		// Are we able to set the pixel format?
	{
		KillGLWindow(); // Reset the display
		MessageBox(NULL, "Can't set the pixelformat.", "ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return false; // Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))
		// Are we able to get a rendering context?
	{
		KillGLWindow(); // Reset the display
		MessageBox(NULL, "Can't create a GL rendering context.", "ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return false; // Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC)) // Try to activate the rendering context
	{
		KillGLWindow(); // Reset the display
		MessageBox(NULL, "Can't activate the GL rendering context.", "ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return false; // Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW); // Show the window
	SetForegroundWindow(hWnd); // Slightly higher priority
	SetFocus(hWnd); // Sets keyboard focus to the window
	ReSizeGLScene(width, height); // Set up our perspective GL screen

	if (!InitGL()) // Initialize our newly created GL window
	{
		KillGLWindow(); // Reset the display
		MessageBox(NULL, "Initialization failed.", "ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return false; // Return FALSE
	}

	return true; // Success
}

LRESULT CALLBACK WndProc(HWND hWnd, // Handle for this window
	UINT uMsg, // Message for this window
	WPARAM wParam, // Additional message information
	LPARAM lParam) // Additional message information
{
	switch (uMsg) // Check for windows messages
	{
	case WM_ACTIVATE: // Watch for window activate message
		{
			if (!HIWORD(wParam)) // Check minimization state
			{
				active = true; // Program is active
			}
			else {
				active = false; // Program is no longer active
			}

			return 0; // Return to the message loop
		}

	case WM_SYSCOMMAND: // Intercept system commands
		{
			switch (wParam) // Check system calls
			{
			case SC_SCREENSAVE: // Screensaver trying to start?
			case SC_MONITORPOWER: // Monitor trying to enter powersave?
				return 0; // Prevent from happening
			}
			break; // Exit
		}

	case WM_CLOSE: // Did we receive a close message?
		{
			PostQuitMessage(0); // Send a quit message
			return 0; // Jump back
		}

	case WM_KEYDOWN: // Is a key being held down?
		{
			keys[wParam] = true; // If so, mark it as TRUE
			return 0; // Jump back
		}

	case WM_KEYUP: // Has a key been released?
		{
			keys[wParam] = false; // If so, mark it as FALSE
			return 0; // Jump back
		}

	case WM_SIZE: // Resize the OpenGL window
		{
			ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
			// LoWord = Width, HiWord = Height
			return 0; // Jump back
		}
	}

	// Pass all unhandled messages to DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	MSG msg; // Windows message structure
	bool done = false; // Bool variable to exit loop

	// Ask the user which screen mode they prefer
	///if (MessageBox(NULL, "Would you like to run in fullscreen mode?","Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		fullscreen = false; // Windowed mode
	}

	// Create our OpenGL window
	if (!CreateGLWindow("bosco & NeHe's Waving Texture Tutorial", 640, 480, 16,
		fullscreen)) {
		return 0; // Quit if window was not created
	}

	while (!done) // Loop that runs while done = FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			// Is there a message waiting?
		{
			if (msg.message == WM_QUIT) // Have we received a quit message?
			{
				done = true; // If so done = TRUE
			}
			else // If not, deal with window messages
			{
				TranslateMessage(&msg); // Translate the message
				DispatchMessage(&msg); // Dispatch the message
			}
		}
		else // If there are no messages
		{
			// Draw the scene.  Watch for ESC key and quit messages from DrawGLScene()
			if (active) // Program active?
			{
				if (keys[VK_ESCAPE]) // Was ESC pressed?
				{
					done = true; // ESC signalled a quit
				}
				else // Not time to quit, Update screen
				{
					DrawGLScene(); // Draw the scene
					SwapBuffers(hDC); // Swap buffers (Double buffering)
				}
			}

			if (keys[VK_F1]) // Is F1 being pressed?
			{
				keys[VK_F1] = false; // If so make key FALSE
				KillGLWindow(); // Kill our current window
				fullscreen = !fullscreen; // Toggle fullscreen / windowed mode
				// Recreate our OpenGL window
				if (!CreateGLWindow("bosco & NeHe's Waving Texture Tutorial",
					640, 480, 16, fullscreen)) {
					return 0; // Quit if window was not created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow(); // Kill the window
	return (msg.wParam); // Exit the program

}
// ---------------------------------------------------------------------------
