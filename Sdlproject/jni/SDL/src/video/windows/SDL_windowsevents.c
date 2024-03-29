/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_config.h"

#if SDL_VIDEO_DRIVER_WINDOWS

#include "SDL_windowsvideo.h"
#include "SDL_windowsshape.h"
#include "SDL_syswm.h"
#include "SDL_vkeys.h"
#include "../../events/SDL_events_c.h"
#include "../../events/SDL_touch_c.h"
#include "../../events/scancodes_windows.h"

/* Dropfile support */
#include <shellapi.h>


/*#define WMMSG_DEBUG*/
#ifdef WMMSG_DEBUG
#include <stdio.h>	
#include "wmmsg.h"
#endif

/* Masks for processing the windows KEYDOWN and KEYUP messages */
#define REPEATED_KEYMASK    (1<<30)
#define EXTENDED_KEYMASK    (1<<24)

#define VK_ENTER    10          /* Keypad Enter ... no VKEY defined? */
#ifndef VK_OEM_NEC_EQUAL
#define VK_OEM_NEC_EQUAL 0x92 
#endif

/* Make sure XBUTTON stuff is defined that isn't in older Platform SDKs... */
#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C
#endif
#ifndef GET_XBUTTON_WPARAM
#define GET_XBUTTON_WPARAM(w) (HIWORD(w))
#endif
#ifndef WM_INPUT
#define WM_INPUT 0x00ff
#endif
#ifndef WM_TOUCH
#define WM_TOUCH 0x0240
#endif

