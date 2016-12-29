
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
#include "clinvar.h"

bool clinvar::ProbMutaciones::operator() (const mutacion& m1, const mutacion& m2) const {
	vector<float> vector_m1 = m1.getCaf();
	vector<float> vector_m2 = m2.getCaf();

	return 1-vector_m1[0] > 1-vector_m2[0];

}

clinvar::clinvar(string nombreDB) {
	load(nombreDB);
}
// Lee los elementos de un fichero dado por el argumento nombreDB
void clinvar::load (string nombreDB) {
	ifstream fe;
	string cadena;

	cout << "Abrimos " << nombreDB << endl;
	fe.open(nombreDB.c_str(), ifstream::in);

	if (fe.fail()) {
		cerr << "Error al abrir el fichero " << nombreDB << endl;
	}
	else {
		// Se lee la cabecera, es decir, las lineas que comienzan con #
		do {
			getline(fe, cadena, '\n');
		} while (cadena.find('#') != string::npos && !fe.eof() );

		// Se lee la tabla de mutaciones, una línea cada vez
		while ( !fe.eof() ) {
			cout << "Leo : " << cadena << endl;
			// Invoco el constructor de mutación
			mutacion mut = mutacion(cadena);
			// Insertamos la mutación en clinvar
			insert(mut);
			getline(fe, cadena, '\n');
		}
		fe.close();
	}
	fe.close();
}

