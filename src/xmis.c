#define BC 6 //button count
#define WK 16 //toolbox button count
#define CBC 28 //colorbox count
#define WKW 25 //tool button size
#define OWW 600 //original window width
#define OWH 500 //original window height
#define BARH 20 //topbar height
#define CBB 15 //colorbox button size

Display* d;
XSetWindowAttributes at, bat;
Window win, topbar, toolbox, scolor, colorbox, vp, cv;
XSizeHints wmsiz;
XWMHints wmh;
XTextProperty winn, iconn;
XEvent xev;
char* wn="draw";
char* in="draw";
int sn, done=0;
unsigned long valuemask, clr=0xf;

XColor exact, closest;
GC gc, gc2, xpmgc;
XGCValues xgcv;

Window btns[BC];
char* bl[BC]={"File", "Edit", "View", "Image", "Colors", "Help"};

Window white[WK];
Window cbtns[CBC];

Pixmap pattern, clipper;
XpmAttributes xpmat;

Pixmap stipple;
static unsigned char stipple_bits[]={0x01, 0x02};

Pixmap cvpm;
int cvw=500, cvh=400;

unsigned long palette[CBC]={
0x000000,0x808080,0x800000,0x808000,0x008000,0x008080,0x000080,0x800080,0x808040,0x004040,0x0080ff,0x004080,0x4000ff,0x804000,
0xffffff,0xc0c0c0,0xff0000,0xffff00,0x00ff00,0x00ffff,0x0000ff,0xff00ff,0xffff80,0x00ff80,0x80ffff,0x8080ff,0xff0080,0xff8040
};
unsigned long fgc=0, bgc=0xffffff;

unsigned char tool=6, prevtool;

void xinit(){
//open display
d=XOpenDisplay("");

//create main window
sn=DefaultScreen(d);
at.background_pixel=WhitePixel(d, sn);
//at.background_pixel=0xffff00;
at.border_pixel=BlackPixel(d, sn);
at.event_mask=ButtonPressMask|KeyPressMask|KeyReleaseMask|StructureNotifyMask;
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

	bat.event_mask=Button1MotionMask|ButtonReleaseMask|ButtonPressMask|ExposureMask;
	//viewport
	bat.background_pixel=0x808080;
	vp=XCreateWindow(d, win, 2*WKW, BARH, OWW-2*WKW, OWH-BARH-2*CBB, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);

	//canvas
	bat.background_pixel=0xffffff;
	cv=XCreateWindow(d, vp, 4, 4, cvw, cvh, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);

	bat.event_mask=ButtonPressMask|ExposureMask;
	//tool buttons
	bat.background_pixel=0xc0c0c0;
	for(int i=0;i<WK;i++){
	white[i]=XCreateWindow(d, toolbox, (i%2)*WKW, (i/2)*WKW, WKW, WKW, 1, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);
	}

	//in colorbox
	scolor=XCreateWindow(d, colorbox, 0, 0, 2*CBB, 2*CBB, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);
	for(int i=0;i<CBC;i++){
	bat.background_pixel=palette[i];
	cbtns[i]=XCreateWindow(d, colorbox, 2*CBB+(i%14)*CBB, (i/14)*CBB, CBB, CBB, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);
	}

bat.event_mask=ButtonPressMask|ExposureMask;
//bat.background_pixel=xgcv.background;
bat.background_pixel=0xc0c0c0;
for(int i=0;i<BC;i++){
btns[i]=XCreateWindow(d, topbar, i*45, 0, 45, 20, 0, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);
}

xpmat.color_key = XPM_COLOR;
xpmat.valuemask = XpmColorKey | XpmColorTable ;
XpmCreatePixmapFromData(d, win, tools, &pattern, &clipper, &xpmat);
xpmgc = XCreateGC(d, win, 0, NULL);
XSetForeground(d, xpmgc, WhitePixel(d, sn));
XSetBackground(d, xpmgc, BlackPixel(d, sn));
XSetClipMask(d, xpmgc, clipper);
XSetClipOrigin(d, xpmgc, 0, 0);

stipple=XCreatePixmapFromBitmapData(d, win, stipple_bits, 2, 2, 0xffffff, 0xc0c0c0, 1);
XSetStipple(d, gc, stipple);

cvpm=XCreatePixmap(d, win, cvw, cvh, 24);
XSetFillStyle(d, gc, FillSolid);
XSetForeground(d, gc, 0xffffff);
XFillRectangle(d, cvpm, gc, 0, 0, cvw, cvh);

at.background_pixel=0xd3d3d3;
at.event_mask=ButtonPressMask|ButtonReleaseMask|Button1MotionMask;
valuemask=CWBackPixel|CWBorderPixel|CWEventMask;


XMapWindow(d, win);
XMapWindow(d, topbar);
XMapWindow(d, toolbox);
XMapWindow(d, colorbox);
XMapWindow(d, scolor);
XMapWindow(d, vp);
XMapWindow(d, cv);
for(int i=0;i<WK;i++)
XMapWindow(d, white[i]);
for(int i=0;i<BC;i++)
XMapWindow(d, btns[i]);
for(int i=0;i<CBC;i++)
XMapWindow(d, cbtns[i]);


}

