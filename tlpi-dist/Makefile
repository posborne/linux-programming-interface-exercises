# Makefile to build all programs in all subdirectories
#
# DIRS is a list of all subdirectories containing makefiles
# (The library directory is first so that the library gets built first)
#

DIRS = 	lib \
    	acl altio \
	cap \
	daemons dirs_links \
	filebuff fileio filelock files filesys getopt \
	inotify \
	loginacct \
	memalloc \
	mmap \
	pgsjc pipes pmsg \
	proc proccred procexec procpri procres \
	progconc \
	psem pshm pty \
	shlibs \
	signals sockets \
	svipc svmsg svsem svshm \
	sysinfo \
	syslim \
	threads time timers tty \
	users_groups \
	vmem \
	xattr\
	e23_test #2016/03/03

# The "namespaces" directory is deliberately excluded from the above
# list because much of the code requires a fairly recent kernel and
# userspace to build. Nevertheless, there is a Makefile in that directory.


BUILD_DIRS = ${DIRS} ${CDIRS}

# Dummy targets for building and clobbering everything in all subdirectories
#
#
all: 	
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE}) ; done

allgen: 
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} allgen) ; done

clean: 
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} clean) ; done

showvariable:
	@ echo ${MAKE}
	@ echo ${BUILD_DIRS}
