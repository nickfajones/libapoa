###############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################

default: dev


###############################################################################
PROJECT         = libapoa

ARCH            = $(shell uname -m)

ifeq ($(ARCH), x86_64)
arch_LIB        = lib64
else ifeq ($(ARCH), i386)
arch_LIB        = lib
else ifeq ($(ARCH), i686)
arch_LIB        = lib
else
$(error target ${ARCH} not supported)
endif


###############################################################################
OBJDIR          = $(PWD)/.obj
SODIR           = $(OBJDIR)_so
EXEDIR          = $(OBJDIR)_bin


###############################################################################
INCROOT         = $(PWD)/inc
INCDIR          = $(INCROOT)/libapoa
SRCDIR          = $(PWD)/src

SUBDIRS         = $(INCDIR) $(SRCDIR)


###############################################################################
CC              = g++

ifeq ($(ARCH), x86_64)
arch_CPPFLAGS   =
else ifeq ($(ARCH), i386)
arch_CPPFLAGS   = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
else ifeq ($(ARCH), i686)
arch_CPPFLAGS   = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
else
$(error target ${ARCH} not supported)
endif

base_CFLAGS     = -Wall -fPIC -pthread $(CFLAGS)
dev_CFLAGS      = $(base_CFLAGS) -Werror
build_CFLAGS    = $(base_CFLAGS)

base_OPT        =
dev_OPT         = $(base_OPT) -g3
build_OPT       = $(base_OPT)

base_CPPFLAGS   = -D_GNU_SOURCE $(arch_CPPFLAGS) -isystem $(INCROOT) -I $(INCDIR)
dev_CPPFLAGS    = $(base_CPPFLAGS)
build_CPPFLAGS  = $(base_CPPFLAGS) -DNDEBUG

base_LDOBJS     =
dev_LDOBJS      = $(base_LDOBJS)
build_LDOBJS    = $(base_LDOBJS)

base_LDFLAGS    =
dev_LDFLAGS     = $(base_LDFLAGS)
build_LDFLAGS   = $(base_LDFLAGS)

base_LIBS       =
dev_LIBS        = $(base_LIBS)
build_LDFLAGS   = $(base_LIBS)


###############################################################################
dev_CPPFLAGS   += -I/opt/nbboost/include
build_CPPFLAGS += -I/opt/nbboost/include
dev_LDOBJS     += /opt/nbboost/$(arch_LIB)/libboost_system-mt.a \
  /opt/nbboost/$(arch_LIB)/libboost_filesystem-mt.a \
  /opt/nbboost/$(arch_LIB)/libboost_thread-mt.a
build_LDOBJS   += /opt/nbboost/$(arch_LIB)/libboost_system-mt.a \
  /opt/nbboost/$(arch_LIB)/libboost_filesystem-mt.a \
  /opt/nbboost/$(arch_LIB)/libboost_thread-mt.a
dev_LDFLAGS    += -L/opt/nbboost/$(arch_LIB) -Wl,-rpath=/opt/nbboost/$(arch_LIB)
build_LDFLAGS  += -L/opt/nbboost/$(arch_LIB) -Wl,-rpath=/opt/nbboost/$(arch_LIB)
dev_LIBS       += -lboost_system-mt -lboost_filesystem-mt -lboost_thread-mt
build_LIBS     += -lboost_system-mt -lboost_filesystem-mt -lboost_thread-mt


##########################################################################
include obj.mk

base_OBJS       = $(LIBAPOA_OBJS)
dev_OBJS        = $(base_OBJS)
build_OBJS      = $(base_OBJS)


##########################################################################
.PHONY: dev build dirs clean install

dev: dirs
	$(MAKE) $(dev_OBJS) \
	  CFLAGS="$(build_CFLAGS)" OPT="$(dev_OPT)" CPPFLAGS="$(dev_CPPFLAGS)"
	$(MAKE) $(SODIR)/libapoa.a OBJS="$(dev_OBJS)" LDOBJS="$(dev_LDOBJS)"

build: dirs
	$(MAKE) $(build_OBJS) \
	  CFLAGS="$(build_CFLAGS)" OPT="$(build_OPT)" CPPFLAGS="$(build_CPPFLAGS)"
	$(MAKE) $(SODIR)/libapoa.so OBJS="$(build_OBJS)" \
	  LDOBJS="$(build_LDOBJS)" LDFLAGS="$(build_LDFLAGS)" LIBS="$(build_LIBS)"

apoaapp: dev $(SODIR)/libapoa.a
	$(MAKE) $(EXEDIR)/apoaapp \
	  CFLAGS="$(dev_CFLAGS)" OPT="$(dev_OPT)" CPPFLAGS="$(dev_CPPFLAGS)" \
	  LDOBJS="$(SODIR)/libapoa.a" LDFLAGS="$(dev_LDFLAGS)" LIBS="$(dev_LIBS)"


##############################################################################
dirs:
	mkdir -p $(OBJDIR) $(SODIR) $(EXEDIR)

clean:
	@startdir=$(PWD)
	for dir in $(SUBDIRS); do \
	  cd $$dir && rm -f *~ .#* && cd $$startdir; \
	done;
	rm -rf $(OBJDIR)*


###############################################################################
install:
	mkdir -p $(DESTDIR)/usr/$(arch_LIB)
	install -m 0755 $(SODIR)/libapoa.so $(DESTDIR)/usr/$(arch_LIB)
	install -m 0644 $(SODIR)/libapoa.a $(DESTDIR)/usr/$(arch_LIB)
	
	mkdir -p $(DESTDIR)/usr/include/libapoa
	install -m 0644 $(INCDIR)/*.h* $(DESTDIR)/usr/include/libapoa
