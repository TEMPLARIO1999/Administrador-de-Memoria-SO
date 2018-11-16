#include <iostream>
using namespace std;
#include <time.h>      //Libreria inicial para el tiempo
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>   //Libreria grafica
#include <SDL/SDL_image.h>   //Libreria de imagenes
#include <SDL/SDL_ttf.h>   //Libreria de texto
#include <pthread.h>
#include <windows.h>
#include <conio.h>

string stats[3] = {"Activo\0", "Detenido\0","En espera\0"};  //Estados posibles de procesos
bool encendido = true;
int PC=0;                //Contador de procesos
const int MemMax=100;    //Memoria en kilobytes
int typeOrg = 0;
int ajuste = 0;

class Process {          //Clase tipo proceso, utilizada para procesos en ejecucion y en espera
	public:
		//Implementacion con Lista doblemente ligada simple
		Process *sig;
		Process *ant;
		//Variables de un proceso, memoria,ID, Estado(activo,detenido,en espera),tiempo requerido de procesamiento
		int memoria;
		int PID;
		int status;   //0 = En ejecucion    1 = detenido
		int tiempo;
		Process(int memoria, int PID, int status, int tiempo) { //Constructor
			sig=ant=NULL;
			this->memoria = memoria;
			this->PID = PID;
			this->status = status;
			this->tiempo = tiempo;
		}
		~Process() {
			delete sig;
			delete ant;
		}   //Destructor
};

class ListProc {                       //Clase lista doblemente ligada
	private:
		Process *raiz;                 //Raiz
		int cont;
	public:
		ListProc(Process *_raiz) {
			raiz = _raiz;
			cont=0;
		}
		bool Push(Process *proc) {
			Process *temp=raiz;
			if(raiz==NULL) raiz=proc;
			else {
				while(temp->sig) temp=temp->sig;
				temp->sig=proc;
			}
		}
		Process * PopTop() {
			Process *temp;
			temp=raiz;
			raiz=raiz->sig;
			cont--;
			return temp;
		}
		Process * GetRaiz() {
			Process *temp=raiz;
			return temp;
		}
		void push_before(Process *proc, int prev) {
			Process *nuevo = proc;
			Process *temp = raiz;
			while (temp->sig!=NULL && temp->memoria!=prev) {
				temp=temp->sig;
			}
			if(temp->memoria==prev && temp==raiz) {
				temp->ant=nuevo;
				nuevo->sig=temp;
				raiz=nuevo;
			} else if(temp->memoria==prev) {
				nuevo->ant=temp->ant;
				nuevo->sig=temp;
				temp->ant=nuevo;
				(nuevo->ant)->sig=nuevo;
			} else if(temp->sig==NULL) {
				temp->sig=nuevo;
				nuevo->ant=temp;
			}
		}
		void rest_sec() {
			Process *temp = raiz;
			while ( temp != NULL ) {
				if(temp->status==0) (temp->tiempo)--;
				temp = temp->sig ;
			}
			return;
		}
		void quit_proc() {
			Process *temp = raiz, *ant=NULL;
			while ( temp != NULL ) {
				ant = temp;
				temp = temp->sig ;
				if((ant->tiempo <= 0) && ant->status==0) ant->PID=0;
			}
			return;
		}
		int pop(int val) {
			Process *temp = raiz;
			while(temp!=NULL && temp->memoria!=val) temp = temp->sig;
			if(temp == raiz) raiz = raiz->sig;
			else if ( temp->sig == NULL ) {
				temp->ant->sig = NULL;
				return 0;
			} else if (temp != NULL) {
				temp->ant->sig = temp->sig;
				temp->sig->ant = temp->ant;
			}
			int x = temp->memoria;
			return x;
		}
		bool Primer_Ajuste(Process *proc) {
			Process *temp=raiz;
			while(temp) {
				if(temp->memoria>=proc->memoria) {
					if(temp!=NULL && temp->PID==0) {
						int aux=(temp->memoria)-(proc->memoria);
						proc->status=0;
						push_before(proc,temp->memoria);
						Process *paux=new Process(aux,0,0,0);
						if(temp->memoria!=proc->memoria) push_before(paux,proc->memoria);
						pop(temp->memoria);
						cont++;
						return true;
					}
				}
				temp=temp->sig;
			}
			return false;
		}
		bool Peor_Ajuste(Process *proc) {
			Process *temp=raiz;
			int max=0;
			while(temp) {
				if(max<=((temp->memoria)-(proc->memoria))) {
					if(temp->PID==0) max=(temp->memoria)-(proc->memoria);
				}
				temp=temp->sig;
			}
			temp=raiz;
			while(temp) {
				if(temp->memoria==max+proc->memoria) {
					if(temp!=NULL && temp->PID==0) {
						proc->status=0;
						push_before(proc,temp->memoria);
						Process *paux=new Process(max,0,0,0);
						if(temp->memoria!=proc->memoria) push_before(paux,proc->memoria);
						pop(temp->memoria);
						cont++;
						return true;
					}
				}
				temp=temp->sig;
			}
			return false;
		}
		bool Mejor_Ajuste(Process *proc) {
			Process *temp=raiz;
			int max=100;
			while(temp) {
				if(max>=((temp->memoria)-(proc->memoria))) {
					if(temp->PID==0)
						if((temp->memoria)-(proc->memoria)>=0) {
							//se cancela todo :v
							max=(temp->memoria)-(proc->memoria);
						}

				}
				temp=temp->sig;
			}
			temp=raiz;
			while(temp) {
				if(temp->memoria==max+proc->memoria) {
					if(temp!=NULL && temp->PID==0) {
						proc->status=0;
						push_before(proc,temp->memoria);
						Process *paux=new Process(max,0,0,0);
						if(temp->memoria!=proc->memoria) push_before(paux,proc->memoria);
						pop(temp->memoria);
						cont++;
						return true;
					}
				}
				temp=temp->sig;
			}
			return false;
		}
		void PageMem() {
			Process *temp=raiz;;
			while(temp->sig) {
				if(temp->PID==0 && (temp->sig)->PID==0) {
					(temp->sig)->memoria=temp->memoria+(temp->sig)->memoria;
					pop(temp->memoria);
				}
				temp=temp->sig;
			}
		}
		void DetenProc(int id) {
			Process *temp=raiz;
			if(id!=0) {
				while(temp!=NULL && temp->PID!=id) temp=temp->sig;
				if(temp==NULL)return;
				else if(temp->PID==id) {
					if(temp->status==0) temp->status=1;
					else if(temp->status==1) temp->status=0;
				}
			}
		}
		void ElimProc(int id) {
			Process *temp=raiz;
			if(id!=0) {
				while(temp!=NULL && temp->PID!=id) temp=temp->sig;
				if(temp==NULL) return;
				else if(temp->PID==id) temp->PID=0;
			}
		}

};

