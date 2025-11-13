// LIBRERIAS //
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>

#include "tetrismen.h"
#include "structs.h"

// DATOS PREDEFINIDIOS POR EL USUARIO //
#define TAMC 40
#define NRTA 16
#define NCTA 19 // 12 para mejor jugabilidad //
#define IZQ	 1
#define DER  2
#define ABJ  3

#define IDT_TIMER1 1

// VARIABLES GLOBALES //

int renElim = 0;
int renElimx = 0;
int aux = 0;

// DECLARACION DE FUNCIONES //
void trazartablero(HDC, int, int, int [NRTA][NCTA], HBRUSH hbr, HPEN);
void trazarElemento(HDC, int , int, STELEMENTO, HBRUSH *hbr); // recibira las brochas por tanto le declaramos el tipo de dato abstracto
void trazarcuadrito(HDC, int, int, int, int, HBRUSH hbr);    // recibira las brochas por tanto le declaramos el tipo de dato abstracto
void asignar_Elemento(STELEMENTO, int [NRTA][NCTA]);
void inicializartablero(int [NRTA][NCTA]);
void imprimirNombre(HDC, int , int, HBRUSH); // IMPRIMIR NOMBRE
void trazarNombre(HDC, int, int, HBRUSH); // NOMBRE
void nuevoElemento(STELEMENTO *);
void iniciBrush(HBRUSH *);

int validarMovimiento(int [NRTA][NCTA], STELEMENTO , int);
int moverElemento(int [NRTA][NCTA], STELEMENTO *, int);
int eliminarRenglones(int [NRTA][NCTA]);
int eliminarRen(int [NRTA][NCTA], int);
int rencondatos(int [NRTA][NCTA], int);
int renlleno(int [NRTA][NCTA], int);
int primer_Col(STELEMENTO);
int ultima_Col(STELEMENTO);
int ultimo_Ren(STELEMENTO);

STELEMENTO transponerElemento(STELEMENTO);
HBRUSH *crearBrochas(int); // para que apuntamos o por referencia el arreglo???


