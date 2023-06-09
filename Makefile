SYSCONF_LINK = c++
CPPFLAGS     = -O3
LDFLAGS      = -O3
LIBS 		 = -lm

DESTDIR = ./
TARGET  = tiny_render 

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga

fclean:
	$(MAKE) clean
	-rm -f $(DESTDIR)$(TARGET)

re:
	$(MAKE) clean
	$(MAKE) all