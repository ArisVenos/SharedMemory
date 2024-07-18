all: reader writer memory readmemory

reader: reader.c
	gcc -o reader reader.c -pthread -lrt

writer: writer.c
	gcc -o writer writer.c -pthread -lrt

memory: memory.c
	gcc -o memory memory.c -pthread -lrt

readmemory: readmemory.c
	gcc -o readmemory readmemory.c -pthread -lrt

clean:
	rm -f reader writer memory readmemory