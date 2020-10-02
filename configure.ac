
AC_PREREQ([2.69])
AC_INIT([asm2bf], [v1.4.2], [kspalaiologos@gmail.com])
AC_CONFIG_SRCDIR([bfasm.c])
AC_CONFIG_HEADERS([config.h])
AC_COPYRIGHT([Copyright (C) Kamila Szewczyk, 2020. Licensed under the terms of MIT license.])

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

AC_ARG_ENABLE([account-segments],
    AS_HELP_STRING([--enable-account-segments], [Makes bfdata respect cross-segment references while defining labels.]), [
        AC_DEFINE([ACCOUNT_SEGMENTS], [], ["A switch for bfdata."])
])

AC_ARG_ENABLE([relative-segmentation],
    AS_HELP_STRING([--enable-relative-segmentation], [Makes bfdata respect cross-segment references towards nonzero segments. Note: This will recalculate segments. bfdata will fail if the segment becomes negative, or overflow if the effective adress overflows the memory storage data type.]), [
        AC_DEFINE([RELATIVE_SEGMENTATION], [], ["A switch for bfdata."])
])

AC_ARG_ENABLE([silient-errors],
    AS_HELP_STRING([--enable-silient-errors], [Shut up all the error and warning messages from bfasm.]), [
        AC_DEFINE([BFASM_NO_ERROR_CODES], [], ["A switch for bfasm."])
])

AC_ARG_ENABLE([library],
    AS_HELP_STRING([--enable-library], [Export bfasm instead of main from bfasm.c]), [
        AC_DEFINE([BFASM_NO_EXPORT_MAIN], [], ["A switch for bfasm."])
])

AC_ARG_ENABLE([nocheck-bfi],
    AS_HELP_STRING([--enable-nocheck-bfi], [Skip all checks in bfi (making it a tiny bit faster).]), [
        AC_DEFINE([BFI_NOCHECKS], [], ["A switch for bfasm."])
])

AC_ARG_ENABLE([bfvm],
    AS_HELP_STRING([--enable-bfvm], [Makes bfasm emit bfvm code.]), [
        AC_DEFINE([BFVM], [], ["A switch for bfasm."])
])

AC_ARG_ENABLE([o0],
    AS_HELP_STRING([--enable-o0], [Disable size optimizations in bfasm.]), [
        AC_DEFINE([DISABLE_OPT], [], ["A switch for bfasm."])
])

AC_ARG_ENABLE([rle-prefix],
    AS_HELP_STRING([--enable-rle-prefix], [Enable prefixed RLE for bfasm.]), [
        AC_DEFINE([RLE], [], ["A switch for bfasm."])
])

AC_ARG_ENABLE([rle-postfix],
    AS_HELP_STRING([--enable-rle-postfix], [Enable postfix RLE for bfasm.]), [
        AC_DEFINE([RLE], [], ["A switch for bfasm."])
        AC_DEFINE([RLE_POSTFIX], [], ["A switch for bfasm."])
])

AC_ARG_ENABLE([bfvm32],
    AS_HELP_STRING([--enable-bfvm32], [Makes the bfvm output code 32-bit.]), [
        AC_DEFINE([BFVM_32], [], ["A switch for bfvm."])
])

AC_ARG_ENABLE([bfvm-heap],
    AS_HELP_STRING([--enable-bfvm-heap=size], [Sets the bfvm heap size.]), [
        AC_DEFINE([BFVM_HEAP], [$enableval], ["A switch for bfvm."])
])

AC_ARG_ENABLE([bfvm-freestanding],
    AS_HELP_STRING([--enable-bfvm-freestanding], [Builds bfvm in freestanding mode (tape located at 0x7000).]), [
        AC_DEFINE([FREESTANDING], [], ["A switch for bfvm."])
])

AC_ARG_ENABLE([double-nocopy],
    AS_HELP_STRING([--enable-double-nocopy], [Utilizes the other set of rules in bconv.c]), [
        AC_DEFINE([FREESTANDING], [], ["A switch for bconv."])
])

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