static SDL_Scancode 
WindowsScanCodeToSDLScanCode( int lParam, int wParam )
{
	SDL_Scancode code;
	char bIsExtended;
	int nScanCode = ( lParam >> 16 ) & 0xFF;

	if ( nScanCode == 0 )
	{
		switch( wParam )
		{
		case VK_CLEAR: return SDL_SCANCODE_CLEAR;
		case VK_MODECHANGE: return SDL_SCANCODE_MODE;
		case VK_SELECT: return SDL_SCANCODE_SELECT;
		case VK_EXECUTE: return SDL_SCANCODE_EXECUTE;
		case VK_HELP: return SDL_SCANCODE_HELP;

		case VK_F13: return SDL_SCANCODE_F13;
		case VK_F14: return SDL_SCANCODE_F14;
		case VK_F15: return SDL_SCANCODE_F15;
		case VK_F16: return SDL_SCANCODE_F16;
		case VK_F17: return SDL_SCANCODE_F17;
		case VK_F18: return SDL_SCANCODE_F18;
		case VK_F19: return SDL_SCANCODE_F19;
		case VK_F20: return SDL_SCANCODE_F20;
		case VK_F21: return SDL_SCANCODE_F21;
		case VK_F22: return SDL_SCANCODE_F22;
		case VK_F23: return SDL_SCANCODE_F23;
		case VK_F24: return SDL_SCANCODE_F24;

		case VK_OEM_NEC_EQUAL: return SDL_SCANCODE_KP_EQUALS;
		case VK_BROWSER_BACK: return SDL_SCANCODE_AC_BACK;
		case VK_BROWSER_FORWARD: return SDL_SCANCODE_AC_FORWARD;
		case VK_BROWSER_REFRESH: return SDL_SCANCODE_AC_REFRESH;
		case VK_BROWSER_STOP: return SDL_SCANCODE_AC_STOP;
		case VK_BROWSER_SEARCH: return SDL_SCANCODE_AC_SEARCH;
		case VK_BROWSER_FAVORITES: return SDL_SCANCODE_AC_BOOKMARKS;
		case VK_BROWSER_HOME: return SDL_SCANCODE_AC_HOME;
		case VK_VOLUME_MUTE: return SDL_SCANCODE_AUDIOMUTE;
		case VK_VOLUME_DOWN: return SDL_SCANCODE_VOLUMEDOWN;
		case VK_VOLUME_UP: return SDL_SCANCODE_VOLUMEUP;
	
		case VK_MEDIA_NEXT_TRACK: return SDL_SCANCODE_AUDIONEXT;
		case VK_MEDIA_PREV_TRACK: return SDL_SCANCODE_AUDIOPREV;
		case VK_MEDIA_STOP: return SDL_SCANCODE_AUDIOSTOP;
		case VK_MEDIA_PLAY_PAUSE: return SDL_SCANCODE_AUDIOPLAY;
		case VK_LAUNCH_MAIL: return SDL_SCANCODE_MAIL;
		case VK_LAUNCH_MEDIA_SELECT: return SDL_SCANCODE_MEDIASELECT;
		
		case VK_OEM_102: return SDL_SCANCODE_NONUSBACKSLASH;

		case VK_ATTN: return SDL_SCANCODE_SYSREQ;
		case VK_CRSEL: return SDL_SCANCODE_CRSEL;
		case VK_EXSEL: return SDL_SCANCODE_EXSEL;
		case VK_OEM_CLEAR: return SDL_SCANCODE_CLEAR;

		case VK_LAUNCH_APP1: return SDL_SCANCODE_APP1;
		case VK_LAUNCH_APP2: return SDL_SCANCODE_APP2;

		default: return SDL_SCANCODE_UNKNOWN;
		}
	}

	if ( nScanCode > 127 )
		return SDL_SCANCODE_UNKNOWN;

	code = windows_scancode_table[nScanCode];

	bIsExtended = ( lParam & ( 1 << 24 ) ) != 0;
	if ( !bIsExtended )
	{
		switch ( code )
		{
		case SDL_SCANCODE_HOME:
			return SDL_SCANCODE_KP_7;
		case SDL_SCANCODE_UP:
			return SDL_SCANCODE_KP_8;
		case SDL_SCANCODE_PAGEUP:
			return SDL_SCANCODE_KP_9;
		case SDL_SCANCODE_LEFT:
			return SDL_SCANCODE_KP_4;
		case SDL_SCANCODE_RIGHT:
			return SDL_SCANCODE_KP_6;
		case SDL_SCANCODE_END:
			return SDL_SCANCODE_KP_1;
		case SDL_SCANCODE_DOWN:
			return SDL_SCANCODE_KP_2;
		case SDL_SCANCODE_PAGEDOWN:
			return SDL_SCANCODE_KP_3;
		case SDL_SCANCODE_INSERT:
			return SDL_SCANCODE_KP_0;
		case SDL_SCANCODE_DELETE:
			return SDL_SCANCODE_KP_PERIOD;
		case SDL_SCANCODE_PRINTSCREEN:
			return SDL_SCANCODE_KP_MULTIPLY;
		default:
			break;
		}
	}
	else
	{
		switch ( code )
		{
		case SDL_SCANCODE_RETURN:
			return SDL_SCANCODE_KP_ENTER;
		case SDL_SCANCODE_LALT:
			return SDL_SCANCODE_RALT;
		case SDL_SCANCODE_LCTRL:
			return SDL_SCANCODE_RCTRL;
		case SDL_SCANCODE_SLASH:
			return SDL_SCANCODE_KP_DIVIDE;
		case SDL_SCANCODE_CAPSLOCK:
			return SDL_SCANCODE_KP_PLUS;
		}
	}

	return code;
}


LRESULT CALLBACK
WIN_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SDL_WindowData *data;
    LRESULT returnCode = -1;

    /* Send a SDL_SYSWMEVENT if the application wants them */
    if (SDL_GetEventState(SDL_SYSWMEVENT) == SDL_ENABLE) {
        SDL_SysWMmsg wmmsg;

        SDL_VERSION(&wmmsg.version);
        wmmsg.subsystem = SDL_SYSWM_WINDOWS;
        wmmsg.msg.win.hwnd = hwnd;
        wmmsg.msg.win.msg = msg;
        wmmsg.msg.win.wParam = wParam;
        wmmsg.msg.win.lParam = lParam;
        SDL_SendSysWMEvent(&wmmsg);
    }

    /* Get the window data for the window */
    data = (SDL_WindowData *) GetProp(hwnd, TEXT("SDL_WindowData"));
    if (!data) {
        return CallWindowProc(DefWindowProc, hwnd, msg, wParam, lParam);
    }