// Inserta una mutación en su lugar correspondiente
void clinvar::insert (const mutacion & x) {
	for (iterator itr(mutDB.begin()); itr != end(); itr++) {
		if ( (*itr) < x) {
			itr++;
			if ( (*itr) > x) {
				itr--;
				// mutDB.insert(itr, x);
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
	for (iterator itr = begin(); itr != end() || borrado; itr++) {
		if ( (*itr).getID() == ID)
			if (mutDB.erase(*itr) == 1)
				borrado = true;
	}
	return borrado;
}

int clinvar::size () {
	return mutDB.size();
}
// Borra la mutación con identificador ID dentro de ClinVar,
// si no lo encuentra devuelve eFnd()
clinvar::iterator clinvar::find_Mut (IDmut ID) {
	iterator devolver = mutDB.end();
	for (iterator itr = mutDB.begin(); itr != mutDB.end() ; itr++) {
		if ( (*itr).getID() == ID)
			if (mutDB.erase(*itr) == 1)
				devolver = itr;
	}
	return devolver;
}


clinvar::enfermedad_iterator  clinvar::find_Enf(IDenf ID) {

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

	set<IDmut> conjunto;
	vector<enfermedad> aux;
	multimap<IDenf, set<mutacion>::iterator >::iterator eitr;

	for (eitr = IDenf_map.begin(); eitr != IDenf_map.end(); eitr++) {
		if ( eitr->first == ID )
			aux = (*eitr->second).getEnfermedades();
	}
	/*
	vector<enfermedad> aux;
	for (itr = ebegin(); itr != eend(); itr++) {
		if ( itr->first == ID) {
			aux = itr->second.getEnfermedades();
		}	
	}
	*/
	for (int i = 0; i < aux.size(); i++) {
		conjunto.insert(aux[i].getID());
	}
	return conjunto;
}

// Devuelve un conjunto de todas las mutaciones que se encuentran asociadas a un gen determinado
// dado por ID. Si no tuviese ninguno, devuelve el conjunto vacío.
// map<IDgen, list< set<mutacion>::iterator> > gen_map;
set<IDmut> clinvar::getMutacionesGen (IDgen ID) {
	set<IDmut> conjunto;

	for (gen_iterator itr(gbegin()); itr != gend(); itr++) {
		if (itr.getID() == ID) {
			conjunto.insert((*itr).getID());
		}
	}
}

// Dado un string keyword devuelve todas las enfermedades cuyo nombre contiene keyword
set<mutacion, clinvar::ProbMutaciones> clinvar::topKMutaciones (int k, string keyword) {
	set<mutacion, ProbMutaciones> devuelve;
	priority_queue<mutacion, vector<mutacion>, ProbMutaciones> cola;
	set<IDmut> mut_asociadas;
	set<IDmut>::iterator mut_itr;
	enfermedad_iterator itr;
	unordered_set<IDmut> aux;


	itr = ebegin();
	// Mientras no se inserten los k primeros elementos
	while ( devuelve.size() != k + 1) {
		mut_asociadas = getMutacionesEnf( (*itr).second.getID() );
		for (mut_itr = mut_asociadas.begin(); mut_itr != mut_asociadas.end() && devuelve.size() != k + 1; mut_itr++)
			// Si la mutación contiene keyword
			if ( (*find_Enf( *mut_itr )).second.nameContains(keyword) ) {
				cola.push( *mut_itr );
				aux.insert( *mut_itr);
			}
		itr++;
	}
	for (; itr != eend(); itr++) {
		mut_asociadas = getMutacionesEnf( (*itr).first );
		for (mut_itr = mut_asociadas.begin(); mut_itr != mut_asociadas.end(); mut_itr++)
			if ( (*find_Enf( *mut_itr )).second.nameContains(keyword) ) {
				cola.push( *mut_itr );
				aux.insert( *mut_itr );
				aux.erase( aux.find( cola.top().getID() ) );
				cola.pop();
			}
	}

	for (int i = 0; i < cola.size(); i++) {
		devuelve.insert(cola.top() );
		cola.pop();
	}

	return devuelve;
	
	/*
	multimap<IDenf, set<mutacion>::iterator>::iterator itr;

	// Se insertan las k primeras mutaciones directamente en la cola
	itr = IDenf_map.begin();
	for (int i = 0; i <= k; itr++) {
		if ( (*itr->second).nameContains(keyword) ) {
			cola.push(*itr);
			aux.insert( (*itr).getID() );
			i++;
		}
	}

	// Para las siguientes se comprueba si la nueva es mayor que la del tope
	for (; itr != IDenf_map.end(); itr++) {

		// Si la mutacion contiene keyword y su probabilidad es mayor que la del tope
		if ( (*itr).nameContains(keyword) && (*itr).getProb > cola.top() ) {
			// Si la mutación en cuestion no está en el conjunto auxiliar
			if ( aux.find( (*itr).getID() ) == aux.end() ) {
				// Se inserta en la cola y en el cjto auxiliar
				cola.push( (*itr) );
				aux.insert( (*itr).getID() );

				// Se elimina la ultima de los dos conjuntos
				cola.pop();
				aux.erase( aux.find( cola.front().getID() ) );
			}
		}
	}

	for (int i = 0; i < cola.size(); i++) {
		devuelve.insert(cola.front() );
		cola.pop();
	}

	return devuelve;
*/
}


// *******************      ITERATOR      *************************************

clinvar::iterator::iterator(const set<mutacion>::iterator itr) {
	it = itr;
}

clinvar::iterator clinvar::begin() {
	iterator itr(mutDB.begin());
	return itr;
}

clinvar::iterator clinvar::end() {
	iterator itr(mutDB.end());
	return itr;
}

bool clinvar::iterator::operator==(const iterator &x) const {
	return (*this).it == x.it;
}

bool clinvar::iterator::operator!=(const iterator &x) const {
	return !(*this == x);
}

clinvar::iterator clinvar::iterator::operator++() {
	this->it++;
	return *this;
}

clinvar::iterator clinvar::iterator::operator++(int i) {
	iterator aux((*this).it);
	++(*this);
	return aux;
}

clinvar::iterator clinvar::iterator::operator--() {
	this->it--;
	return *this;
}

clinvar::iterator clinvar::iterator::operator--(int i) {
	iterator aux((*this).it);
	--(*this);
	return aux;
}

const mutacion & clinvar::iterator::operator*() {
	return *it;
}
clinvar::iterator clinvar::lower_bound(string cromosoma, unsigned int posicion) {
	/* iterator itr = end();
	bool encontrado = false;

	for (iterator iter = begin(); iter != end() && !encontrado; iter++) {
		if ( )
	}
	*/
}

clinvar::iterator clinvar::upper_bound(string cromosoma, unsigned int posicion) {

}
// *******************      ENFERMEDAD_ITERATOR      **************************

clinvar::enfermedad_iterator clinvar::ebegin() {
	return EnfDB.begin();
}

clinvar::enfermedad_iterator clinvar::eend() {
	return EnfDB.end();
}


// *******************      GEN_ITERATOR      *********************************

clinvar::gen_iterator::gen_iterator(const map<IDgen, list < set<mutacion>::iterator > >::iterator itr) {
	itmap = itr;
}

clinvar::gen_iterator clinvar::gen_iterator::operator++() {
	/*
	this->itlist++;
	if (this->itlist == gen_map->second.end()) {
		this->itmap++;
		this->itlist = this->itmap.begin();
	}
	return *this;
	*/
}

bool clinvar::gen_iterator::operator==(const gen_iterator &x) const {
	return (*this).itmap == x.itmap;
}

bool clinvar::gen_iterator::operator!=(const gen_iterator &x) const {
	return !(*this == x);
}

clinvar::gen_iterator clinvar::gen_iterator::operator++(int i) {
	gen_iterator aux(this->itmap);
	++(*this);
	return aux;
}

clinvar::gen_iterator clinvar::gen_iterator::operator--() {
	this->itmap--;
	return *this;
}

clinvar::gen_iterator clinvar::gen_iterator::operator--(int i) {
	gen_iterator aux(this->itmap);
	--(*this);
	return aux;
}

const mutacion & clinvar::gen_iterator::operator*() {
	return *(*itlist);
}

// Devuelve el ID de el gen al que apunta gen_iterator
IDgen clinvar::gen_iterator::getID() {
	return itmap->first;
}

clinvar::gen_iterator clinvar::gbegin() {
	gen_iterator gen_itr(gen_map.begin());
	return gen_itr;
}

clinvar::gen_iterator clinvar::gend() {
	gen_iterator gen_itr(gen_map.end());
	return gen_itr;
}