void redraw_scolor(){
	XSetFillStyle(d, gc, FillStippled);
	XSetForeground(d, gc, 0xffffff);
	XFillRectangle(d, scolor, gc, 0, 0, 2*CBB, 2*CBB);

	XSetFillStyle(d, gc, FillSolid);
	XSetForeground(d, gc, bgc);
	XFillRectangle(d, scolor, gc, CBB-4, CBB-4, CBB, CBB);
	XSetForeground(d, gc, fgc);
	XFillRectangle(d, scolor, gc, 3, 3, CBB, CBB);
}

int redraw_toolbtn(Window w, int i){
	XClearWindow(d, w);
	if(i==tool){
		XSetFillStyle(d, gc, FillStippled);
		XSetForeground(d, gc, 0xffffff);
		XFillRectangle(d, w, gc, 0, 0, WKW, WKW);
	}
	XSetClipOrigin(d, xpmgc, 4-i*16, 4);
	XCopyArea(d, pattern, w, xpmgc, i*16, 0, 16, 16, 4, 4);
}

int redraw_toolbtns(Window w){
	for(int i=0;i<WK;i++)
	if(w==white[i]){
		redraw_toolbtn(w, i);
		return 1;
	}
	return 0;
}

int redraw_menubtns(Window w){
	for(int i=0;i<BC;i++)
	if(w==btns[i]){
		XSetForeground(d, gc, 0);
		XSetBackground(d, gc, 0xc0c0c0);
		XDrawImageString(d, btns[i], gc, 5, 15, bl[i], strlen(bl[i]));
		return 1;
	}
	return 0;
}

// functions for drawing
void nop(int x, int y){
return;
}

int penx, peny;
void pen(int x, int y){
	XSetFillStyle(d, gc, FillSolid);
	XSetForeground(d, gc, fgc);
	XDrawPoint(d, cv, gc, x, y);
	XDrawPoint(d, cvpm, gc, x, y);
	penx=x; peny=y;
}

void mpen(int x, int y){
	XSetFillStyle(d, gc, FillSolid);
	XSetForeground(d, gc, fgc);
	XDrawLine(d, cv, gc, penx, peny, x, y);
	XDrawLine(d, cvpm, gc, penx, peny, x, y);
	penx=x; peny=y;
}

int mod(int num){
if(num<0) return -num;
return num;
}

int rectx, recty, prevrx, prevry;
void rect(int x, int y){
	prevrx=rectx=x; prevry=recty=y;
}

