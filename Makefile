PROG=brute_http_basic_auth
SRCS=main.c
LDFLAGS+=`curl-config --libs`
CFLAGS+=`curl-config --cflags`
MAN=

.include<bsd.prog.mk>
