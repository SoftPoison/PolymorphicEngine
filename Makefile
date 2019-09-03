poly: poly.c make.sh
	./make.sh

dependencies: findnop.c merge.c
	make findnop && make merge

findnop: findnop.c
	gcc -o findnop findnop.c -Wall && chmod 755 findnop

merge: merge.c
	gcc -o merge merge.c -Wall && chmod 755 merge

clean:
	rm -f poly
