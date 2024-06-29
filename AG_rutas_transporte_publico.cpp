
/* 
 * File:   main.cpp
 * Created on 24 de junio de 2024, 12:07 PM
 */
//el programa es un AG que calcula la ruta con menor trayecto que recorre todos los nodos del grafo
#include <iostream>
#include <ctime>
#include <vector>
#include <cmath>
#include <algorithm>
#include <utility>
using namespace std;
#define Tcasamiento 0.4
#define Tmutacion 0.2
#define IND 10
#define N 10
//grafo no dirigido de 10 nodos
int grafo[N][N]={{0,14,13,6,0,2,15,6,8,1},//N=10
                            {14,0,5,7,12,8,1,0,0,0},
                            {13,5,0,19,17,0,2,7,3,0},
                            {6,7,19,0,15,18,13,16,11,20},
                            {0,12,17,15,0,1,14,0,8,4},
                            {2,8,0,18,1,0,0,11,0,10},
                            {15,1,2,13,14,0,0,8,0,18},
                            {6,0,7,16,0,11,8,0,7,2},
                            {8,0,3,11,8,0,0,7,0,0},
                            {1,0,0,20,4,10,18,2,0,0}};

//funcion que compara 2 individuos
bool iguales(vector<int> ind1,vector<int> ind2){
    for(int i=0;i<ind1.size();i++) if(ind1[i]!=ind2[i]) return false;
    return true;
}
//funcion que indica si una ruta es aberracion, es decir que contiene un trayecto que no se encuentra en el grafo
bool aberracion(vector<int> v,int grafo[][N]){
    for(int i=0;i<v.size()-1;i++){
        if(grafo[v[i]][v[i+1]]==0) return true;
    }
    return false;
}
//funcion que busca que el individuo a ingresar no se encuentre en la poblacion, con el fin de conseguir mejores
//resultados al reducir las repeticiones al momento de generar la poblacion inicial
bool nuevo(vector<int> v,vector<vector<int>> poblacion){
    for(int i=0;i<poblacion.size();i++)
        if(iguales(v,poblacion[i])) return false;
    return true;
}

void generaPoblacionInicial(vector<vector<int>>&poblacion,int grafo[][N]){
    int cont=0;
    
    srand(time(NULL));
    while(cont<IND){
        vector<int> v;
        //se llena la ruta en orden del 0 al N-1, esos son los nodos
        //se reorganiza aleatoriamente con el shuffle
        for(int i=0;i<N;i++) v.push_back(i);
        random_shuffle(v.begin(),v.end());
        //se verifica que sea una ruta valida y nueva
        if(!aberracion(v,grafo) and nuevo(v,poblacion)){
            poblacion.push_back(v);
            cont++;
        }
    }
}
//el fitness en este caso es la inversa del tamaÃ±o del recorrido, esto ya que buscamos el menor recorrido
//al invertirlo los valores mayores seran la mejor solucion
double calculafitness(vector<int> cromo,int grafo[][N]){
    int cont=0;
    double fit;
    for(int i=0;i<cromo.size()-1;i++){
        cont+=grafo[cromo[i]][cromo[i+1]];
    }
    fit=1.0/cont;
    return fit;
}
//el porcentaje de supervivencia para cada uno de los individuos(rutas) de la poblacion
void calculasupervivencia(vector<vector<int>> poblacion,vector<double>&supervivencia,int grafo[][N]){
    double sumafitness=0.0;
    for(int i=0;i<poblacion.size();i++)
        sumafitness+=calculafitness(poblacion[i],grafo);
    for(int i=0;i<poblacion.size();i++){
        double fit=round(100*(double)calculafitness(poblacion[i],grafo)/sumafitness);
        
        supervivencia.push_back(fit);
    }
}
//se usa los porcentajes de supervivencia para cargar la ruleta de acuerdo a esos valores
void cargaruleta(vector<double>supervivencia,int *ruleta){
    int ind=0;
    for(int i=0;i<supervivencia.size();i++){
        for(int j=0;j<supervivencia[i];j++){
            ruleta[ind]=i;
            ind++;
        }
    }
}
//usando la ruleta, se selecciona a los padres
void seleccion(vector<vector<int>> poblacion,vector<vector<int>>&padres,int grafo[][N]){
    int npadres,cont=0;
    vector<double> supervivencia;      
    int ruleta[100]{-1};
    calculasupervivencia(poblacion,supervivencia,grafo);
    cargaruleta(supervivencia,ruleta);
    npadres=round(poblacion.size()*Tcasamiento);
    while(1){
        int ind=rand()%100;
        if(ruleta[ind]>-1)
            padres.push_back(poblacion[ruleta[ind]]);
        cont++;
        if(cont>=npadres)break;
    }
}
//funcion para mostrar poblacion y poder observar como va variando en las generaciones
void muestrapoblacion(vector<vector<int>> poblacion,int grafo[][N]){
    for(int i=0;i<poblacion.size();i++){
        
        for(int j=0;j<poblacion[i].size();j++){
            cout << poblacion[i][j] << "  ";
        }
        cout <<" fo="<< calculafitness(poblacion[i],grafo) <<endl;
    }
}
//funcion que indica si un nodo no se encuentra en una ruta, se usa para generar el hijo
bool nuevoNodo(int nodo,vector<int> hijo){
    for(int i=0;i<hijo.size();i++) if(nodo==hijo[i]) return false;
    return true;
}
//el hijo se genera mediante cruzamiento en orden, en este caso el individuo se trata de una permutacion
//por eso se genera segun indica el ppt para estos casos
void generahijo(vector<int>padre,vector<int>madre,vector<int>&hijo){
    int pos1=rand()%padre.size();
    int pos2=rand()%madre.size();
    
    if(pos1>pos2) swap(pos1,pos2);
    for(int i=0;i<padre.size();i++) hijo.push_back(-1);
    
    for(int i=pos1;i<pos2;i++)
        hijo[i]=padre[i];
    int indP=pos2;
    int indH=pos2;
    for(int i=0;i<padre.size();i++){
        if(indP==padre.size()) indP=0;
        if(indH==padre.size()) indH=0;
        if(nuevoNodo(madre[indP],hijo)){
            hijo[indH]=madre[indP];
            indH++;
        }
        indP++;
    }
}
//funcion de cruzamiento que genera hijos y los aÃ±ade a la poblacion
void casamiento(vector<vector<int>> &poblacion,vector<vector<int>> padres){
    
    for(int i=0;i<padres.size();i++)
        for(int j=0;j<padres.size();j++)
            if(i!=j){
                vector<int> cromo;
                generahijo(padres[i],padres[j],cromo);
                poblacion.push_back(cromo);
           }
}
//funcion de mutacion en este caso usa el swap por ser permutaciones, desordenando la ruta pero conservando
//adyacencias
//al ser el metodo mas aleatorio es util en el sentido explorativo ya que notamos que las soluciones brindadas eran
//bastante dependientes de la poblacion inicial, con esta mutacion es mas probable encontrar mejores rutas
void mutacion(vector<vector<int>> &poblacion,vector<vector<int>> padres){
    int cont=0;
    int nmuta=round(padres[0].size()*Tmutacion);
   // cout << nmuta << endl;
    for(int i=0;i<padres.size();i++){
        cont=0;
        while(cont<nmuta){
            int ind1=rand()%padres[0].size();
            int ind2=rand()%padres[0].size();
            int aux=padres[i][ind1];
            padres[i][ind1]=padres[i][ind2];
            padres[i][ind2]=aux;
            cont++;
        }
        poblacion.push_back(padres[i]);     
    }
}