#ifdef WMMSG_DEBUG
    {		
        FILE *log = fopen("wmmsg.txt", "a");		
        fprintf(log, "Received windows message: %p ", hwnd);
        if (msg > MAX_WMMSG) {
            fprintf(log, "%d", msg);
        } else {
            fprintf(log, "%s", wmtab[msg]);
        }
        fprintf(log, " -- 0x%X, 0x%X\n", wParam, lParam);
        fclose(log);
    }
#endif

    if (IME_HandleMessage(hwnd, msg, wParam, &lParam, data->videodata))
        return 0;

    switch (msg) {

    case WM_SHOWWINDOW:
        {
            if (wParam) {
                SDL_SendWindowEvent(data->window, SDL_WINDOWEVENT_SHOWN, 0, 0);
            } else {
                SDL_SendWindowEvent(data->window, SDL_WINDOWEVENT_HIDDEN, 0, 0);
            }
        }
        break;

    case WM_ACTIVATE:
        {
            BOOL minimized;

            minimized = HIWORD(wParam);
            if (!minimized && (LOWORD(wParam) != WA_INACTIVE)) {
                SDL_SendWindowEvent(data->window, SDL_WINDOWEVENT_SHOWN, 0, 0);
                SDL_SendWindowEvent(data->window,
                                    SDL_WINDOWEVENT_RESTORED, 0, 0);
                if (IsZoomed(hwnd)) {
                    SDL_SendWindowEvent(data->window,
                                        SDL_WINDOWEVENT_MAXIMIZED, 0, 0);
                }
                if (SDL_GetKeyboardFocus() != data->window) {
                    SDL_SetKeyboardFocus(data->window);
                }

				if(SDL_GetMouse()->relative_mode) {
					LONG cx, cy;
					RECT rect;
					GetWindowRect(hwnd, &rect);

					cx = (rect.left + rect.right) / 2;
					cy = (rect.top + rect.bottom) / 2;

					/* Make an absurdly small clip rect */
					rect.left = cx-1;
					rect.right = cx+1;
					rect.top = cy-1;
					rect.bottom = cy+1;

					ClipCursor(&rect);
				}

                /*
                 * FIXME: Update keyboard state
                 */
                WIN_CheckClipboardUpdate(data->videodata);
            } else {
                if (SDL_GetKeyboardFocus() == data->window) {
                    SDL_SetKeyboardFocus(NULL);
                }
                if (minimized) {
                    SDL_SendWindowEvent(data->window,
                                        SDL_WINDOWEVENT_MINIMIZED, 0, 0);
                }
            }
        }
        returnCode = 0;
        break;

	case WM_MOUSEMOVE:
		if(SDL_GetMouse()->relative_mode)
			break;
        SDL_SendMouseMotion(data->window, 0, LOWORD(lParam), HIWORD(lParam));
        break;

	case WM_INPUT:
	{
		HRAWINPUT hRawInput = (HRAWINPUT)lParam;
		RAWINPUT inp;
		UINT size = sizeof(inp);

		if(!SDL_GetMouse()->relative_mode)
			break;

		GetRawInputData(hRawInput, RID_INPUT, &inp, &size, sizeof(RAWINPUTHEADER));

		/* Mouse data */
		if(inp.header.dwType == RIM_TYPEMOUSE)
		{
			RAWMOUSE* mouse = &inp.data.mouse;

			if((mouse->usFlags & 0x01) == MOUSE_MOVE_RELATIVE)
			{
				SDL_SendMouseMotion(data->window, 1, (int)mouse->lLastX, (int)mouse->lLastY);
			}
			else
			{
				// synthesize relative moves from the abs position
				static SDL_Point initialMousePoint;
				if ( initialMousePoint.x == 0 && initialMousePoint.y == 0 )
				{
					initialMousePoint.x = mouse->lLastX;
					initialMousePoint.y = mouse->lLastY;
				}

				SDL_SendMouseMotion(data->window, 1, (int)(mouse->lLastX-initialMousePoint.x), (int)(mouse->lLastY-initialMousePoint.y) );

				initialMousePoint.x = mouse->lLastX;
				initialMousePoint.y = mouse->lLastY;
			}
		}
		break;
	}

    case WM_LBUTTONDOWN:
        SDL_SendMouseButton(data->window, SDL_PRESSED, SDL_BUTTON_LEFT);
        break;

    case WM_LBUTTONUP:
        SDL_SendMouseButton(data->window, SDL_RELEASED, SDL_BUTTON_LEFT);
        break;

    case WM_RBUTTONDOWN:
        SDL_SendMouseButton(data->window, SDL_PRESSED, SDL_BUTTON_RIGHT);
        break;

    case WM_RBUTTONUP:
        SDL_SendMouseButton(data->window, SDL_RELEASED, SDL_BUTTON_RIGHT);
        break;

    case WM_MBUTTONDOWN:
        SDL_SendMouseButton(data->window, SDL_PRESSED, SDL_BUTTON_MIDDLE);
        break;

    case WM_MBUTTONUP:
        SDL_SendMouseButton(data->window, SDL_RELEASED, SDL_BUTTON_MIDDLE);
        break;

    case WM_XBUTTONDOWN:
        SDL_SendMouseButton(data->window, SDL_PRESSED, SDL_BUTTON_X1 + GET_XBUTTON_WPARAM(wParam) - 1);
        returnCode = TRUE;
        break;

    case WM_XBUTTONUP:
        SDL_SendMouseButton(data->window, SDL_RELEASED, SDL_BUTTON_X1 + GET_XBUTTON_WPARAM(wParam) - 1);
        returnCode = TRUE;
        break;

    case WM_MOUSEWHEEL:
        {
            // FIXME: This may need to accumulate deltas up to WHEEL_DELTA
            short motion = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

            SDL_SendMouseWheel(data->window, 0, motion);
            break;
        }

#ifdef WM_MOUSELEAVE
    /* FIXME: Do we need the SDL 1.2 hack to generate WM_MOUSELEAVE now? */
    case WM_MOUSELEAVE:
        if (SDL_GetMouseFocus() == data->window) {
            SDL_SetMouseFocus(NULL);
        }
        returnCode = 0;
        break;
#endif /* WM_MOUSELEAVE */

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        {
			SDL_Scancode code = WindowsScanCodeToSDLScanCode( lParam, wParam );
			if ( code != SDL_SCANCODE_UNKNOWN ) {
                SDL_SendKeyboardKey(SDL_PRESSED, code );
            }
        }
        returnCode = 0;
        break;

    case WM_SYSKEYUP:
    case WM_KEYUP:
        {
            SDL_Scancode code = WindowsScanCodeToSDLScanCode( lParam, wParam );
            if ( code != SDL_SCANCODE_UNKNOWN ) {
                if (code == SDL_SCANCODE_PRINTSCREEN &&
                    SDL_GetKeyboardState(NULL)[code] == SDL_RELEASED) {
                    SDL_SendKeyboardKey(SDL_PRESSED, code);
                }
                SDL_SendKeyboardKey(SDL_RELEASED, code);
            }
        }
        returnCode = 0;
        break;

    case WM_CHAR:
        {
            char text[4];

            /* Convert to UTF-8 and send it on... */
            if (wParam <= 0x7F) {
                text[0] = (char) wParam;
                text[1] = '\0';
            } else if (wParam <= 0x7FF) {
                text[0] = 0xC0 | (char) ((wParam >> 6) & 0x1F);
                text[1] = 0x80 | (char) (wParam & 0x3F);
                text[2] = '\0';
            } else {
                text[0] = 0xE0 | (char) ((wParam >> 12) & 0x0F);
                text[1] = 0x80 | (char) ((wParam >> 6) & 0x3F);
                text[2] = 0x80 | (char) (wParam & 0x3F);
                text[3] = '\0';
            }
            SDL_SendKeyboardText(text);
        }
        returnCode = 0;
        break;

#ifdef WM_INPUTLANGCHANGE
    case WM_INPUTLANGCHANGE:
        {
            WIN_UpdateKeymap();
        }
        returnCode = 1;
        break;
#endif /* WM_INPUTLANGCHANGE */

#ifdef WM_GETMINMAXINFO
    case WM_GETMINMAXINFO:
        {
            MINMAXINFO *info;
            RECT size;
            int x, y;
            int w, h;
            int min_w, min_h;
            int max_w, max_h;
            int style;
            BOOL menu;
			BOOL constrain_max_size;

            /* If we allow resizing, let the resize happen naturally */
            if (SDL_IsShapedWindow(data->window))
                Win32_ResizeWindowShape(data->window);

            /* Get the current position of our window */
            GetWindowRect(hwnd, &size);
            x = size.left;
            y = size.top;

            /* Calculate current size of our window */
            SDL_GetWindowSize(data->window, &w, &h);
            SDL_GetWindowMinimumSize(data->window, &min_w, &min_h);
            SDL_GetWindowMaximumSize(data->window, &max_w, &max_h);

            /* Store in min_w and min_h difference between current size and minimal 
               size so we don't need to call AdjustWindowRectEx twice */
            min_w -= w;
            min_h -= h;
            if (max_w && max_h) {
                max_w -= w;
                max_h -= h;
                constrain_max_size = TRUE;
            } else {
                constrain_max_size = FALSE;
            }

            size.top = 0;
            size.left = 0;
            size.bottom = h;
            size.right = w;

            style = GetWindowLong(hwnd, GWL_STYLE);
            /* DJM - according to the docs for GetMenu(), the
               return value is undefined if hwnd is a child window.
               Aparently it's too difficult for MS to check
               inside their function, so I have to do it here.
             */
            menu = (style & WS_CHILDWINDOW) ? FALSE : (GetMenu(hwnd) != NULL);
            AdjustWindowRectEx(&size, style, menu, 0);
            w = size.right - size.left;
            h = size.bottom - size.top;

            /* Fix our size to the current size */
            info = (MINMAXINFO *) lParam;
            if (SDL_GetWindowFlags(data->window) & SDL_WINDOW_RESIZABLE) {
                info->ptMinTrackSize.x = w + min_w;
                info->ptMinTrackSize.y = h + min_h;
                if (constrain_max_size) {
                    info->ptMaxTrackSize.x = w + max_w;
                    info->ptMaxTrackSize.y = h + max_h;
                }
            } else {
                info->ptMaxSize.x = w;
                info->ptMaxSize.y = h;
                info->ptMaxPosition.x = x;
                info->ptMaxPosition.y = y;
                info->ptMinTrackSize.x = w;
                info->ptMinTrackSize.y = h;
                info->ptMaxTrackSize.x = w;
                info->ptMaxTrackSize.y = h;
            }
        }
        returnCode = 0;
        break;
#endif /* WM_GETMINMAXINFO */

    case WM_WINDOWPOSCHANGED:
        {
            RECT rect;
            int x, y;
            int w, h;
            Uint32 window_flags;

            if (!GetClientRect(hwnd, &rect) ||
                (rect.right == rect.left && rect.bottom == rect.top)) {
                break;
            }
            ClientToScreen(hwnd, (LPPOINT) & rect);
            ClientToScreen(hwnd, (LPPOINT) & rect + 1);

            window_flags = SDL_GetWindowFlags(data->window);
            if ((window_flags & SDL_WINDOW_INPUT_GRABBED) &&
                (window_flags & SDL_WINDOW_INPUT_FOCUS)) {
                ClipCursor(&rect);
            }

            x = rect.left;
            y = rect.top;
            SDL_SendWindowEvent(data->window, SDL_WINDOWEVENT_MOVED, x, y);

            w = rect.right - rect.left;
            h = rect.bottom - rect.top;
            SDL_SendWindowEvent(data->window, SDL_WINDOWEVENT_RESIZED, w,
                                h);
        }
        break;

    case WM_SETCURSOR:
        {
            Uint16 hittest;

            hittest = LOWORD(lParam);
            if (hittest == HTCLIENT) {
                SetCursor(SDL_cursor);
                returnCode = TRUE;
            }
        }
        break;

        /* We were occluded, refresh our display */
    case WM_PAINT:
        {
            RECT rect;
            if (GetUpdateRect(hwnd, &rect, FALSE)) {
                ValidateRect(hwnd, &rect);
                SDL_SendWindowEvent(data->window, SDL_WINDOWEVENT_EXPOSED,
                                    0, 0);
            }
        }
        returnCode = 0;
        break;

        /* We'll do our own drawing, prevent flicker */
    case WM_ERASEBKGND:
        {
        }
        return (1);

#if defined(SC_SCREENSAVE) || defined(SC_MONITORPOWER)
    case WM_SYSCOMMAND:
        {
            /* Don't start the screensaver or blank the monitor in fullscreen apps */
            if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
                (wParam & 0xFFF0) == SC_MONITORPOWER) {
                if (SDL_GetVideoDevice()->suspend_screensaver) {
                    return (0);
                }
            }
        }
        break;
