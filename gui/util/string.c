/*
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * @file
 * @brief String utilities
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "gui/app/gui.h"

/**
 * @brief Convert a string to lower case.
 *
 * @param string to be converted
 *
 * @return converted string
 *
 * @note Only characters from A to Z will be converted and this is an in-place conversion.
 */
char *strlower(char *in)
{
    char *p = in;

    while (*p) {
        if (*p >= 'A' && *p <= 'Z')
            *p += 0x20;

        p++;
    }

    return in;
}

/**
 * @brief Convert a string to upper case.
 *
 * @param string to be converted
 *
 * @return converted string
 *
 * @note Only characters from a to z will be converted and this is an in-place conversion.
 */
char *strupper(char *in)
{
    char *p = in;

    while (*p) {
        if (*p >= 'a' && *p <= 'z')
            *p -= 0x20;

        p++;
    }

    return in;
}

/**
 * @brief Swap characters in a string.
 *
 * @param in string to be processed
 * @param from character to be swapped
 * @param to character to swap in
 *
 * @return processed string
 *
 * @note All occurrences will be swapped and this is an in-place processing.
 */
char *strswap(char *in, char from, char to)
{
    char *p = in;

    while (*p) {
        if (*p == from)
            *p = to;

        p++;
    }

    return in;
}

/**
 * @brief Remove all space characters from a string,
 *        but leave text enclosed in quotation marks untouched.
 *
 * @param in string to be processed
 *
 * @return processed string
 *
 * @note This is an in-place processing.
 */
char *trim(char *in)
{
    char *src, *dest;
    int freeze = False;

    src = dest = in;

    while (*src) {
        if (*src == '"')
            freeze = !freeze;

        if (freeze || (*src != ' '))
            *dest++ = *src;

        src++;
    }

    *dest = 0;

    return in;
}

/**
 * @brief Remove a comment from a string,
 *        but leave text enclosed in quotation marks untouched.
 *
 *        A comment starts either with a semicolon anywhere in the string
 *        or with a number sign character at the very beginning.
 *
 * @param in string to be processed
 *
 * @return string without comment
 *
 * @note This is an in-place processing, i.e. @a in will be shortened.
 */
char *decomment(char *in)
{
    char *p;
    int nap = False;

    p = in;

    if (*p == '#')
        *p = 0;

    while (*p) {
        if (*p == '"')
            nap = !nap;

        if ((*p == ';') && !nap) {
            *p = 0;
            break;
        }

        p++;
    }

    return in;
}

/**
 * @brief Extract a part of a string delimited by a separator character.
 *
 * @param in string to be analyzed
 * @param out pointer suitable to store the extracted part
 * @param sep separator character
 * @param num number of separator characters to be skipped before extraction starts
 * @param maxout maximum length of extracted part (including the trailing null byte)
 */
void cutString(char *in, char *out, char sep, int num, size_t maxout)
{
    int n;
    unsigned int i, c;

    for (c = 0, n = 0, i = 0; in[i]; i++) {
        if (in[i] == sep)
            n++;
        if (n >= num && in[i] != sep && c + 1 < maxout)
            out[c++] = in[i];
        if (n >= num && in[i + 1] == sep)
            break;
    }

    if (c < maxout)
        out[c] = 0;
}

/**
 * @brief Extract a numeric part of a string delimited by a separator character.
 *
 * @param in string to be analyzed
 * @param sep separator character
 * @param num number of separator characters to be skipped before extraction starts
 *
 * @return extracted number (numeric part)
 */
int cutInt(char *in, char sep, int num)
{
    char tmp[64];

    cutStr(in, tmp, sep, num);

    return atoi(tmp);
}

/**
 * @brief A strchr() that can handle NULL pointers.
 *
 * @param str string to examine
 * @param c character to find
 *
 * @return return value of strchr() or NULL, if @a str is NULL
 */
char *gstrchr(const char *str, int c)
{
    if (!str)
        return NULL;

    return strchr(str, c);
}

/**
 * @brief A strcmp() that can handle NULL pointers.
 *
 * @param a string to be compared
 * @param b string which is compared
 *
 * @return return value of strcmp() or -1, if @a a or @a b are NULL
 */
int gstrcmp(const char *a, const char *b)
{
    if (!a && !b)
        return 0;
    if (!a || !b)
        return -1;

    return strcmp(a, b);
}

/**
 * @brief A strncmp() that can handle NULL pointers.
 *
 * @param a string to be compared
 * @param b string which is compared
 * @param n number of characters compared at the most
 *
 * @return return value of strncmp() or -1, if @a a or @a b are NULL
 */
int gstrncmp(const char *a, const char *b, size_t n)
{
    if (!a && !b)
        return 0;
    if (!a || !b)
        return -1;

    return strncmp(a, b, n);
}

/**
 * @brief Duplicate a string.
 *
 *        If @a str is NULL, it returns NULL.
 *        The string is duplicated by calling strdup().
 *
 * @param str string to be duplicated
 *
 * @return duplicated string (newly allocated)
 */
char *gstrdup(const char *str)
{
    if (!str)
        return NULL;

    return strdup(str);
}

/**
 * @brief Assign a duplicated string.
 *
 *        The string is duplicated by calling #gstrdup().
 *
 * @param old pointer to a variable suitable to store the new pointer
 * @param str string to be duplicated
 *
 * @note @a *old is freed prior to the assignment.
 */
void setdup(char **old, const char *str)
{
    free(*old);
    *old = gstrdup(str);
}

/**
 * @brief Assign a newly allocated string
 *        containing the path created from a directory and a filename.
 *
 * @param old pointer to a variable suitable to store the new pointer
 * @param dir directory
 * @param name filename
 *
 * @note @a *old is freed prior to the assignment.
 */
void setddup(char **old, const char *dir, const char *name)
{
    free(*old);
    *old = malloc(strlen(dir) + strlen(name) + 2);
    if (*old)
        sprintf(*old, "%s/%s", dir, name);
}
