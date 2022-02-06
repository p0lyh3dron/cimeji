#include "app.h"

ControlPanel* gPanel = nullptr;

ControlPanel::ControlPanel()
{
#if 0
    shimeji_t *pShimeji = shimeji_init( "/home/karl/Downloads/shimejiee/img/rai/" );
    if( !pShimeji ) {
        fprintf( stderr, "Failed to initialize shimeji.\n" );
        return;
    }
    auto label  = new QLabel( this );
    QPixmap pixmap( pShimeji->apData[ 0 ]->aWidth, pShimeji->apData[ 0 ]->aHeight );
    if ( pixmap.loadFromData( pShimeji->apData[ 0 ]->apBuf, pShimeji->apData[ 0 ]->aDataSize, "PNG" ) )
    {
        fprintf( stderr, "Loaded\n" );
        label->setPixmap( pixmap );
    }
    //QPixmap pixmap( "/home/karl/Downloads/shimejiee/img/rai/shime18.png" );
    label->show();
#endif
}

ControlPanel::~ControlPanel()
{
}