//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "../../Framework/MyFramework/SourceWindows/MYFWWinMain.h"
#include <delayimp.h>

#include "Core/OpenCVCore.h"
#include "Utility/Helpers.h"

using namespace cv;

void TestSimpleModification();
void TestDFT();
void TestVideo();
void TestMorph();
void TestThreshold();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    GameCore* pGameCore = new OpenCVCore();

    int result = MYFWWinMain( pGameCore, 1200, 700 );

    delete pGameCore;

    return result;
}

FARPROC WINAPI delayHook(unsigned dliNotify, PDelayLoadInfo pdli)
{
    switch( dliNotify )
    {
    case dliNotePreLoadLibrary:
        if( strcmp( pdli->szDll, "pthreadVC2.dll" ) == 0 )
        {
#if _WIN64
            FARPROC ret = (FARPROC)LoadLibrary( "pthreadVC2-x64.dll" );
            if( ret == 0 )
            {
                DWORD err = GetLastError();
                assert( false );
            }
            return ret;
#else
            return (FARPROC)LoadLibrary( "pthreadVC2-x86.dll" );
#endif
        }
        break;

    default:
        return 0;
    }

    return 0;
}

#if WINVER >= 0x0602
const PfnDliHook __pfnDliNotifyHook2 = delayHook;
#else
PfnDliHook __pfnDliNotifyHook2 = delayHook;
#endif
