void xevl(){
Atom wm_delete_window;
Window ntrdwin, ulwin;
char c;
short f;
int useless;
char hertz[12];
int slv=14;
char pk=0;

xinit();

wm_delete_window = XInternAtom(d, "WM_DELETE_WINDOW", False);
XSetWMProtocols(d, win, &wm_delete_window, 1);

while(!done){
XNextEvent(d, &xev);
switch(xev.type){
case Expose:
	//if(xev.xexpose.window==text)
	//XDrawImageString(d, text, gc2, 5, 15, hertz, strlen(hertz));

	for(int i=0;i<BC;i++)
	if(xev.xexpose.window==btns[i]) XDrawImageString(d, btns[i], gc, 5, 15, bl[i], strlen(bl[i]));
	break;
case ButtonPress:
	if(xev.xbutton.button==Button1){
		if(xev.xbutton.window==btns[0]){
		}else if(xev.xbutton.window==btns[1]){
		}
	}
	break;
case ButtonRelease:
	break;
case ClientMessage:
	if (xev.xclient.data.l[0] == wm_delete_window) {
		done=1;
	}
	break;
case KeyPress:
	XLookupString(&xev.xkey, &c, 1, NULL, NULL);
	//printf("character: %c\n", c);
	f=0;
	switch(c){
	case 'q': break;
	case 'w': break;
	default:
	}
	break;
case KeyRelease:
	break;
case MotionNotify:
	break;
}
}
xclose();
}
