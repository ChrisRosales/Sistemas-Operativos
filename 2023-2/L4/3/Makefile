
 #
 # ssoo/xalloc.98/Makefile
 #
 # makefile de la practica de gestion de memoria dinamica
 #

CC= gcc
CFLAGS= -g -Wall -lm

OBJS_XRUN= xrun.o xalloc.o

xrun: $(OBJS_XRUN)
	$(CC) -o $@ $(OBJS_XRUN) $(CFLAGS)

clean:	
	rm -f xrun *.o

#
# Se puede compilar para que xrun no llame al modulo xalloc (se hace xmalloc
# igual a malloc,...)
# 
# El comando que debe usarse para compilar esta version es:
#               make xrun 
#
