#include "render.h"

// um
#pragma comment(lib, "d2d1")

#include <windows.h>
#include <d2d1.h>

#include <thread>
#include <vector>

template <class T> void SafeRelease( T **ppT )
{
    if ( *ppT )
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

// stupid and inefficient, blech, but i don't know a better way that won't interfere with sharex
class ShimejiWindow
{
public:
    bool    Create( shimeji_t *spShimeji );
    bool    SetBitmap( unsigned int index );
    void    Destroy();
    void    Draw();
    void    Update();
    void    CursorMoveUpdate();

    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();

    LRESULT HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );

    shimeji_t*                  apShimeji = nullptr;

    // awful
    bool                        aGrabbed = false;
    POINT                       aLastLocation;

    HWND                        aHWND;
    ID2D1HwndRenderTarget*		apRenderTarget = nullptr;

    std::vector< ID2D1Bitmap* > aBitmaps;
    unsigned int                aBitmapIndex = 0;

    static LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        ShimejiWindow *pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (ShimejiWindow*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->aHWND = hwnd;
        }
        else
        {
            pThis = (ShimejiWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
};


class Renderer
{
public:
    bool    Init();
    void    Update();
    void    CreateShimeji( shimeji_t *spShimeji );

	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();

    ID2D1Factory*				apFactory;

    std::vector< ShimejiWindow* > aWindows;
    std::vector< shimeji_t* >     aShimejiQueue;
};


Renderer* gpRenderer = nullptr;

constexpr int IMAGE_SIZE[2] = {128, 128};

#define TEST_TITLEBAR 1

#if TEST_TITLEBAR
constexpr int WINDOW_SIZE[2] = {128, 128+30};
#else
constexpr int WINDOW_SIZE[2] = {128, 128};
#endif

bool ShimejiWindow::Create( shimeji_t *spShimeji )
{
    if ( spShimeji == nullptr )
        return false;

    apShimeji = spShimeji;

    WNDCLASS wc = {0};

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = "shimeji_window";

    RegisterClass(&wc);

    LPCSTR lpWindowName = "shimeji_window_name";

#if TEST_TITLEBAR
    DWORD dwStyle = 0;
    DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;  // don't show in taskbar
#else
    DWORD dwStyle = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;  // WS_POPUP|WS_VISIBLE|WS_SYSMENU;
    DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED;  // don't show in taskbar + stay on top
#endif

    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;

    HWND hWndParent = 0;
    HMENU hMenu = 0;

    aHWND = CreateWindowEx(
        dwExStyle, wc.lpszClassName, lpWindowName, dwStyle, x, y,
        WINDOW_SIZE[0], WINDOW_SIZE[1], hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    if ( aHWND )
        SetLayeredWindowAttributes( aHWND, RGB(0,0,0), 150, LWA_ALPHA );

    return (aHWND ? TRUE : FALSE);
}

bool ShimejiWindow::SetBitmap( unsigned int index )
{
    if ( aBitmaps.size() < index )
    {
        printf( "[Renderer] bitmap index out of range: %d\n", index );
        return false;
    }

    aBitmapIndex = index;

    OnPaint();
}

void ShimejiWindow::Destroy()
{
}

void ShimejiWindow::Draw()
{
}


void ShimejiWindow::Update()
{
    // CursorMoveUpdate();

    // OnPaint();
}


void ShimejiWindow::CursorMoveUpdate()
{
    if ( aGrabbed )
    {
        POINT currentpos;
        GetCursorPos( &currentpos );
        int x = currentpos.x - aLastLocation.x;
        int y = currentpos.y - aLastLocation.y;
        MoveWindow( aHWND, x, y, WINDOW_SIZE[0], WINDOW_SIZE[1], false);
    }
}


LRESULT ShimejiWindow::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch ( uMsg )
    {
    case WM_DESTROY:
        DiscardGraphicsResources();
        PostQuitMessage( 0 );
        return 0;

    /*case WM_PAINT:
        OnPaint();
        break;*/

    case WM_LBUTTONDOWN:
        aGrabbed = true;
        GetCursorPos( &aLastLocation );
        RECT rect;
        GetWindowRect( aHWND, &rect );
        aLastLocation.x = aLastLocation.x - rect.left;
        aLastLocation.y = aLastLocation.y - rect.top;
        break;
    
    case WM_LBUTTONUP:
        aGrabbed = false;
        break;
    }
    
    return DefWindowProc( aHWND, uMsg, wParam, lParam );
}


HRESULT ShimejiWindow::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if ( apRenderTarget == NULL )
    {
        D2D1_SIZE_U size = D2D1::SizeU( IMAGE_SIZE[0], IMAGE_SIZE[1] );

        hr = gpRenderer->apFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat( DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED )
            ),
            D2D1::HwndRenderTargetProperties( aHWND, size ),
            &apRenderTarget
        );

        if ( SUCCEEDED( hr ) )
        {
            ID2D1Bitmap* bitmap = 0;

            for ( int i = 0;; i++ )
            {
                if ( apShimeji->apData[i] == nullptr )
                    break;

                hr = apRenderTarget->CreateBitmap(
                    size,
                    apShimeji->apData[i]->apBuf,
                    4*128,
                    D2D1::BitmapProperties(
                        D2D1::PixelFormat( DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED ),
                        128, 128),
                    &bitmap);

                if ( SUCCEEDED( hr ) )
                {
                    aBitmaps.push_back( bitmap );
                }
                else
                {
                    printf( "[Renderer] failed to make a bitmap for image index %d\n", i );
                    break;
                }
            }
        }
    }
    return hr;
}