/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	static int tablero[NRTA][NCTA];
	static STELEMENTO elem_actual, elem_proximo;
	static HINSTANCE hInstance;	
	static HPEN hpenB;
	static HBRUSH hbrA;
	static HBRUSH *hbrArre;
	int i, select;
	int wmId = LOWORD(wParam);
	
	switch(Message) {
		case WM_CREATE:{
			srand(time(NULL));
			
			hpenB = CreatePen(PS_DOT, 1, RGB(160, 82 , 45)); // CREAR LAPIZ ROJO
			hbrA = CreateSolidBrush(RGB(230, 230, 250)); // BROCHA LAVANDA
			hbrArre = crearBrochas(11);
			iniciBrush(hbrArre);
			
			nuevoElemento(&elem_actual );
			nuevoElemento(&elem_proximo);
			inicializartablero(tablero);
			SetTimer(hwnd, IDT_TIMER1, 500, NULL);
			
			break;
		}
		case WM_TIMER:{
			switch(wParam)
			{
				case IDT_TIMER1:{
					if(!moverElemento(tablero, &elem_actual, ABJ)){
						asignar_Elemento(elem_actual, tablero);
						if(!eliminarRenglones(tablero)){
							KillTimer(hwnd, IDT_TIMER1);
							PlaySound("sound - muere.wav", NULL, SND_FILENAME|SND_ASYNC);
							select = MessageBox(hwnd, "Has perdido :P", "Deseas seguir jugando?", MB_YESNO | MB_ICONQUESTION);
           					if (select == IDYES){
           						PlaySound("sound - aceptar.wav", NULL, SND_FILENAME|SND_ASYNC);
								inicializartablero(tablero);
								SetTimer(hwnd, IDT_TIMER1, 500, NULL);
								renElim = 0;
							   }else{
							   	DestroyWindow(hwnd);
							   }
							   
						}
						elem_actual = elem_proximo;
						nuevoElemento(&elem_proximo);
					//	nuevoElemento(&elem_actual);
					}
					InvalidateRect(hwnd, NULL, TRUE);
					
					// IMPLEMENTACION DE LOS NIVELES //
					if(renElim >= 5){
						SetTimer(hwnd, IDT_TIMER1, 300, NULL);
					}
					if(renElim >= 10){
						SetTimer(hwnd, IDT_TIMER1, 250, NULL);
					}
					if(renElim >= 15){
						SetTimer(hwnd, IDT_TIMER1, 200, NULL);
					}
					if(renElim  >= 20){
						SetTimer(hwnd, IDT_TIMER1, 150, NULL);
					}
					if(renElim  >= 25){
						SetTimer(hwnd, IDT_TIMER1, 100, NULL);
					}
					//
					break;
				}

			}
			break;
		}
		case WM_KEYDOWN:{
			switch(wParam)
			{
				case VK_LEFT:{
					moverElemento(tablero,&elem_actual, IZQ);
					InvalidateRect(hwnd, NULL, TRUE);
				break;
				}
				case VK_RIGHT:{
					moverElemento(tablero, &elem_actual, DER);
					InvalidateRect(hwnd, NULL, TRUE);
				break;
				}
				case VK_DOWN:{
					if(!moverElemento(tablero, &elem_actual, ABJ)){
						asignar_Elemento(elem_actual, tablero);
						if(!eliminarRenglones(tablero)){
							KillTimer(hwnd, IDT_TIMER1);
							PlaySound("sound - muere.wav", NULL, SND_FILENAME|SND_ASYNC);
							
							select = MessageBox(hwnd, "Has perdido :P", "Deseas seguir jugando?", MB_YESNO | MB_ICONQUESTION);
           					if (select == IDYES){
           						PlaySound("sound - aceptar.wav", NULL, SND_FILENAME|SND_ASYNC);
								inicializartablero(tablero);
								SetTimer(hwnd, IDT_TIMER1, 500, NULL);
								renElim = 0;
							   }else{
							   	DestroyWindow(hwnd);
							   }
				
						}
						elem_actual = elem_proximo;
						nuevoElemento(&elem_proximo);
					//	nuevoElemento(&elem_actual);
					}
					InvalidateRect(hwnd, NULL, TRUE);
				break;
				}
				case VK_UP:{
					elem_actual = transponerElemento(elem_actual);
					InvalidateRect(hwnd, NULL, TRUE);
				break;
				}
				case VK_SPACE:{
					PlaySound("Musica - Jazzy For Fun.wav", NULL, SND_FILENAME|SND_ASYNC);
					KillTimer(hwnd,IDT_TIMER1);
					select = MessageBox(hwnd, "PRESIONA OTRA VEZ PARA REANUDAR", "PAUSE", MB_OK);
					if(select == IDOK){
					PlaySound("sound - aceptar.wav", NULL, SND_FILENAME|SND_LOOP);
					SetTimer(hwnd, IDT_TIMER1, 500, NULL); // incognita: ¿porque funciona?
					}
					break;
				}
			}
			break;
		}
		//
		case WM_COMMAND:{
			switch(wmId)
			{
				case IDM_NUEVO:{
					PlaySound("sound - aceptar.wav", NULL, SND_FILENAME|SND_ASYNC);
					KillTimer(hwnd,IDT_TIMER1);
					select = MessageBox(hwnd, "Estas en medio de un juego, ¿Realmente deseas reinciarlo?", "¿Nueva partida?", MB_YESNO | MB_ICONQUESTION);
					if(select == IDYES){
					PlaySound("sound -nuevojuego.wav", NULL, SND_FILENAME|SND_ASYNC);
					KillTimer(hwnd, IDT_TIMER1);
					inicializartablero(tablero);
					renElim = 0;
					SetTimer(hwnd, IDT_TIMER1, 500, NULL);
					nuevoElemento(&elem_actual);
					InvalidateRect(hwnd, NULL, TRUE);
					}else{
						SetTimer(hwnd, IDT_TIMER1, 500, NULL);
					}
					
					break;
				}
				case IDM_CREDITOS:{
					PlaySound("sound - aceptar.wav", NULL, SND_FILENAME|SND_ASYNC);
					KillTimer(hwnd,IDT_TIMER1);
					select = MessageBox(hwnd,"DATOS DE LOS ALUMNOS \n\nING EN COMPUTACION\nEDGAR HAZIEL REYNA GARCIA\nADRIAN HERNANDEZ MARIN\nANA JAZMIN RIUZ LACES\nPROGRAMACION ESTRUCTURADA\nSEMESTRE:PRIMERO\n\nDATOS DEL PROFESOR:\n\nJ.JESUS ARELLANO PIMENTEL\nFECHA:2024", "Datos", MB_OK | MB_ICONINFORMATION);
					if(select == IDOK){
					PlaySound("sound - aceptar.wav", NULL, SND_FILENAME|SND_ASYNC);
					SetTimer(hwnd, IDT_TIMER1, 500, NULL);
				}
					break;
				}
				case IDM_SALIR:{
					DestroyWindow(hwnd);
					break;
				}
			}
			break;
		}
		//
		
		case WM_PAINT:{
			hdc = BeginPaint(hwnd, &ps);
			TCHAR szMessage[64];
			TCHAR ssszMessage[64]; 
			
			SetBkMode(hdc, TRANSPARENT);
		
			if(renElim > renElimx)
				aux = renElim;
				renElimx = aux;
	
		// COLOR DE FONDO //
			SelectObject(hdc, hbrArre[10]);
			Rectangle(hdc, 0, 0, 1080, 700);
		// COLOR DE TABLERO Y DE ESTADISTICAS
			SelectObject(hdc, hbrArre[5]);
			
		//	SelectObject(hdc, hpenB); // activar para darle color a los bordes de las estadisticas
			Rectangle(hdc, 40, 40, 801, 680); // tabla
			Rectangle(hdc, 840, 40, 1030, 200); // estadisticas
			Rectangle(hdc, 840, 250, 1030, 450);
		//	SelectObject(hdc, hpenB);
		
			wsprintf(szMessage, "Puntuacion: %d", renElim);
			TextOut(hdc, 880, 380, szMessage, strlen(szMessage));
			
			wsprintf(ssszMessage, "Puntuacion Maxima:  %d", renElimx);
			TextOut(hdc, 860, 340, ssszMessage, strlen(ssszMessage));

			imprimirNombre(hdc, 40, 40, hbrA);
			
			TextOut (hdc, 390, 8, "TETRIX.C", 8);
			TextOut (hdc, 860, 8, "SIGUIENTE ELEMENTO:", 18);
			TextOut (hdc, 885, 260, "ESTADISTICAS:", 12);
			
			
			
			if(renElim < 5){
				TextOut(hdc, 900, 300, "Nivel 0", 7);
			}
			if(renElim >= 5){
				TextOut(hdc, 900, 300, "Nivel 1", 7);
			}
			if(renElim >= 10){
				TextOut(hdc, 900, 300, "Nivel 2", 7);
			}
			if(renElim >= 15){
				TextOut(hdc, 900, 300, "Nivel 3", 7);
			}
			if(renElim  >= 20){
				TextOut(hdc, 900, 300, "Nivel 4", 7);
			}
			if(renElim  >= 25){
				TextOut(hdc, 900, 300, "Nivel 5", 7);
			}
			
			
			
		    trazarElemento(hdc, 40, 40, elem_actual, hbrArre );
		    trazarElemento(hdc, 540, 60, elem_proximo, hbrArre);
			trazartablero(hdc, 40, 40, tablero, hbrArre[3], hpenB);
			
		//	DrawIconEx (hdc, 0, 60, LoadIcon (hInstance, "tetris(2"), 0, 0, 0, NULL, DI_NORMAL);
		
			EndPaint(hwnd, &ps);
			break;
		}
		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: {
			DeleteObject(hpenB);
			DeleteObject(hbrA);
			KillTimer(hwnd, IDT_TIMER1);
			
			if(hbrArre != NULL){
				for(i = 0; i < 11; i++){
					DeleteObject(hbrArre[i]);
				}
				free(hbrArre);
			}
			
			PostQuitMessage(0);
			break;
		}
		
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}


