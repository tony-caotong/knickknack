LIBHTP_INC=/root/libhtp-0.5.25/
LIBHTP_LIB=/root/libhtp-0.5.25/htp/.libs/

htp: test_libhtp.c
	gcc -I $(LIBHTP_INC) -g -O0 -c test_libhtp.c
	gcc test_libhtp.o -o htp -g -L $(LIBHTP_LIB) \
		-Wl,--rpath=$(LIBHTP_LIB) \
		-O0 -lpcap -lnids -lhtp
clean:
	rm -f htp
