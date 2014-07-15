utf8proc_pgsql.so: utf8proc_pgsql.o
	ld -shared -o utf8proc_pgsql.so utf8proc_pgsql.o

utf8proc_pgsql.o: utf8proc_pgsql.c
	gcc -Wall -fpic -c -I`pg_config --includedir-server` \
		-o utf8proc_pgsql.o utf8proc_pgsql.c

clean:
	rm -f *.o *.so