Process *New_Process() {                //Funcion que crea un nuevo proceso de manera aleatoria
	int memoria,tiempo,status;
	memoria = (rand()%20)+5;
	memoria+=(memoria%2);
	tiempo = (rand()%10)+5;
	status = rand()%2;
	Process *New_proc=new Process(memoria,++PC,status,tiempo);
	return New_proc;
}

Process *ejec=new Process(MemMax,0,0,0);  //Proceso inicial con full memoria(raiz)
ListProc proc_ejec(ejec);                 //Objeto con ejec como raiz
Process *Esp=NULL,*New_proc;              //Proceso inicial a NULL(raiz)
ListProc proc_esp(Esp);                  //Objeto con esp como raiz

void *ManageProcess(void *threadid) {
	long *tid;
	tid = (long*)threadid;
	bool (ListProc::*administradores[3])(Process*) = {&ListProc::Mejor_Ajuste, &ListProc::Primer_Ajuste, &ListProc::Peor_Ajuste};
	srand(time(NULL));
	int num_process=10;                     //No. De procesos de inicio
	for(int i=0; i<num_process; i++) {             //Carga procesos iniciales
		New_proc=New_Process();
		proc_esp.Push(New_proc);
	}
	do {
		New_proc=proc_esp.PopTop();
		while(!(proc_ejec.*administradores[ajuste])(New_proc)) {
			Sleep(2000);
			proc_ejec.rest_sec();
			proc_ejec.quit_proc();
			proc_ejec.PageMem();
		}
		Sleep(2000);
		proc_ejec.rest_sec();
		proc_ejec.quit_proc();
		proc_ejec.PageMem();
		if(encendido) New_proc=New_Process();
		else New_proc=new Process(1,0,0,0);
		proc_esp.Push(New_proc);
	} while(proc_ejec.GetRaiz()->memoria!=MemMax);
	pthread_exit(NULL);
}

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int SCREEN_BPP = 32;
SDL_Surface *background = NULL;
SDL_Surface *ma = NULL;
SDL_Surface *pa = NULL;
SDL_Surface *pea = NULL;
SDL_Surface *message = NULL;
SDL_Surface *p_esp = NULL;
SDL_Surface *p_ejec = NULL;
SDL_Surface *mem_libre = NULL;
SDL_Surface *screen = NULL;
SDL_Event event;
TTF_Font *font = NULL;
SDL_Color white = {255, 255, 255};
SDL_Color red = {255, 0, 0};

SDL_Surface *load_image( std::string filename ) {
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;
	loadedImage = IMG_Load( filename.c_str() );
	if( loadedImage != NULL ) {
		optimizedImage = SDL_DisplayFormat( loadedImage );
		SDL_FreeSurface( loadedImage );
		if( optimizedImage != NULL ) {
			SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
		}
	}
	return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL ) {
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface( source, clip, destination, &offset );
}

bool init() {
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) return false;
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
	if( screen == NULL ) return false;
	if( TTF_Init() == -1 ) return false;
	SDL_WM_SetCaption( "Administrador de memoria SO", NULL );
	return true;
}

