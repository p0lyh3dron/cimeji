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


void PrintLastError( const char* msg )
{
    DWORD dLastError = GetLastError();
    LPCTSTR strErrorMessage = NULL;

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        dLastError,
        0,
        (LPSTR) &strErrorMessage,
        0,
        NULL);

    printf( "Error: %s\nWin32 API Error %d: %s\n", msg, dLastError, strErrorMessage );
}


// https://stackoverflow.com/a/31411628/12778316
static NTSTATUS(__stdcall *NtDelayExecution)(BOOL Alertable, PLARGE_INTEGER DelayInterval) = (NTSTATUS(__stdcall*)(BOOL, PLARGE_INTEGER)) GetProcAddress(GetModuleHandle("ntdll.dll"), "NtDelayExecution");
static NTSTATUS(__stdcall *ZwSetTimerResolution)(IN ULONG RequestedResolution, IN BOOLEAN Set, OUT PULONG ActualResolution) = (NTSTATUS(__stdcall*)(ULONG, BOOLEAN, PULONG)) GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwSetTimerResolution");

static void SleepShort( float milliseconds )
{
    static bool once = true;
    if (once) {
        ULONG actualResolution;
        ZwSetTimerResolution(1, true, &actualResolution);
        once = false;
    }

    LARGE_INTEGER interval;
    interval.QuadPart = -1 * (int)(milliseconds * 10000.0f);
    NtDelayExecution(false, &interval);
}


// --------------------------------------------------------------------------------------------


class AvatarData
{
public:
    bool    Create( avatar_t *spShimeji, ID2D1HwndRenderTarget* spRenderTarget );
    void    Destroy();

    std::vector< ID2D1Bitmap* > aBitmaps;
};


// stupid and inefficient, blech, but i don't know a better way that won't interfere with sharex
class ShimejiWindow
{
public:
    bool    Create( avatar_t *spShimeji );
    bool    SetBitmap( unsigned int index );
    void    Destroy();
    void    Draw();
    void    Update();
    void    CursorMoveUpdate();

    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    UpdateAlphaTestWindow();
    void    Resize();

    LRESULT HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );

    avatar_t*                   apAvatar = nullptr;
    // AvatarData*                 apData = nullptr;

    std::vector< ID2D1Bitmap* > aBitmaps;
    u16                         aBitmapIndex = 0;

    POINT                       aLastLocation;

    HWND                        aHWND;
    ID2D1HwndRenderTarget*		apRenderTarget = nullptr;

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
    void    CreateShimeji( avatar_t *spShimeji );

    ShimejiWindow*    GetAvatarWindow( avatar_t *spShimeji );

	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();

    ID2D1Factory*				apFactory;

    std::vector< ShimejiWindow* > aWindows;
    std::vector< avatar_t* >     aShimejiQueue;
};


Renderer* gpRenderer = nullptr;

constexpr int IMAGE_SIZE[2] = {128, 128};

#define TEST_TITLEBAR 0

#if TEST_TITLEBAR
constexpr int WINDOW_SIZE[2] = {128, 128+30};
#else
constexpr int WINDOW_SIZE[2] = {128, 128};
#endif

bool ShimejiWindow::Create( avatar_t *spShimeji )
{
    if ( spShimeji == nullptr )
        return false;

    apAvatar = spShimeji;

    WNDCLASS wc = {0};

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = "shimeji_window";

    RegisterClass(&wc);

    LPCSTR lpWindowName = "shimeji_window_name";

#if TEST_TITLEBAR
    DWORD dwStyle = 0;
    // DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;  // don't show in taskbar
    DWORD dwExStyle = WS_EX_TOPMOST;  // don't show in taskbar
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
        SetLayeredWindowAttributes( aHWND, RGB(0,0,0), 0, LWA_COLORKEY );
    
    if ( aHWND )
        UpdateAlphaTestWindow();

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

    Draw();

    return true;
}


void ShimejiWindow::Destroy()
{
}


void ShimejiWindow::Update()
{
    CursorMoveUpdate();

    //UpdateAlphaTestWindow();
}


// constexpr int SC_DRAGMOVE = 0xf012;
constexpr int SC_DRAGMOVE = SC_MOVE | HTCAPTION;


