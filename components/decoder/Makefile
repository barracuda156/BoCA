########## BoCA directory makefile ##########

BOCA_PATH = ../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(BOCA_PATH)/Makefile-options

FOLDERS = alac cuesheet faad2 fdkaac flac mac mpg123 opus sndfile speex vorbis

ifeq ($(BUILD_UNIX),True)
ifneq ($(BUILD_HAIKU),True)
ifneq ($(BUILD_OPENBSD),True)
	FOLDERS += cdio
else
	FOLDERS += cdparanoia
endif
endif
else ifeq ($(BUILD_WIN32),True)
	FOLDERS += cdrip mediafoundation wma

ifeq ($(BUILD_X86),True)
	FOLDERS += winamp
endif
endif

.PHONY: $(FOLDERS)

all: $(FOLDERS)

$(FOLDERS):
	+ $(call makein,$@)

clean: $(foreach FOLDER,$(FOLDERS),$(FOLDER)##clean)

$(foreach FOLDER,$(FOLDERS),$(FOLDER)##clean):
	$(call cleanin,$(subst ##clean,,$@))

install: $(foreach FOLDER,$(FOLDERS),$(FOLDER)##install)

$(foreach FOLDER,$(FOLDERS),$(FOLDER)##install):
	$(call makein,$(subst ##install,,$@),install)

uninstall: $(foreach FOLDER,$(FOLDERS),$(FOLDER)##uninstall)

$(foreach FOLDER,$(FOLDERS),$(FOLDER)##uninstall):
	$(call makein,$(subst ##uninstall,,$@),uninstall)
