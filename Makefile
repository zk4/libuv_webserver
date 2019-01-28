webserver: webserver.c  http_parser/http_parser.o
	gcc -o webserver webserver.c -luv http_parser/http_parser.o

http_parser/http_parser.o:
	$(MAKE) -C http_parser http_parser.o

clean:
	make -C http_parser clean
	rm webserver