void ShimejiWindow::CursorMoveUpdate()
{
    if ( apAvatar->aGrabbed )
    {
        POINT currentpos;
        GetCursorPos( &currentpos );
        apAvatar->aPos[0] = currentpos.x - aLastLocation.x;
        apAvatar->aPos[1] = currentpos.y - aLastLocation.y;

        // check if cursor released
        apAvatar->aGrabbed = ((GetKeyState( VK_LBUTTON ) & 0x8000) != 0);
        // apAvatar->aGrabbed = 1;
    }

    if ( apAvatar->aGrabbed )
    {
        POINT currentpos;
        GetCursorPos( &currentpos );
        apAvatar->aPos[0] = currentpos.x - aLastLocation.x;
        apAvatar->aPos[1] = currentpos.y - aLastLocation.y;
        MoveWindow( aHWND, apAvatar->aPos[0], apAvatar->aPos[1], WINDOW_SIZE[0], WINDOW_SIZE[1], false );

        // INSTANT WINDOW DRAGGING !!!!!!!
        // https://stackoverflow.com/a/66919909/12778316
        // uh, small issue, this seems to block SetWindowPos calls, wtf
        // SendMessage( aHWND, WM_SYSCOMMAND, SC_DRAGMOVE, 0 );
    }
    else if ( aLastLocation.x != apAvatar->aPos[0] || aLastLocation.y != apAvatar->aPos[1] )
    {
        //MoveWindow( aHWND, apAvatar->aPos[0], apAvatar->aPos[1], WINDOW_SIZE[0], WINDOW_SIZE[1], false );

        SetWindowPos(
            aHWND, HWND_TOP, apAvatar->aPos[0], apAvatar->aPos[1], -1, -1,
            SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOREPOSITION | SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS
            // SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOREPOSITION | SWP_NOCOPYBITS | SWP_NOACTIVATE
            // SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_ASYNCWINDOWPOS
        );

        /*auto windowpositionstructure = BeginDeferWindowPos(1);

        windowpositionstructure = DeferWindowPos(
            windowpositionstructure,
            aHWND,
            0,
            apAvatar->aPos[0],
            apAvatar->aPos[1],
            0, 0,
            SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS
        );

        EndDeferWindowPos( windowpositionstructure );*/

        /*
        MultiWindowInitialStructure = Native_Methods.DeferWindowPos(
            MultiWindowInitialStructure,
            tmp.WidgetHandle, HWND.NOTOPMOST, tempX, tmp.WidgetRectTop, tmp.WidgetWidth, tmp.WidgetHeight, SWP.NOREDRAW | SWP.NOZORDER | SWP.NOACTIVATE | SWP.NOSIZE);
        */

        aLastLocation.x = apAvatar->aPos[0];
        aLastLocation.y = apAvatar->aPos[1];
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

    case WM_LBUTTONDOWN:
        apAvatar->aGrabbed = true;
        GetCursorPos( &aLastLocation );
        RECT rect;
        GetWindowRect( aHWND, &rect );
        aLastLocation.x = aLastLocation.x - rect.left;
        aLastLocation.y = aLastLocation.y - rect.top;
        break;
    
    case WM_LBUTTONUP:
        apAvatar->aGrabbed = false;
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
            // TODO: test and see if bitmaps made with this render target can work with other render targets
            // if not, well, try to find a way too i guess
            ID2D1Bitmap* bitmap = 0;

            for ( int i = 0; i < apAvatar->aData.size(); i++ )
            {
                hr = apRenderTarget->CreateBitmap(
                    size,
                    apAvatar->aData[i]->apBuf,
                    4*128,
                    D2D1::BitmapProperties(
                        D2D1::PixelFormat( DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED ),
                        apAvatar->aWidth, apAvatar->aHeight ),
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

bool AvatarData::Create( avatar_t* spAvatar, ID2D1HwndRenderTarget* spRenderTarget )
{
    ID2D1Bitmap* bitmap = 0;

    D2D1_SIZE_U size = D2D1::SizeU( spAvatar->aWidth, spAvatar->aHeight );

    for ( int i = 0; i < spAvatar->aData.size(); i++ )
    {
        HRESULT hr = spRenderTarget->CreateBitmap(
            size,
            spAvatar->aData[i]->apBuf,
            4*128,
            D2D1::BitmapProperties(
                D2D1::PixelFormat( DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED ),
                spAvatar->aWidth, spAvatar->aHeight ),
            &bitmap);

        if ( SUCCEEDED( hr ) )
        {
            aBitmaps.push_back( bitmap );
        }
        else
        {
            printf( "[Renderer] failed to make a bitmap for image index %d\n", i );
            return false;
        }
    }

    return true;
}

void ShimejiWindow::DiscardGraphicsResources()
{
    if ( apRenderTarget ) apRenderTarget->Release();

    for ( auto bitmap: aBitmaps )
    {
        bitmap->Release();
    }
}

void ShimejiWindow::Draw()
{
    HRESULT hr = CreateGraphicsResources();
    if ( SUCCEEDED( hr ) )
    {
        PAINTSTRUCT ps;
        BeginPaint( aHWND, &ps );

        RECT rect;
        GetWindowRect( aHWND, &rect );

        apRenderTarget->BeginDraw();

        apRenderTarget->Clear( D2D1::ColorF( 0, 0, 0, 0 ) );

        apRenderTarget->DrawBitmap(
            aBitmaps[aBitmapIndex],
            {0, 0, (float)IMAGE_SIZE[0], (float)IMAGE_SIZE[1]}
        );

        hr = apRenderTarget->EndDraw();

        if ( FAILED( hr ) || hr == D2DERR_RECREATE_TARGET )
        {
            DiscardGraphicsResources();
        }

        EndPaint( aHWND, &ps );

        UpdateAlphaTestWindow();
    }
}

void ShimejiWindow::UpdateAlphaTestWindow()
{
    return;

    if ( apRenderTarget != NULL )
    {
        int alpha = 160;

        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = alpha;
        bf.AlphaFormat = AC_SRC_ALPHA;

        RECT rect;
        GetWindowRect( aHWND, &rect );

        HDC dc = GetDC( aHWND );
        HDC newDC = CreateCompatibleDC( dc );

        BOOL ret = UpdateLayeredWindow(
            aHWND, nullptr,

            0, // move window here (optional)
            0, // resize window (optional)

            newDC, 0,

            0, &bf, ULW_ALPHA
        );

        if ( !ret )
            PrintLastError( "bruh" );
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
    // if ( FAILED( D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &apFactory ) ) )
    if ( FAILED( D2D1CreateFactory( D2D1_FACTORY_TYPE_MULTI_THREADED, &apFactory ) ) )
    {
        return false;  // Fail CreateWindowEx.
    }

	return true;
}


void Renderer::Update()
{
    while ( aShimejiQueue.size() )
    {
        // HACK RACE CONDITION FIXME
        Sleep( 10 );

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


void Renderer::CreateShimeji( avatar_t *spShimeji )
{
    ShimejiWindow* window = new ShimejiWindow;
    window->Create( spShimeji );

    ShowWindow( window->aHWND, 1 );
    window->SetBitmap( 0 );

    aWindows.push_back( window );
}


ShimejiWindow* Renderer::GetAvatarWindow( avatar_t *spShimeji )
{
    for ( auto window: aWindows )
    {
        if ( window->apAvatar == spShimeji )
            return window;
    }

    return nullptr;
}


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
#ifdef _WIN32
                // don't have this cpu core go at full tilt for a simple desktop pet please 
                SleepShort( 0.2 );
#endif
				gpRenderer->Update();
			}
		}
	);

    // unstable from race conditions, very cool
	/*gpMoveThread = new std::thread(
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
	);*/
}


void render_add( shimeji_surface_t *spSurface, avatar_t *spShimeji )
{
    // not thread safe, cringe

    if ( spShimeji )
        gpRenderer->aShimejiQueue.push_back( spShimeji );
}


void render_set_image( avatar_t *spAvatar, u16 sIndex, bool sFlip )
{
    ShimejiWindow* window = gpRenderer->GetAvatarWindow( spAvatar );
    if ( !window )
    {
        printf( "[Renderer] window not found for avatar\n" );
        return;
    }

    window->SetBitmap( sIndex );
}


void render_clear()
{
}

