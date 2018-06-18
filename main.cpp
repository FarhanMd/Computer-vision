#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//#define _WIN32_WINNT
#define _WIN32_WINNT 0x501
#include <stdio.h>
#include <conio.h>
#include<io.h>
#include <winsock2.h>
#pragma comment(lib, "libws2_32.a")
#include <ws2tcpip.h>
#include <windows.h>
#include "uuirtdrv.h"
#define PORT_NO "2800"
HINSTANCE		hinstLib = NULL;

// Driver handle for UUIRT device
HUUHANDLE		hDrvHandle;

unsigned int	drvVersion;

// Globals to hold last-learned IR-Code and its format...
/*char	gIRCode[2048] = "0000 0074 0000 0009 0147 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 006E 00DA 1BA4";*/
char    gIRCode[2048] = "0000 0074 0000 0009 0147 006F 00DA 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 006E 00DA 13E2";
char    gIRCode_2[2048] = "0000 0074 0000 0009 0147 006E 00DA 006E 006D 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 126F";
char    gIRCode_3[2048] = "0000 0074 0000 0009 0147 006F 006D 006F 00DA 006F 006D 006F 006D 006F 006D 006F 006D 006F 006D 006F 00DA 1176";
char    gIRCode_4[2048] = "0000 0074 0000 0009 0147 006E 006D 006E 00DA 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 1312";
char    gIRCode_5[2048] = "0000 0074 0000 0009 0147 006E 00DA 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 1AEE";
char    gIRCode_6[2048] ="0000 0074 0000 0009 0147 006E 00DA 006E 00DA 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 006E 00DA 1318";
char    gIRCode_7[2048] = "0000 0074 0000 0009 0147 006E 00DA 006E 006D 006E 006D 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 127C";
char    gIRCode_8[2048] = "0000 0074 0000 0009 0147 006E 006D 006E 00DA 006E 006D 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 1340";
char    gIRCode_9[2048] = "0000 0074 0000 0009 0147 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 006E 006D 006E 00DA 1E15";
char    gIRCode_10[2048] = "0000 0074 0000 0009 0147 006E 006D 006E 006D 006E 006D 006E 00DA 006E 006D 006E 006D 006E 006D 006E 00DA 186C";
char    gIRCode_11[2048] = "0000 0074 0000 0009 0147 006E 00DA 006E 00DA 006E 006D 006E 00DA 006E 006D 006E 006D 006E 006D 006E 00DA 14D6";
char    gIRCode_12[2048] = "0000 0074 0000 0009 0147 006E 006D 006E 006D 006E 00DA 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 1053";
char    gIRCode_13[2048] = "0000 0074 0000 0009 0147 006E 006D 006E 00DA 006E 00DA 006E 00DA 006E 006D 006E 006D 006E 006D 006E 00DA 2650";
char    gIRCode_14[2048] = "0000 0074 0000 0009 0147 006E 00DA 006E 006D 006E 00DA 006E 00DA 006E 006D 006E 006D 006E 006D 006E 00DA 3020";
char    gIRCode_15[2048] = "0000 0074 0000 0009 0147 006E 00DA 006E 00DA 006E 00DA 006E 00DA 006E 006D 006E 006D 006E 006D 006E 006D 2E2F";
char    gIRCode_16[2048] ="0000 0074 0000 0009 0147 006E 00DA 006E 00DA 006E 006D 006E 006D 006E 00DA 006E 006D 006E 006D 006E 00DA 1EC7";
char    gIRCode_17[2048] ="0000 0074 0000 0009 0147 006E 00DA 006E 006D 006E 006D 006E 006D 006E 00DA 006E 006D 006E 006D 006E 006D 178B";
int		gIRCodeFormat = UUIRTDRV_IRFMT_PRONTO;

// Globals used during Learning...
char	gLearnBuffer[2048];
int		gLearnFormat;

// UUIRT .dll funtion pointers. These will be assigned when calling LoadDLL()
pfn_UUIRTOpen				fnUUIRTOpen;
pfn_UUIRTClose				fnUUIRTClose;
pfn_UUIRTGetDrvInfo			fn_UUIRTGetDrvInfo;
pfn_UUIRTGetUUIRTInfo		fn_UUIRTGetUUIRTInfo;
pfn_UUIRTGetUUIRTConfig		fn_UUIRTGetUUIRTConfig;
pfn_UUIRTSetUUIRTConfig		fn_UUIRTSetUUIRTConfig;
pfn_UUIRTSetReceiveCallback	fn_UUIRTSetReceiveCallback;
pfn_UUIRTTransmitIR			fn_UUIRTTransmitIR;
pfn_UUIRTLearnIR			fn_UUIRTLearnIR;

