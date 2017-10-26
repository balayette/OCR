#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ocr], [0.1], [nmanichon@gmail.com])
AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_SRCDIR([src/headers/imgprocessing/boxes.h])
AC_CONFIG_HEADERS([src/headers/misc/config.h])

AC_PROG_MAKE_SET
AC_PROG_CC

AC_CHECK_LIB(SDL, SDL_CreateRGBSurface, , AC_MSG_ERROR([
                                                        *** Unable to find the SDL library.
                                                        *** https://www.libsdl.org/
                                                        ]))

AC_CHECK_LIB(SDL_image, IMG_LoadPNG_RW, , AC_MSG_ERROR([
                                                        *** Unable to find the SDL_image libary
                                                        (http://www.libsdl.org/projects/SDL_image/)
                                                        ]))
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN],
               [test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/doxyconfig])])

               AC_OUTPUT(Makefile man/Makefile doc/Makefile)
