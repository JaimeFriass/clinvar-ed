CPPFLAGS = g++ -std=c++11 -c

# EJECUTABLES

bin/principal: obj/principal.o obj/enfermedad.o obj/mutacion.o obj/clinvar.o
		g++ -std=c++11 -o bin/principal obj/enfermedad.o obj/mutacion.o obj/clinvar.o

# ARCHIVOS OBJETO

obj/principal.o: src/principal.cpp
	$(CPPFLAGS) -Iinclude src/principal.cpp -o obj/pincipal.o

obj/clinvar.o: src/clinvar.cpp include/clinvar.h
	$(CPPFLAGS) -Iinclude src/clinvar.cpp -o obj/clinvar.o

obj/enfermedad.o: src/enfermedad.cpp include/enfermedad.h
	$(CPPFLAGS) -Iinclude src/enfermedad.cpp -o obj/enfermedad.o

obj/mutacion.o: src/mutacion.cpp include/mutacion.h
	$(CPPFLAGS) -Iinclude src/mutacion.cpp -o obj/mutacion.o