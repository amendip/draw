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
Window win, topbar, toolbox, scolor, colorbox, vp, cv, options;
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

Pixmap pattern, clipper, airbp, airbc, otp, otc;
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
	//options
	options=XCreateWindow(d, toolbox, 5, WK*WKW/2+5, 40, 64, 1, DefaultDepth(d, sn), InputOutput, DefaultVisual(d, sn), valuemask, &bat);

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
XpmCreatePixmapFromData(d, win, airb, &airbp, &airbc, &xpmat);
XpmCreatePixmapFromData(d, win, options_transparency, &otp, &otc, &xpmat);
xpmgc = XCreateGC(d, win, 0, NULL);
XSetForeground(d, xpmgc, WhitePixel(d, sn));
XSetBackground(d, xpmgc, BlackPixel(d, sn));
XSetClipMask(d, xpmgc, clipper);

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
XMapWindow(d, options);
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

//redraw functions

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
		XSetFillStyle(d, gc, FillSolid);
	}
	XSetClipMask(d, xpmgc, clipper);
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

// conf variables
int filltype=1, linethick=1, spraysize=2, bgtransp=0, erasersize=0, zoomby=0, brushtype=9;

void redraw_options(){
	int bs;
	int zooms[4]={1,2,6,8};
	char timez[3]="1x";
	XClearWindow(d, options);
	switch(tool){
		case 0: case 1:
		for(int i=0;i<2;i++){
			if(i==bgtransp){
				XSetForeground(d, gc, 0x000080);
				XFillRectangle(d, options, gc, 0, 4+i*30, 40, 26);
			}
			XSetClipMask(d, xpmgc, otc);
			XSetClipOrigin(d, xpmgc, 4, 6+i*8);
			XCopyArea(d, otp, options, xpmgc, 0, i*22, 35, 23, 4, 6+i*30);
			//XSetForeground(d, gc, 0x000000);
			//XFillRectangle(d, options, gc, 20-i-2, 7+14*i, 4+i*2, 4+i*2);
		}
		break;
		case 2:
		for(int i=0;i<4;i++){
			if(i==erasersize){
				XSetForeground(d, gc, 0x000080);
				XFillRectangle(d, options, gc, 0, i*16, 40, 16);
				XSetForeground(d, gc, 0xffffff);
				XFillRectangle(d, options, gc, 20-i-2, 7+14*i, 4+i*2, 4+i*2);
			}else{
				XSetForeground(d, gc, 0x000000);
				XFillRectangle(d, options, gc, 20-i-2, 7+14*i, 4+i*2, 4+i*2);
			}
		}
		break;
		case 5:
		for(int i=0;i<4;i++){
			timez[0]='0'+zooms[i];
			if(i==zoomby){
				XSetForeground(d, gc, 0x000080);
				XFillRectangle(d, options, gc, 0, i*16, 40, 16);
				XSetBackground(d, gc, 0x000080);
				XSetForeground(d, gc, 0xffffff);
				XDrawImageString(d, options, gc, 8, 13+15*i, timez, 2);
				XFillRectangle(d, options, gc, 30-zooms[i]/2, 7+15*i, zooms[i], zooms[i]);
			}else{
				XSetBackground(d, gc, 0xc0c0c0);
				XSetForeground(d, gc, 0x000000);
				XDrawImageString(d, options, gc, 8, 13+15*i, timez, 2);
				XFillRectangle(d, options, gc, 30-zooms[i]/2, 7+15*i, zooms[i], zooms[i]);
			}
		}
		break;
		case 7:
		for(int i=0;i<12;i++){
			bs=2+3*(2-i%3);
			if(i==brushtype){
				XSetForeground(d, gc, 0x000080);
				XFillRectangle(d, options, gc, 1+(i%3)*13, 2+(i/3)*16, 12, 12);
				XSetForeground(d, gc, 0xffffff);
			}else XSetForeground(d, gc, 0x000000);
			switch(i/3){
				case 0:
					XFillArc(d, options, gc, 3+(i%3)*13, 4+(i%3)*1.5, bs, bs, 0, 23040);
				break;
				case 1:
					XFillRectangle(d, options, gc, 3+(i%3)*13, 20+(i%3)*1.5, bs, bs);
				break;
				case 2:
					XDrawLine(d, options, gc, 3+(i%3)*13, 36+(i%3)*1.5+bs, 3+(i%3)*13+bs, 36+(i%3)*1.5);
				break;
				case 3:
					XDrawLine(d, options, gc, 3+(i%3)*13, 52+(i%3)*1.5, 3+(i%3)*13+bs, 52+(i%3)*1.5+bs);
				break;
			}
		}
		break;
		case 8:
		XSetClipMask(d, xpmgc, airbc);
		for(int i=0;i<3;i++){
			XSetClipOrigin(d, xpmgc, 3+(i%2)*20+(i/2)*5-i*16, 6+(i/2)*30);
			if(i==spraysize){
				XSetForeground(d, gc, 0x000080);
				XFillRectangle(d, options, gc, 3+(i%2)*20+(i/2)*5, 6+(i/2)*30, 16+(i/2)*8, 23);
				XSetFunction(d, xpmgc, GXcopyInverted);
				XCopyArea(d, airbp, options, xpmgc, i*16, 0, 16+(i/2)*8, 23, 3+(i%2)*20+(i/2)*5, 6+(i/2)*30);
				XSetFunction(d, xpmgc, GXcopy);
			}else{
				XCopyArea(d, airbp, options, xpmgc, i*16, 0, 16+(i/2)*8, 23, 3+(i%2)*20+(i/2)*5, 6+(i/2)*30);
			}
		}
		break;
		case 12: case 13: case 14: case 15:
		for(int i=0;i<3;i++){
			if(i==filltype){
				XSetForeground(d, gc, 0x000080);
				XFillRectangle(d, options, gc, 0, i*21, 40, 21);
				if(i>0){
					XSetForeground(d, gc, 0x808080);
					XFillRectangle(d, options, gc, 7, 7+21*i, 26, 9);
				}
				if(i<2){
					XSetForeground(d, gc, 0xffffff);
					XDrawRectangle(d, options, gc, 7, 7+21*i, 26, 9);
				}
			}else{
				if(i>0){
					XSetForeground(d, gc, 0x808080);
					XFillRectangle(d, options, gc, 7, 7+21*i, 26, 9);
				}
				if(i<2){
					XSetForeground(d, gc, 0x000000);
					XDrawRectangle(d, options, gc, 7, 7+21*i, 26, 9);
				}
			}
		}
		break;
	}
	//XSetClipOrigin(d, xpmgc, 4-i*16, 4);
	//XCopyArea(d, pattern, w, xpmgc, i*16, 0, 16, 16, 4, 4);
}

