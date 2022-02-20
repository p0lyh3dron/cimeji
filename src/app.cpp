#include "app.h"

ControlPanel* gPanel = nullptr;

ControlPanel::ControlPanel()
{
    #if 0
    avatar_t *pShimeji = avatar_create( "/home/karl/Downloads/shimejiee/img/rai/" );
    if( !pShimeji ) {
        fprintf( stderr, "Failed to initialize shimeji.\n" );
        return;
    }
#endif

#if 1
    auto label  = new QLabel( this );
    QPixmap pixmap( "C:\\UserPrograms\\shimejiee\\img\\rai\\shime1.png" );
    label->setPixmap( pixmap );

    //QPixmap pixmap( "/home/karl/Downloads/shimejiee/img/rai/shime18.png" );

    label->show();
#endif

    setWindowFlags( Qt::Drawer );
    setAttribute( Qt::WA_QuitOnClose );
}

ControlPanel::~ControlPanel()
{
}
