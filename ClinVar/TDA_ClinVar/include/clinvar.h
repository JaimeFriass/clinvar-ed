#ifndef __CLINVAR_H
#define __CLINVAR_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <unordered_map>
#include <unordered_set>
using namespace std;

#include "mutacion.h"
#include "enfermedad.h"

typedef string IDgen;
typedef string IDmut;
typedef string IDenf;

class clinvar {
public:
	/* @brief iterador sobre mutaciones en orden creciente de cromosoma/posicion
	*/
    class iterator {
        private:
        	set<mutacion>::iterator it;
        public:
        	const mutacion & operator+();
        	iterator(const set<mutacion>::iterator itr);
            bool operator==(const iterator &x) const;
            bool operator!=(const iterator &x) const;
            iterator operator++();
            iterator operator++(int i);
            const mutacion & operator*();
            iterator operator--();
            iterator operator--(int i);
    };

    /* @brief iterador sobre enfermedades
	*/
	// Nos vale utilizar el iterador del map
	typedef map<IDenf, enfermedad>::iterator enfermedad_iterator;

	/* @brief iterador sobre mutaciones considerando el orden creciente del ID del gen
	*/
	class gen_iterator {
	public:
        gen_iterator(const map<IDgen, list < set<mutacion>::iterator > >::iterator itr);
		const mutacion & operator*(); //const - no se puede modificar la mutacion y alterar el orden del set
		gen_iterator operator++();
        gen_iterator operator++(int i);
        gen_iterator operator--();
        gen_iterator operator--(int i);
        bool operator==(const gen_iterator &x) const;
        bool operator!=(const gen_iterator &x) const;
		IDgen getID();
	private:
		map<IDgen, list < set<mutacion>::iterator > >::iterator itmap;
		list<set<mutacion>::iterator >::iterator itlist;
		clinvar *ptrclinvar;

	};

	class ProbMutaciones {
	public:
		bool operator() (const mutacion& m1, const mutacion& m2) const;
	};
    /**
     @brief Constructor de un ClinVar.
     @param nombreDB    nombre del archivo a cargar

     Llama a la función load e inserta todas las mutaciones en el ClinVar.
     */
	clinvar(string nombreDB);
    /**
     @brief Carga un clinvar desde un archivo.
     @param nombreDB    nombre del archivo a cargar

     Abre el archivo argumentado e inserta todas sus mutaciónes en el objeto ClinVar.
     */
    void load (string nombreDB);
    /**
     @brief Inserta una mutación en el ClinVar
     @param x   mutación a insertar

     Inserta una mutación en todas las estructuras establecidas correctamente.
     */
    void insert (const mutacion & x);
    /**
     @brief Borra una mutación
     @param ID     id de la mutación a borrar

     Elimina una mutación de la base de datos, si esta tiene solo una enfermedad relacionada
     también se elimina esta.
     */
    bool erase (IDmut ID);
    /**
     @brief Busca una mutación.
     @param ID     id de la mutación a buscar
     @returns   itr     iterador a la ubicación de la mutación
     
     Busca una mutación en la base de datos y devuelve su posición.
     */
    iterator find_Mut (IDmut ID);
    /**
     @brief Busca una enfermedad.
     @param ID     id de la enfermedad a buscar
     @returns   itr     iterador a la ubicación de la enfermedad
     
     Busca una enfermedad en la base de datos y devuelve su posición.
     */
    enfermedad_iterator  find_Enf(IDenf ID);
    /**
     @brief Devuelve las enfermedades asociadas a una mutación.
     @param mut     mutación a devolver sus enfermedades
     @returns vector con las enfermedades asociadas
     
     Devuelve un vector con las enfermedades asociadas a la mutación argumentada.
     */
    vector<enfermedad> getEnfermedades(const mutacion & mut);
    /**
     @brief Devuelve las enfermedades que contienen "keyword".
     @param keyword     cadena que deben contener las enfermedades buscadas
     @returns   lista con las enfermedades buscadas
     
     Devuelve una lista con las enfermedades que contienen la cadena "keyword".
     */
    list<IDenf> getEnfermedades(string keyword);
    /**
     @brief Devuelve las mutaciones asociadas a una enfermedad
     @param ID     id de la enfermedad
     @returns   conjunto de mutaciones asociadas
     
     Devuelve un conjunto con las mutaciones asociadas a una enfermedad de determinado id.
     */
    set<IDmut> getMutacionesEnf (IDenf ID);
    /**
     @brief Devuelve las mutaciones asociadas a un gen
     @param ID     id del gen
     @returns   conjunto de mutaciones asociadas
     
     Devuelve un conjunto con las mutaciones asociadas a la enfermedad de determinado id.
     */
    set<IDmut> getMutacionesGen (IDgen ID);
    /**
     @brief Devuelve las k mutaciones ordenadas por probabilidad principales que contienen "keyword".
     @param k   numero de mutaciones deseadas
     @param keyword     cadena a buscar entre las mutaciones
     @returns   conjunto de mutaciones mas probables
     
     Devuelve las k mutaciones más probables que contienen "keyword" de la base de datos.
     */
    set<mutacion,ProbMutaciones> topKMutaciones (int k, string keyword);

    /* Métodos auxiliares de ayuda */

    /**
     @brief Tamaño del ClinVar
     @returns   numero de mutaciones del ClinVar
     
     Devuelve el número de mutaciones de un ClinVar.
     */
    int size ();
    /**
     @brief Imprime las enfermedades del ClinVar
     
     Imprime en pantalla todas las enfermedades que tiene el ClinVar actualmente.
     */
    void imprimirEnfDB();
    /**
     @brief Imprime las mutaciones del ClinVar
     
     Imprime en pantalla todas las mutaciones que tiene el ClinVar actualmente.
     */
    void imprimirMutDB();
    /**
     @brief Imprime los genes del ClinVar
     
     Imprime en pantalla todos los genes que tiene el ClinVar actualmente.
     */
    void imprimirGenMap();
    /**
     @brief Añade un gen y su posicion al gen_map
     @param ID      id del gen
     @param itr     ubicación de la mutación
     Añade un determinado gen al ClinVar junto con la posición de su mutación asociada, si existe ya el ID, 
     se añade la posición a su lista de iteradores determinada, si no, se crea una nueva lista.
     */
    void addGen(IDgen ID, set<mutacion>::iterator itr);
    
    /* Métodos relacionados con los iteradores */
    iterator begin();
    iterator end();
    iterator lower_bound(string cromosoma, unsigned int posicion);
    iterator upper_bound(string cromosoma, unsigned int posicion);
    enfermedad_iterator ebegin();
    enfermedad_iterator eend();
    gen_iterator gbegin();
    gen_iterator gend();
private:
    set<mutacion> mutDB;    //base de datos que contiene toda la información asociada a una mutacion
    map<IDenf, enfermedad> EnfDB; // Base de datos de enfermedades
    unordered_map <IDmut,set<mutacion>::iterator> IDm_map; // Asocia IDmutacion con mutación
    map<IDgen, list< set<mutacion>::iterator> > gen_map; // Asocia genes con mutaciones
    multimap<IDenf,set<mutacion>::iterator> IDenf_map; // Asocia enfermedad con mutaciones
};

#endif