bool load_files() {
	background = ma = load_image( "imagenes/fondos/ma.bmp" );
	pa = load_image( "imagenes/fondos/pa.bmp" );
	pea = load_image( "imagenes/fondos/pea.bmp" );
	font = TTF_OpenFont( "openS.ttf", 15);
	if( !background or !pa or !pea or !ma) return false;
	if( !font ) return false;
	return true;
}

void clean_up() {
	SDL_FreeSurface( background );
	SDL_FreeSurface( message );
	TTF_CloseFont( font );
	TTF_Quit();
	SDL_Quit();
}

void imprimir_rectangulo (SDL_Surface *screen, int x, int y, SDL_Color color, int prof) {
	SDL_Rect rect = {x, y, 100, prof*6};
	Uint32 a = SDL_MapRGB(screen->format, color.r, color.g, color.b);
	SDL_FillRect(screen, &rect, a);
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
	a = SDL_MapRGB(screen->format, 0, 0, 1);
	rect = {x+1, y+1, 100, 1};
	SDL_FillRect(screen, &rect, a);
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
}

void apply() {
	int x=1100, y=60, pos, m_libre=100;
	Process *tempEjec = proc_ejec.GetRaiz();
	Process *tempEsp = NULL;
	string new_process, process_espera, process_ejec, memoria_libre;
	apply_surface( 0, 0, background, screen);

	while (tempEjec!=NULL) {
		if(tempEjec->PID != 0) imprimir_rectangulo(screen, x, y, red, tempEjec->memoria);
		else imprimir_rectangulo(screen, x, y, white, tempEjec->memoria);
		y+=(6*tempEjec->memoria+1);
		tempEjec=tempEjec->sig;
	}
	tempEsp = proc_esp.GetRaiz();
	tempEjec = proc_ejec.GetRaiz();
	if(New_proc->PID!=0){
		new_process =  "Próximo Proceso: PID "+to_string(New_proc->PID)+" MEMORIA "+ to_string(New_proc->memoria) + "KB TIEMPO " + to_string(New_proc->tiempo)+" SEG ";
		message = TTF_RenderText_Solid( font, new_process.c_str(), white );
	}
	
	int pos_x_m1 = 50, pos_x_m2 = 570, pos_y_m1 = 120, pos_y_m2 = 120;
	while ( tempEsp != NULL && tempEjec!=NULL){
		if(tempEsp->PID!=0 && tempEsp!=NULL){
			process_espera = " PID "+ to_string(tempEsp->PID) +" MEMORIA "+to_string(tempEsp->memoria)+"KB TIEMPO "+to_string(tempEsp->tiempo)+" SEG ";
			p_esp = TTF_RenderText_Solid( font, process_espera.c_str(), white );
			pos_y_m1+=25;
			apply_surface(pos_x_m1, pos_y_m1, p_esp, screen );
		}
		if(tempEjec->PID!=0)
			if(tempEjec!=NULL){
				process_ejec = " PID "+ to_string(tempEjec->PID) +" MEMORIA "+to_string(tempEjec->memoria)+"KB TIEMPO "+to_string(tempEjec->tiempo)+" SEG ";
				p_ejec = TTF_RenderText_Solid( font, process_ejec.c_str(), white );
				pos_y_m2+=25;
				apply_surface(pos_x_m2, pos_y_m1, p_ejec, screen );
				m_libre-=tempEjec->memoria;
			}
		tempEsp=tempEsp->sig;
		tempEjec=tempEjec->sig;
	}
	memoria_libre = "Memoria libre: "+to_string(m_libre)+" KB";
	mem_libre = TTF_RenderText_Solid( font, memoria_libre.c_str(), white );
	apply_surface(600, 30, message, screen );
	apply_surface(850, 660, mem_libre, screen);
	SDL_Flip(screen);
	Sleep(1000);
}

void *ManagePrint(void *threadid) {
	long *tid;
	tid = (long*)threadid;
	bool quit = false;
	if( init() == false ) {
		SDL_Quit();
	}
	if( load_files() == false ) {
		SDL_Quit();
	}
	while(SDL_PollEvent(&event) >= 0) {
		apply();
	}
	pthread_exit(NULL);
}

void *ManageInterruptions(void *threadid) {
	long *tid;
	tid = (long*)threadid;
	while(1) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if( event.type == SDL_KEYDOWN ) {
				switch( event.key.keysym.sym ) {
					case SDLK_F1: {
						ajuste=0;
						background=ma;
						apply();
						break;
					}
					case SDLK_F2: {
						ajuste=1;
						background=pa;
						apply();
						break;
					}
					case SDLK_F3: {
						ajuste=2;
						background=pea;
						apply();
						break;
					}
				}
			}
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	pthread_t manejar_procesos;
	pthread_create(&manejar_procesos, NULL, ManageProcess, (void *)0);
	pthread_t manejar_tiempo;
	pthread_create(&manejar_tiempo, NULL, ManageInterruptions, (void *)1);
	pthread_t manejar_impresion;
	pthread_create(&manejar_impresion, NULL, ManagePrint, (void *)2);
	pthread_exit(NULL);
	return 0;
}
