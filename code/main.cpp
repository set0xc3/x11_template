#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include "base_def.h"

struct app_context
{
    b8 quit;
    i32 screen_width, screen_height;
    
    GC gc;
    Window window;
    Display *display;
    XVisualInfo *visual;
    GLXContext gl_context;
};

int 
main(int argc, char **argv)
{
    local_variable app_context app;
    ZERO_STRUCT(&app);
    
    i32 screen = 0;
    u32 black, white = 0;
    
    app.display = XOpenDisplay((char *)0);
    if (!app.display)
        return -1;
    
    screen = DefaultScreen(app.display);
    black=BlackPixel(app.display, screen);
	white=WhitePixel(app.display, screen);
    
    int nitems_return[5] =
    {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 
        24,
        GLX_DOUBLEBUFFER,
        None
    };
    app.visual = glXChooseVisual(app.display, 0, nitems_return);
    if (!app.visual) 
        return -1;
    
    app.gl_context = glXCreateContext(app.display, app.visual, 0, true);
    if (!app.gl_context) 
        return -1;
    
    app.window = XCreateSimpleWindow(app.display, 
                                     RootWindow(app.display, screen), 
                                     0, 0, 
                                     1280, 720, 
                                     5, 
                                     black, white);
    
    XStoreName(app.display, app.window, "X Windows-System X11");
	//XSetStandardProperties(app.display, app.window,"My Window", "HI!", None, 0, 0, 0);
    
    glXMakeCurrent(app.display, app.window, app.gl_context);
    
    u64 input_mask = ExposureMask|ButtonPressMask|KeyPressMask;
    XSelectInput(app.display, app.window, input_mask);
    
    XMapWindow(app.display, app.window);
	XFlush(app.display);
    
    app.gc = XCreateGC(app.display, app.window, 0, 0);  
    XSetBackground(app.display, app.gc, white);
	XSetForeground(app.display, app.gc, black);
    
    XClearWindow(app.display, app.window);
	XMapRaised(app.display, app.window);
    
    // Process window close event in X11 event loop for(;;)
	Atom wnd_del = XInternAtom(app.display, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(app.display, app.window, &wnd_del, 1);
    
    while (!app.quit)
    {
        while (XPending(app.display) > 0) 
        {
            XEvent event;
            XNextEvent(app.display, &event);
            
            if (event.type == ClientMessage)
            {
                fprintf(stderr, " [Event] User closed window. => Shutdown \n");
                app.quit = true;
            }
            
            if (event.type == KeyPress) 
            {
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                if (keysym == XK_Escape) 
                {
                    fprintf(stderr, " [Event] User closed window. => Shutdown \n");
                    app.quit = true;
                }
            }
            
            if(event.type == Expose)    
                fprintf(stderr,  " [Event] Expose event. \n");
            
            if(event.type == KeyPress)    
                fprintf(stderr,  " [Event] Key pressed. \n");
            
            if(event.type == ButtonPress) 
                fprintf(stderr,  " [Event] Mouse button pressed. \n");
            
            if(event.type == PointerMotionMask) 
                fprintf(stderr,  " [Event] Mouse button pressed. \n");	
        }
        
        glClearColor(0.15, 0.15, 0.15, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBegin(GL_TRIANGLES);
        glColor3f(0.68, 0.84, 0.0);
        glVertex2f(-0.5, -0.5);
        glVertex2f( 0.5, -0.5);
        glVertex2f( 0.0,  0.5);
        glEnd();
        
        XDrawRectangle(app.display, app.window, app.gc, 0, 0, 100, 100); 
        
        char string[] = "Test";
        XDrawString(app.display, app.window, app.gc, 20, 20, string, strlen(string));
        
        glXSwapBuffers(app.display, app.window);
        
        usleep(1);
    }
    
    glXDestroyContext(app.display, app.gl_context);
    
    XFreeGC(app.display, app.gc);
	XDestroyWindow(app.display, app.window);
	XCloseDisplay(app.display);	
    
    return 0;
}