/*****************************************************************************/
/* unLoadDLL: Disconnects from .DLL and unloads it from memory				 */
/*																			 */
/* returns: none															 */
/*																			 */
/*****************************************************************************/
void unLoadDLL(void)
{
	if (hinstLib)
		FreeLibrary(hinstLib);
	hinstLib = NULL;
}

/*****************************************************************************/
/* loadDLL: Establish contact with the UUIRTDRV dll and assign function      */
/*			entry points													 */
/*																			 */
/* returns: TRUE on success, FALSE on failure								 */
/*																			 */
/*****************************************************************************/
BOOL loadDLL(void)
{
    // Get a handle to the DLL module.

    hinstLib = LoadLibrary("uuirtdrv");

    // If the handle is valid, try to get the function address.

    if (hinstLib != NULL)
    {
        fnUUIRTOpen = (pfn_UUIRTOpen) GetProcAddress(hinstLib, "UUIRTOpen");
        fnUUIRTClose = (pfn_UUIRTClose) GetProcAddress(hinstLib, "UUIRTClose");
		fn_UUIRTGetDrvInfo  = (pfn_UUIRTGetDrvInfo) GetProcAddress(hinstLib, "UUIRTGetDrvInfo");
		fn_UUIRTGetUUIRTInfo = (pfn_UUIRTGetUUIRTInfo) GetProcAddress(hinstLib, "UUIRTGetUUIRTInfo");
		fn_UUIRTGetUUIRTConfig = (pfn_UUIRTGetUUIRTConfig) GetProcAddress(hinstLib, "UUIRTGetUUIRTConfig");
		fn_UUIRTSetUUIRTConfig = (pfn_UUIRTSetUUIRTConfig) GetProcAddress(hinstLib, "UUIRTSetUUIRTConfig");
		fn_UUIRTSetReceiveCallback = (pfn_UUIRTSetReceiveCallback) GetProcAddress(hinstLib, "UUIRTSetReceiveCallback");
		fn_UUIRTTransmitIR = (pfn_UUIRTTransmitIR) GetProcAddress(hinstLib, "UUIRTTransmitIR");
		fn_UUIRTLearnIR = (pfn_UUIRTLearnIR) GetProcAddress(hinstLib, "UUIRTLearnIR");

		if (!fnUUIRTOpen ||
			!fnUUIRTClose ||
			!fn_UUIRTGetDrvInfo ||
			!fn_UUIRTGetUUIRTInfo ||
			!fn_UUIRTGetUUIRTConfig ||
			!fn_UUIRTSetUUIRTConfig ||
			!fn_UUIRTSetReceiveCallback ||
			!fn_UUIRTTransmitIR ||
			!fn_UUIRTLearnIR)
		{
			unLoadDLL();
			return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}

/*****************************************************************************/
/* IRReceiveCallback: Receive IR Callback Procedure						     */
/*																			 */
/* This procedure is called by the UUIRT .dll whenever an IRcode is received */
/* The IRcode is passed to the callback in UIR format.						 */
/*																			 */
/*****************************************************************************/
void WINAPI IRReceiveCallback (char *IREventStr, void *userData)
{
	COORD coord;
	HANDLE hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO conInfo;

	// Move the cursor to the bottom of the screen
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdOut, &conInfo);
	coord.X = conInfo.srWindow.Left;
	coord.Y = conInfo.srWindow.Bottom-1;
	SetConsoleCursorPosition(hStdOut, coord);

	printf("<IR Receive: Code = %s, UserData = %08x!!!\n", IREventStr, (UINT32)userData);

	// Move the cursor back to its original position
	SetConsoleCursorPosition(hStdOut, conInfo.dwCursorPosition);
}

/*****************************************************************************/
/* IRLearnCallback: Learn IR Callback Procedure						         */
/*																			 */
/* This procedure is called by the UUIRT .dll during the LEARN process		 */
/* to allow user feedback on Learn progress, signal quality and (if needed)  */
/* carrier frequency.														 */
/*																			 */
/*****************************************************************************/
void WINAPI IRLearnCallback (unsigned int progress, unsigned int sigQuality, unsigned long carrierFreq, void *userData)
{
	COORD coord;
	HANDLE hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO conInfo;

	// Move the cursor to the bottom of the screen
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdOut, &conInfo);
	coord.X = conInfo.srWindow.Left;
	coord.Y = conInfo.srWindow.Bottom-2;
	SetConsoleCursorPosition(hStdOut, coord);

	printf("<Learn Progress: %d%%, Signal = %d%%, Freq = %ld, UserData = %08x!!!\n", progress, sigQuality & 0xff, carrierFreq, (UINT32)userData);

	// Move the cursor back to its original position
	SetConsoleCursorPosition(hStdOut, conInfo.dwCursorPosition);
}


