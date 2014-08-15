dummy: systemrun.c
	gcc -fPIC -shared -o systemrun.so systemrun.c -I../../../include
