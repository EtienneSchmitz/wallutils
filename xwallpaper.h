#include "X11/bitmaps/gray"
#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xlib.h>
#include <X11/Xmu/CurUtil.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Display* dpy;
static int screen;
static Window root;

// Read black/white X bitmap file
static Pixmap ReadXBMFile(char* filename, unsigned int* width, unsigned int* height, int* x_hot, int* y_hot)
{
    Pixmap bitmap;
    int status = XReadBitmapFile(dpy, root, filename, width, height, &bitmap, x_hot, y_hot);
    if (status == BitmapSuccess) {
        return (bitmap);
    } else if (status == BitmapOpenFailed) {
        fprintf(stderr, "can't open file: %s\n", filename);
    } else if (status == BitmapFileInvalid) {
        fprintf(stderr, "bad bitmap format file: %s\n", filename);
    }
    fprintf(stderr, "insufficient memory for bitmap: %s", filename);
    exit(1);
}

// Read colorful X pixmap file
static Pixmap ReadXPMFile(char* filename, unsigned int* width, unsigned int* height)
{
    Pixmap pixmap;
    XpmAttributes attributes;
    // shapemask is set to NULL and is only used for transparency
    int status = XpmReadFileToPixmap(dpy, root, filename, &pixmap, NULL, &attributes);
    if (status == XpmSuccess) {
        *width = attributes.width;
        *height = attributes.height;
        return (pixmap);
    }
    fprintf(stderr, "could not read XPM: %s", filename);
    exit(1);
}

// Set the background image to the given pixmap
static void SetBackgroundToPixmapAndFree(Pixmap bitmap, unsigned int width, unsigned int height)
{
    XGCValues gc_init;
    GC gc = XCreateGC(dpy, root, GCForeground | GCBackground, &gc_init);
    Pixmap pix = XCreatePixmap(dpy, root, width, height, (unsigned int)DefaultDepth(dpy, screen));
    XCopyPlane(dpy, bitmap, pix, gc, 0, 0, width, height, 0, 0, (unsigned long)1);
    XSetWindowBackgroundPixmap(dpy, root, pix);
    XFreeGC(dpy, gc);
    XFreePixmap(dpy, bitmap);
    XFreePixmap(dpy, pix);
    XClearWindow(dpy, root);
}

int SetBackground(char* filename)
{
    unsigned int ww;
    unsigned int hh;

    dpy = XOpenDisplay("");
    if (!dpy) {
        return -1;
    }
    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);

    //Pixmap bitmap = ReadBitmapFile(filename, &ww, &hh, (int*)NULL, (int*)NULL);
    Pixmap bitmap = ReadXPMFile(filename, &ww, &hh);
    SetBackgroundToPixmapAndFree(bitmap, ww, hh);

    if (dpy) {
        XCloseDisplay(dpy);
    }

    return 0;
}
