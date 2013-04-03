/*
    ALIENS: A silly little game demonstrating the SDL and mixer libraries
    Copyright (C) 1998  Sam Lantinga

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@devolution.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"

#include <signal.h>
#include <rfb/rfbclient.h>

#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "aliens"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


struct { int sdl; int rfb; } buttonMapping[]={
        {1, rfbButton1Mask},
        {2, rfbButton2Mask},
        {3, rfbButton3Mask},
        {4, rfbButton4Mask},
        {5, rfbButton5Mask},
        {0,0}
};

static int enableResizable = 1, viewOnly, listenLoop, buttonMask;
#ifdef SDL_ASYNCBLIT
        int sdlFlags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL;
#else
 //       int sdlFlags = SDL_HWSURFACE | SDL_HWACCEL;
#endif
static int realWidth, realHeight, bytesPerPixel, rowStride;
static char *sdlPixels;

static int rightAltKeyDown, leftAltKeyDown;

SDL_Window *window;

SDL_Renderer *renderer;

SDL_Surface* sdl;

static rfbBool resize(rfbClient* client) {
	LOGE("RESIZE");
	 int width=client->width,height=client->height,
	                depth=client->format.bitsPerPixel;

	        /*if (enableResizable)
	                sdlFlags |= SDL_RESIZABLE;*/

	        client->updateRect.x = client->updateRect.y = 0;
	        client->updateRect.w = width; client->updateRect.h = height;
	        //rfbBool okay=SDL_VideoModeOK(width,height,depth,sdlFlags);

	        /*if(!okay)
	                for(depth=24;!okay && depth>4;depth/=2)
	                        okay=SDL_VideoModeOK(width,height,depth,sdlFlags);*/
	        //SDL_Init(SDL_INIT_VIDEO);

	        window = SDL_CreateWindow("MIO",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,client->width,client->height,0);

	       //SDL_Renderer *renderer = SDL_CreateRenderer(window,1,0);


	       sdl = SDL_GetWindowSurface(window);
	       LOGE("No se pude iniciar %s\n", SDL_GetError());
	        /*depth= client->format.bitsPerPixel;
			sdl->format->Rshift= client->format.redShift;
			sdl->format->Gshift= client->format.greenShift;
			sdl->format->Bshift= client->format.blueShift;
			sdl->format->Rmask= client->format.redMax;
			sdl->format->Gmask= client->format.greenMax;
			sdl->format->Bmask = client->format.blueMax;*/

	        if(sdl == NULL) LOGE("ES NULL");


	       /* SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	       			   SDL_RenderClear(renderer);


	       			   SDL_RenderPresent(renderer);*/
			//SDL_Surface* sdl=SDL_SetVideoMode(width,height,depth,sdlFlags);

			//rfbClientSetClientData(client,(void*)SDL_Init, sdl);
			//client->width = sdl->pitch / (depth / 8);
			/*if (sdlPixels) {
					free(client->frameBuffer);
					sdlPixels = NULL;
			}*/
	        bytesPerPixel = client->format.bitsPerPixel / 8;
	        		 int size = client->width * bytesPerPixel * client->height;


	        		 //sdlPixels = (char*)client->frameBuffer;
	        		 //sdlPixels = (char*)malloc(size);

	        		 client->frameBuffer = (uint8_t*)malloc(size);
			 /*LOGE("SE VA RESERVA EL BUFFER");
			client->frameBuffer=(uint8_t*)sdl->pixels;
			 LOGE("SE A RESERVADO EL BUFFER");*/

			/*client->format.bitsPerPixel=depth;
			client->format.redShift=sdl->format->Rshift;
			client->format.greenShift=sdl->format->Gshift;
			client->format.blueShift=sdl->format->Bshift;
			client->format.redMax=sdl->format->Rmask>>client->format.redShift;
			client->format.greenMax=sdl->format->Gmask>>client->format.greenShift;
			client->format.blueMax=sdl->format->Bmask>>client->format.blueShift;*/
			SetFormatAndEncodings(client);




	        return TRUE;
}