/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+2);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, "tetris"); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(hInstance, "tetris"); /* use the name "A" to use the project icon */
	wc.lpszMenuName = "MAINMENU";

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","TetrisV2.3.0",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		1080, /* width */
		760, /* height */
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}

//IMPLEMENTACION DE FUNCIONES //

void trazarcuadrito(HDC hdc, int x, int y, int i, int j, HBRUSH hbr)
{
	RECT r;
	
	r.left = x+TAMC*j;
	r.top = y+TAMC*i;
	r.right = x+TAMC+TAMC*j;
	r.bottom = y+TAMC+TAMC*i;
	
	FillRect(hdc, &r, hbr);
}

void trazartablero(HDC hdc, int x, int y, int tab[NRTA][NCTA], HBRUSH hbr, HPEN hpenR)
{
	HPEN lapiz;
	int i = 0, j = 0;
	
	for(i = 0; i < NRTA;i++){
		for(j = 0; j < NCTA;j++){
			if(tab[i][j]){
				trazarcuadrito(hdc, x, y, i, j, hbr);
			}
		}
	}
	
	lapiz =  SelectObject(hdc, hpenR);
	
	for(j = 0; j <= NCTA;j++){
		MoveToEx(hdc, x+TAMC*j, y, NULL);
		LineTo(hdc, x+TAMC*j, y+NRTA*TAMC);
	}
	 
	for(i = 0; i <= NRTA;i ++){ // NCTA en lugar de NRTA, funciona igual ??
		MoveToEx(hdc, x, y+TAMC*i, NULL);
		LineTo(hdc, x+NCTA*TAMC, y+TAMC*i);
	}
	
	SelectObject(hdc, lapiz);
	
}

