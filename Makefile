clean :
	rm -f *.b#? *.s#? *.l#? *.pro *~

zip :
	rm -f MikeTsao-Dream.zip && zip MikeTsao-OK_Wake.zip *.dri *.xln *.ger *.gpi

cleancam :
	rm -f *.dri *.xln *.ger *.gpi
