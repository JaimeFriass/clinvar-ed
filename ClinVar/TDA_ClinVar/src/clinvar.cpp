
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
	return (1-vector_m1[0]) > (1-vector_m2[0]);

}

clinvar::clinvar(string nombreDB) {
	load(nombreDB);
}

void clinvar::load (string nombreDB) {
	ifstream fe;
	string cadena;
	cout << "Abrimos " << nombreDB << endl;
	// fe.open(nombreDB);
	fe.open(nombreDB.c_str(), ifstream::in);
	if (fe.fail()) {
	// if (!fe) {
		cerr << "Error al abrir el fichero " << nombreDB << endl;
	}
	else {
		// Se lee la cabecera, es decir, las lineas que comienzan con #
		do {
			getline(fe, cadena, '\n');
		} while (cadena.find('#') != string::npos && !fe.eof() );

		// Se lee la tabla de mutaciones, una línea cada vez
		while ( !fe.eof()) {
			// Invoco el constructor de mutación
			mutacion mut = mutacion(cadena);
			cout << "Añadida mutación: " << mut.getID();
			// Insertamos la mutación en clinvar
			insert(mut);
			getline(fe, cadena, '\n');
		}
		fe.close();

	}
	fe.close();
}

void clinvar::imprimirEnfDB() {
	enfermedad_iterator itr;
	cout << "----------- ENFDB --------------" << endl;
	for (itr = ebegin(); itr != eend(); itr++) {
		cout << "\t" << (*itr).second.getID() << endl;
	}
}

void clinvar::imprimirMutDB() {
	cout << "----------- MUTDB --------------" << endl;
	for (iterator itr(begin()); itr != end(); itr++) {
		cout << "\t" << (*itr).getID() << endl;
	}
}

void clinvar::imprimirGenMap() {
	cout << "------------GEN MAP --------------" << endl;
	map<IDgen, list< set<mutacion>::iterator> >::iterator itr = gen_map.begin();
	list < set<mutacion>::iterator >::iterator itr_list;
	for (itr = gen_map.begin(); itr != gen_map.end(); itr++) {
		cout << "\tID: " << (*itr).first << endl;
		for ( itr_list = ((*itr).second).begin() ; itr_list != (*itr).second.end(); itr_list++)
			cout << "\t\t" << (*(*itr_list)).getID() << endl;
	}
	cout << "\n";
}

void clinvar::insert (const mutacion & x) {

	set<mutacion>::iterator itr;
	pair< set<mutacion>::iterator , bool > mut_insertada;

	mut_insertada = mutDB.insert(x);
	itr = mut_insertada.first;		// Guardamos el LUGAR DONDE SE HA INSERTADO

	// Insertamos en IDm_map
	pair<IDmut, set<mutacion>::iterator> mut_a_insertar;
	mut_a_insertar.first = x.getID();
	mut_a_insertar.second = itr;
	IDm_map.insert(mut_a_insertar);

	vector<enfermedad> aux_enf = x.getEnfermedades();   // Al ser privadas las enfermedades lo guardamos en un auxiliar
	vector<enfermedad>::iterator enf_itr;
	pair< map<IDenf, enfermedad>::iterator , bool > enf_insertada;
	pair<IDenf, enfermedad> enfermedad_a_insertar;		// Par para insertar en EnfDB
	pair<IDenf, set<mutacion>::iterator> mut_asociadas;	// Par para insertar luego en IDenf_map

	for (int i = 0; i < aux_enf.size() ; i++) {
		enfermedad_a_insertar.first = aux_enf[i].getID();
		enfermedad_a_insertar.second = aux_enf[i];
		enf_insertada = EnfDB.insert(enfermedad_a_insertar);	// Insertamos en EnfDB
		// cout << "\t[INSERT] Enfermedad " << aux_enf[i].getName() << " insertada en EnfDB: " << enf_insertada.second << endl;

		mut_asociadas.first = aux_enf[i].getID();
		mut_asociadas.second = itr;    // Lugar donde se insertó la mutación previamente
		IDenf_map.insert(mut_asociadas);  // Insertamos en ID_ENF_MAP

	}
	cout << "\n\n";

	// Insertamos genes de la mutacion en GEN_MAP
	vector<string> genes = x.getGenes();
	// cerr << "[INSERT] Insertamos " << genes.size() << " genes: " << endl;
	for (int i = 0; i < genes.size(); i++) {
		// cerr << "\t[INSERT] Insertado gen " << genes[i] << endl;
		addGen(genes[i], itr);
	}

}
void clinvar::addGen(IDgen ID, set<mutacion>::iterator itr ) {
	map < IDgen, list < set<mutacion>::iterator > >::iterator iter_map;
	list < set<mutacion>::iterator > lista;
	bool encontrado = false;

	// Si encuentra ya una key con IDgen, inserta el iterador argumentado a su correspondiente lista
	for (iter_map = gen_map.begin(); iter_map != gen_map.end(); iter_map++) {
		if ( (*iter_map).first == ID) {
			((*iter_map).second).push_back(itr);
			encontrado = true;
		}
	}
	// Si no hay una key de IDgen, se crea una con el iterador argumentado
	if (!encontrado) {
		lista.push_back(itr);
		pair<IDgen, list< set<mutacion>::iterator> > par;
		par.first = ID;
		par.second = lista;
		// Se inserta en gen_map
		gen_map.insert(par);
	}
}

