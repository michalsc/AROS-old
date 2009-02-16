/*
    Copyright  1995-2008, The AROS Development Team. All rights reserved.
    $Id: gdi.c 27757 2008-01-26 15:05:40Z verhaegs $

    Desc: Host-side part of GDI hidd. Handles windows and receives events.
    Lang: English.
*/

#include <windows.h>
#include <stdio.h>
#include "gdi.h"

#define D(x) x

DWORD gdi_id;

/****************************************************************************************/

LRESULT CALLBACK window_callback(HWND win, UINT msg, WPARAM wp, LPARAM lp)
{
    return DefWindowProc(win, msg, wp, lp);
}

/****************************************************************************************/

DWORD WINAPI gdithread_entry(LPVOID p)
{
    BOOL res;
    MSG msg;
    ATOM wcl;
    HWND win;
    WINDOWPLACEMENT wpos;
    WNDCLASS wcl_desc = {
        CS_SAVEBITS,
        window_callback,
        0,
        0,
        NULL,
        NULL,
        COLOR_WINDOW,
        NULL
    };

    wcl_desc.hInstance = GetModuleHandle(NULL);
    wcl = RegisterClass(&wcl_desc);
    D(printf("[GDI] Created window class 0x%p\n", wcl));
    if (wcl) {
    	do {
            res = GetMessage(&msg, NULL, 0, 0);
            D(printf("[GDI] GetMessage returned %ld\n", res));
            if (res > 0) {
            	D(printf("[GDI] Got message %lu\n", msg.message));
            	switch (msg.message) {
            	case NOTY_WINCREATE:
            	    win = CreateWindow(wcl, "AROS Screen", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, msg.wParam,  msg.lParam,
            	    		       NULL, NULL, wcl_desc.hInstance, NULL);
            	    ShowWindow(win, SW_SHOW);
            	    UpdateWindow(win);
            	    break;
            	case NOTY_WINDISPOSE:
            	    DestroyWindow(msg.hwnd);
            	    break;
            	case NOTY_RESIZEWINDOW:
            	    wpos.length = sizeof(wpos);
            	    if (GetWindowPlacement(msg.hwnd, &wpos)) {
            	        wpos.rcNormalPosition.right = wpos.rcNormalPosition.left + msg.wParam;
            	        wpos.rcNormalPosition.bottom = wpos.rcNormalPosition.top + msg.lParam;
            	        SetWindowPlacement(msg.hwnd, &wpos);
            	    }
            	    break;
            	default:
            	    DispatchMessage(&msg);
            	}
	    }
        } while (res > 0);
        UnregisterClass(wcl, wcl_desc.hInstance);
    }
}

/****************************************************************************************/

DWORD __declspec(dllexport) GDI_Init(void)
{
    HANDLE th;

    th = CreateThread(NULL, 0, gdithread_entry, NULL, 0, &gdi_id);
    D(printf("[GDI] Started thread 0x%p\n", th));
    if (th) {
        CloseHandle(th);
    }
    return th ? 1 : 0;
}
    
/****************************************************************************************/

ULONG __declspec(dllexport) GDI_PutMsg(HWND win, UINT msg, WPARAM wp, LPARAM lp)
{
    if (win)
    	return PostMessage(win, msg, wp, lp);
    else
        return PostThreadMessage(gdi_id, msg, wp, lp);
}