// tool option picking
void optionclick(int x, int y){
	int bs;
	int zooms[4]={1,2,6,8};
	char timez[3]="1x";
	XClearWindow(d, options);
	switch(tool){
		case 0: case 1:
		bgtransp=y/32;
		break;
		case 2:
		erasersize=y/16;
		break;
		case 5:
		zoomby=y/16;
		break;
		case 7:
		brushtype=(y/16)*3+x/13;
		break;
		case 8:
		if(y<32) spraysize=(x/20);
		else spraysize=2;
		break;
		case 12: case 13: case 14: case 15:
		filltype=y/21;
		break;
	}
	redraw_options();
}

// functions for drawing

// auxiliary functions
void nop(int x, int y){
return;
}

int mod(int num){
if(num<0) return -num;
return num;
}

// Pencil functions
int penx, peny;
void pen(int x, int y){
	XSetForeground(d, gc, fgc);
	XDrawPoint(d, cv, gc, x, y);
	XDrawPoint(d, cvpm, gc, x, y);
	penx=x; peny=y;
}

void mpen(int x, int y){
	XSetForeground(d, gc, fgc);
	XDrawLine(d, cv, gc, penx, peny, x, y);
	XDrawLine(d, cvpm, gc, penx, peny, x, y);
	penx=x; peny=y;
}