void inicializartablero(int tab[NRTA][NCTA])
{
	int i = 0, j = 0;
	
	for(i = 0; i < NRTA;i++){
		for(j = 0; j < NCTA;j++){
			tab[i][j] = 0;
		}
	}
}

void trazarElemento(HDC hdc, int x, int y, STELEMENTO elem, HBRUSH *hbr)
{
	int i = 0, j = 0;

	for( i = 0; i < NRET;i++){
		for(j = 0; j < NCET;j++){
			if(elem.arre[i][j]){ // 1 TRUE 0 FALSE
			trazarcuadrito(hdc, x, y, elem.ren+i, elem.col+j, *hbr);
			}
		}
	}
}

void nuevoElemento(STELEMENTO *elem)
{
	int i = 0, j = 0;
	int ale = rand()%10; 
	
	elem->ren = Elementos[ale].ren;
	elem->col = NCTA/2-NCET/2;
	
	for(i = 0; i < NRET;i++){
		for(j = 0; j < NCET;j++){
			elem->arre[i][j] = Elementos[ale].arre[i][j];
		}
	}
}

int moverElemento(int tab[NRTA][NCTA], STELEMENTO *elem, int dir)
{
	if(!validarMovimiento(tab, *elem, dir)){
		// PlaySound("sound - impacto.wav", NULL, SND_FILENAME|SND_ASYNC); // SONIDO PARA EL CONTACTO CON UNA PIEZA
		return 0;
	}
	switch(dir)
	{
		case IZQ:{
			elem->col--;
			break;
		}
		case DER:{
			elem->col++;
			break;
		}
		case ABJ:{
			elem->ren++;
			break;
		}
	}
	return 1;
}

int validarMovimiento(int tab[NRTA][NCTA], STELEMENTO elem, int dir)
{
	int dder = 0;
	int dabj = 0;
	int i = 0, j = 0;
	
	switch(dir)
	{
		case IZQ:{
			if(elem.col+primer_Col(elem) <= 0)
			{
				return 0;	
			}
			elem.col--;
			break;
		}
		case DER:{
			dder  = NCET-1-ultima_Col(elem);
			if(elem.col-dder >= NCTA-NCET){
				return 0;
			}
			elem.col++;
			break;
		}
		case ABJ:{
			dabj = NRET-1-ultimo_Ren(elem);
			if(elem.ren-dabj >= NRTA-NRET){
				return 0;
			}
			elem.ren++;
			break;
		}
	}
	
	for(i  = 0; i < NRET; i++){
		for(j  = 0; j < NCET; j++){
			if(tab[i+elem.ren][j+elem.col] && elem.arre[i][j]){
				return 0;
			}
		}
	}
	
	return 1;
}

