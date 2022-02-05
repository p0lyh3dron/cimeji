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

		allow_input_passthrough( pSurface );

#ifdef __linux__
			cairo_t *cr = shimeji_cairo_init( pSurface );
#endif /* __linux__  */

		for ( int i = 0; ; ++i ) {
			int x = 200 + 200 * sin( 2 + ( float )i * 0.01 );
			int y = 200 + 200 * cos( ( float )i * 0.02 );
#ifdef __linux__
			draw_rectangle( cr, x, y, 128, 128 );
#endif /* __linux__  */
			shimeji_surface_clear( pSurface );
		}

#ifdef __linux__
		cairo_destroy( cr );
#endif /* __linux__  */

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