dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(winxound, 3.4.0)


AC_SUBST(MALLOC_CHECK_)

AC_SUBST(G_THREADS_IMPL_NONE)

__cplusplus=1
AC_SUBST(__cplusplus)

PLAT_GTK=1
AC_SUBST(PLAT_GTK)


m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC

AM_PROG_LIBTOOL



PKG_CHECK_MODULES(WINXOUND_GTKMM, [gtkmm-2.4 >= 2.12 vte webkit-1.0])




AC_OUTPUT([
Makefile
src/Makefile

])
