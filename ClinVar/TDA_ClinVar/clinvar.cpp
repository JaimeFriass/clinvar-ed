#include <iostream>
#include <string>
#include <vector>
#include "clinvar.h"

using namespace std;

// Lee los elementos de un fichero dado por el argumento nombreDB
void clinvar::load (string nombreDB) {

}

// Inserta una mutación en su lugar correspondiente
void clinvar::insert (const mutacion & x) {
	for (iterator itr = mutDB.begin(); itr != mutDB.end(); itr++) {
		if ( (*itr) < x) {
			itr++;
			if ( (*itr) > x) {
				itr--;
				mutDB.insert(itr, x);
			}
		}
	}
}

// Borra una mutación de la base de datos dado su id:
// Devuelve verdadero si se ha borrado correctamente, falso si no.
// No solo borra la mutacion del repositorio principal sino que ademas se
// encarga de borrar toda la referencia a dicho elemento dentro de el.
// En El caso de que una enfermedad estuviese asociada unicamente a la mutacion que 
// esta siendo eliminada, esta enfermedad tambien debe eliminarse de Clinvar.
bool clinvar::erase (IDmut ID) {
	bool borrado = false;
	for (iterator itr = mutDB.begin(); itr != mutDB.end() || borrado; itr++) {
		if ( (*itr).getID() == ID)
			if (mutDB.erase(itr) == 1)
				borrado = true;
	}

	return borrado;
}

// Borra la mutación con identificador ID dentro de ClinVar,
// si no lo encuentra devuelve end()
clinvar::iterator clinvar::find_Mut (IDmut ID) {
	iterator devolver = mutDB.end();
	for (iterator itr = mutDB.begin(); itr != mutDB.end() ; itr++) {
		if ( (*itr).getID() == ID)
			if (mutDB.erase(itr) == 1)
				devolver = itr;
	}
	return devolver;
}

// Borra la enfermedad con identificador ID dentro de ClinVar,
// si no lo encuentra devuelve eend()
enfermedad_iterator  clinvar::find_Enf(Idenf ID) {

}
vector<enfermedad> clinvar::getEnfermedades(mutacion & mut) {
	return mut.getEnfermedades();
}

// Devuelve una lista de los identificadores de enfermedad que contienen la palabra
// keyword como parte del nombre de la enfermedad. Utilziar enfermedad.nameContains() para programarlo.
list<IDenf> clinvar::getEnfermedades(string keyword) {
	enfermedad_iterator itr;
	list<IDenf> lista;
	for (itr = EnfDB.begin(); itr != EnfDB.end(); itr++) {
		if ( itr->second.nameContains(keyword))
			lista.push_back( itr->first );
	}
	return lista;
}

// Devuelve un conjunto ordenado (en orden creciente de IDmut) de todas las mutaciones que
// se encuentran asociadas a la enfermedad con identificador ID. Si no tuviese ninguna enfermedad
// asociada, devuelve el conjunto vacío.
set<IDmut> clinvar::getMutacionesEnf (IDenf ID) {
	enfermedad_iterator itr;
	set<IDmut> conjunto;
	vector<enfermedad> aux;
	for (itr = EnfDB.begin(); itr != EnfDB.end(); itr++) {
		if ( itr->first == ID) {
			aux = itr->second.getEnfermedades();
		}	
	}
	for (int i = 0; i < aux.size(); i++) {
		conjunto.insert(aux[i]);
	}
	return conjunto;
}

// Devuelve un conjunto de todas las mutaciones que se encuentran asociadas a un gen determinado
// dado por ID. Si no tuviese ninguno, devuelve el conjunto vacío.
set<IDmut> clinvar::getMutacionesGen (IDgen ID) {
	set<IDmut> conjunto;
	gen_iterator itr;

	for (itr = gen_map.begin(); itr != gen_map.end(); itr++) {
		if 
	}
}
set<mutacion,ProbMutaciones> topKMutaciones (int k, string keyword);
