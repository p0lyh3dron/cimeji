#include "app.h"
#include <QtCore/qthread.h>

#include <assert.h>
#include <stdio.h>

#include <spng.h>

#include "linux/surface.h"

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

int x = 0;
int y = 0;

void draw( cairo_t *cr ) {
	cairo_set_source_rgba( cr, 1.0, 0.0, 0.0, 0.5 );
	cairo_rectangle( cr, x, y, 128, 128 );
	cairo_fill( cr );
}

class Main : public QThread {
	void run() override {
		spng_ctx *ctx = spng_ctx_new( 0 );
		spng_ctx_free( ctx );
		
		shimeji_surface_t *pSurface = shimeji_surface_init();

		allow_input_passthrough( pSurface );

		cairo_surface_t* surf = cairo_xlib_surface_create( 
			pSurface->apDisplay, pSurface->aOverlayWin, 
			pSurface->aVInfo.visual, pSurface->aWidth, 
			pSurface->aHeight 
		);
		cairo_t* cr = cairo_create( surf );

		for ( int i = 0; ; ++i ) {
			x = 200 + 200 * sin( 2 + ( float )i * 0.01 );
			y = 200 + 200 * cos( ( float )i * 0.02 );
			draw( cr );
			shimeji_surface_clear( pSurface );
		}

		cairo_destroy( cr );
		cairo_surface_destroy( surf );

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