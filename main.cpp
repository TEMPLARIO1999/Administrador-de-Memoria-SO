#include <iostream>
using namespace std;
#include <time.h>      //Libreria inicial para el tiempo
#include <stdlib.h>
//#include <SDL/SDL.h>   //Libreria grafica
#include <pthread.h>
#include <windows.h>

string stats[3] = {"En espera\0", "Activo\0", "Detenido\0"};  //Estados posibles de procesos
int PC=0;                //Contador de procesos
const int MemMax=100;   //Memoria en kilobytes

class Process{           //Clase tipo proceso, utilizada para procesos en ejecucion y en espera
	public:
		//Implementacion con Lista doblemente ligada simple
		Process *sig;
		Process *ant;
		//Variables de un proceso, memoria,ID, Estado(activo,detenido,en espera),tiempo requerido de procesamiento
		int memoria;
		int PID;
		int status;
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

class ListProcE {
	private:
		Process *raiz;
		int cont;
	public:
		ListProcE(Process *_raiz){
			cont=0;
			raiz = _raiz;
		}
		bool Push(Process *proc){
			Process *temp=raiz;
			if(cont<20){
				if(raiz==NULL) raiz=proc;
				else{
					while(temp->sig) temp=temp->sig;
					temp->sig=proc;
				}
				cont++;
				return true;
			}else{
				return false;
			}
		}
		Process * Pop(){
			Process *temp;
			if(cont>0){
				temp=raiz;
				raiz=raiz->sig;
				cont--;
				return temp;
			}else{
				return temp;
			}
		}
		void to_show(){
			Process *temp = raiz;
			while ( temp != NULL ){
				cout << temp->PID <<" : "<< temp->memoria <<endl;
				temp = temp->sig ;
			}
			cout << "null" << endl;
			return;
		}
		Process * Top(){
			return raiz;
		}
};

class ListProc {                       //Clase lista doblemente ligada
	private:
		Process *raiz;                 //Raiz
		int cont=0;
	public:
		ListProc(Process *_raiz) {
			raiz = _raiz;
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
				cout << temp->PID <<" : "<< temp->memoria << " tiempo : " << temp->tiempo << " seg "<<endl;
				temp = temp->sig ;
			}
			cout << "null" << endl;
			return;
		}
		void rest_sec(){
			Process *temp = raiz;
			while ( temp != NULL ){
				(temp->tiempo)--;
				temp = temp->sig ;
			}	
			return;
		}
		void quit_proc(){
			Process *temp = raiz, *ant=NULL;
			while ( temp != NULL ){
				ant = temp;
				temp = temp->sig ;
				if(ant->tiempo <= 0) this->pop(ant->memoria);
			}	
			return;
		}
		int pop(int val) {
			Process *temp = raiz;
			while(temp!=NULL && temp->memoria!=val) temp = temp->sig;
			if(temp == raiz) raiz = raiz->sig;
			else if ( temp->sig == NULL ) {
				if (temp->memoria == val ) temp->ant->sig = NULL;
				else return 0;
			} else if (temp != NULL) {
				temp->ant->sig = temp->sig;
				temp->sig->ant = temp->ant;
			} 
			int x = temp->memoria;
			return x;
		}
		bool Primer_Ajuste(Process *proc){
			Process *temp=raiz;
			while(temp->memoria<proc->memoria){
				temp=temp->sig;
			}
			if(temp->memoria>proc->memoria){
				if(temp!=NULL){
					int aux=(temp->memoria)-(proc->memoria); 
					push_before(proc,temp->memoria);
					Process *paux=new Process(aux,0,0,0);
					push_before(paux,proc->memoria);
					pop(temp->memoria);
				}
				cont++;
				return true;
			}else return false;
		}
};

Process *New_Process(){                 //Funcion que crea un nuevo proceso de manera aleatoria                              
	int memoria,tiempo,status;
	memoria = (rand()%20)+1; 
	memoria+=(memoria%2); 
	tiempo = (rand()%15)+5;
	status = rand()%2;
	Process *New_proc=new Process(memoria,++PC,status,tiempo);
	return New_proc;
}

void *ManageProcess(void *threadid) {
    long tid;
    tid = (long)threadid;
    
    
    Process *ejec=new Process(MemMax,0,0,0);  //Proceso inicial con full memoria(raiz)
	ListProc proc_ejec(ejec);                 //Objeto con ejec como raiz
	Process *Esp=NULL,*New_proc;              //Proceso inicial a NULL(raiz)
	ListProcE proc_esp(Esp);                  //Objeto con esp como raiz
	srand(time(NULL));
	int num_process=20;                     //No. De procesos de inicio
	for(int i=0; i<num_process; i++){              //Carga procesos iniciales
		New_proc=New_Process();
		proc_esp.Push(New_proc);
	}
	int i=1;
	do{
		New_proc=proc_esp.Pop();  //Obtiene un proceso en espera
		while(!proc_ejec.Primer_Ajuste(New_proc)){
			Sleep(5000);
			proc_ejec.rest_sec();
			proc_ejec.quit_proc();
			proc_ejec.to_show();
		}
		cout<<i++;
		New_proc=New_Process();
		proc_esp.Push(New_proc);
	}while(1);
	/*while(i<num_process){
		New_proc=proc_esp.Pop();
		if(!proc_ejec.Primer_Ajuste(New_proc)) break;
		i++;
	}*/
	cout<<endl;
	proc_esp.to_show();
	proc_ejec.to_show();
	
	
    pthread_exit(NULL);
}

void *ManageTime(void *threadid) {
    long tid;
    tid = (long)threadid;
	
	while(1){
		cout << "Mientras la otra funcion maneja los procesos, yo puedo manejar las interrupciones.\n\n";
		Sleep(10200);
	}
	
    pthread_exit(NULL);
}

int main(){
	pthread_t manejar_procesos;
	pthread_create(&manejar_procesos, NULL, ManageProcess, (void *)0);
	pthread_t manejar_tiempo;
	pthread_create(&manejar_tiempo, NULL, ManageTime, (void *)1);
	pthread_exit(NULL);
	return 0;
}

