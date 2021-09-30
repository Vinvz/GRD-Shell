output: main.o
	gcc gvs.o -lreadline -o gvs

main.o: gvs.c
	gcc -c gvs.c -lreadline  

clean:
	rm *.o gvs

ls_test:
	./gvs ls