static rfbKeySym SDL_key2rfbKeySym(SDL_KeyboardEvent* e) {
        /*rfbKeySym k = 0;
        SDLKey sym = e->keysym.sym;

        switch (sym) {
        case SDLK_BACKSPACE: k = XK_BackSpace; break;
        case SDLK_TAB: k = XK_Tab; break;
        case SDLK_CLEAR: k = XK_Clear; break;
        case SDLK_RETURN: k = XK_Return; break;
        case SDLK_PAUSE: k = XK_Pause; break;
        case SDLK_ESCAPE: k = XK_Escape; break;
        case SDLK_SPACE: k = XK_space; break;
        case SDLK_DELETE: k = XK_Delete; break;
        case SDLK_KP0: k = XK_KP_0; break;
        case SDLK_KP1: k = XK_KP_1; break;
        case SDLK_KP2: k = XK_KP_2; break;
        case SDLK_KP3: k = XK_KP_3; break;
        case SDLK_KP4: k = XK_KP_4; break;
        case SDLK_KP5: k = XK_KP_5; break;
        case SDLK_KP6: k = XK_KP_6; break;
        case SDLK_KP7: k = XK_KP_7; break;
        case SDLK_KP8: k = XK_KP_8; break;
        case SDLK_KP9: k = XK_KP_9; break;
        case SDLK_KP_PERIOD: k = XK_KP_Decimal; break;
        case SDLK_KP_DIVIDE: k = XK_KP_Divide; break;
        case SDLK_KP_MULTIPLY: k = XK_KP_Multiply; break;
        case SDLK_KP_MINUS: k = XK_KP_Subtract; break;
        case SDLK_KP_PLUS: k = XK_KP_Add; break;
        case SDLK_KP_ENTER: k = XK_KP_Enter; break;
        case SDLK_KP_EQUALS: k = XK_KP_Equal; break;
        case SDLK_UP: k = XK_Up; break;
        case SDLK_DOWN: k = XK_Down; break;
        case SDLK_RIGHT: k = XK_Right; break;
        case SDLK_LEFT: k = XK_Left; break;
        case SDLK_INSERT: k = XK_Insert; break;
        case SDLK_HOME: k = XK_Home; break;
        case SDLK_END: k = XK_End; break;
        case SDLK_PAGEUP: k = XK_Page_Up; break;
        case SDLK_PAGEDOWN: k = XK_Page_Down; break;
        case SDLK_F1: k = XK_F1; break;
        case SDLK_F2: k = XK_F2; break;
        case SDLK_F3: k = XK_F3; break;
        case SDLK_F4: k = XK_F4; break;
        case SDLK_F5: k = XK_F5; break;
        case SDLK_F6: k = XK_F6; break;
        case SDLK_F7: k = XK_F7; break;
        case SDLK_F8: k = XK_F8; break;
        case SDLK_F9: k = XK_F9; break;
        case SDLK_F10: k = XK_F10; break;
        case SDLK_F11: k = XK_F11; break;
        case SDLK_F12: k = XK_F12; break;
        case SDLK_F13: k = XK_F13; break;
        case SDLK_F14: k = XK_F14; break;
        case SDLK_F15: k = XK_F15; break;
        case SDLK_NUMLOCK: k = XK_Num_Lock; break;
        case SDLK_CAPSLOCK: k = XK_Caps_Lock; break;
        case SDLK_SCROLLOCK: k = XK_Scroll_Lock; break;
        case SDLK_RSHIFT: k = XK_Shift_R; break;
        case SDLK_LSHIFT: k = XK_Shift_L; break;
        case SDLK_RCTRL: k = XK_Control_R; break;
        case SDLK_LCTRL: k = XK_Control_L; break;
        case SDLK_RALT: k = XK_Alt_R; break;
        case SDLK_LALT: k = XK_Alt_L; break;

#if 0
        case SDLK_COMPOSE: k = XK_Compose; break;
#endif
        case SDLK_MODE: k = XK_Mode_switch; break;
        case SDLK_HELP: k = XK_Help; break;
        case SDLK_PRINT: k = XK_Print; break;
        case SDLK_SYSREQ: k = XK_Sys_Req; break;
        case SDLK_BREAK: k = XK_Break; break;
        default: break;
        }*/
        /* both SDL and X11 keysyms match ASCII in the range 0x01-0x7f */
        /*if (k == 0 && sym > 0x0 && sym < 0x100) {
                k = sym;
                if (e->keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) {
                        if (k >= '1' && k <= '9')
                                k &= ~0x10;
                        else if (k >= 'a' && k <= 'f')
                                k &= ~0x20;
                }
        }
        if (k == 0) {
                if (e->keysym.unicode < 0x100)
                        k = e->keysym.unicode;
                else
                        rfbClientLog("Unknown keysym: %d\n", sym);
        }

        return k;*/
}

