###############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################


LIBAPOA_OBJS += \
  $(OBJDIR)/common.o \
  $(OBJDIR)/basic_application_handler.o \
  $(OBJDIR)/basic_application_handler_service.o \
  $(OBJDIR)/application_handler_impl.o \
  $(OBJDIR)/basic_thread_handler.o \
  $(OBJDIR)/basic_thread_handler_service.o \
  $(OBJDIR)/thread_handler_impl.o \
  $(OBJDIR)/basic_thread_pool.o \
  $(OBJDIR)/basic_thread_pool_service.o \
  $(OBJDIR)/basic_thread_pool_impl.o \
  $(OBJDIR)/basic_signal_handler_service.o \
  $(OBJDIR)/signal_handler_impl.o \
  $(OBJDIR)/basic_process_context.o \
  $(OBJDIR)/basic_process_handler_service.o \
  $(OBJDIR)/process_handler_impl.o


###############################################################################
$(OBJDIR)/common.o: $(SRCDIR)/common.cpp $(INCDIR)/common.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@


###############################################################################
$(OBJDIR)/basic_application_handler.o: $(SRCDIR)/basic_application_handler.cpp \
  $(INCDIR)/basic_application_handler.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/basic_application_handler_service.o: $(SRCDIR)/basic_application_handler_service.cpp \
  $(INCDIR)/basic_application_handler_service.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/application_handler_impl.o: $(SRCDIR)/application_handler_impl.cpp \
  $(INCDIR)/application_handler_impl.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@


###############################################################################
$(OBJDIR)/basic_thread_handler.o: $(SRCDIR)/basic_thread_handler.cpp \
  $(INCDIR)/basic_thread_handler.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/basic_thread_handler_service.o: $(SRCDIR)/basic_thread_handler_service.cpp \
  $(INCDIR)/basic_thread_handler_service.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/thread_handler_impl.o: $(SRCDIR)/thread_handler_impl.cpp \
  $(INCDIR)/thread_handler_impl.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@


###############################################################################
$(OBJDIR)/basic_thread_pool.o: $(SRCDIR)/basic_thread_pool.cpp \
  $(INCDIR)/basic_thread_pool.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/basic_thread_pool_service.o: $(SRCDIR)/basic_thread_pool_service.cpp \
  $(INCDIR)/basic_thread_pool_service.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/basic_thread_pool_impl.o: $(SRCDIR)/basic_thread_pool_impl.cpp \
  $(INCDIR)/basic_thread_pool_impl.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@


###############################################################################
$(OBJDIR)/basic_signal_handler_service.o: $(SRCDIR)/basic_signal_handler_service.cpp \
  $(INCDIR)/basic_signal_handler_service.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/signal_handler_impl.o: $(SRCDIR)/signal_handler_impl.cpp \
  $(INCDIR)/signal_handler_impl.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@


###############################################################################
$(OBJDIR)/basic_process_context.o: $(SRCDIR)/basic_process_context.cpp \
  $(INCDIR)/basic_process_context.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/basic_process_handler_service.o: $(SRCDIR)/basic_process_handler_service.cpp \
  $(INCDIR)/basic_process_handler_service.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@

$(OBJDIR)/process_handler_impl.o: $(SRCDIR)/process_handler_impl.cpp \
  $(INCDIR)/process_handler_impl.hpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPT) $< -c -o $@


###############################################################################
$(SODIR)/libapoa.a: $(OBJS)
	$(AR) rcsT $@ $^ $(LDOBJS)

$(SODIR)/libapoa.so: $(OBJS)
	$(CC) -shared -o $@ $^ $(LDOBJS) $(LDFLAGS) $(LIBS)


###############################################################################
$(EXEDIR)/apoaapp: apoaapp.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OPT) $< -o $@ $(LDOBJS) $(LDFLAGS) $(LIBS)