int primer_Col(STELEMENTO elem)
{
	int i = 0, j = 0;
	
	for(j  = 0; j < NCET;j++){
		for(i  = 0; i < NRET;i++){
			if(elem.arre[i][j]){
				return j;
			}
		}
	}
	return 0;
}

int ultima_Col(STELEMENTO elem)
{
	int i = 0, j= 0;
	
	for(j  = NCET-1; j >= 0 ;j--){
		for(i  = 0; i < NRET;i++){
			if(elem.arre[i][j]){
				return j;
			}
		}
	}
	return 0;
}

int ultimo_Ren(STELEMENTO elem)
{
	int i = 0, j = 0;
	
	for(i = NRET-1; i >= 0 ;i--){
		for(j = 0; j < NCET;j++){
			if(elem.arre[i][j]){
				return i;
		}
	}
}
	return 0;
}

void asignar_Elemento(STELEMENTO elem,  int tab[NRTA][NCTA])
{
	int i = 0, j = 0;
	
		for(i = 0; i < NRET ; i++){
			for(j = 0; j < NCET; j++){
				if(elem.arre[i][j]){
					PlaySound("sound - impacto.wav", NULL, SND_FILENAME|SND_ASYNC); //  SONIDO PARA CUANDO ENTRE EN CONTACTO CON ALGO 
					tab[elem.ren+i][elem.col+j] = elem.arre[i][j];
			}
		}
	}
}

int eliminarRenglones(int tab[NRTA][NCTA])
{
	int i = 0, j = 0;
	
	i = NRTA-1;
	while(rencondatos(tab, i)){
		if(renlleno(tab, i)){		
			PlaySound("sound - destroyblock.wav", NULL, SND_FILENAME|SND_ASYNC); // SONIDO PARA CUANDO SE COMPLETA UNA LINEA //
			eliminarRen(tab, i);
			i++;
			renElim++;
		}
		i--; 
		if(i < 0)
		return 0;
	}
	return 1;
}

int rencondatos(int tab[NRTA][NCTA], int ren)
{
	int j = 0;
	
	for(j = 0; j < NCTA; j++){
		if(tab[ren][j]){
			return 1;
		}
	}
	return 0;
}

int renlleno(int tab[NRTA][NCTA], int ren)
{
	int j = 0, cont = 0;
	
		for(j = 0; j < NCTA; j++){
		if(tab[ren][j]){
			cont++;
		}
	}
	if(cont == NCTA){
		return 1;
	}
	return 0;
}

int eliminarRen(int tab[NRTA][NCTA], int ren)
{
	int i = 0,j = 0;
	
	i = ren;
	
	do{
		for(j = 0; j < NCTA; j++){
			tab[i][j] = tab[i-1][j];
		}
		i--;
	}while(i > 0 && rencondatos(tab, i));
	if(i >= 0){
		for(j = 0; j < NCTA; j++){
			tab[i][j] = 0;
		}
	}
}

STELEMENTO transponerElemento(STELEMENTO elem)
{
	STELEMENTO elemT = elem;
	STELEMENTO elemA = elemT;

	int i = 0,j = 0;
	
	for(i = 0; i < NRET; i++){
		for(j = 0; j < NCET;j++){
			elemT.arre[j][i] = elem.arre[i][j];
		}
	}
	for(i = 0; i < NRET; i++){
		for(j = 0; j < NCET;j++){
			elemT.arre[j][i] = elem.arre[i][j];
		}
	}
    for (i = 0; i < NRET; i++){
        for (j = 0; j < NCET; j++){
            elemA.arre[j][NRET-i-1] = elem.arre[i][j];
        }
    }
    for (i = 0; i < NRET; i++){
        for (j = 0; j < NCET; j++){
            elemT.arre[i][j] = elemA.arre[i][j];
        }
    }

	if(elemT.col < 0){
		elemT.col = 0;
	}else if(elemT.col+NCET >= NCTA){
		elemT.col = NCTA-NCET;
	}
	
	if(elemT.ren < 0){
		elemT.ren = 0;
	}else if(elemT.ren+NRET >= NRTA){
		elemT.ren = NRTA-NRET;
	}

	return elemT;
}