static uint32_t get(rfbClient *cl, int x, int y)
{
        switch (bytesPerPixel) {
        case 1: return ((uint8_t *)cl->frameBuffer)[x + y * cl->width];
        case 2: return ((uint16_t *)cl->frameBuffer)[x + y * cl->width];
        case 4: return ((uint32_t *)cl->frameBuffer)[x + y * cl->width];
        default:
                rfbClientErr("Unknown bytes/pixel: %d", bytesPerPixel);
                exit(1);
        }
}

static void put(int x, int y, uint32_t v)
{
        switch (bytesPerPixel) {
        case 1: ((uint8_t *)sdlPixels)[x + y * rowStride] = v; break;
        case 2: ((uint16_t *)sdlPixels)[x + y * rowStride] = v; break;
        case 4: ((uint32_t *)sdlPixels)[x + y * rowStride] = v; break;
        default:
                rfbClientErr("Unknown bytes/pixel: %d", bytesPerPixel);
                exit(1);
        }
}

static void resizeRectangleToReal(rfbClient *cl, int x, int y, int w, int h)
{
        int i0 = x * realWidth / cl->width;
        int i1 = ((x + w) * realWidth - 1) / cl->width + 1;
        int j0 = y * realHeight / cl->height;
        int j1 = ((y + h) * realHeight - 1) / cl->height + 1;
        int i, j;

        for (j = j0; j < j1; j++)
                for (i = i0; i < i1; i++) {
                        int x0 = i * cl->width / realWidth;
                        int x1 = ((i + 1) * cl->width - 1) / realWidth + 1;
                        int y0 = j * cl->height / realHeight;
                        int y1 = ((j + 1) * cl->height - 1) / realHeight + 1;
                        uint32_t r = 0, g = 0, b = 0;

                        for (y = y0; y < y1; y++)
                                for (x = x0; x < x1; x++) {
                                        uint32_t v = get(cl, x, y);
#define REDSHIFT cl->format.redShift
#define REDMAX cl->format.redMax
#define GREENSHIFT cl->format.greenShift
#define GREENMAX cl->format.greenMax
#define BLUESHIFT cl->format.blueShift
#define BLUEMAX cl->format.blueMax
                                        r += (v >> REDSHIFT) & REDMAX;
                                        g += (v >> GREENSHIFT) & GREENMAX;
                                        b += (v >> BLUESHIFT) & BLUEMAX;
                                }
                        r /= (x1 - x0) * (y1 - y0);
                        g /= (x1 - x0) * (y1 - y0);
                        b /= (x1 - x0) * (y1 - y0);

                        put(i, j, (r << REDSHIFT) | (g << GREENSHIFT) |
                                (b << BLUESHIFT));
                }
}

