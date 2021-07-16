#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include "./include/glad/glad.h"
#include <windows.h>
#include <iostream>
#include "Application.h"
#include "quat.h"

int WINAPI WinMain( HINSTANCE,HINSTANCE,PSTR,int );
LRESULT CALLBACK WndProc( HWND,UINT,WPARAM,LPARAM );

#if _DEBUG

#pragma comment( linker,"/subsystem:console" )

int main( int argc,const char** argv )
{

	return WinMain( GetModuleHandle( NULL ),NULL,
		GetCommandLineA(),
		SW_SHOWDEFAULT );
}

#else
#pragma comment( linker,"/subsystem:windows" )

#endif

#pragma comment( lib,"opengl32.lib" )

typedef const char*
(WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL( WINAPI* PFNWGLSWAPINTERVALEXTPROC ) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

//from wglext.h

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

typedef HGLRC( WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC ) (HDC,HGLRC,const int*);

//from wgl.h

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL( WINAPI* PFNWGLSWAPINTERVALEXTPROC ) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

//actual work

Application* gApplication = 0;
GLuint gVertexArrayObject = 0;

int WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow )
{
	gApplication = new Application();

	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof( WNDCLASSEX );
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon( NULL,IDI_APPLICATION );
	wndclass.hIconSm = LoadIcon( NULL,IDI_APPLICATION );
	wndclass.hCursor = LoadCursor( NULL,IDC_ARROW );
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = "Win32 Game Window)";
	RegisterClassEx( &wndclass );

	int screenWidth = GetSystemMetrics( SM_CXSCREEN );
	int screenHeight = GetSystemMetrics( SM_CYSCREEN );
	int clientWidth = 800;
	int clientHeight = 600;
	RECT windowRect;
	SetRect( &windowRect,
		(screenWidth / 2) - (clientWidth / 2),
		(screenHeight / 2) - (clientHeight / 2),
		(screenWidth / 2) + (clientWidth / 2),
		(screenHeight / 2) + (clientHeight / 2)
	);

	DWORD style = (WS_OVERLAPPED | WS_CAPTION |
		WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	// | WS_THICKFRAME to resize

	AdjustWindowRectEx( &windowRect,style,FALSE,0 );
	HWND hwnd = CreateWindowEx( 0,wndclass.lpszClassName,
		"Game Window",style,windowRect.left,windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,NULL,hInstance,szCmdLine );
	HDC hdc = GetDC( hwnd );

	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd,0,sizeof( PIXELFORMATDESCRIPTOR ) );
	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int pixelFormat = ChoosePixelFormat( hdc,&pfd );
	SetPixelFormat( hdc,pixelFormat,&pfd );

	//temporary opengl context

	HGLRC tempRC = wglCreateContext( hdc );
	wglMakeCurrent( hdc,tempRC );
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = ( PFNWGLCREATECONTEXTATTRIBSARBPROC )
		wglGetProcAddress( "wglCreateContextAttribsARB" );

	//binding opengl core and deleting legacy context

	const int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB,
		3,
		WGL_CONTEXT_MINOR_VERSION_ARB,
		3,
		WGL_CONTEXT_FLAGS_ARB,
		0,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0,
	};
	HGLRC hglrc = wglCreateContextAttribsARB( hdc,0,attribList );
	wglMakeCurrent( NULL,NULL );
	wglDeleteContext( tempRC );
	wglMakeCurrent( hdc,hglrc );

	//make glad load opengl core functions

	if ( !gladLoadGL() )
	{
		std::cout << "Could not initialize GLAD\n";
	}
	else
	{
		std::cout << "OpenGL Version " << GLVersion.major << std::endl;
	}

	//enabling vsynch to the window

	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT =
		( PFNWGLGETEXTENSIONSSTRINGEXTPROC )wglGetProcAddress( "wglGetExtensionsStringEXT" );
	bool swapControlSupported = strstr( _wglGetExtensionsStringEXT(),
		"WGL_EXT_swap_control" ) != 0;

	int vsynch = 0;
	if ( swapControlSupported )
	{
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
			( PFNWGLSWAPINTERVALEXTPROC )wglGetProcAddress( "wglSwapIntervalEXT" );
		PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT =
			( PFNWGLGETSWAPINTERVALEXTPROC )wglGetProcAddress( "wglGetSwapIntervalEXT" );
		if ( wglSwapIntervalEXT( 1 ) )
		{
			std::cout << "Enabled vsynch\n";
			vsynch = wglGetSwapIntervalEXT();
		}
		else
		{
			std::cout << "Could not enable vsynch\n";
		}
	}
	else // !swapControlSupported
	{
		std::cout << "WGL_EXT_swap_control not supported\n";
	}

	//creating and binding VertexArrayObject( VAO )

	glGenVertexArrays( 1,&gVertexArrayObject );
	glBindVertexArray( gVertexArrayObject );

	//displaying window

	ShowWindow( hwnd,SW_SHOW );
	UpdateWindow( hwnd );
	gApplication->Initialize();

	//eventually, game loop :)

	DWORD lastTick = GetTickCount();
	MSG msg;
	while ( true )
	{
		if ( PeekMessage( &msg,NULL,0,0,PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )
			{
				break;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		DWORD thisTick = GetTickCount();
		float dt = float( thisTick - lastTick ) * 0.001f;
		lastTick = thisTick;
		if ( gApplication != 0 )
		{
			gApplication->Update( dt );
		}
		if ( gApplication != 0 )
		{
			RECT clientRect;
			GetClientRect( hwnd,&clientRect );
			clientWidth = clientRect.right = clientRect.left;
			clientHeight = clientRect.bottom - clientRect.top;
			glViewport( 0,0,clientWidth,clientHeight );
			glEnable( GL_DEPTH_TEST );
			glEnable( GL_CULL_FACE );
			glPointSize( 5.f );
			glBindVertexArray( gVertexArrayObject );
			glClearColor( .5f,.6f,.7f,1.f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
				GL_STENCIL_BUFFER_BIT );
			float aspect = float( clientWidth ) / float( clientHeight );
			gApplication->Render( aspect );
		}
		if ( gApplication != 0 )
		{
			SwapBuffers( hdc );
			if ( vsynch != 0 )
			{
				glFinish();
			}
		}
	} // end of the game loop
	
	if ( gApplication != 0 )
	{
		std::cout << "Expected application to be null on exit" << std::endl;
		delete gApplication;
	}
	return int( msg.wParam );
}

//creating event handler

LRESULT CALLBACK WndProc( HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam )
{
	switch ( iMsg )
	{
	case WM_CLOSE:
		if ( gApplication != 0 )
		{
			gApplication->Shutdown();
			delete gApplication;
			gApplication = 0;
			DestroyWindow( hwnd );
		}
		else
		{
			std::cout << "Already shut dowm!" << std::endl;
		}
		break;
	case WM_DESTROY:
		if ( gVertexArrayObject != 0 )
		{
			HDC hdc = GetDC( hwnd );
			HGLRC hglrc = wglGetCurrentContext();
			glBindVertexArray( 0 );
			glDeleteVertexArrays( 1,&gVertexArrayObject );
			gVertexArrayObject = 0;
			wglMakeCurrent( NULL,NULL );
			wglDeleteContext( hglrc );
			ReleaseDC( hwnd,hdc );
			PostQuitMessage( 0 );
		}
		else
		{
			std::cout << "Multiple destroy messages" << std::endl;
		}
		break;
	case WM_PAINT:
	case WM_ERASEBKGND:
		return 0;
	}
	return DefWindowProc( hwnd,iMsg,wParam,lParam );
}