HBRUSH *crearBrochas(int tam)
{
	HBRUSH * arre;
	
	arre = (HBRUSH *) malloc(sizeof(HBRUSH)*11); // Aplicamos memoria dinamica para reservar el espacio que utilizaremos
	
	if(arre == NULL){
		MessageBox(NULL, "No hay espacio en memoria para las brochas", "ERROR", MB_OK | MB_ICONERROR);
		return NULL;
	}
	
	arre[0] = CreateSolidBrush(RGB(216, 191, 216));  //  thistle //
	arre[1] = CreateSolidBrush(RGB(135, 206, 235));  //  skyblue //
 	arre[2] = CreateSolidBrush(RGB(255, 218, 185));  //  peachpuff //
	arre[3] = CreateSolidBrush(RGB(143, 188, 143));  //  darkseagreen //
	arre[4] = CreateSolidBrush(RGB(238, 232, 170));  //  palegoldenroad //
	arre[5] = CreateSolidBrush(RGB(255, 255, 224));  //  lightyellow //
	arre[6] = CreateSolidBrush(RGB(255, 160, 122));  //  lightsalmon //
	arre[7] = CreateSolidBrush(RGB(230, 230, 250));  //  lavender //
	arre[8] = CreateSolidBrush(RGB(245, 245, 220));  //  beige //
	arre[9] = CreateSolidBrush(RGB(102,  0 , 102));  //  honeydew //
	arre[10]= CreateSolidBrush(RGB(210,  180, 140)); // tan
	
	return arre;
}

void iniciBrush(HBRUSH *hbrArre)
{
	int i = 0;
	
	for ( i = 0; i < 10; i++){
		Elementos[i].hbrElem = hbrArre[i];
	}
}

void trazarNombre(HDC hdc, int x, int y, HBRUSH hbrA)
{
	RECT r;
	
	r.left = x;
	r.top = y;
	r.right = x+TAMC;
	r.bottom = y+TAMC;
	
	FillRect(hdc, &r, hbrA);
}

void imprimirNombre(HDC hdc, int x, int y, HBRUSH hbrA)
{
	// LETRA H  //
	trazarNombre(hdc, 80 , 120, hbrA); trazarNombre(hdc,  80, 160, hbrA); trazarNombre(hdc,  80, 200, hbrA); trazarNombre(hdc,  80, 240, hbrA); trazarNombre(hdc,  80, 280, hbrA);
	trazarNombre(hdc, 160, 120, hbrA); trazarNombre(hdc, 160, 160, hbrA); trazarNombre(hdc, 160, 200, hbrA); trazarNombre(hdc, 160, 240, hbrA); trazarNombre(hdc, 160, 280, hbrA);
	trazarNombre(hdc, 120, 200, hbrA);
	
	// LETRA R //
	trazarNombre(hdc, 320, 120, hbrA); trazarNombre(hdc, 320, 160, hbrA); trazarNombre(hdc, 320, 200, hbrA); trazarNombre(hdc, 320, 240, hbrA); trazarNombre(hdc, 320, 280, hbrA);
	trazarNombre(hdc, 360, 120, hbrA); trazarNombre(hdc, 400, 160, hbrA); trazarNombre(hdc, 360, 200, hbrA); trazarNombre(hdc, 400, 240, hbrA); trazarNombre(hdc, 400, 280, hbrA);
	trazarNombre(hdc, 400, 120, hbrA);
	// LETRA G //
	trazarNombre(hdc, 560, 120, hbrA); trazarNombre(hdc, 560, 160, hbrA); trazarNombre(hdc, 560, 200, hbrA); trazarNombre(hdc, 560, 240, hbrA); trazarNombre(hdc, 560, 280, hbrA);
	trazarNombre(hdc, 600, 280, hbrA); trazarNombre(hdc, 640, 280, hbrA); trazarNombre(hdc, 640, 200, hbrA); trazarNombre(hdc, 680, 200, hbrA); trazarNombre(hdc, 680, 240, hbrA);
	trazarNombre(hdc, 600, 120, hbrA); trazarNombre(hdc, 640, 120, hbrA); trazarNombre(hdc, 680, 280, hbrA);
}
