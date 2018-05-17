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

CXXFLAGS= $(OPTFLAGS) -Wall -I$(SRC_PATH) -I/winG/home-win/Porphyrograph/vst_plugins/VST3_SDK/pluginterfaces/vst2.x/ -I/winG/home-win/Porphyrograph/vst_plugins/VST3_SDK/public.sdk/source/vst2.x/ -I/winG/home-win/Porphyrograph/vst_plugins/VST3_SDK -I/winG/home-win/Porphyrograph/vst_plugins/MVerb_vst/MVerbEcho/
LDFLAGS+= -g 

PROG=PG_wav$(EXESUF)

#---------------------------------------------
#-----Dirs------------------------------------
#---------------------------------------------

########### TERMPORARY DIRECTORIES
# temporary storage of object files
local_libdir = ../obj_wav
# temporary storage of binary files
local_execdir = ../bin
# temporary file to store errors and warning
error_file = ./.make_error

#---------------------------------------------
#-----Source files----------------------------
#---------------------------------------------

SRC_BASE = pg-wav_RW_VST.cpp pg-udp.cpp pg-audio-IO.cpp mdaDelay.cpp 

HDR_BASE = config.h pg-all_include.h \
      pg-wav_RW_VST.h pg-udp.h pg-audio_IO.h mdaDelay.h 

#---------------------------------------------
#-----Compile and link------------------------
#---------------------------------------------

SRC_FILES = $(SRC_BASE:%=$(SRC_PATH)/%) 
OBJ_FILES = $(SRC_BASE:%.cpp=$(local_libdir)/%.o) 

NB_LINE = -1

PG_wav$(EXESUF): $(local_execdir)/PG_wav

all: $(PROG)

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

$(local_execdir)/PG_wav: create_dir $(OBJ_FILES)
	@echo
	@echo "Changing to object files directory"
	@cd ../obj_wav
	@echo
	-@$(CXX) $(LDFLAGS) -o $@ $(OBJ_FILES) $(LDLIBS) 2> $(error_file)
	@echo LD PG_wav
	@cat $(error_file)
	@test `cat $(error_file) | wc -l` -eq 0 
	@rm -f $(error_file)

create_dir:
	@echo "Creating object files directory"
	@mkdir -p ../obj_wav
	@echo "Creating binary directory"
	@mkdir -p $(local_execdir)/
	@echo

install:  all
	install -m755 $(local_execdir)/PG_wav $(prefix)/bin/PG_wav

install-man:
# ifneq ($(CONFIG_WIN32),yes)
# 	if [ -f $(SRC_PATH)/doc/PG_wav.1 ] ; then \
# 	    install -d $(mandir)/man1 ; \
# 	    install -m 644 $(SRC_PATH)/doc/PG_wav.1 $(mandir)/man1 ; \
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
	rm -rf PG_wav-$(VERSION) PG_wav-$(VERSION).tar.gz bin obj_wav
	mkdir PG_wav-$(VERSION) bin obj_wav
	cp -rf $(SUBMISSIONS) PG_wav-$(VERSION)
	cp -f Makefile.archive PG_wav-$(VERSION)/Makefile
	tar zcf PG_wav-$(VERSION).tgz bin obj_wav PG_wav-$(VERSION) 

# DO NOT DELETE

