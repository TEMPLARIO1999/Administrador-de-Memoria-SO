#include <iostream>
using namespace std;
#include <time.h>      //Libreria inicial para el tiempo
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>   //Libreria grafica
#include <pthread.h>
#include <conio.h>
#include <windows.h>

string stats[3] = {"Activo\0", "Detenido\0","En espera\0"};  //Estados posibles de procesos
bool encendido = true;
int PC=0;                //Contador de procesos
const int MemMax=100;    //Memoria en kilobytes
int typeOrg = 0;
int ajuste = 2;

class Process{           //Clase tipo proceso, utilizada para procesos en ejecucion y en espera
	public:
		//Implementacion con Lista doblemente ligada simple
		Process *sig;
		Process *ant;
		//Variables de un proceso, memoria,ID, Estado(activo,detenido,en espera),tiempo requerido de procesamiento
		int memoria;
		int PID;
		int status;   //0 = En ejecucion    1 = detenido    
		int tiempo;  
		Process(int memoria, int PID, int status, int tiempo){ //Constructor
			sig=ant=NULL;
			this->memoria = memoria;
			this->PID = PID;
			this->status = status;
			this->tiempo = tiempo;
		}
		~Process(){
			delete sig; delete ant;
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
		bool Push(Process *proc){
			Process *temp=raiz;
			if(raiz==NULL) raiz=proc;
			else{
				while(temp->sig) temp=temp->sig;
				temp->sig=proc;
			}
		}
		Process * PopTop(){
			Process *temp;
			temp=raiz;
			raiz=raiz->sig;
			cont--;
			return temp;
		}
		Process * GetRaiz(){
			Process *temp=raiz;
			return temp;
		}
		void push_before(Process *proc, int prev){
			Process *nuevo = proc;
			Process *temp = raiz;
			while (temp->sig!=NULL && temp->memoria!=prev){
				temp=temp->sig;
			}
			if(temp->memoria==prev && temp==raiz){
				temp->ant=nuevo;
				nuevo->sig=temp;
				raiz=nuevo;
			}else if(temp->memoria==prev){
				nuevo->ant=temp->ant;
				nuevo->sig=temp;
				temp->ant=nuevo;
				(nuevo->ant)->sig=nuevo;
			}else if(temp->sig==NULL){
				temp->sig=nuevo;
				nuevo->ant=temp;
			}
		}
		void to_show(){
			Process *temp = raiz;
			while ( temp != NULL ){
				if(temp->PID!=0) cout <<" PID "<< temp->PID <<" MEMORIA "<< temp->memoria <<" TIEMPO "<<temp->tiempo<<" SEG "<<endl;
				else cout<<" ---------------- "<<temp->memoria<<" ---------------- "<<endl;
				temp = temp->sig ;
			}
			cout << endl;
			return;
		}
		void rest_sec(){
			Process *temp = raiz;
			while ( temp != NULL ){
				if(temp->status==0) (temp->tiempo)--;
				temp = temp->sig ;
			}	
			return;
		}
		void quit_proc(){
			Process *temp = raiz, *ant=NULL;
			while ( temp != NULL ){
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
			}else if (temp != NULL) {
				temp->ant->sig = temp->sig;
				temp->sig->ant = temp->ant;
			} 
			int x = temp->memoria;
			return x;
		}
		bool Primer_Ajuste(Process *proc){
			Process *temp=raiz;
			while(temp){
				if(temp->memoria>=proc->memoria){
					if(temp!=NULL && temp->PID==0){
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
		bool Peor_Ajuste(Process *proc){
			Process *temp=raiz; int max=0;
			while(temp){
				if(max<=((temp->memoria)-(proc->memoria))){
					if(temp->PID==0) max=(temp->memoria)-(proc->memoria);
				}
				temp=temp->sig;
			}
			temp=raiz; 
			while(temp){
				if(temp->memoria==max+proc->memoria){
					if(temp!=NULL && temp->PID==0){
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
						if((temp->memoria)-(proc->memoria)>=0){
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
		void PageMem(){
			Process *temp=raiz;;
			while(temp->sig){
				if(temp->PID==0 && (temp->sig)->PID==0){
					(temp->sig)->memoria=temp->memoria+(temp->sig)->memoria;
					pop(temp->memoria);
				}
				temp=temp->sig;
			}
		}
		void DetenProc(int id){
			Process *temp=raiz;
			if(id!=0){
				while(temp!=NULL && temp->PID!=id) temp=temp->sig;
				if(temp==NULL)return;
				else if(temp->PID==id){
					if(temp->status==0) temp->status=1;
					else if(temp->status==1) temp->status=0;
				}
			}
		}
		void ElimProc(int id){
			Process *temp=raiz;
			if(id!=0){
				while(temp!=NULL && temp->PID!=id) temp=temp->sig;
				if(temp==NULL) return;
				else if(temp->PID==id) temp->PID=0;
			}
		}
		void Interruption(){
			char tecla; int id=0;
			if(kbhit()){
				tecla = getch();
				if(tecla == 'd') {
					cin>>id;
					DetenProc(id);
				}
				else if(tecla == 'q') encendido = false;
				else if(tecla == 'e') {
					cin>>id;
					ElimProc(id);
				}
			}
		}
		
};

Process *New_Process(){                 //Funcion que crea un nuevo proceso de manera aleatoria                              
	int memoria,tiempo,status;
	memoria = (rand()%20)+1; 
	memoria+=(memoria%2); 
	tiempo = (rand()%3)+5;
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
	for(int i=0; i<num_process; i++){              //Carga procesos iniciales
		New_proc=New_Process();
		proc_esp.Push(New_proc);
	}
	do{
		New_proc=proc_esp.PopTop();  //Obtiene un proceso en espera
		//cout <<"process "<< New_proc->PID <<" : "<< New_proc->memoria << " tiempo : " << New_proc->tiempo << " seg "<<endl;
		while(!(proc_ejec.*administradores[ajuste])(New_proc)){
			Sleep(2000);
		//	system("cls");
			proc_ejec.rest_sec();
			proc_ejec.quit_proc();
			proc_ejec.PageMem();
		//	proc_ejec.to_show();
		//	cout <<"process "<< New_proc->PID <<" : "<< New_proc->memoria << " tiempo : " << New_proc->tiempo << " seg "<<endl;
		}
		Sleep(2000);
		//system("cls");
		proc_ejec.rest_sec();
		proc_ejec.quit_proc();
		proc_ejec.PageMem();
		//proc_ejec.to_show();
		if(encendido) New_proc=New_Process();
		else New_proc=new Process(1,0,0,0);
		proc_esp.Push(New_proc);
	}while(proc_ejec.GetRaiz()->memoria!=MemMax);
    pthread_exit(NULL);
}

void *ManagePrint(void *threadid){
	long *tid;
	tid = (long*)threadid;
	
	int pos_x=1100;
	int pos_y;
	int tam_h_px=6;
	int tam_w_px=150;
	int aux_tam;
	Process *tempEjec;
	
	SDL_Event event;
	SDL_Surface *screen;
	SDL_Surface *fondo = SDL_LoadBMP("task.bmp");
	screen = SDL_SetVideoMode(1280, 720, 32, SDL_SWSURFACE);
	SDL_BlitSurface(fondo, NULL, screen, NULL);
	SDL_WM_SetCaption("Administrador de memoria", NULL);
//	RECTANGULOS :v
	SDL_Rect rect;
	Uint32 a;
	while(SDL_WaitEvent(&event)){
		tempEjec = proc_ejec.GetRaiz();
		aux_tam = (tam_h_px*tempEjec->memoria/2);
		pos_y=60;
		while (tempEjec!=NULL){
			rect = {pos_x, pos_y, tam_w_px, aux_tam};
			if(tempEjec->PID!=0) a = SDL_MapRGB(screen->format, 255, 0, 0);
			else a = SDL_MapRGB(screen->format, 255, 255, 255);
			SDL_FillRect(screen, &rect, a);
			SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
			
			rect = {pos_x, pos_y+1, tam_w_px, 1};
			a = SDL_MapRGB(screen->format, 0, 0, 255);
			SDL_FillRect(screen, &rect, a);
			SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
			
			pos_y+=(tam_h_px*tempEjec->memoria);
			tempEjec=tempEjec->sig;
		}
		SDL_Delay(2000);
	}
	SDL_Quit();
	
	
//	while(1){
//		Process *tempEsp = proc_esp.GetRaiz();
//		Process *tempEjec = proc_ejec.GetRaiz();
//		if(New_proc->PID!=0) cout <<" NEXT PROCESS "<<" PID "<< New_proc->PID <<" MEMORIA "<< New_proc->memoria <<" TIEMPO "<<New_proc->tiempo<<" SEG "<<endl<<endl;
//		cout<<" PROCESOS EN ESPERA           \t\t           PROCESOS EN EJECUCION"<<endl<<endl;
//		while ( tempEsp != NULL && tempEjec!=NULL){
//			if(tempEsp->PID!=0 && tempEsp!=NULL) cout <<" PID "<< tempEsp->PID <<" MEMORIA "<< tempEsp->memoria <<" TIEMPO "<<tempEsp->tiempo<<" SEG ";
//			cout<<"\t\t";
//			if(tempEjec->PID!=0){
//				if(tempEjec!=NULL) cout <<" PID "<< tempEjec->PID <<" MEMORIA "<< tempEjec->memoria <<" TIEMPO "<<tempEjec->tiempo<<" SEG ";
//			}else{
//				if(tempEjec!=NULL) cout<<" ---------------- "<<tempEjec->memoria<<" ---------------- ";
//			}
//			cout<<"\t\t"<<endl;
//			tempEsp=tempEsp->sig;
//			tempEjec=tempEjec->sig;
//		}
//		cout<<endl<<endl;
//		cout<<"    D = PARAR/REANUDAR PROCESO    E = ELIMINAR PROCESO    Q = TERMINAR EJECUCION    "<<endl;
//	Sleep(2000);
//	system("cls");
//	}
	pthread_exit(NULL);
}

void *ManageInterruptions(void *threadid) {
    long *tid;
    tid = (long*)threadid;
    SDL_Event event;
    while(SDL_WaitEvent(&event)){
		if( event.type == SDL_KEYDOWN ){
			switch( event.key.keysym.sym ) {
                case SDLK_F1: ajuste=0; break;
                case SDLK_F2: ajuste=1; break;
                case SDLK_F3: ajuste=2; break;
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv){
	pthread_t manejar_procesos;
	pthread_create(&manejar_procesos, NULL, ManageProcess, (void *)0);
	pthread_t manejar_tiempo;
	pthread_create(&manejar_tiempo, NULL, ManageInterruptions, (void *)1);
	pthread_t manejar_impresion;
	pthread_create(&manejar_impresion, NULL, ManagePrint, (void *)2);
	pthread_exit(NULL);
	return 0;
}
