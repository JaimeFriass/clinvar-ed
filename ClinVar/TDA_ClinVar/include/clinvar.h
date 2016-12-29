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
	clinvar(string nombreDB);
    void load (string nombreDB);
    void insert (const mutacion & x);
    bool erase (IDmut ID);
    int size ();
    iterator find_Mut (IDmut ID);
    enfermedad_iterator  find_Enf(IDenf ID);
    vector<enfermedad> getEnfermedades(mutacion & mut);
    list<IDenf> getEnfermedades(string keyword);
    set<IDmut> getMutacionesEnf (IDenf ID);
    set<IDmut> getMutacionesGen (IDgen ID);
    set<mutacion,ProbMutaciones> topKMutaciones (int k, string keyword);
    
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