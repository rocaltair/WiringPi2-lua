LIBNAME = lwiringpi
LIB = $(LIBNAME).so


OBJS = lwiringpi.o

CFLAG = -Wall \
	-fPIC

WIRINGPI_LIBPATH = -L/usr/local/lib/
WIRINGPI_LIB = -lwiringPi

LUA_INC = -I/usr/include/lua5.1/
LUA_LIBPATH = -L/usr/lib/arm-linux-gnueabihf/
LUA_LIBS = -llua5.1 -lm

$(LIB) : $(OBJS)
	$(CC) -shared -o $@ $(LUA_LIBPATH) $(LUA_LIBS) $(WIRINGPI_LIBPATH) $(WIRINGPI_LIB) $<

lwiringpi.o : lwiringpi.c
	$(CC) -o $@ -c $(CFLAG) $(LUA_INC)  $<

install :
	cp $(LIB) /usr/lib/arm-linux-gnueabihf/lua/5.1/

clean : 
	rm -f $(OBJS) $(LIB)

.PHONY : clean install