void ShimejiWindow::DiscardGraphicsResources()
{
    if ( apRenderTarget ) apRenderTarget->Release();
    //if ( apBrush ) apBrush->Release();

    for ( auto bitmap: aBitmaps )
    {
        bitmap->Release();
    }
}

void ShimejiWindow::OnPaint()
{
    HRESULT hr = CreateGraphicsResources();
    if ( SUCCEEDED( hr ) )
    {
        PAINTSTRUCT ps;
        BeginPaint( aHWND, &ps );

        apRenderTarget->BeginDraw();

        apRenderTarget->Clear( D2D1::ColorF( 0, 0, 0, 0 ) );
        apRenderTarget->DrawBitmap( aBitmaps[aBitmapIndex] );

        hr = apRenderTarget->EndDraw();

        if ( FAILED( hr ) || hr == D2DERR_RECREATE_TARGET )
        {
            DiscardGraphicsResources();
        }

        EndPaint( aHWND, &ps );
    }
}

void ShimejiWindow::Resize()
{
    if ( apRenderTarget != NULL )
    {
        //RECT rc;
        //GetClientRect( m_hwnd, &rc );

        // D2D1_SIZE_U size = D2D1::SizeU( rc.right, rc.bottom );
        D2D1_SIZE_U size = D2D1::SizeU( 128, 128 );

        apRenderTarget->Resize( size );
        // InvalidateRect( m_hwnd, NULL, FALSE );
    }
}


// -------------------------------------------------------------------


std::thread* gpRenderThread = nullptr;
std::thread* gpMoveThread = nullptr;


bool Renderer::Init()
{
    if ( FAILED( D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &apFactory ) ) )
    {
        return false;  // Fail CreateWindowEx.
    }

	return true;
}


template <class T>
size_t vec_index( std::vector<T> &vec, T item )
{
    for (size_t i = 0; i < vec.size(); i++)
    {
        if (vec[i] == item)
            return i;
    }

    return SIZE_MAX;
}

template <class T>
void vec_remove( std::vector<T> &vec, T item )
{
    vec.erase( vec.begin() + vec_index( vec, item ) );
}


void Renderer::Update()
{
    // Sleep( 10 );

    while ( aShimejiQueue.size() )
    {
        CreateShimeji( aShimejiQueue[0] );
        vec_remove( aShimejiQueue, aShimejiQueue[0] );
    }

    for ( auto window: aWindows )
    {
        window->Update();

        MSG msg = {};
        if ( PeekMessage( &msg, window->aHWND, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
}


void Renderer::CreateShimeji( shimeji_t *spShimeji )
{
    // TEST: create one window
    ShimejiWindow* window = new ShimejiWindow;
    window->Create( spShimeji );

    ShowWindow( window->aHWND, 1 );
    window->SetBitmap( 0 );

    aWindows.push_back( window );
}


// -------------------------------------------------------------------


void Renderer::DiscardGraphicsResources()
{
    if ( apFactory ) apFactory->Release();

    for ( auto window: aWindows )
    {
        window->DiscardGraphicsResources();
    }
}


// ---------------------------------------------------------------


shimeji_surface_t* shimeji_surface_init()
{
	return new shimeji_surface_t;
}


void shimeji_surface_free( shimeji_surface_t* spSurface )
{
	if ( spSurface )
		delete spSurface;
}


void render_start( shimeji_surface_t* spSurface )
{
	gpRenderer = new Renderer;

	gpRenderThread = new std::thread(
		[&]()
		{
            if ( !gpRenderer->Init() )
                return;

			while ( true )
			{
				gpRenderer->Update();
			}
		}
	);

	gpMoveThread = new std::thread(
		[&]()
		{
			while ( true )
			{
                for ( size_t i = 0; i < gpRenderer->aWindows.size(); i++ )
                {
                    gpRenderer->aWindows[i]->CursorMoveUpdate();
                }
			}
		}
	);
}


void render_add( shimeji_surface_t *spSurface, shimeji_t *spShimeji )
{
    if ( spShimeji )
        gpRenderer->aShimejiQueue.push_back( spShimeji );
}