extern "C"{ static void update(rfbClient* cl,int x,int y,int w,int h) {
	LOGE("UPDATE");
	if (sdlPixels) {
			resizeRectangleToReal(cl, x, y, w, h);
			w = ((x + w) * realWidth - 1) / cl->width + 1;
			h = ((y + h) * realHeight - 1) / cl->height + 1;
			x = x * realWidth / cl->width;
			y = y * realHeight / cl->height;
			w -= x;
			h -= y;
	}
	int size_small = 480 * bytesPerPixel * 800;

	//aux_sdl->pixels = malloc(size_small);
	//char a[size_small];
	uint8_t *cpy = (uint8_t*)malloc(size_small);
	int size = 0;
	int ini = 0;
	int pos=0;
	int small_pos = 0;

	for (int i = 0 ;i<=480*1.6;i++){
		size = cl->width*bytesPerPixel;

		memcpy(&cpy[small_pos*bytesPerPixel],&cl->frameBuffer[pos*bytesPerPixel],size);
		pos +=cl->width;
		small_pos += 480;
		//ini += bytesPerPixel;
	}


	//size = 480 * bytesPerPixel;
	//memcpy(&cpy[800],&cl->frameBuffer[size],size);
	//memcpy(a,cpy,size_small);
	memcpy(sdl->pixels,cpy,size_small);
	 /*SDL_Rect rect;
	        rect.x = x;
	        rect.y = y;
	        rect.h = h;
	        rect.w = w;*/
	        //SDL_RenderPresent(renderer);
	SDL_UpdateWindowSurface(window);
	//SDL_UpdateRect((SDL_Surface*)rfbClientGetClientData(cl, (void*)SDL_Init), (Sint32)x, (Sint32)y, (Uint32)w,(Uint32) h);

}
}
static void setRealDimension(rfbClient *client, int w, int h)
{
      /*  SDL_Surface* sdl;

        if (w < 0) {
                const SDL_VideoInfo *info = SDL_GetVideoInfo();
                w = info->current_h;
                h = info->current_w;
        }

        if (w == realWidth && h == realHeight)
                return;

        if (!sdlPixels) {
                int size;

                sdlPixels = (char *)client->frameBuffer;
                rowStride = client->width;

                bytesPerPixel = client->format.bitsPerPixel / 8;
                size = client->width * bytesPerPixel * client->height;
                client->frameBuffer = (uint8_t*)malloc(size);
                if (!client->frameBuffer) {
                        rfbClientErr("Could not allocate %d bytes", size);
                        exit(1);
                }
                memcpy(client->frameBuffer, sdlPixels, size);
        }

        sdl = (SDL_Surface*)rfbClientGetClientData(client, (void*)SDL_Init);
        if (sdl->w != w || sdl->h != h) {
                int depth = sdl->format->BitsPerPixel;
                sdl = SDL_SetVideoMode(w, h, depth, sdlFlags);
                rfbClientSetClientData(client,(void*) SDL_Init, sdl);
                sdlPixels = (char*)sdl->pixels;
                rowStride = sdl->pitch / (depth / 8);
        }

        realWidth = w;
        realHeight = h;
        update(client, 0, 0, client->width, client->height);*/
}

static void kbd_leds(rfbClient* cl, int value, int pad) {
        /* note: pad is for future expansion 0=unused */
        fprintf(stderr,"Led State= 0x%02X\n", value);
        fflush(stderr);
}

/* trivial support for textchat */
static void text_chat(rfbClient* cl, int value, char *text) {
        switch(value) {
        case rfbTextChatOpen:
                fprintf(stderr,"TextChat: We should open a textchat window!\n");
                TextChatOpen(cl);
                break;
        case rfbTextChatClose:
                fprintf(stderr,"TextChat: We should close our window!\n");
                break;
        case rfbTextChatFinished:
                fprintf(stderr,"TextChat: We should close our window!\n");
                break;
        default:
                fprintf(stderr,"TextChat: Received \"%s\"\n", text);
                break;
        }
        fflush(stderr);
}


static void cleanup(rfbClient* cl)
{
  /*
    just in case we're running in listenLoop:
    close viewer window by restarting SDL video subsystem
  */
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_InitSubSystem(SDL_INIT_VIDEO);
  if(cl)
    rfbClientCleanup(cl);
}