void mrect(int x, int y){
	int rlrectx=(rectx<x)*rectx+(rectx>x)*x, rlrecty=(recty<y)*recty+(recty>y)*y;
	int rlrectprevx=(rectx<prevrx)*rectx+(rectx>prevrx)*prevrx, rlrectprevy=(recty<prevry)*recty+(recty>prevry)*prevry;
	int clrx=(rectx>x)*prevrx+(rectx<x)*x+(x==rectx)*((prevrx<=rectx)*prevrx+(prevrx>rectx)*rectx);
	int clry=(recty>y)*prevry+(recty<y)*y+(y==recty)*((prevry<=recty)*prevry+(prevry>recty)*recty);
	XSetFillStyle(d, gc, FillSolid);
	XSetForeground(d, gc, fgc);
	if(
		((prevrx-rectx)>0)!=((x-rectx)>0) ||
		((prevry-recty)>0)!=((y-recty)>0)
	)
	XCopyArea(d, cvpm, cv, gc, rlrectprevx, rlrectprevy, mod(prevrx-rectx), mod(prevry-recty), rlrectprevx, rlrectprevy);
	if(mod(prevrx-rectx)>mod(rectx-x)) XCopyArea(d, cvpm, cv, gc, clrx, rlrecty, mod(prevrx-x), mod(y-recty), clrx, rlrecty);
	if(mod(prevry-recty)>mod(recty-y)) XCopyArea(d, cvpm, cv, gc, rlrectprevx, clry, mod(prevrx-rectx), mod(prevry-y), rlrectprevx, clry);
	XFillRectangle(d, cv, gc, rlrectx, rlrecty, mod(x-rectx), mod(y-recty));
	prevrx=x; prevry=y;
}

void rrect(int x, int y){
	XSetFillStyle(d, gc, FillSolid);
	XSetForeground(d, gc, fgc);
	XCopyArea(d, cvpm, cv, gc, 0, 0, cvw, cvh, 0, 0);
	XFillRectangle(d, cv, gc, (rectx<x)*rectx+(rectx>x)*x, (recty<y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
	XFillRectangle(d, cvpm, gc, (rectx<x)*rectx+(rectx>x)*x, (recty<y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
}

int linex, liney, prevlx, prevly;
void line(int x, int y){
	prevlx=linex=x; prevly=liney=y;
}

void mline(int x, int y){
	int rllinex=(linex<prevlx)*linex+(linex>prevlx)*prevlx, rlliney=(liney<prevly)*liney+(liney>prevly)*prevly;
	XSetFillStyle(d, gc, FillSolid);
	XSetForeground(d, gc, fgc);
	XCopyArea(d, cvpm, cv, gc, rllinex, rlliney, mod(prevlx-linex)+1, mod(prevly-liney)+1, rllinex, rlliney);
	//XCopyArea(d, cvpm, cv, gc, rllinex, rlliney, mod(prevlx-linex)+1, mod(prevly-liney)+1, rllinex, rlliney);
	XDrawLine(d, cv, gc, linex, liney, x, y);
	prevlx=x; prevly=y;
}

void rline(int x, int y){
	int rllinex=(linex<x)*linex+(linex>x)*x, rlliney=(liney<y)*liney+(liney>y)*y;
	XSetFillStyle(d, gc, FillSolid);
	XSetForeground(d, gc, fgc);
	XCopyArea(d, cvpm, cv, gc, rllinex, rlliney, mod(x-linex), mod(y-liney), rllinex, rlliney);
	XDrawLine(d, cv, gc, linex, liney, x, y);
	XDrawLine(d, cvpm, gc, linex, liney, x, y);
}

void (*pdrawfs[WK])(int x, int y)={pen,pen,pen,pen,pen,pen,pen,pen,pen,pen,line,pen,rect,pen,pen,pen};
void (*mdrawfs[WK])(int x, int y)={pen,pen,pen,pen,pen,pen,mpen,pen,pen,pen,mline,pen,mrect,pen,pen,pen};
void (*rdrawfs[WK])(int x, int y)={nop,nop,nop,nop,nop,nop,nop,nop,nop,nop,rline,nop,rrect,nop,nop,nop};

void xclose(){
XUnmapWindow(d, win);
XDestroyWindow(d, win);
XCloseDisplay(d);
}
