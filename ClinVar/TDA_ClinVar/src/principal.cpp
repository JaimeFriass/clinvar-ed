
#include <iostream>
#include <fstream>
using namespace std;

#include "mutacion.h"
#include "clinvar.h"
#include "enfermedad.h"

int main (int argc, char *argv[] ) {
	clinvar clv("clinvar_20160831.vcf");

	// Imprimimos el número de elementos almacenados en clv
	//cout << "Lectura del fichero finalizada. Mutaciones cargadas: " << clv.size() << endl;
}