//funcion que elimina todas las rutas que contienen conexiones de nodos inexistentes e igualmente elimina todas
//las rutas repetidas
void eliminaaberraciones(vector<vector<int>> &poblacion,int grafo[][N]){
    for(int i=0;i<poblacion.size();i++){
        if(aberracion(poblacion[i],grafo)){
            poblacion.erase(poblacion.begin()+i); 
            i--;
        }
    }
//elimina repetidos
    for(int i=0;i<poblacion.size();i++)
        for(int j=i+1;j<poblacion.size();j++){
            if(iguales(poblacion[i],poblacion[j])){
                poblacion.erase(poblacion.begin()+j);
                j--;
            }
        }
}
//funcion que muestra la mejor ruta de una poblacion
void muestramejor(vector<vector<int>> poblacion,int grafo[][N]){
    int mejor=0;
    for(int i=0;i<poblacion.size();i++)
        if(calculafitness(poblacion[mejor],grafo)<calculafitness(poblacion[i],grafo))
            mejor=i;
    //mostramos la inversa del fitness para que se aprecie el valor de la distancia minima que se busca
    cout << endl<<"La mejor solucion es:" <<1.0/calculafitness(poblacion[mejor],grafo)<<endl;
    for(int i=0;i<poblacion[mejor].size();i++){
        cout << poblacion[mejor][i] << "  ";        
    }
    cout << endl;
}
//funcion que compara individuos segun el fitness a usar para ordenar con el sort
bool comparaIndividuos(vector<int>&a, vector<int>&b){
    return calculafitness(a,grafo) > calculafitness(b,grafo);
}
//ordena de mayor a menor la poblacion y selecciona los mejores de ellos para la siguiente generacion
//asegurando el rol explotativo del algoritmo
void seleccionaSupervivientes(vector<vector<int>>&poblacion){
    sort(poblacion.begin(),poblacion.end(),comparaIndividuos);
    if(poblacion.size()>IND) poblacion.resize(IND);
}
//funcion AG con todo lo antes mencionado
void AG(int grafo[][N]){
    vector<vector<int>> poblacion;
    generaPoblacionInicial(poblacion,grafo);
    int cont=0;
    while(1){
        vector<vector<int>> padres;
        seleccion(poblacion,padres,grafo);
        casamiento(poblacion,padres);//corregido
        mutacion(poblacion,padres);//swaps aleatorios pero tmb se generaran aberraciones aunque no tantas
                                                   //como el casamiento, pueden salir buenas opciones
//        cout<<"mutacion:"<<endl;
//        muestrapoblacion(poblacion,grafo);
        eliminaaberraciones(poblacion,grafo);
//        cout<<"poblacion(elimina aberraciones):"<<endl;
//        muestrapoblacion(poblacion,grafo);
        seleccionaSupervivientes(poblacion);
        //mostramos supervivientes de cada generacion para ver como van mejorando los fitness en cada iteracion
        cout<<"poblacion(supervivientes):"<<endl;
        muestrapoblacion(poblacion,grafo);
        muestramejor(poblacion,grafo);
        //como es un grafo considerable con infinidad de rutas, usamos 200 generaciones para asegurarnos de recibir
        //las mejores soluciones
        cont++;
        cout<<"generacion: "<<cont<<endl;
        if(cont==200) break;
    }
}

int main(int argc, char** argv) {
//    int grafo[N][N]={{0,1,4,0,0},//N=5
//                            {1,0,2,5,0},
//                            {4,2,0,0,3},
//                            {0,5,0,0,1},
//                            {0,0,3,1,0}};
    //al final cambiamos el grafo como global para facilitar el funcionamiento de algunas funciones
    AG(grafo);
    return 0;
}
