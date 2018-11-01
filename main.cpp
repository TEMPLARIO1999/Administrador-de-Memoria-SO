

/*Hice un cambio :v borre el codigo*/











































#include <iostream>
using namespace std;
#include <time.h>
#include <stdlib.h>

string stats[3] = {"En espera\0", "Activo\0", "Detenido\0"};
int PC=0;

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
