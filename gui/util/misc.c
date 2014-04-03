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
 * @brief Miscellaneous utilities
 */

#include <string.h>

#include "misc.h"

/**
 * @brief Read characters from @a file.
 *
 * @param str pointer to a buffer to receive the read characters
 * @param size number of characters read at the most (including a terminating null-character)
 * @param file file to read from
 *
 * @return str (success) or NULL (error)
 *
 * @note Reading stops with an end-of-line character or at end of file.
 */
char *fgetstr(char *str, int size, FILE *file)
{
    char *s;

    s = fgets(str, size, file);

    if (s)
        s[strcspn(s, "\n\r")] = 0;

    return s;
}