// Brush functions
int brushx, brushy;
void brush(int x, int y){
	int brushsize=2+3*(2-brushtype%3);
	XSetForeground(d, gc, fgc);

	switch(brushtype/3){
		case 0:
			XFillArc(d, cv, gc, x-brushsize/2, y-brushsize/2, brushsize, brushsize, 0, 23040);
		break;
		case 1:
			XFillRectangle(d, cv, gc, x-brushsize/2, y-brushsize/2, brushsize, brushsize);
		break;
		case 2:
			XDrawLine(d, cv, gc, x-brushsize/2, y+brushsize/2, x+brushsize/2, y-brushsize/2);
		break;
		case 3:
			XDrawLine(d, cv, gc, x-brushsize/2, y-brushsize/2, x+brushsize/2, y+brushsize/2);
		break;
	}
//brushtype=(brushtype+1)%11;
	brushx=x; brushy=y;
}

void mbrush(int x, int y){
	int brushsize=2+3*(2-brushtype%3);
	int diag=1-2*((x>=brushx && y>=brushy) || (x<brushx && y<brushy));
	float m=(y-brushy)/(float)(x-brushx);
	int s=brushsize/2, sy=s/sqrt(m*m+1), sx=(1-2*(m<0))*sqrt(s*s-sy*sy);
	XPoint brushcon[4];
	XSetForeground(d, gc, fgc);

	//XDrawLine(d, cv, gc, brushx, brushy, x, y);
	//XDrawLine(d, cvpm, gc, brushx, brushy, x, y);
	switch(brushtype/3){
		default:
		case 0:
			brushcon[0].x=brushx-sx;
			brushcon[0].y=brushy+sy;
			brushcon[1].x=x-sx;
			brushcon[1].y=y+sy;
			brushcon[2].x=x+sx;
			brushcon[2].y=y-sy;
			brushcon[3].x=brushx+sx;
			brushcon[3].y=brushy-sy;
		break;
		case 1:
			brushcon[0].x=brushx-s;
			brushcon[0].y=brushy-diag*s;
			brushcon[1].x=x-s;
			brushcon[1].y=y-diag*s;
			brushcon[2].x=x+s;
			brushcon[2].y=y+diag*s;
			brushcon[3].x=brushx+s;
			brushcon[3].y=brushy+diag*s;
		break;
		case 2:
			brushcon[0].x=brushx-s;
			brushcon[0].y=brushy+s;
			brushcon[1].x=x-s;
			brushcon[1].y=y+s;
			brushcon[2].x=x+s;
			brushcon[2].y=y-s;
			brushcon[3].x=brushx+s;
			brushcon[3].y=brushy-s;
		break;
		case 3:
			brushcon[0].x=brushx-s;
			brushcon[0].y=brushy-s;
			brushcon[1].x=x-s;
			brushcon[1].y=y-s;
			brushcon[2].x=x+s;
			brushcon[2].y=y+s;
			brushcon[3].x=brushx+s;
			brushcon[3].y=brushy+s;
		break;
	}
	XFillPolygon(d, cv, gc, brushcon, 4, Nonconvex, CoordModeOrigin);
	XFillPolygon(d, cvpm, gc, brushcon, 4, Nonconvex, CoordModeOrigin);
	
	switch(brushtype/3){
		case 0:
			XFillArc(d, cv, gc, x-s, y-s, brushsize, brushsize, 0, 23040);
			XFillArc(d, cvpm, gc, x-s, y-s, brushsize, brushsize, 0, 23040);
		break;
		case 1:
			XFillRectangle(d, cv, gc, x-s, y-s, brushsize, brushsize);
			XFillRectangle(d, cvpm, gc, x-s, y-s, brushsize, brushsize);
		break;
		case 2:
			XDrawLine(d, cv, gc, x-s, y+s, x+s, y-s);
			XDrawLine(d, cvpm, gc, x-s, y+s, x+s, y-s);
		break;
		case 3:
			XDrawLine(d, cv, gc, x-s, y-s, x+s, y+s);
			XDrawLine(d, cvpm, gc, x-s, y-s, x+s, y+s);
		break;
	}
	
	brushx=x; brushy=y;
}

// Eraser functions
int ersrx, ersry;
void ersr(int x, int y){
	int ersrsize=4+2*erasersize;
	XSetForeground(d, gc, bgc);

	XFillRectangle(d, cv, gc, x-ersrsize/2, y-ersrsize/2, ersrsize, ersrsize);
	ersrx=x; ersry=y;
}

