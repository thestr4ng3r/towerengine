CC := g++
CFLAGS := -g -fPIC -Wall

OBJPATH := obj/
TARGETPATH := ./lib/
LINKNAME := libtowerengine.so
SONAME := $(LINKNAME).1
LIBNAME := $(LINKNAME).1.0.0
STATICLIBNAME := libtowerengine.a
SRCPATH := ./src/
INCLUDEPATH := ./include/

LIBINSTALLPATH := /usr/lib/
INCLUDEINSTALLPATH := /usr/include/towerengine

OBJ := $(OBJPATH)mesh.o \
       $(OBJPATH)vertex.o \
       $(OBJPATH)triangle.o \
       $(OBJPATH)material.o \
       $(OBJPATH)shader.o \
       $(OBJPATH)buffers.o \
       $(OBJPATH)utils.o \
       $(OBJPATH)face_shader.o \
       $(OBJPATH)vector.o \
       $(OBJPATH)vector2.o \
       $(OBJPATH)position.o \
       $(OBJPATH)animation.o \
       $(OBJPATH)entity.o \
       $(OBJPATH)engine.o \
       $(OBJPATH)world.o \
       $(OBJPATH)camera.o \
       $(OBJPATH)object.o \
       $(OBJPATH)meshobject.o \
       $(OBJPATH)resources.o
       
RESOURCES_SOURCE := $(SRCPATH)/resources.cpp
RESOURCES_HEADER := $(INCLUDEPATH)/tresources.h 

INCLUDE := -I$(INCLUDEPATH) -I$(SRCPATH) -I/usr/include/libxml2
LIBS := -L/usr/lib/x86_64-linux-gnu -lIL -lILU -lGL -lGLU -lxml2

MAKEFLAGS += -swr

all: start

start: $(RESOURCES_SOURCE) $(RESOURCES_HEADER) $(OBJ) link create-links static


$(RESOURCES_SOURCE): shader/* resources
	@echo "-----------------------------------------------------------------------"
	@echo "creating resources source ..."
	@stringres -s resources

$(RESOURCES_HEADER): resources
	@echo "-----------------------------------------------------------------------"
	@echo "creating resources headers ..."
	@stringres -h resources
	@mv tresources.h include

link: $(OBJ)
	@echo "......................................................................"
	@echo "creating shared library ..."
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(SONAME) -o $(TARGETPATH)$(LIBNAME) $(OBJ) $(LIBS)

create-links:
	@echo "......................................................................"
	@echo "creating links ..."
	ldconfig -n $(TARGETPATH)
	ln -sf $(SONAME) $(TARGETPATH)$(LINKNAME)

install: 
	cp -fv $(TARGETPATH)$(LIBNAME) $(TARGETPATH)$(SONAME) $(TARGETPATH)$(LINKNAME) $(LIBINSTALLPATH)
	cp -fv $(INCLUDEPATH)* $(INCLUDEINSTALLPATH)

uninstall:
	rm $(GLOBALPATH)$(LIBNAME) $(GLOBALPATH)$(SONAME) $(GLOBALPATH)$(LINKNAME)
	rm $(INCLUDEINSTALLPATH)/*

install-static:
	cp $(TARGETPATH)$(STATICLIBNAME) $(GLOBALPATH)

uninstall-static:
	rm $(GLOBALPATH)$(STATICLIBNAME)

static: $(OBJ)
	@echo "......................................................................"
	@echo "creating static library ..."
	ar rcs $(TARGETPATH)$(STATICLIBNAME) $(OBJ)

$(OBJPATH)%.o: $(SRCPATH)%.cpp $(INCLUDEPATH)*.h
	@echo "-----------------------------------------------------------------------"
	@echo "compiling $< ..."
	$(CC) $(CFLAGS) $(INCLUDE) -c -o "$@" "$<"

clean:
	@rm -fv $(OBJPATH)*.o

distclean: clean
	@rm -fv $(LIBNAME)
