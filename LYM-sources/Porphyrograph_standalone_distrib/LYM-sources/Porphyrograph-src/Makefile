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

CPPFLAGS=-D CRITON -D LINUX

CXXFLAGS= $(OPTFLAGS) $(CPPFLAGS) -Wall -I$(SRC_PATH) 
LDFLAGS+= -g 

PROG=PG_Criton$(EXESUF)

#---------------------------------------------
#-----Dirs------------------------------------
#---------------------------------------------

########### TERMPORARY DIRECTORIES
# temporary storage of object files
local_libdir = ../obj_Criton
# temporary storage of binary files
local_execdir = ../bin
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
      pg-stroke.cpp \
      pg-SVG_GPU.cpp \
      pg-texture.cpp \
      pg-udp.cpp 

HDR_BASE = config.h pg-all_include.h \
      pg-conf.h pg_script_header_Criton.h pg_shader_header_Criton.h\
      pg-init.h \
      pg-main.h \
      pg-script.h \
      pg-shader.h  \
      pg-stroke.h \
      pg-SVG_GPU.h \
      pg-texture.h \
      pg-udp.h

#---------------------------------------------
#-----Compile and link------------------------
#---------------------------------------------

SRC_FILES = $(SRC_BASE:%=$(SRC_PATH)/%) 
OBJ_FILES = $(SRC_BASE:%.cpp=$(local_libdir)/%.o) 

NB_LINE = -1

PG_Criton$(EXESUF): $(local_execdir)/PG_Criton

all: $(PROG)

$(local_libdir)/%.o : $(SRC_PATH)/%.cpp $(SRC_PATH)/$(HDR_BASE)
	@echo " " > $(error_file)
	@echo -n "CC $* "
	-@$(CXX) $(CXXFLAGS) -c -o $(local_libdir)/$*.o $*.cpp 2>> $(error_file)
	@cat $(error_file)
	@test `cat $(error_file) | grep -E "^(\./|/)?([^:]*:)+[0-9]+: err" | wc -l` -eq 0

$(local_execdir)/PG_Criton: create_dir $(OBJ_FILES)
	@echo
	@echo "Changing to object files directory"
	@cd ../obj_Criton
	@echo
	-@$(CXX) $(LDFLAGS) -o $@ $(OBJ_FILES) $(LDLIBS) 2> $(error_file)
	@echo LD PG_Criton
	@cat $(error_file)
	@test `cat $(error_file) | wc -l` -eq 0 
	@rm -f $(error_file)

create_dir:
	@echo "Creating object files directory"
	@mkdir -p ../obj_Criton
	@echo "Creating binary directory"
	@mkdir -p $(local_execdir)/
	@echo

install:  all
	install -m755 $(local_execdir)/PG_Criton $(prefix)/bin/PG_Criton

install-man:
# ifneq ($(CONFIG_WIN32),yes)
# 	if [ -f $(SRC_PATH)/doc/PG_Criton.1 ] ; then \
# 	    install -d $(mandir)/man1 ; \
# 	    install -m 644 $(SRC_PATH)/doc/PG_Criton.1 $(mandir)/man1 ; \
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
	-@rm -f $(OBJ_FILES) $(error_file)

dist:
	rm -rf PG_Criton-$(VERSION) PG_Criton-$(VERSION).tar.gz bin obj_Criton
	mkdir PG_Criton-$(VERSION) bin obj_Criton
	cp -rf $(SUBMISSIONS) PG_Criton-$(VERSION)
	cp -f Makefile.archive PG_Criton-$(VERSION)/Makefile
	tar zcf PG_Criton-$(VERSION).tgz bin obj_Criton PG_Criton-$(VERSION) 

# DO NOT DELETE