#endif /* System has screensaver support */

    case WM_CLOSE:
        {
            SDL_SendWindowEvent(data->window, SDL_WINDOWEVENT_CLOSE, 0, 0);
        }
        returnCode = 0;
        break;

	case WM_TOUCH:
		{
			UINT i, num_inputs = LOWORD(wParam);
			PTOUCHINPUT inputs = SDL_stack_alloc(TOUCHINPUT, num_inputs);
			if (data->videodata->GetTouchInputInfo((HTOUCHINPUT)lParam, num_inputs, inputs, sizeof(TOUCHINPUT))) {
				RECT rect;
				float x, y;

				if (!GetClientRect(hwnd, &rect) ||
				    (rect.right == rect.left && rect.bottom == rect.top)) {
					break;
				}
				ClientToScreen(hwnd, (LPPOINT) & rect);
				ClientToScreen(hwnd, (LPPOINT) & rect + 1);
				rect.top *= 100;
				rect.left *= 100;
				rect.bottom *= 100;
				rect.right *= 100;

				for (i = 0; i < num_inputs; ++i) {
					PTOUCHINPUT input = &inputs[i];

					const SDL_TouchID touchId = (SDL_TouchID)
												((size_t)input->hSource);
					if (!SDL_GetTouch(touchId)) {
						SDL_Touch touch;

						touch.id = touchId;
						touch.x_min = 0;
						touch.x_max = 1;
						touch.native_xres = touch.x_max - touch.x_min;
						touch.y_min = 0;
						touch.y_max = 1;
						touch.native_yres = touch.y_max - touch.y_min;
						touch.pressure_min = 0;
						touch.pressure_max = 1;
						touch.native_pressureres = touch.pressure_max - touch.pressure_min;

						if (SDL_AddTouch(&touch, "") < 0) {
							continue;
						}
					}

					// Get the normalized coordinates for the window
					x = (float)(input->x - rect.left)/(rect.right - rect.left);
					y = (float)(input->y - rect.top)/(rect.bottom - rect.top);

					if (input->dwFlags & TOUCHEVENTF_DOWN) {
						SDL_SendFingerDown(touchId, input->dwID, SDL_TRUE, x, y, 1);
					}
					if (input->dwFlags & TOUCHEVENTF_MOVE) {
						SDL_SendTouchMotion(touchId, input->dwID, SDL_FALSE, x, y, 1);
					}
					if (input->dwFlags & TOUCHEVENTF_UP) {
						SDL_SendFingerDown(touchId, input->dwID, SDL_FALSE, x, y, 1);
					}
				}
			}
			SDL_stack_free(inputs);

			data->videodata->CloseTouchInputHandle((HTOUCHINPUT)lParam);
			return 0;
		}
		break;

    case WM_DROPFILES:
        {
            UINT i;
            HDROP drop = (HDROP) wParam;
            UINT count = DragQueryFile(drop, 0xFFFFFFFF, NULL, 0);
            for (i = 0; i < count; ++i) {
                UINT size = DragQueryFile(drop, i, NULL, 0) + 1;
                LPTSTR buffer = SDL_stack_alloc(TCHAR, size);
                if (buffer) {
                    if (DragQueryFile(drop, i, buffer, size)) {
                        char *file = WIN_StringToUTF8(buffer);
                        SDL_SendDropFile(file);
                        SDL_free(file);
                    }
                    SDL_stack_free(buffer);
                }
            }
            DragFinish(drop);
            return 0;
        }
        break;
    }

    /* If there's a window proc, assume it's going to handle messages */
    if (data->wndproc) {
        return CallWindowProc(data->wndproc, hwnd, msg, wParam, lParam);
    } else if (returnCode >= 0) {
        return returnCode;
    } else {
        return CallWindowProc(DefWindowProc, hwnd, msg, wParam, lParam);
    }
}