/*****************************************************************************/
/* LearnThread: Learn IR Thread function									 */
/*																			 */
/* This function executes as a separate thread which calls the UUIRTLearnIR  */
/* function.  In this example, the UUIRTLearnIR function is called from this */
/* separate thread to allow the main console thread to continue monitoring   */
/* the keyboard so that the user may abort the learn process. Depending on   */
/* the application, the UUIRTLearnIR may be called from the main thread if   */
/* an asynchronous method (such as a timer) is available to monitor user     */
/* input.																	 */
/*																			 */
/*****************************************************************************/
DWORD WINAPI LearnThread( LPVOID lpParameter )
{
	BOOL *pbAbortLearn = (BOOL *)lpParameter;

	printf("\nCalling LearnIR...");
	if (!fn_UUIRTLearnIR(hDrvHandle, gLearnFormat, gLearnBuffer, IRLearnCallback, (void *)0x5a5a5a5a, pbAbortLearn, 0, NULL, NULL))
	{
		printf("\n\t*** ERROR calling UUIRTLearnIR! ***\n");
	}
	else
	{
		if (!*pbAbortLearn)
		{
			printf("...Done...IRCode = %s\n",gLearnBuffer);
			strcpy(gIRCode, gLearnBuffer);
			gIRCodeFormat = gLearnFormat & 0xff;
		}
		else
		{
			printf("...*** LEARN ABORTED ***\n");
		}
	}
	return 0;
}
int main(int argc, char* argv[])
{
    WSADATA wsaData;
    int iResult;

// Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof (hints));

    //struct initialization of addrinfo hint
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, PORT_NO, &hints, &result);

    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    //socket object for connection
    SOCKET ListenSocket = INVALID_SOCKET;

    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    //listening to socket. whether the connection is establishe dor not
    if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    SOCKET ClientSocket;
    ClientSocket = INVALID_SOCKET;

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    UUINFO UsbUirtInfo;
	UINT32 uConfig;
	char ch;

	printf("UUIRTDRV Example Program...\n");
	printf("===========================\n\n");

	if (!loadDLL())
	{
		printf("ERROR: Unable to load uuirtdrv,dll!\n");
		Sleep(1000);
		return 0;
	}

	if (!fn_UUIRTGetDrvInfo(&drvVersion))
	{
		printf("ERROR: Unable to retrieve uuirtdrv version!\n");
		Sleep(1000);
		unLoadDLL();
		return 0;
	}

	if (drvVersion != 0x0100)
	{
		printf("ERROR: Invalid uuirtdrv version!\n");
		Sleep(1000);
		unLoadDLL();
		return 0;
	}

	hDrvHandle = fnUUIRTOpen();
	if (hDrvHandle == INVALID_HANDLE_VALUE)
	{
		DWORD err;

		err = GetLastError();

		if (err == UUIRTDRV_ERR_NO_DLL)
		{
			printf("ERROR: Unable to find USB-UIRT Driver. Please make sure driver is Installed!\n");
		}
		else if (err == UUIRTDRV_ERR_NO_DEVICE)
		{
			printf("ERROR: Unable to connect to USB-UIRT device!  Please ensure device is connected to the computer!\n");
		}
		else if (err == UUIRTDRV_ERR_NO_RESP)
		{
			printf("ERROR: Unable to communicate with USB-UIRT device!  Please check connections and try again.  If you still have problems, try unplugging and reconnecting your USB-UIRT.  If problem persists, contact Technical Support!\n");
		}
		else if (err == UUIRTDRV_ERR_VERSION)
		{
			printf("ERROR: Your USB-UIRT's firmware is not compatible with this API DLL. Please verify you are running the latest API DLL and that you're using the latest version of USB-UIRT firmware!  If problem persists, contact Technical Support!\n");
		}
		else
		{
			printf("ERROR: Unable to initialize USB-UIRT (unknown error)!\n");
		}

		unLoadDLL();

		printf("Press any key to exit...");
		getch();
		printf("\n");

		return 0;
	}

	printf("\n");

	// Register a callback function for IR receive...
	fn_UUIRTSetReceiveCallback(hDrvHandle, &IRReceiveCallback, (void *)0xA5A5A5A5);