bool clinvar::erase (IDmut ID) {
	bool borrado = false;
	iterator itr(find_Mut(ID));
	vector<enfermedad> enfs = getEnfermedades(*itr);
	set<IDmut> mut_relacionadas;

	// Borra las enfermedades asociadas si solo se asocian a la mutacion
	// en cuestión
	for (int i = 0; i < enfs.size(); i++) {
		mut_relacionadas = getMutacionesEnf( enfs[i].getID());
		// Si solo se relaciona con esta mutación
		if (mut_relacionadas.size() == 1)
			// Borra de EnfDB
			EnfDB.erase(find_Enf(enfs[i].getID()));
	}
	// Borra de MutDB
	if ((mutDB.erase(*itr)) > 0)
		borrado = true;

	return borrado;
}

int clinvar::size () {
	return mutDB.size();
}

clinvar::iterator clinvar::find_Mut (IDmut ID) {
	iterator devolver( end() );	// Si no la encuentra devuelve eend()
	for (iterator itr = begin(); itr != end() ; itr++) {
		if ( (*itr).getID() == ID)	// Si coincide el id
				devolver = itr;
	}
	return devolver;
}

clinvar::enfermedad_iterator  clinvar::find_Enf(IDenf ID) {
	enfermedad_iterator devuelve = eend();	// Si no la encuentra devuelve eend()
	for (enfermedad_iterator itr = ebegin(); itr != eend(); itr++)
		if ( (*itr).second.getID() == ID)	// Si coincide el id
			devuelve = itr;
	return devuelve;
}

vector<enfermedad> clinvar::getEnfermedades(const mutacion & mut) {
	return mut.getEnfermedades();
}

list<IDenf> clinvar::getEnfermedades(string keyword) {
	enfermedad_iterator itr;
	list<IDenf> lista;
	for (itr = EnfDB.begin(); itr != EnfDB.end(); itr++) {
		if ( itr->second.nameContains(keyword))
			lista.push_back( itr->first );
	}
	return lista;
}

set<IDmut> clinvar::getMutacionesEnf (IDenf ID) {

	set<IDmut> conjunto;
	multimap<IDenf, set<mutacion>::iterator >::iterator eitr;

	for (eitr = IDenf_map.begin(); eitr != IDenf_map.end(); eitr++) {
		if ( eitr->first == ID ) 	// Si coincide el ID
			conjunto.insert((*eitr->second).getID());		// Se inserta en el conjunto
	}
	return conjunto;
}

set<IDmut> clinvar::getMutacionesGen (IDgen ID) {
	set<IDmut> conjunto;
	map<IDgen, list< set<mutacion>::iterator> >::iterator itr = gen_map.begin();
	list < set<mutacion>::iterator >::iterator itr_list;
	for (itr = gen_map.begin(); itr != gen_map.end(); itr++) {
		// Si coincide el ID
		if ( (*itr).first == ID )
			for ( itr_list = ((*itr).second).begin() ; itr_list != (*itr).second.end(); itr_list++)
				// Inserta en el conjunto los id
				conjunto.insert((*(*itr_list)).getID());
	}
	return conjunto;
}

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
		for (mut_itr = mut_asociadas.begin(); mut_itr != mut_asociadas.end() && aux.size() != k + 1; mut_itr++)
			// Si la mutación contiene keyword
			if ( ((*find_Enf( *mut_itr )).second.nameContains(keyword) ) ) {
				// Se inserta en la cola
				cola.push( *mut_itr );
				// Se inserta en el conjunto
				aux.insert( *mut_itr);
			}
		itr++;
	}

	for (; itr != eend(); itr++) {
		mut_asociadas = getMutacionesEnf( (*itr).first );
		for (mut_itr = mut_asociadas.begin(); mut_itr != mut_asociadas.end(); mut_itr++)
			// Si contiene keyword
			if ( (*find_Enf( *mut_itr )).second.nameContains(keyword) ) {
				// Se inserta en la cola
				cola.push( *mut_itr );
				// Se inserta en el conjunto
				aux.insert( *mut_itr );
				// Se borra del conjunto el ultimo elemento de la cola
				aux.erase( aux.find( cola.top().getID() ) );
				// Se elimina el ultimo elemento de la cola
				cola.pop();
			}
	}

	// Pasa los datos de la cola al conjunto a devolver
	for (int i = 0; i < cola.size(); i++) {
		devuelve.insert(cola.top() );
		cola.pop();
	}

	return devuelve;
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

	iterator itr = end();
	bool encontrado = false;

	for (iterator ite = begin(); ite != end() && !encontrado; ite++) {
		if ( ( (*ite).getChr() >= cromosoma) && ((*ite).getPos() >= posicion)) {
			itr = ite;
			encontrado = true;
		}
	}
	return itr;
}

clinvar::iterator clinvar::upper_bound(string cromosoma, unsigned int posicion) {

	iterator itr = end();
	bool encontrado = false;

	for (iterator ite = begin(); ite != end() && !encontrado; ite++) {
		if ( ( (*ite).getChr() >= cromosoma) ) {
			itr = ite;
			encontrado = true;
		}
	}
	return itr;
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
	this->itlist++;
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

