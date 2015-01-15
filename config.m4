dnl $Id$
dnl config.m4 for extension hz2py

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(hz2py, for hz2py support,
dnl Make sure that the comment is aligned:
dnl [  --with-hz2py             Include hz2py support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(hz2py, whether to enable hz2py support,
[  --enable-hz2py           Enable hz2py support])

if test "$PHP_HZ2PY" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-hz2py -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/hz2py.h"  # you most likely want to change this
  dnl if test -r $PHP_HZ2PY/$SEARCH_FOR; then # path given as parameter
  dnl   HZ2PY_DIR=$PHP_HZ2PY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for hz2py files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       HZ2PY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$HZ2PY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the hz2py distribution])
  dnl fi

  dnl # --with-hz2py -> add include path
  dnl PHP_ADD_INCLUDE($HZ2PY_DIR/include)

  dnl # --with-hz2py -> check for lib and symbol presence
  dnl LIBNAME=hz2py # you may want to change this
  dnl LIBSYMBOL=hz2py # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $HZ2PY_DIR/$PHP_LIBDIR, HZ2PY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_HZ2PYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong hz2py lib version or lib not found])
  dnl ],[
  dnl   -L$HZ2PY_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(HZ2PY_SHARED_LIBADD)

  PHP_NEW_EXTENSION(hz2py, hz2py.c utf8vector.c pinyin.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
