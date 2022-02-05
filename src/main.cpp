#include "app.h"
#include <QtCore/qthread.h>

#include <assert.h>
#include <stdio.h>

#include <spng.h>

#include "render.h"

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

class Main : public QThread {
	void run() override {
		spng_ctx *ctx = spng_ctx_new( 0 );
		spng_ctx_free( ctx );
		
		shimeji_surface_t *pSurface = shimeji_surface_init();
		render_start( pSurface );

		while( 1 );

		shimeji_surface_free( pSurface );
		return;
	}
};

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );
    app.setDesktopSettingsAware( true );

    gPanel = new ControlPanel;
    gPanel->resize( 200, 800 );
    gPanel->show();

	Main *pThread = new Main;
	pThread->start();

    return app.exec();
}