void mersr(int x, int y){
	int ersrsize=4+2*erasersize;
	int diag=1-2*((x>=ersrx && y>=ersry) || (x<ersrx && y<ersry));
	int s=ersrsize/2;
	XPoint ersrcon[4];
	XSetForeground(d, gc, bgc);

	ersrcon[0].x=ersrx-s;
	ersrcon[0].y=ersry-diag*s;
	ersrcon[1].x=x-s;
	ersrcon[1].y=y-diag*s;
	ersrcon[2].x=x+s;
	ersrcon[2].y=y+diag*s;
	ersrcon[3].x=ersrx+s;
	ersrcon[3].y=ersry+diag*s;
	XFillPolygon(d, cv, gc, ersrcon, 4, Nonconvex, CoordModeOrigin);
	XFillPolygon(d, cvpm, gc, ersrcon, 4, Nonconvex, CoordModeOrigin);

	XFillRectangle(d, cv, gc, x-s, y-s, ersrsize, ersrsize);
	XFillRectangle(d, cvpm, gc, x-s, y-s, ersrsize, ersrsize);

	ersrx=x; ersry=y;
}

// Rectangle functions
int rectx, recty, prevrx, prevry;
void rect(int x, int y){
	prevrx=rectx=x; prevry=recty=y;
}

void mrect(int x, int y){
	int rlrectx=(rectx<=x)*rectx+(rectx>x)*x, rlrecty=(recty<=y)*recty+(recty>y)*y;
	int rlrectprevx=(rectx<=prevrx)*rectx+(rectx>prevrx)*prevrx, rlrectprevy=(recty<=prevry)*recty+(recty>prevry)*prevry;
	int clrx=(rectx>x)*prevrx+(rectx<x)*x+(x==rectx)*((prevrx<=rectx)*prevrx+(prevrx>rectx)*rectx);
	int clry=(recty>y)*prevry+(recty<y)*y+(y==recty)*((prevry<=recty)*prevry+(prevry>recty)*recty);
	if(filltype==0){
		XCopyArea(d, cvpm, cv, gc, rlrectprevx, rlrectprevy, mod(prevrx-rectx)+1, mod(prevry-recty)+1, rlrectprevx, rlrectprevy);
	}else{
		if(
			((prevrx-rectx)>0)!=((x-rectx)>0) ||
			((prevry-recty)>0)!=((y-recty)>0)
		)
		XCopyArea(d, cvpm, cv, gc, rlrectprevx, rlrectprevy, mod(prevrx-rectx)+1, mod(prevry-recty)+1, rlrectprevx, rlrectprevy);
		if(mod(prevrx-rectx)>mod(rectx-x)) XCopyArea(d, cvpm, cv, gc, clrx, rlrecty, mod(prevrx-x)+1, mod(y-recty)+1, clrx, rlrecty);
		if(mod(prevry-recty)>mod(recty-y)) XCopyArea(d, cvpm, cv, gc, rlrectprevx, clry, mod(prevrx-rectx)+1, mod(prevry-y)+1, rlrectprevx, clry);
	}
	XSetForeground(d, gc, fgc);
	switch(filltype){
		case 0:
			XDrawRectangle(d, cv, gc, rlrectx, rlrecty, mod(x-rectx), mod(y-recty));
		break;
		case 2:
			XFillRectangle(d, cv, gc, rlrectx, rlrecty, mod(x-rectx), mod(y-recty));
		break;
		case 1:
			XSetForeground(d, gc, bgc);
			XFillRectangle(d, cv, gc, rlrectx, rlrecty, mod(x-rectx), mod(y-recty));
			XSetForeground(d, gc, fgc);
			XDrawRectangle(d, cv, gc, rlrectx, rlrecty, mod(x-rectx), mod(y-recty));
		break;
	}
	prevrx=x; prevry=y;
}

