
#include <iostream>
#include <fstream>
using namespace std;

#include "mutacion.h"
#include "clinvar.h"
#include "enfermedad.h"

int main () {
	clinvar clv("../clinvar_20160831.vcf");

	cout << "Lectura del fichero finalizada. Mutaciones cargadas: " << clv.size() << endl;

	// Imprimimos MutDB
	// clv.imprimirMutDB();
	// Imprimimos ENFDB
	// clv.imprimirEnfDB();
	// Imprimimos GEN MAP
	// clv.imprimirGenMap();

	// FIND_MUT
	clinvar::iterator find_mut_itr(clv.find_Mut("rs786200939"));
	cout << "Find_Mut: " << (*find_mut_itr).getID() << endl << endl;

	// FIND_ENF
	clinvar::enfermedad_iterator enf_itr;
	enf_itr = clv.find_Enf("C3150401");
	cout << "\nFind_Mut: " << (*enf_itr).second.getID() << endl << endl;

	// GET ENFERMEDADES
	vector<enfermedad> vector_enfermedades;
	vector_enfermedades = clv.getEnfermedades(*find_mut_itr);
	cout << "GetEnfermedades: mutación rs536657086" << endl;
	for (int i = 0; i < vector_enfermedades.size(); i++)
		cout << "\t" << vector_enfermedades[i].getName() << endl;

	// GET ENFERMEDADES KEYWORD
	list<IDenf> lista_enfermedades = clv.getEnfermedades("syndrome");
	list<IDenf>::iterator itr_lista_enf;
	cout << "\nGetEnfermedades: keyword 'syndrome' -> " << lista_enfermedades.size() << endl;
	// Se imprimen aprovechando el metodo find_Enf
	for (itr_lista_enf = lista_enfermedades.begin(); itr_lista_enf != lista_enfermedades.end(); itr_lista_enf++)
		cout << "\t" << (*clv.find_Enf(*itr_lista_enf)).second.getName() << endl;

	// GET MUTACIONES ENF
	set<IDmut> mutaciones_enf = clv.getMutacionesEnf("CN169374");
	set<IDmut>::iterator itr_set_mut;
	cout << "\nGetMutacionesEnf: enfermedad 'CN169374' -> " << mutaciones_enf.size() << endl;
	for (itr_set_mut = mutaciones_enf.begin(); itr_set_mut != mutaciones_enf.end(); itr_set_mut++)
		cout << "\t" << *itr_set_mut << endl;

	// GET MUTACIONES GEN
	 set<IDmut> mutaciones_gen = clv.getMutacionesGen("SDF4:51150");
	 cout << "\nGetMutacionesGen: gen 'SDF4:51150' -> " << mutaciones_gen.size() << endl;
	 for (itr_set_mut = mutaciones_gen.begin(); itr_set_mut != mutaciones_gen.end(); itr_set_mut++)
		cout << "\t" << *itr_set_mut << endl;

	// ERASE
	string mutacion_a_eliminar = "rs115173026";
	cout << "\nProcedemos a eliminar una mutación: " << mutacion_a_eliminar;
	if (clv.erase(mutacion_a_eliminar))
		cout << "\nMutación " << mutacion_a_eliminar << " eliminada correctamente." << endl;
	else
		cout << "\nError al eliminar la mutación" << mutacion_a_eliminar << endl;

	// TOP K MUTACIONES
	/*
	set<mutacion, clinvar::ProbMutaciones> top = clv.topKMutaciones(10, "disease");
	set<mutacion, clinvar::ProbMutaciones>::iterator itr;
	cout << "TOP K MUTACIONES" << endl;
	for (itr = top.begin(); itr != top.end(); itr++) {
		cout << (*itr).getID() << endl;
	}
	*/
}