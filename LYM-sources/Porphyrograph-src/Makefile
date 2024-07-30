####################################################################
## 
## 
##     File Makefile
## 
##
##
## This program is free software; you can redistribute it and#or
## modify it under the terms of the GNU Lesser General Public License
## as published by the Free Software Foundation; either version 2.1
## of the License, or (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
## 
## You should have received a copy of the GNU Lesser General Public
## License along with this program; if not, write to the Free
## Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.
####################################################################

include config.mak

VERSION=1.0.3

VPATH=$(SRC_PATH)

CPPFLAGS=-D CORE -D LINUX

CXXFLAGS= $(OPTFLAGS) $(CPPFLAGS) -I$(SRC_PATH) 
LDFLAGS+= -g 

PROG=PG

#---------------------------------------------
#-----Dirs------------------------------------
#---------------------------------------------

########### TERMPORARY DIRECTORIES
# temporary storage of object files
local_libdir = ./obj
# temporary storage of binary files
local_execdir = ./bin
# temporary file to store errors and warning
error_file = ./.make_error

#---------------------------------------------
#-----Source files----------------------------
#---------------------------------------------

SRC_BASE = pg-conf.cpp \
      pg-init.cpp \
      pg-main.cpp \
      pg-script.cpp \
      pg-shader.cpp \
      pg-texture.cpp \
      pg-audio.cpp \
      pg-mesh.cpp \
      pg-update.cpp \
      pg-clipArt.cpp \
      pg-sensor.cpp \
      pg-color.cpp \
      pg-callBack.cpp \
      pg-flash.cpp \
      pg-gui.cpp \
      pg-media.cpp \
      pg-messages.cpp \
      pg-path.cpp \
      pg-scenarioDyn.cpp \
      pg-udp.cpp 

HDR_BASE = config.h \
		pg-conf.h \
      pg-init.h \
      pg-main.h \
      pg-script.h \
      pg-shader.h \
      pg-texture.h \
      pg-audio.h \
      pg-mesh.h \
      pg-update.h \
      pg-clipArt.h \
      pg-sensor.h \
      pg-color.h \
      pg-callBack.h \
      pg-flash.h \
      pg-gui.h \
      pg-media.h \
      pg-messages.h \
      pg-path.h \
      pg-scenarioDyn.h \
      pg-udp.h 

#---------------------------------------------
#-----Compile and link------------------------
#---------------------------------------------

SRC_FILES = $(SRC_BASE:%=$(SRC_PATH)/%) 
OBJ_FILES = $(SRC_BASE:%.cpp=$(local_libdir)/%.o) 

NB_LINE = -1

all: $(PROG)

PG: $(local_execdir)/PG

$(local_libdir)/%.o : %.cpp 
	@echo " " > $(error_file)
	@echo -n "CC $* "
	$(CXX) $(CXXFLAGS) -o $@ -c $< 2>> $(error_file)
	@cat $(error_file)
	@test `cat $(error_file) | grep -E "^(\./|/)?([^:]*:)+[0-9]+: err" | wc -l` -eq 0

$(local_execdir)/PG: create_dir $(OBJ_FILES)
	@echo
	@echo "Changing to object files directory"
	@cd $(local_libdir)
	@echo
	$(CXX) $(LDFLAGS) -o $@ $(OBJ_FILES) $(LDLIBS) 2> $(error_file)
	@echo LD PG
	@cat $(error_file)
	@test `cat $(error_file) | wc -l` -eq 0 
	@rm -f $(error_file)

create_dir:
	@echo "Creating object files directory"
	@mkdir -p $(local_libdir)/
	@echo "Creating binary directory"
	@mkdir -p $(local_execdir)/
	@echo

install:  all
	install -m755 $(local_execdir)/PG $(prefix)/bin/PG

install-man:
# ifneq ($(CONFIG_WIN32),yes)
# 	if [ -f $(SRC_PATH)/doc/PG.1 ] ; then \
# 	    install -d $(mandir)/man1 ; \
# 	    install -m 644 $(SRC_PATH)/doc/PG.1 $(mandir)/man1 ; \
# 	fi
# endif

#---------------------------------------------
#-----Other stuff-----------------------------
#---------------------------------------------
SUBMISSIONS = $(SRC_PATH)/*cpp $(SRC_PATH)/*h  README INSTALL.txt

depend:
	makedepend $(CXXFLAGS) -Y $(SRC_FILES)

clean:
	@echo "Cleaning temporary files"
	-@rm -rf $(local_libdir) $(error_file)
	
mrproper: clean
	@echo "Cleaning binary"
	-@rm -rf $(local_execdir)  $(SRC_PATH)/include/pg_scripts/%.h
	
dist:
	rm -rf pgnew-$(VERSION) pgnew-$(VERSION).tar.gz bin obj
	mkdir pgnew-$(VERSION) bin obj
	cp -rf $(SUBMISSIONS) pgnew-$(VERSION)
	cp -f Makefile.archive pgnew-$(VERSION)/Makefile
	tar zcf pgnew-$(VERSION).tgz bin obj pgnew-$(VERSION) 