void rrect(int x, int y){
	XSetForeground(d, gc, fgc);
	XCopyArea(d, cvpm, cv, gc, 0, 0, cvw, cvh, 0, 0);
	switch(filltype){
		case 0:
			XDrawRectangle(d, cv, gc, (rectx<=x)*rectx+(rectx>x)*x, (recty<=y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
			XDrawRectangle(d, cvpm, gc, (rectx<=x)*rectx+(rectx>x)*x, (recty<=y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
		break;
		case 2:
			XFillRectangle(d, cv, gc, (rectx<=x)*rectx+(rectx>x)*x, (recty<=y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
			XFillRectangle(d, cvpm, gc, (rectx<=x)*rectx+(rectx>x)*x, (recty<=y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
		break;
		case 1:
			XSetForeground(d, gc, bgc);
			XFillRectangle(d, cv, gc, (rectx<=x)*rectx+(rectx>x)*x, (recty<=y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
			XFillRectangle(d, cvpm, gc, (rectx<=x)*rectx+(rectx>x)*x, (recty<=y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
			XSetForeground(d, gc, fgc);
			XDrawRectangle(d, cv, gc, (rectx<=x)*rectx+(rectx>x)*x, (recty<=y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
			XDrawRectangle(d, cvpm, gc, (rectx<=x)*rectx+(rectx>x)*x, (recty<=y)*recty+(recty>y)*y, mod(x-rectx), mod(y-recty));
		break;
	}
}

// Line functions
int linex, liney, prevlx, prevly;
void line(int x, int y){
	prevlx=linex=x; prevly=liney=y;
}

void mline(int x, int y){
	int rllinex=(linex<=prevlx)*linex+(linex>prevlx)*prevlx, rlliney=(liney<=prevly)*liney+(liney>prevly)*prevly;
	XSetForeground(d, gc, fgc);
	XCopyArea(d, cvpm, cv, gc, rllinex, rlliney, mod(prevlx-linex)+1, mod(prevly-liney)+1, rllinex, rlliney);
	//XCopyArea(d, cvpm, cv, gc, rllinex, rlliney, mod(prevlx-linex)+1, mod(prevly-liney)+1, rllinex, rlliney);
	XDrawLine(d, cv, gc, linex, liney, x, y);
	prevlx=x; prevly=y;
}

void rline(int x, int y){
	int rllinex=(linex<x)*linex+(linex>x)*x, rlliney=(liney<y)*liney+(liney>y)*y;
	XSetForeground(d, gc, fgc);
	XCopyArea(d, cvpm, cv, gc, rllinex, rlliney, mod(x-linex), mod(y-liney), rllinex, rlliney);
	XDrawLine(d, cv, gc, linex, liney, x, y);
	XDrawLine(d, cvpm, gc, linex, liney, x, y);
}

// Curve functions
#define N_SEG 30
void cubic_bezier(
Window w,
int x1, int y1,
int x2, int y2,
int x3, int y3,
int x4, int y4){
	double pts[N_SEG+1][2], t, omt, a, b, c, e, x, y;
	for(int i=0;i<=N_SEG;i++){
		t=(double)i/(double)N_SEG;
		omt=1.0-t;

		a=omt*omt*omt;
		b=3.0*t*omt*omt;
		c=3.0*t*t*omt;
		e=t*t*t;

		x=a*x1+b*x2+c*x3+e*x4;
		y=a*y1+b*y2+c*y3+e*y4;
		pts[i][0]=x;
		pts[i][1]=y;
	}
	
	for(int i=0;i<N_SEG;i++){
		XDrawLine(d, w, gc, pts[i][0], pts[i][1], pts[i+1][0], pts[i+1][1]);
	}
}

int curvex, curvey, prevcx, prevcy, crvh1x, crvh1y, crvh2x, crvh2y, crvstate=0;
void curve(int x, int y){
	switch(crvstate){
	case 0:
	prevcx=curvex=x; prevcy=curvey=y;
	break;
	case 1:
	crvh2x=prevcx; crvh2y=prevcy;
	crvh1x=x; crvh1y=y;
	break;
	case 2:
	default:
	crvh2x=x; crvh2y=y;
	break;
	}
}

void mcurve(int x, int y){
	int rlcurvex=(curvex<=prevcx)*curvex+(curvex>prevcx)*prevcx, rlcurvey=(curvey<=prevcy)*curvey+(curvey>prevcy)*prevcy;
	int crvhx=(crvh1x<=crvh2x)*crvh1x+(crvh1x>crvh2x)*crvh2x, crvhy=(crvh1y<=crvh2y)*crvh1y+(crvh1y>crvh2y)*crvh2y;
	int cbbx=(rlcurvex<=crvhx)*rlcurvex+(rlcurvex>crvhx)*crvhx, cbby=(rlcurvey<=crvhy)*rlcurvey+(rlcurvey>crvhy)*crvhy;
	int cbbw, cbbh;
	cbbw=(curvex<=prevcx)*prevcx+(curvex>prevcx)*curvex, cbbh=(curvey<=prevcy)*prevcy+(curvey>prevcy)*curvey;
	crvhx=(crvh1x<=crvh2x)*crvh2x+(crvh1x>crvh2x)*crvh1x, crvhy=(crvh1y<=crvh2y)*crvh2y+(crvh1y>crvh2y)*crvh1y;
	cbbw=(cbbw<=crvhx)*crvhx+(cbbw>crvhx)*cbbw-cbbx+1, cbbh=(cbbh<=crvhy)*crvhy+(cbbh>crvhy)*cbbh-cbby+1;
	XSetForeground(d, gc, fgc);
	switch(crvstate){
	case 0:
	XCopyArea(d, cvpm, cv, gc, rlcurvex, rlcurvey, mod(prevcx-curvex)+1, mod(prevcy-curvey)+1, rlcurvex, rlcurvey);
	XDrawLine(d, cv, gc, curvex, curvey, x, y);
	prevcx=x; prevcy=y;
	break;
	case 1:
	crvh1x=x; crvh1y=y;
	XCopyArea(d, cvpm, cv, gc, cbbx, cbby, cbbw, cbbh, cbbx, cbby);
	cubic_bezier(cv, curvex, curvey, crvh1x, crvh1y, crvh2x, crvh2y, prevcx, prevcy);
	break;
	case 2:
	default:
	crvh2x=x; crvh2y=y;
	XCopyArea(d, cvpm, cv, gc, cbbx, cbby, cbbw, cbbh, cbbx, cbby);
	cubic_bezier(cv, curvex, curvey, crvh1x, crvh1y, crvh2x, crvh2y, prevcx, prevcy);
	break;
	}
}

void rcurve(int x, int y){
	int rlcurvex=(curvex<x)*curvex+(curvex>x)*x, rlcurvey=(curvey<y)*curvey+(curvey>y)*y;
	XSetForeground(d, gc, fgc);
	if(crvstate==2){
		crvh2x=x; crvh2y=y;
		cubic_bezier(cv, curvex, curvey, crvh1x, crvh1y, crvh2x, crvh2y, prevcx, prevcy);
		cubic_bezier(cvpm, curvex, curvey, crvh1x, crvh1y, crvh2x, crvh2y, prevcx, prevcy);
	}
	crvstate=(crvstate+1)%3;
}

// Polyline functions
XPoint polyps[1024];
int plinex, pliney, prevplx, prevply, polypslen=0;
void pline(int x, int y){
	if(!polypslen){
		polyps[0].x=prevplx=plinex=x; polyps[0].y=prevply=pliney=y;
		polypslen=1;
	}else{
		plinex=prevplx; pliney=prevply;
		XDrawLine(d, cv, gc, plinex, pliney, x, y);
		prevplx=x; prevply=y;
	}
}

void mpline(int x, int y){
	int rlplinex=(plinex<=prevplx)*plinex+(plinex>prevplx)*prevplx, rlpliney=(pliney<=prevply)*pliney+(pliney>prevply)*prevply;
	XSetForeground(d, gc, fgc);
	XCopyArea(d, cvpm, cv, gc, rlplinex, rlpliney, mod(prevplx-plinex)+1, mod(prevply-pliney)+1, rlplinex, rlpliney);
	//XCopyArea(d, cvpm, cv, gc, rlplinex, rlpliney, mod(prevplx-plinex)+1, mod(prevply-pliney)+1, rlplinex, rlpliney);
	XDrawLine(d, cv, gc, plinex, pliney, x, y);
	prevplx=x; prevply=y;
}

void rpline(int x, int y){
	int rlplinex=(plinex<x)*plinex+(plinex>x)*x, rlpliney=(pliney<y)*pliney+(pliney>y)*y;
	polyps[polypslen].x=x; polyps[polypslen].y=y;
	polypslen++;
	if((mod(x-polyps[0].x)<8 && mod(y-polyps[0].y)<8) || (polypslen>2 && x==polyps[polypslen-2].x && y==polyps[polypslen-2].y) || polypslen>=1024){
		switch(filltype){
			case 1:
			XSetForeground(d, gc, bgc);
			XFillPolygon(d, cv, gc, polyps, polypslen, Complex, CoordModeOrigin);
			XFillPolygon(d, cvpm, gc, polyps, polypslen, Complex, CoordModeOrigin);
			XSetForeground(d, gc, fgc);
			for(int i=0;i<polypslen-1;i++){
			XDrawLine(d, cv, gc, polyps[i].x, polyps[i].y, polyps[i+1].x, polyps[i+1].y);
			XDrawLine(d, cvpm, gc, polyps[i].x, polyps[i].y, polyps[i+1].x, polyps[i+1].y);
			}
			XDrawLine(d, cv, gc, polyps[0].x, polyps[0].y, x, y);
			XDrawLine(d, cvpm, gc, polyps[0].x, polyps[0].y, x, y);
			break;
			case 0:
			XSetForeground(d, gc, fgc);
			XDrawLine(d, cv, gc, polyps[0].x, polyps[0].y, x, y);
			XDrawLine(d, cvpm, gc, polyps[0].x, polyps[0].y, x, y);
			break;
			case 2:
			XSetForeground(d, gc, fgc);
			XFillPolygon(d, cv, gc, polyps, polypslen, Complex, CoordModeOrigin);
			XFillPolygon(d, cvpm, gc, polyps, polypslen, Complex, CoordModeOrigin);
			break;
		}
		polypslen=0;
		return;
	}
	XCopyArea(d, cvpm, cv, gc, rlplinex, rlpliney, mod(x-plinex), mod(y-pliney), rlplinex, rlpliney);
	XDrawLine(d, cv, gc, plinex, pliney, x, y);
	XDrawLine(d, cvpm, gc, plinex, pliney, x, y);
}

// Fill functions
void filler(int x, int y, XImage *xim, unsigned long ogfpc){
	unsigned long fpcolor;
	if(x<0) x=0;
	else if(x>=cvw) x=cvw-1;
	if(y<0) y=0;
	else if(y>=cvh) y=cvh-1;
	fpcolor=XGetPixel(xim, x, y);
	if(fpcolor==ogfpc && fpcolor!=fgc){
		XDrawPoint(d, cvpm, gc, x, y);
		XPutPixel(xim, x, y, fgc);
		filler(x + 1, y, xim, ogfpc);
		filler(x, y + 1, xim, ogfpc);
		filler(x - 1, y, xim, ogfpc);
		filler(x, y - 1, xim, ogfpc);
	}
}

void fill(int x, int y){
	XImage *fillpixel;
	fillpixel=XGetImage(d, cvpm, 0, 0, cvw, cvh, AllPlanes, ZPixmap);
	XSetForeground(d, gc, fgc);
	filler(x, y, fillpixel, XGetPixel(fillpixel, x, y));
	XCopyArea(d, cvpm, cv, gc, 0, 0, cvw, cvh, 0, 0);
	XDestroyImage(fillpixel);
}

// Ellipse functions
int elpsx, elpsy, prevex, prevey;
void elps(int x, int y){
	prevex=elpsx=x; prevey=elpsy=y;
}

void melps(int x, int y){
	int rlelpsx=(elpsx<x)*elpsx+(elpsx>x)*x, rlelpsy=(elpsy<y)*elpsy+(elpsy>y)*y;
	int rlelpsprevx=(elpsx<=prevex)*elpsx+(elpsx>prevex)*prevex, rlelpsprevy=(elpsy<=prevey)*elpsy+(elpsy>prevey)*prevey;
	int clrx=(elpsx>x)*prevex+(elpsx<x)*x+(x==elpsx)*((prevex<=elpsx)*prevex+(prevex>elpsx)*elpsx);
	int clry=(elpsy>y)*prevey+(elpsy<y)*y+(y==elpsy)*((prevey<=elpsy)*prevey+(prevey>elpsy)*elpsy);
	XSetForeground(d, gc, fgc);
	//if(
	//	((prevex-elpsx)>0)!=((x-elpsx)>0) ||
	//	((prevey-elpsy)>0)!=((y-elpsy)>0)
	//)
	XCopyArea(d, cvpm, cv, gc, rlelpsprevx, rlelpsprevy, mod(prevex-elpsx)+1, mod(prevey-elpsy)+1, rlelpsprevx, rlelpsprevy);
	//if(mod(prevex-elpsx)>mod(elpsx-x)) XCopyArea(d, cvpm, cv, gc, clrx, rlelpsy, mod(prevex-x), mod(y-elpsy), clrx, rlelpsy);
	//if(mod(prevey-elpsy)>mod(elpsy-y)) XCopyArea(d, cvpm, cv, gc, rlelpsprevx, clry, mod(prevex-elpsx), mod(prevey-y), rlelpsprevx, clry);
	switch(filltype){
		case 0:
			XDrawArc(d, cv, gc, rlelpsx, rlelpsy, mod(x-elpsx), mod(y-elpsy), 0, 23040);
		break;
		case 2:
			XFillArc(d, cv, gc, rlelpsx, rlelpsy, mod(x-elpsx), mod(y-elpsy), 0, 23040);
		break;
		case 1:
			XSetForeground(d, gc, bgc);
			XFillArc(d, cv, gc, rlelpsx, rlelpsy, mod(x-elpsx), mod(y-elpsy), 0, 23040);
			XSetForeground(d, gc, fgc);
			XDrawArc(d, cv, gc, rlelpsx, rlelpsy, mod(x-elpsx), mod(y-elpsy), 0, 23040);
		break;
	}
	prevex=x; prevey=y;
}

void relps(int x, int y){
	XSetForeground(d, gc, fgc);
	XCopyArea(d, cvpm, cv, gc, 0, 0, cvw, cvh, 0, 0);
	switch(filltype){
		case 0:
			XDrawArc(d, cv, gc, (elpsx<x)*elpsx+(elpsx>x)*x, (elpsy<y)*elpsy+(elpsy>y)*y, mod(x-elpsx), mod(y-elpsy), 0, 23040);
			XDrawArc(d, cvpm, gc, (elpsx<x)*elpsx+(elpsx>x)*x, (elpsy<y)*elpsy+(elpsy>y)*y, mod(x-elpsx), mod(y-elpsy), 0, 23040);
		break;
		case 2:
			XFillArc(d, cv, gc, (elpsx<x)*elpsx+(elpsx>x)*x, (elpsy<y)*elpsy+(elpsy>y)*y, mod(x-elpsx), mod(y-elpsy), 0, 23040);
			XFillArc(d, cvpm, gc, (elpsx<x)*elpsx+(elpsx>x)*x, (elpsy<y)*elpsy+(elpsy>y)*y, mod(x-elpsx), mod(y-elpsy), 0, 23040);
		break;
		case 1:
			XSetForeground(d, gc, bgc);
			XFillArc(d, cv, gc, (elpsx<x)*elpsx+(elpsx>x)*x, (elpsy<y)*elpsy+(elpsy>y)*y, mod(x-elpsx), mod(y-elpsy), 0, 23040);
			XFillArc(d, cvpm, gc, (elpsx<x)*elpsx+(elpsx>x)*x, (elpsy<y)*elpsy+(elpsy>y)*y, mod(x-elpsx), mod(y-elpsy), 0, 23040);
			XSetForeground(d, gc, fgc);
			XDrawArc(d, cv, gc, (elpsx<x)*elpsx+(elpsx>x)*x, (elpsy<y)*elpsy+(elpsy>y)*y, mod(x-elpsx), mod(y-elpsy), 0, 23040);
			XDrawArc(d, cvpm, gc, (elpsx<x)*elpsx+(elpsx>x)*x, (elpsy<y)*elpsy+(elpsy>y)*y, mod(x-elpsx), mod(y-elpsy), 0, 23040);
		break;
	}
}

void (*pdrawfs[WK])(int x, int y)={pen,pen,ersr,fill,pen,pen,pen,brush,pen,pen,line,curve,rect,pline,elps,pen};
void (*mdrawfs[WK])(int x, int y)={pen,pen,mersr,nop,pen,pen,mpen,mbrush,pen,pen,mline,mcurve,mrect,mpline,melps,pen};
void (*rdrawfs[WK])(int x, int y)={nop,nop,nop,nop,nop,nop,nop,nop,nop,nop,rline,rcurve,rrect,rpline,relps,nop};

void xclose(){
XUnmapWindow(d, win);
XDestroyWindow(d, win);
XCloseDisplay(d);
}