//variables used for receiving data
    while(1)
    {
        char recvbuf[10];
        int result;
       printf("\nserver waiting\n");

        //for receiving data from connection
        result = recv(ClientSocket, recvbuf, 10, 0);
        printf("Bytes received: %d\n", iResult);
        HANDLE hStdOut;
		CONSOLE_SCREEN_BUFFER_INFO conInfo;
		COORD coord;
		DWORD cWritten;

		// Move cursor to top-left of screen
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hStdOut, &conInfo);
		coord.X = conInfo.srWindow.Left;
		coord.Y = conInfo.srWindow.Top;
		SetConsoleCursorPosition(hStdOut, coord);

		// Clear screen
		FillConsoleOutputCharacter(
			hStdOut,          // screen buffer handle
			' ',       // fill with spaces
			(conInfo.srWindow.Right - conInfo.srWindow.Left + 1)*(conInfo.srWindow.Bottom-conInfo.srWindow.Top+1),            // number of cells to fill
			coord,            // first cell to write to
			&cWritten);       // actual number written


		// Retrieve information about the USB-UIRT device...
		if (!fn_UUIRTGetUUIRTInfo(hDrvHandle, &UsbUirtInfo))
		{
			printf("\t*** ERROR Retrieving USB-UIRT Information! ***\n");
		}
		else
		{
			printf("\tUSB-UIRT Info: FirmwareVer=%d.%d ProtVer=%d.%d FirmwareDate=%d/%d/%d\n",
					UsbUirtInfo.fwVersion>>8,
					UsbUirtInfo.fwVersion&0xff,
					UsbUirtInfo.protVersion>>8,
					UsbUirtInfo.protVersion&0xff,
					UsbUirtInfo.fwDateMonth,
					UsbUirtInfo.fwDateDay,
					UsbUirtInfo.fwDateYear+2000);
		}

		printf("\n");
        switch(recvbuf[0])
        {
            case '1':printf("\nTransmitting IR Code (blocking)...");
				if (!fn_UUIRTTransmitIR(hDrvHandle,
										gIRCode /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										1 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
				{
					printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
				}
				else
				{
					printf("...Returned from call (Done)!\n");
				}
				printf("Press any key to continue...");
				//getch();
				break;
        case '2':printf("\nTransmitting IR Code (blocking)...");
				if (!fn_UUIRTTransmitIR(hDrvHandle,
										gIRCode_2 /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										1 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
				{
					printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
				}
				else
				{
					printf("...Returned from call (Done)!\n");
				}
				printf("Press any key to continue...");
				//getch();
				break;
            case '3':
            printf("\nTransmitting IR Code (blocking)...");
				if (!fn_UUIRTTransmitIR(hDrvHandle,
										gIRCode_3 /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										1 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
				{
					printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
				}
				else
				{
					printf("...Returned from call (Done)!\n");
				}
				printf("Press any key to continue...");
				//getch();
                break;
            case '4':printf("\nTransmitting IR Code (blocking)...");
				if (!fn_UUIRTTransmitIR(hDrvHandle,
										gIRCode_4 /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										1 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
				{
					printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
				}
				else
				{
					printf("...Returned from call (Done)!\n");
				}
				printf("Press any key to continue...");
				//getch();
                break;
           case '5':printf("\nTransmitting IR Code (blocking)...");
				if (!fn_UUIRTTransmitIR(hDrvHandle,
										gIRCode_13 /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										1 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
				{
					printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
				}
				else
				{
					printf("...Returned from call (Done)!\n");
				}
				printf("Press any key to continue...");
				//getch();
                break;
                case '6':printf("\nTransmitting IR Code (blocking)...");
				if (!fn_UUIRTTransmitIR(hDrvHandle,
										gIRCode_14 /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										1 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
				{
					printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
				}
				else
				{
					printf("...Returned from call (Done)!\n");
				}
				printf("Press any key to continue...");
				//getch();
                break;
                case '7':printf("\nTransmitting IR Code (blocking)...");
				if (!fn_UUIRTTransmitIR(hDrvHandle,
										gIRCode_11 /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										1 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
				{
					printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
				}
				else
				{
					printf("...Returned from call (Done)!\n");
				}
				printf("Press any key to continue...");
				//getch();
                break;
                case '8':printf("\nTransmitting IR Code (blocking)...");
				if (!fn_UUIRTTransmitIR(hDrvHandle,
										gIRCode_12 /* IRCode */,
										gIRCodeFormat /* codeFormat */,
										1 /* repeatCount */,
										0 /* inactivityWaitTime */,
										NULL /* hEvent */,
										NULL /* reserved1 */,
										NULL /* reserved2 */
										))
				{
					printf("\n\t*** ERROR calling UUIRTTransmitIR! ***\n");
				}
				else
				{
					printf("...Returned from call (Done)!\n");
				}
				printf("Press any key to continue...");
				//getch();
                break;
        }
    }
    /*result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }*/
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
