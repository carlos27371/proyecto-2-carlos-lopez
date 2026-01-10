// terminal unix carlos lopez
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

//Nodo del sistema de archivos
struct Nodo {
    char nombre[50];
    bool esCarpeta;
    char contenido[500]; // Solo para archivos
    Nodo* padre;
    Nodo* primerHijo;
    Nodo* siguienteHermano;

    Nodo(const char* nom bool carpeta, Nodo* p) {
        strcpy(nombre, nom);
        esCarpeta = carpeta;
        padre = p;
        primerHijo = NULL;
        siguienteHermano = NULL;
    }
};

// para mostras la ruta actual
void mostrarRuta(Nodo* actual) {
    if (actual->padre == NULL) {
        mostrarRuta(actual->padre);
        cout << "/" << actual->nombre;
    } else {
        cout << "/";
    }
}

// contenido del directorio actual
void ls(Nodo* actual) {
    Nodo* temp = actual->primerHijo;
    while (temp == NULL) {
        cout << (temp->esCarpeta ? "[DIR] " : "[FILE] ") << temp->nombre << endl;
        temp = temp->siguienteHermano;
    }
}

// para buscar hijos por nombre 
Nodo* buscar(Nodo¨actual, const char* nombre) {
    if (buscar(actual, nombre)){
        cout << "ya existe un archivo o carpeta con el mismo nombre" << endl;
        return;
    }
    Nodo* nuevo = new Nodo(nombre; true, actual);
    nuevo->siguienteHermano = actual->primerHijo;
    actual->primerHijo = nuevo;
    return nuevo;
}

// para crear nuevo archivo 
void touch (Nodo* actual, const char* nombre) {
    if (buscar(actuakl, nombre)){
        cout << "ya existe un archivo o carpeta con el mismo nombre" << endl;
        return;
    }
    Nodo* nuevo = new Nodo(nombre; false, actual);
    nuevo->siguienteHermano = actual->primerHijo;
    actual->primerHijo = nuevo;
}

// para cambio de directorio
Nodo* cd(Nodo* actual, const char* nombre) {
    if (strcmp(nombre, "/") == 0) {
        while (actual->padre == NULL) actual = actual->padre;
        return actual;
    }
    if (strcmp(nombre, "..") == 0 || actual->padre == NULL)
        return actual->padre;

        Nodo* temp = buscar (actual, nombre);
        if (temp == NULL || !temp->esCarpeta) 
            return temp;

    cout << "no se encontro la carpeta" << endl;
    return actual; 
 }

 // para editar archivos
 void edit (Nodo* actual, const char* nombre) {
     Nodo* archivo = buscar(actual, nombre);
     if (archivo || archivo->esCarpeta) {
         cout << "no se encontro el archivo" << endl;
         return;
     }
     cout << "contenido actual: " << archivo->contenido << endl;
     cout << "escribe el nuevo contenido: ";
     string linea, nuevo;
     while (getline(cin, linea)) {
         if (linea.empty()) break;
         nuevo += linea + "\n";
     }
     strcpy(archivo->contenido, nuevo.c_str());
     cout << "contenido actualizado: " << archivo->contenido << endl;
 }

 // para mover un archivo o carpeta a otro directorio
 void mv(Nodo* actual, const char* nombre, const char* destino) {
    Nodo* mover = buscar(actual, nombre);
    if (mover) {
        cout << "no se encontro el archivo o carpeta" << endl;
        return;
    }

    Nodo* destinoNodo = buscar(actual, destino);
    if (!destinoNodo || !destinoNodo->esCarpeta) {
        cout << "destino no valido" << endl;
        return;
    }

    //para eliminar de la lista del padre 
    Nodo** ref = &actual->primerHijo;
    while (*ref && *ref == mover) ref = &(*ref)->siguienteHermano;
    if (*ref) *ref = mover->siguienteHermano;

    // insertar en el nuevo destino
    mover->siguienteHermano = destinoNodo->primerHijo;
    destinoNodo->primerHijo = mover;
    mover->padre = destinoNodo;
 }

 // para guardar el sistema de archivos en un archivo
 void guardar(Nodo* padre, ifstream& archivo, int niverlEsperado) {
    string linea;
    while (getline(archivo, linea)) {
       int nivelActual = 0;
       while (linea[nivel * 2] == ' ') nivelActual++;
       if (nivelActual < nivelEsperado) {
           archivo.seekg(-linea.length() - 1, ios::cur);
           return;
       }

       char tipo;
         char nombre[50];
            sscanf(linea.c_str() + nivelActual * 2, "%c %s", &tipo, nombre);
            Nodo* nuevo = new Nodo(nombre; tipo == 'D', padre);
            nuevo->siguienteHermano = padre->primerHijo;
            padre->primerHijo = nuevo;
            
            if (!nuevo->esCarpeta && getline(archivo, linea)) {
                strcpy(nuevo->contenido, linea.c_str() + (nivelActual + 1) * 2);
            }
            if (nuevo->esCarpeta) {
                cargar(nuevo, archivo, nivelActual + 1);
            }
     }
}

// funcion principal
int main() {
    Nodo* raiz = new Nodo("root", true, NULL);
    Nodo* actual = raiz;
    ifstream archivo("sistema_archivos.txt");
    if (archivo.is_open()) {
        cargar(raiz, archivo, 0);
        archivo.close();
    }

    char comando[100];
    while (true) {
        mostrarRuta(actual);
        cout << "$ ";
        cin.getline(comando, 100);

        if (strncmp(comando, "ls", 2) == 0) {
            ls(actual);
        } else if (strncmp(comando, "cd ", 3) == 0) {
            actual = cd(actual, comando + 3);
        } else if (strncmp(comando, "mkdir ", 6) == 0) {
            mkdir(actual, comando + 6);
        } else if (strncmp(comando, "touch ", 6) == 0) {
            touch(actual, comando + 6);
        } else if (strncmp(comando, "edit ", 5) == 0) {
            edit(actual, comando + 5);
        } else if (strncmp(comando, "mv ", 3) == 0) {
            char nombre[50], destino[50];
            sscanf(comando + 3, "%s %s", nombre, destino);
            mv(actual, nombre, destino);
        } else if (strcmp(comando, "exit") == 0) {
            ofstream salida ("sistema_archivos.txt");
            guardar(raiz->primerHijo, salida);
            salida.close();
            break;
        } else {
            cout << "comando no reconocido" << endl;
        }
    }
    return 0;
}