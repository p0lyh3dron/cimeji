#include "app.h"
#include <QtCore/qthread.h>

#include <assert.h>
#include <stdio.h>

#include <spng.h>

#include "render.h"
#include "shimeji.h"

#ifdef _WIN32
	#define TEST_SHIMEJI "C:\\UserPrograms\\shimejiee\\img\\rai"
#else
	#define TEST_SHIMEJI "/home/karl/Downloads/shimejiee/img/rai/"
#endif

class Main : public QThread {
	void run() override {
		shimeji_surface_t *pSurface = shimeji_surface_init();
		render_start( pSurface );

		shimeji_t *pShimeji = shimeji_init( TEST_SHIMEJI );
		render_add( pSurface, pShimeji );

		while ( 1 )
		{
#ifdef _WIN32
			sleep( 50 );
#endif
		}

		shimeji_surface_free( pSurface );
		return;
	}
};

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );
    app.setDesktopSettingsAware( true );

    gPanel = new ControlPanel;
    gPanel->resize( 120, 300 );
    gPanel->show();

	Main *pThread = new Main;
	pThread->start();

    return app.exec();
}