void
WIN_PumpEvents(_THIS)
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

static int app_registered = 0;
LPTSTR SDL_Appname = NULL;
Uint32 SDL_Appstyle = 0;
HINSTANCE SDL_Instance = NULL;

/* Register the class for this application */
int
SDL_RegisterApp(char *name, Uint32 style, void *hInst)
{
    WNDCLASS class;

    /* Only do this once... */
    if (app_registered) {
        ++app_registered;
        return (0);
    }
    if (!name && !SDL_Appname) {
        name = "SDL_app";
#if defined(CS_BYTEALIGNCLIENT) || defined(CS_OWNDC)
        SDL_Appstyle = (CS_BYTEALIGNCLIENT | CS_OWNDC);
#endif
        SDL_Instance = hInst ? hInst : GetModuleHandle(NULL);
    }

    if (name) {
        SDL_Appname = WIN_UTF8ToString(name);
        SDL_Appstyle = style;
        SDL_Instance = hInst ? hInst : GetModuleHandle(NULL);
    }

    /* Register the application class */
    class.hCursor = NULL;
    class.hIcon =
        LoadImage(SDL_Instance, SDL_Appname, IMAGE_ICON, 0, 0,
                  LR_DEFAULTCOLOR);
    class.lpszMenuName = NULL;
    class.lpszClassName = SDL_Appname;
    class.hbrBackground = NULL;
    class.hInstance = SDL_Instance;
    class.style = SDL_Appstyle;
    class.lpfnWndProc = WIN_WindowProc;
    class.cbWndExtra = 0;
    class.cbClsExtra = 0;
    if (!RegisterClass(&class)) {
        SDL_SetError("Couldn't register application class");
        return (-1);
    }

    app_registered = 1;
    return (0);
}

/* Unregisters the windowclass registered in SDL_RegisterApp above. */
void
SDL_UnregisterApp()
{
    WNDCLASS class;

    /* SDL_RegisterApp might not have been called before */
    if (!app_registered) {
        return;
    }
    --app_registered;
    if (app_registered == 0) {
        /* Check for any registered window classes. */
        if (GetClassInfo(SDL_Instance, SDL_Appname, &class)) {
            UnregisterClass(SDL_Appname, SDL_Instance);
        }
        SDL_free(SDL_Appname);
        SDL_Appname = NULL;
    }
}

#endif /* SDL_VIDEO_DRIVER_WINDOWS */

/* vi: set ts=4 sw=4 expandtab: */
