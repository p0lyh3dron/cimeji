#include "app.h"
#include <QtCore/qthread.h>

#include <assert.h>
#include <stdio.h>

#include <spng.h>

#include "render.h"
#include "shimeji.h"
#include "input.h"

#include "script.h"

#ifdef _WIN32
	#define TEST_SHIMEJI "rai"
#else
	#define TEST_SHIMEJI "/home/karl/Downloads/Eliwood by Eeveetachi/img"
#endif

constexpr int TEST_COUNT = 1;


class Main : public QThread
{
	void run() override
	{
		shimeji_surface_t *pSurface = shimeji_surface_init();
		render_start( pSurface );
		init_input();

		// TODO: parse a kv file for default avatar, avatar paths, and other random settings

		// Startup the Python Environment
		pyenv_setup();

		// test behavior
		behavior_t* behavior = nullptr;

		// load a behavior script
		if ( !(behavior = pyenv_load_behavior( "avatar_think_test" )) )
			throw std::runtime_error( "Failed to load test behavior" );

		std::vector< avatar_t* > shimejiList;

		for ( int i = 0; i < TEST_COUNT; i++ )
		{
			avatar_t *pShimeji = avatar_create( TEST_SHIMEJI );
			render_add( pShimeji );
			pShimeji->apBehavior = behavior;
			shimejiList.push_back( pShimeji );
		}

		env_init();

		auto startTime = std::chrono::high_resolution_clock::now();

		while( 1 )
		{
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration< float, std::chrono::seconds::period >( currentTime - startTime ).count(  );

			if ( time > 0.2f )
				time = 0.2f;

#ifdef _WIN32
			// don't have this cpu core go at full tilt for a simple desktop pet please 
			usleep( 1 );
#endif
			env_update();

			bool ret = true;

			for ( auto shimeji: shimejiList )
			{
				ret = pyenv_run_behavior( shimeji->apBehavior, shimeji, time );
			}

			//printf( "%i, %i\n", x, y );
			//printf( "%p\n", get_shimeji_at_mouse() );

			startTime = currentTime;
		}

		// Close the Python Environment
		Py_Finalize();
		
		close_input();
		render_clear();
		env_shutdown();
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

