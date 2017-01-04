CPPFLAGS = g++ -std=c++11

# EJECUTABLES

bin/principal: obj/principal.o obj/enfermedad.o obj/mutacion.o obj/clinvar.o
		g++ obj/principal.o obj/enfermedad.o obj/mutacion.o obj/clinvar.o -o bin/principal

# ARCHIVOS OBJETO

obj/principal.o: src/principal.cpp
	$(CPPFLAGS) -c src/principal.cpp -o obj/principal.o -Iinclude

obj/clinvar.o: src/clinvar.cpp include/clinvar.h
	$(CPPFLAGS) -c src/clinvar.cpp -o obj/clinvar.o -Iinclude

obj/enfermedad.o: src/enfermedad.cpp include/enfermedad.h
	$(CPPFLAGS) -c src/enfermedad.cpp -o obj/enfermedad.o -Iinclude

obj/mutacion.o: src/mutacion.cpp include/mutacion.h
	$(CPPFLAGS) -c src/mutacion.cpp -o obj/mutacion.o -Iinclude

clean:
	echo "Limpiando..."
	rm $(OBJ)/*.o
mrproper: clean
	rm $(BIN)/principal