static rfbBool handleSDLEvent(rfbClient *cl, SDL_Event *e)
{
        switch(e->type) {

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEMOTION:
        {
                int x, y, state, i;
                if (viewOnly)
                        break;

                if (e->type == SDL_MOUSEMOTION) {
                        x = e->motion.x;
                        y = e->motion.y;
                        state = e->motion.state;
                }
                else {
                        x = e->button.x;
                        y = e->button.y;
                        state = e->button.button;
                        for (i = 0; buttonMapping[i].sdl; i++)
                                if (state == buttonMapping[i].sdl) {
                                        state = buttonMapping[i].rfb;
                                        if (e->type == SDL_MOUSEBUTTONDOWN)
                                                buttonMask |= state;
                                        else
                                                buttonMask &= ~state;
                                        break;
                                }
                }
                if (sdlPixels) {
                        x = x * cl->width / realWidth;
                        y = y * cl->height / realHeight;
                }
                SendPointerEvent(cl, x, y, buttonMask);
                buttonMask &= ~(rfbButton4Mask | rfbButton5Mask);
                break;
        }
        /*case SDL_KEYUP:
        case SDL_KEYDOWN:
                if (viewOnly)
                        break;
                SendKeyEvent(cl, SDL_key2rfbKeySym(&e->key),
                        e->type == SDL_KEYDOWN ? TRUE : FALSE);
                if (e->key.keysym.sym == SDLK_RALT)
                        rightAltKeyDown = e->type == SDL_KEYDOWN;
                if (e->key.keysym.sym == SDLK_LALT)
                        leftAltKeyDown = e->type == SDL_KEYDOWN;
                break;
        case SDL_QUIT:
                if(listenLoop)
                  {
                    cleanup(cl);
                    return FALSE;
                  }
                else
                  {
                    rfbClientCleanup(cl);
                    exit(0);
                  }
        case SDL_ACTIVEEVENT:
                if (!e->active.gain && rightAltKeyDown) {
                        SendKeyEvent(cl, XK_Alt_R, FALSE);
                        rightAltKeyDown = FALSE;
                        rfbClientLog("released right Alt key\n");
                }
                if (!e->active.gain && leftAltKeyDown) {
                        SendKeyEvent(cl, XK_Alt_L, FALSE);
                        leftAltKeyDown = FALSE;
                        rfbClientLog("released left Alt key\n");
                }


                break;
*/
        }
        return TRUE;
}

static void got_selection(rfbClient *cl, const char *text, int len)
{

}



extern "C" int main(int argc, char *argv[]){
	LOGE("HOLA");
	rfbClient* cl;
	int i, j;
	SDL_Event e;

	SDL_Init(SDL_INIT_EVERYTHING);
	/*SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
					SDL_DEFAULT_REPEAT_INTERVAL);
	atexit(SDL_Quit);
	signal(SIGINT, exit);*/

	do {
		/* 16-bit: cl=rfbGetClient(5,3,2); */
		cl=rfbGetClient(8,3,4);

		cl->serverHost = "192.168.1.3";
		cl->serverPort = 5900;

		cl->MallocFrameBuffer=resize;
		cl->canHandleNewFBSize = TRUE;
		cl->GotFrameBufferUpdate=update;
		cl->HandleKeyboardLedState=kbd_leds;
		cl->HandleTextChat=text_chat;
		cl->GotXCutText = got_selection;
		cl->listenPort = LISTEN_PORT_OFFSET;
		cl->listen6Port = LISTEN_PORT_OFFSET;
		if(!rfbInitClient(cl,&argc,argv))
		{
		  cl = NULL; /* rfbInitClient has already freed the client struct */
		  cleanup(cl);
		  break;
		}
		LOGE("ALL WRONG %s\n", SDL_GetError());
		//resize(cl);
		  //LOGE("No se pude iniciar %s\n", SDL_GetError());
		//ShowBMP("data/Simple_tux.bmp",(SDL_Surface*)rfbClientGetClientData(cl, (void*)SDL_Init),0, 0);
		  //update(cl,0,0,640,480);

		  //LOGE("No se pude actualizar %s\n", SDL_GetError());


		/*SDL_Surface* sdl=SDL_SetVideoMode(400,400,32,sdlFlags);
		sdl->pixels = cl->frameBuffer;
		update(cl,0,0,20,20);
		update(cl,20,20,40,40);
		update(cl,40,40,80,80);
		update(cl,80,80,100,100);*/



		while(1) {
		/*if(SDL_PollEvent(&e)) {
		  /*
			handleSDLEvent() return 0 if user requested window close.
			In this case, handleSDLEvent() will have called cleanup().
		  */
		 /* if(!handleSDLEvent(cl, &e))
			break;
		}
		else {*/
		  i=WaitForMessage(cl,500);

		  //update(cl,0,0,0,0);

		  if(i<0)
			{
			  LOGE("IS OUT 1");
			  cleanup(cl);
			  break;
			}
		  if(i)
			if(!HandleRFBServerMessage(cl))
			  {
				LOGE("IS OUT 2");
				cleanup(cl);
				break;
			  }

		}
		//}
	}
	while(listenLoop);
	LOGE("ADIOS");
	return 0;
}
