#include <iostream>
using namespace std;
#include <time.h>      //Libreria inicial para el tiempo
#include <stdlib.h>
//#include <SDL/SDL.h>   //Libreria grafica
#ifdef _OPENMP         //Libreria para utilizar procesamiento paralelo
 #include <omp.h>
 #define TRUE 1
 #define FALSE 0
#else
 #define omp_get_thread_num() 0
 #define omp_get_num_threads() 1
#endif

string stats[3] = {"En espera\0", "Activo\0", "Detenido\0"};  //Estados posibles de procesos
int PC=0;                //Contador de procesos
const int MemMax=10;   //Memoria en kilobytes

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

class ListProc {                       //Clase lista doblemente ligada
	private:
		Process *raiz;                 //Raiz
	public:
		ListProc(Process *_raiz) {
			raiz = _raiz;
		}
		void SetRaiz(Process *_raiz){
			raiz=_raiz;
		}
		void push_front(Process *proc){
			Process *nuevo=proc;
			if (raiz != NULL) {
				nuevo->sig = raiz;
				raiz->ant = nuevo;
			}
			raiz = nuevo;
			return;
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
				cout << temp->PID <<" : "<< temp->memoria <<endl;
				temp = temp->sig ;
			}
			cout << "null" << endl;
			return;
		}
		int pop_front(){
			Process *temp = NULL;
			if ( raiz != NULL ) {
				temp = raiz;
				raiz = raiz->sig;
			}
			int x = temp->PID;
			//delete temp;
			return x;	
		}
		int pop(int val) {
			Process *temp = raiz;
			while(temp!=NULL && temp->memoria!=val) temp = temp->sig;
			if(temp == raiz) raiz = raiz->sig;
			else if ( temp->sig == NULL ) {
				if (temp->PID == val ) temp->ant->sig = NULL;
				else return 0;
			} else if (temp != NULL) {
				temp->ant->sig = temp->sig;
				temp->sig->ant = temp->ant;
			} 
			int x = temp->memoria;
			return x;
		}
		void push_Back(Process *proc){
			Process *temp=raiz;
			if(raiz!=NULL){
				while(temp->sig){
					temp=temp->sig;
				}
				temp->sig=proc;
				proc->ant=temp;
			}else raiz=proc;
		}
		void Primer_Ajuste(Process *proc){
			Process *temp=raiz;
			while(temp!=NULL || !(temp->memoria>proc->memoria && temp->PID!=0)){
				temp=temp->sig;
			}
			if(temp!=NULL){
				int aux=(temp->memoria)-(proc->memoria);
				this->push_before(proc,temp->memoria);
				Process *paux=new Process(aux,0,0,0);
				this->push_before(paux,proc->memoria);
				this->pop(temp->memoria);
			}else if((temp->memoria>proc->memoria && temp->PID!=0)){
				
			}
		}
		Process *Top(){
			return raiz;
		}
};

Process *New_Process(){                 //Funcion que crea un nuevo proceso de manera aleatoria                              
	int memoria,tiempo,status;
	memoria = (rand()%30)+20; 
	memoria+=(memoria%2); 
	tiempo = (rand()%15)+5;
	status = rand()%2;
	Process *New_proc=new Process(memoria,++PC,status,tiempo);
	return New_proc;
}

int main(){
	Process *ejec=new Process(MemMax,0,0,0);  //Proceso inicial con full memoria(raiz)
	ListProc proc_ejec(ejec);                 //Objeto con ejec como raiz
	Process *Esp=NULL,*New_proc;              //Proceso inicial a NULL(raiz)
	ListProc proc_esp(Esp);                   //Objeto con esp como raiz
	srand(time(NULL));
	int num_process = (rand()%15)+15;              //No. De procesos de inicio
	for(int i=0; i<num_process; i++){              //Carga procesos iniciales
		New_proc=New_Process();
		proc_esp.push_Back(New_proc);
	}int i=0;
	proc_esp.to_show();
	while(i<num_process){
		New_proc=proc_esp.Top();
		cout<<New_proc->PID<<endl;
		proc_ejec.Primer_Ajuste(New_proc);
		proc_esp.pop_front();
		i++;
	}
	cout<<endl;
	proc_ejec.to_show();
	return 0;
}


/*Hice un cambio :v borre el codigo*/











































#include <iostream>
using namespace std;
#include <time.h>
#include <stdlib.h>

string stats[3] = {"En espera\0", "Activo\0", "Detenido\0"};
int PC=0;

//JAJAJAJAJ, ESTOY COMENTANDO EL CÓDIGO.

class Process{
	public:
	int memoria;
	int PID;
	int status;
	int tiempo;
		Process(int memoria, int PID, int status, int tiempo){
			this->memoria = memoria;
			this->PID = PID;
			this->status = status;
			this->tiempo = tiempo;
		}
		~Process(){}
};

int main(){
	srand(time(NULL));
	int new_process = (rand()%30)+15, memoria, status, tiempo;
	Process *arr_proc = new Process(0,0,0,0);
	Process *aux = arr_proc;
	for(int i=0; i<new_process; i++, arr_proc++){
		memoria = (rand()%300)+20;
		memoria+=(memoria%2);
		tiempo = (rand()%15)+5;
		status = rand()%2;
		arr_proc = new Process(memoria, ++PC, status, tiempo);
		cout << arr_proc->PID << " " << "Proceso " << i+1 << " " << arr_proc->memoria << "k " <<arr_proc->tiempo << " " << stats[arr_proc->status] << endl;
	}
	arr_proc = aux;
}
