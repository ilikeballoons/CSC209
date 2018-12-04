PORT = 51303
CFLAGS = -DPORT=\$(PORT) -Wall -std=c99 -g

hcq_server: hcq_server.o hcq.o
	gcc $(CFLAGS) -o hcq_server hcq_server.o hcq.o

hcq_server.o: hcq_server.c hcq_server.h
	gcc $(CFLAGS) -c hcq_server.c

hcq.o: hcq.c hcq.h
	gcc $(CFLAGS) -c hcq.c

clean:
	rm hcq_server *.o
