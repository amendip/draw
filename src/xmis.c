#define BC 6 //button count
#define WK 16 //toolbox button count
#define CBC 28 //colorbox count
#define WKW 25 //tool button size
#define OWW 500 //original window width
#define OWH 400 //original window height
#define BARH 20 //topbar height
#define CBB 15 //colorbox button size

Display* d;
XSetWindowAttributes at, bat;
Window win, topbar, toolbox, colorbox, vp, cv;
XSizeHints wmsiz;
XWMHints wmh;
XTextProperty winn, iconn;
XEvent xev;
char* wn="wave generator";
char* in="wave generator";
int sn, done=0;
unsigned long valuemask, clr=0xf;

XColor exact, closest;
GC gc, gc2;
XGCValues xgcv;

Window btns[BC];
char* bl[BC]={"File", "Edit", "View", "Image", "Colors", "Help"};

Window white[WK];
Window cbtns[CBC];

void xinit(){
//open display
d=XOpenDisplay("");

//create main window
sn=DefaultScreen(d);
at.background_pixel=WhitePixel(d, sn);
//at.background_pixel=0xffff00;
at.border_pixel=BlackPixel(d, sn);
at.event_mask=ButtonPressMask|KeyPressMask|KeyReleaseMask;
valuemask=CWBackPixel|CWBorderPixel|CWEventMask;
win=XCreateWindow(d, RootWindow(d, sn), 200, 200, OWW, OWH, 2, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &at);

//wm hints
wmsiz.flags=USPosition|USSize;
XSetWMNormalHints(d, win, &wmsiz);
wmh.initial_state=NormalState;
wmh.flags=StateHint;
XSetWMHints(d, win, &wmh);
XStringListToTextProperty(&wn, 1, &winn);
XSetWMName(d, win, &winn);
XStringListToTextProperty(&in, 1, &iconn);
XSetWMIconName(d, win, &iconn);

//colors
XAllocNamedColor(d, XDefaultColormap(d, sn), "black", &exact, &closest);
xgcv.foreground=exact.pixel;
XAllocNamedColor(d, XDefaultColormap(d, sn), "web gray", &exact, &closest);
xgcv.background=exact.pixel;
valuemask=GCForeground|GCBackground;
gc=XCreateGC(d, win, valuemask, &xgcv);
gc2=XCreateGC(d, win, 0, NULL);
XSetBackground(d, gc2, WhitePixel(d, sn));
XSetForeground(d, gc2, BlackPixel(d, sn));

//button
valuemask=CWBackPixel|CWBorderPixel|CWEventMask;
bat.background_pixel=WhitePixel(d, sn);
bat.border_pixel=BlackPixel(d, sn);
//bat.border_pixel=0x404040;

	//bat.event_mask=ExposureMask;
	bat.event_mask=0;
	bat.background_pixel=0x800000;
	//topbar
	topbar=XCreateWindow(d, win, 0, 0, OWW, BARH, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);
	
	//toolbox
	toolbox=XCreateWindow(d, win, 0, BARH, 2*WKW, OWH-2*CBB-BARH, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);

	//colorbox
	colorbox=XCreateWindow(d, win, 0, OWH-2*CBB, OWW, 2*CBB, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);

	//viewport
	bat.background_pixel=0x808080;
	vp=XCreateWindow(d, win, 2*WKW, BARH, OWW-BARH-2*CBB, OWH-2*WKW, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);

	//canvas
	bat.background_pixel=0xffffff;
	cv=XCreateWindow(d, vp, 4, 4, 300, 200, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);

	bat.event_mask=Button1MotionMask|ButtonReleaseMask|ButtonPressMask|ExposureMask;
	//tool buttons
	bat.background_pixel=0x008000;
	for(int i=0;i<WK;i++){
	white[i]=XCreateWindow(d, toolbox, (i%2)*WKW, (i/2)*WKW, WKW, WKW, 1, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);
	}

	for(int i=0;i<CBC;i++){
	cbtns[i]=XCreateWindow(d, colorbox, (i%14)*CBB, (i/14)*CBB, CBB, CBB, 1, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);
	}

bat.event_mask=ButtonPressMask|ExposureMask;
bat.background_pixel=xgcv.background;
for(int i=0;i<BC;i++){
btns[i]=XCreateWindow(d, topbar, i*45, 0, 45, 20, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);
}

at.background_pixel=0xd3d3d3;
at.event_mask=ButtonPressMask|ButtonReleaseMask|Button1MotionMask;
valuemask=CWBackPixel|CWBorderPixel|CWEventMask;


XMapWindow(d, win);
XMapWindow(d, topbar);
XMapWindow(d, toolbox);
XMapWindow(d, colorbox);
XMapWindow(d, vp);
XMapWindow(d, cv);
for(int i=0;i<WK;i++)
XMapWindow(d, white[i]);
for(int i=0;i<BC;i++)
XMapWindow(d, btns[i]);
for(int i=0;i<CBC;i++)
XMapWindow(d, cbtns[i]);
}

void xclose(){
XUnmapWindow(d, win);
XDestroyWindow(d, win);
XCloseDisplay(d);
}
