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

CXXFLAGS= $(OPTFLAGS) -Wall -I$(SRC_PATH)
LDFLAGS+= -g 

PROG=PG_TVWinstall$(EXESUF)

#---------------------------------------------
#-----Dirs------------------------------------
#---------------------------------------------

########### TERMPORARY DIRECTORIES
# temporary storage of object files
local_libdir = ../obj_TVWinstall
# temporary storage of binary files
local_execdir = ../bin
# temporary file to store errors and warning
error_file = ./.make_error

#---------------------------------------------
#-----Source files----------------------------
#---------------------------------------------

SRC_BASE = pg-conf.cpp \
      pg-draw.cpp \
      pg-main.cpp \
      pg-script.cpp \
      pg-shader.cpp \
      pg-stroke.cpp \
      pg-texture.cpp \
      pg-udp.cpp

HDR_BASE = config.h pg-all_include.h \
      pg-conf.h\
      pg-draw.h \
      pg-main.h \
      pg-script.h  pg_script_header.h \
      pg-shader.h pg_shader_header.h \
      pg-stroke.h \
      pg-texture.h \
      pg-udp.h

#---------------------------------------------
#-----Compile and link------------------------
#---------------------------------------------

SRC_FILES = $(SRC_BASE:%=$(SRC_PATH)/%) 
OBJ_FILES = $(SRC_BASE:%.cpp=$(local_libdir)/%.o) 

ifeq ($(BUILD_FFMPEG),yes)
OBJ_FILES += $(local_libdir)/pg-video.o
HDR_BASE += pg-video.h
endif

NB_LINE = -1

PG_TVWinstall$(EXESUF): $(local_execdir)/PG_TVWinstall

all: $(PROG)

$(local_libdir)/pg-video.o:$(SRC_PATH)/pg-video.cpp $(SRC_PATH)/$(HDR_BASE)
	@echo > $(error_file)
	@echo -n "CC pg-video "
	-@$(CXX) $(CXXFLAGS) -o $@ -c $< 2>> $(error_file)
ifeq ($(BUILD_FFMPEG),yes)
	-@cat $(error_file) | grep -v "^$(FFMPEG_GREP)"
else
	@cat $(error_file)
endif
	@test `cat $(error_file) | grep -E "^(\./|/)?([^:]*:)+[0-9]+: err" | wc -l` -eq 0

$(local_libdir)/%.o : $(SRC_PATH)/%.cpp $(SRC_PATH)/$(HDR_BASE)
	@echo " " > $(error_file)
	@echo -n "CC $* "
	-@$(CXX) $(CXXFLAGS) -c -o $(local_libdir)/$*.o $*.cpp 2>> $(error_file)
ifeq ($(BUILD_FFMPEG),yes)
	-@cat $(error_file) | grep -v "^$(FFMPEG_GREP)"
else
	@cat $(error_file)
endif
	@test `cat $(error_file) | grep -E "^(\./|/)?([^:]*:)+[0-9]+: err" | wc -l` -eq 0

$(local_execdir)/PG_TVWinstall: create_dir $(OBJ_FILES)
	@echo
	@echo "Changing to object files directory"
	@cd ../obj_TVWinstall
	@echo
	-@$(CXX) $(LDFLAGS) -o $@ $(OBJ_FILES) $(LDLIBS) 2> $(error_file)
	@echo LD PG_TVWinstall
	@cat $(error_file)
	@test `cat $(error_file) | wc -l` -eq 0 
	@rm -f $(error_file)

create_dir:
	@echo "Creating object files directory"
	@mkdir -p ../obj_TVWinstall
	@echo "Creating binary directory"
	@mkdir -p $(local_execdir)/
	@echo

install:  all
	install -m755 $(local_execdir)/PG_TVWinstall $(prefix)/bin/PG_TVWinstall

install-man:
# ifneq ($(CONFIG_WIN32),yes)
# 	if [ -f $(SRC_PATH)/doc/PG_TVWinstall.1 ] ; then \
# 	    install -d $(mandir)/man1 ; \
# 	    install -m 644 $(SRC_PATH)/doc/PG_TVWinstall.1 $(mandir)/man1 ; \
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
	rm -rf PG_TVWinstall-$(VERSION) PG_TVWinstall-$(VERSION).tar.gz bin obj_TVWinstall
	mkdir PG_TVWinstall-$(VERSION) bin obj_TVWinstall
	cp -rf $(SUBMISSIONS) PG_TVWinstall-$(VERSION)
	cp -f Makefile.archive PG_TVWinstall-$(VERSION)/Makefile
	tar zcf PG_TVWinstall-$(VERSION).tgz bin obj_TVWinstall PG_TVWinstall-$(VERSION) 

# DO NOT DELETE

