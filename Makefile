webserver: webserver.c  http_parser/http_parser.o
	gcc -o webserver webserver.c -luv http_parser/http_parser.o

http_parser/http_parser.o:
	$(make) -C http-parser http_parser.o

