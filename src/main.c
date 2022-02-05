#include <assert.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

int x = 0;
int y = 0;
Display *d;

void
allow_input_passthrough (Window w)
{
    XserverRegion region = XFixesCreateRegion (d, NULL, 0);

    XFixesSetWindowShapeRegion (d, w, ShapeBounding, 0, 0, 0);
    XFixesSetWindowShapeRegion (d, w, ShapeInput, 0, 0, region);

    XFixesDestroyRegion (d, region);
}

void draw(cairo_t *cr) {
    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.5);
    cairo_rectangle(cr, x, y, 128, 128);
    cairo_fill(cr);
}

int main() {
    d = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(d);
    int default_screen = XDefaultScreen(d);

    // these two lines are really all you need
    XSetWindowAttributes attrs;
    attrs.override_redirect = 1;

    XVisualInfo vinfo;
    if (!XMatchVisualInfo(d, DefaultScreen(d), 32, TrueColor, &vinfo)) {
        printf("No visual found supporting 32 bit color, terminating\n");
        return -1;//exit(EXIT_FAILURE);
    }
    // these next three lines add 32 bit depth, remove if you dont need and change the flags below
    attrs.colormap = XCreateColormap(d, root, vinfo.visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;

    // Window XCreateWindow(
    //     Display *display, Window parent,
    //     int x, int y, unsigned int width, unsigned int height, unsigned int border_width,
    //     int depth, unsigned int class, 
    //     Visual *visual,
    //     unsigned long valuemask, XSetWindowAttributes *attributes
    // );
    Window overlay = XCreateWindow(
        d, root,
        1920, 0, 1920, 1080, 0,
        vinfo.depth, InputOutput, 
        vinfo.visual,
        CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel, &attrs
    );

    allow_input_passthrough( overlay );

    XMapWindow(d, overlay);

    cairo_surface_t* surf = cairo_xlib_surface_create(d, overlay,
                                  vinfo.visual,
                                  1920, 1080);
    cairo_t* cr = cairo_create(surf);

    for (int i = 0; ; ++i) {
	    x = 200 + 200 * sin( 2 + (float)i * 0.01 );
	    y = 200 + 200 * cos( (float)i * 0.02 );
	    draw(cr);
	    /*cairo_save( cr );
	    cairo_set_operator( cr, CAIRO_OPERATOR_SOURCE );
	    cairo_paint( cr );
	    cairo_restore( cr );*/
	    usleep( 5000 );
	    XFlush(d);
	    XClearWindow( d, overlay );
    }

    cairo_destroy(cr);
    cairo_surface_destroy(surf);

    XUnmapWindow(d, overlay);
    XCloseDisplay(d);
    return 0;
}
