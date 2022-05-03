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
	if(xev.xexpose.window==cv){
		XCopyArea(d, cvpm, cv, gc, 0, 0, cvw, cvh, 0, 0);
	}else if(xev.xexpose.window==scolor){
		redraw_scolor();
	}else{
		if(redraw_toolbtns(xev.xexpose.window));
		else if(redraw_menubtns(xev.xexpose.window));
	}

	expose_end:
	break;
case ButtonPress:
	if(xev.xbutton.button==Button1){
		if(xev.xbutton.window==cv){
			(*pdrawfs[tool])(xev.xbutton.x, xev.xbutton.y);
		}else if(xev.xbutton.window==scolor){
			fgc^=bgc;
			bgc^=fgc;
			fgc^=bgc;
			redraw_scolor();
		}else{
			for(int i=0;i<WK;i++)
			if(xev.xbutton.window==white[i]){
				prevtool=tool;
				tool=i;
				redraw_toolbtn(white[prevtool], prevtool);
				redraw_toolbtn(white[tool], tool);
				goto bpress_end;
			}

			for(int i=0;i<BC;i++)
			if(xev.xbutton.window==btns[i]){
				goto bpress_end;
			}

			for(int i=0;i<CBC;i++)
			if(xev.xbutton.window==cbtns[i]){
				fgc=palette[i];
				redraw_scolor();
			}
		}
	}else if(xev.xbutton.button==Button3){
		if(0){
		}else{
			for(int i=0;i<CBC;i++)
			if(xev.xbutton.window==cbtns[i]){
				bgc=palette[i];
				redraw_scolor();
			}
		}
	}
	bpress_end:
	break;
case ButtonRelease:
	if(xev.xbutton.button==Button1){
		if(xev.xbutton.window==cv){
			(*rdrawfs[tool])(xev.xbutton.x, xev.xbutton.y);
		}
	}
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
	if(xev.xmotion.window==cv){
		(*mdrawfs[tool])(xev.xbutton.x, xev.xbutton.y);
	}
	break;
case ConfigureNotify:
	XResizeWindow(d, topbar, xev.xconfigure.width, BARH);
	XResizeWindow(d, toolbox, 2*WKW, xev.xconfigure.height-2*CBB-BARH);
	XMoveResizeWindow(d, colorbox, 0, xev.xconfigure.height-2*CBB, xev.xconfigure.width, 2*CBB);
	XResizeWindow(d, vp, xev.xconfigure.width-2*WKW, xev.xconfigure.height-BARH-2*CBB);
	xev.xresizerequest.height;
	break;
}
}
xclose();
}
