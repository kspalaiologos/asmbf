
AC_PREREQ([2.69])
AC_INIT([asm2bf], [v1.4.2], [kspalaiologos@gmail.com])
AC_CONFIG_SRCDIR([bfasm.c])
AC_CONFIG_HEADERS([config.h])
AC_COPYRIGHT([Copyright (C) Kamila Szewczyk, 2020. Licensed under the terms of MIT license.])
AC_LANG([C])

AC_DEFUN([AX_CHECK_COMPILE_FLAG],
[AC_PREREQ(2.64)dnl for _AC_LANG_PREFIX and AS_VAR_IF
AS_VAR_PUSHDEF([CACHEVAR],[ax_cv_check_[]_AC_LANG_ABBREV[]flags_$4_$1])dnl
AC_CACHE_CHECK([whether _AC_LANG compiler accepts $1], CACHEVAR, [
  ax_check_save_flags=$[]_AC_LANG_PREFIX[]FLAGS
  _AC_LANG_PREFIX[]FLAGS="$4 $1"
  AC_COMPILE_IFELSE([m4_default([$5],[AC_LANG_PROGRAM()])],
    [AS_VAR_SET(CACHEVAR,[yes])],
    [AS_VAR_SET(CACHEVAR,[no])])
  _AC_LANG_PREFIX[]FLAGS=$ax_check_save_flags])
AS_VAR_IF(CACHEVAR,yes,
  [m4_default([$2], :)],
  [m4_default([$3], :)])
AS_VAR_POPDEF([CACHEVAR])dnl
])dnl AX_CHECK_COMPILE_FLAGS

AC_DEFUN([CHK_FLAG2], [
    AX_CHECK_COMPILE_FLAG([$1],
            [AC_SUBST([$2], ["$$2 $1"])], [
                AC_MSG_WARN([$1 is beneficial for debug builds, but it's not present.])
            ], [-Werror])
])

AC_DEFUN([CHK_FLAG], [
    CHK_FLAG2($1, [LOC_F])
])

AC_DEFUN([PROG_SWITCH], [
    AC_ARG_ENABLE([$1],
        AS_HELP_STRING([--enable-$1], [$2]), [
            AC_DEFINE([$3], [], [$2])
    ])
])

# Checks for programs.
AC_PROG_YACC
AC_PROG_CC
AC_PROG_LEX
AC_PROG_MAKE_SET
AC_CACHE_SAVE

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h limits.h stddef.h stdint.h stdlib.h string.h])
AC_CACHE_SAVE

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CACHE_SAVE

AC_ARG_ENABLE([version], AS_HELP_STRING([--enable-version], [Sets the dist version for asm2bf]), [
    AC_SUBST([VERSION], [$enableval])
], [
    AC_SUBST([VERSION], [trunk])
])

AC_ARG_WITH([target],
            AS_HELP_STRING([--with-target=@<:@auto/release/debug/coverage@:>@],
                           [Build target (default: auto)]),
            [asmbf_distro_target=$withval],
            [asmbf_distro_target=auto])

case "$asmbf_distro_target" in
    "release")
        AC_SUBST([DISTRO], ["-Ofast -march=native -funroll-loops -fomit-frame-pointer"])
        ;;
    "debug")
        AC_SUBST([LOC_F], [])
        CHK_FLAG([-fsanitize=pointer-compare])
        CHK_FLAG([-fsanitize=pointer-subtract])
        CHK_FLAG([-fsanitize=address])
        CHK_FLAG([-fsanitize=leak])
        CHK_FLAG([-fsanitize=undefined])
        CHK_FLAG([-fcf-protection=full])
        CHK_FLAG([-fstack-protector-strong])
        CHK_FLAG([-fstack-check])
        
        AC_SUBST([DISTRO], m4_normalize(["
            -Wall -Wextra
            -g3 -O2
            $LOC_F
        "]))
        ;;
    "coverage")
        AX_CHECK_COMPILE_FLAG([-coverage],
            ,
            [AC_MSG_FAILURE([-coverage is required for coverage builds.])],
            [-Werror])
        
        AC_SUBST([DISTRO], ["-g3 -O0 -coverage"])
        ;;
    *)
        AC_SUBST([DISTRO], ["-O2"])
        ;;
esac

PROG_SWITCH([account-segments], m4_normalize([
    Makes bfdata respect cross-segment references while defining labels.
]), [ACCOUNT_SEGMENTS])

PROG_SWITCH([relative-segmentation], m4_normalize([
    Makes bfdata respect cross-segment references towards nonzero segments.
    Note: This will recalculate segments.
    bfdata will fail if the segment becomes negative,
    or overflow if the effective adress overflows the memory storage data type.
]), [RELATIVE_SEGMENTATION])

PROG_SWITCH([silient-errors], [
    Shut up all the error and warning messages from bfasm.
], [BFASM_NO_ERROR_CODES])

PROG_SWITCH([library], [
    Export bfasm instead of main from bfasm.c
], [BFASM_NO_EXPORT_MAIN])

PROG_SWITCH([nocheck-bfi], [
    Skip all checks in bfi (making it a tiny bit faster).
], [BFI_NOCHECKS])

PROG_SWITCH([bfvm], [
    Makes bfasm emit bfvm code.
], [BFVM])

PROG_SWITCH([o0], [
    Disable size optimizations in bfasm.
], [DISABLE_OPT])

AC_ARG_ENABLE([rle-prefix],
    AS_HELP_STRING([--enable-rle-prefix], [Enable prefixed RLE for bfasm.]), [
        AC_DEFINE([RLE], [], [Enable prefixed RLE for bfasm.])
])

AC_ARG_ENABLE([rle-postfix],
    AS_HELP_STRING([--enable-rle-postfix], [Enable postfix RLE for bfasm.]), [
        AC_DEFINE([RLE], [], [Enable postfix RLE for bfasm.])
        AC_DEFINE([RLE_POSTFIX], [], [Enable postfix RLE for bfasm.])
])

PROG_SWITCH([bfvm32], [
    Makes the bfvm output code 32-bit.
], [BFVM_32])

PROG_SWITCH([bfvm-heap], [
    Sets the bfvm heap size.
], [BFVM_HEAP])

PROG_SWITCH([bfvm-freestanding], [
    Builds bfvm in freestanding mode (tape located at 0x7000).
], [FREESTANDING])

PROG_SWITCH([double-nocopy], [
    Utilizes the other set of rules in bconv.c
], [DOUBLE_NOCOPY])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRCOLL
AC_CHECK_FUNCS([asprintf floor memchr memset pow sqrt strchr strcspn strdup strerror strpbrk strrchr strstr strtoul])
AC_CACHE_SAVE

AC_CONFIG_FILES([Makefile
                 bfpp/Makefile
                 hla/Makefile
                 redpower/Makefile])
AC_OUTPUT
