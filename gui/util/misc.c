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

#include <ctype.h>
#include <string.h>

#include "misc.h"

/**
 * @brief Read characters from @a file.
 *
 * @param str memory location of a buffer to receive the read characters
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

/**
 * @brief Constrain a @a value to be in the range of 0 to 100.
 *
 * @param value value to be checked
 *
 * @return a value in the range of 0 to 100
 */
float constrain(float value)
{
    if (value < 0.0f)
        return 0.0f;
    if (value > 100.0f)
        return 100.0f;

    return value;
}

/**
 * @brief Convert MM:SS:FF (minute/second/frame) to seconds.
 *
 * @param msf string in MM:SS:FF format
 *
 * @return seconds equivalent to @a msf (0 in case of error)
 */
float msf2sec(const char *msf)
{
    int i;

    for (i = 0; i < 8; i++)
        switch (i) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 6:
        case 7:
            if (!isdigit(msf[i]))
                return 0.0f;
            break;

        case 2:
        case 5:
            if (msf[i] != ':')
                return 0.0f;
            break;
        }

    return (msf[0] - '0') * 600 + (msf[1] - '0') * 60 +
           (msf[3] - '0') * 10 + (msf[4] - '0') +
           ((msf[6] - '0') * 10 + (msf[7] - '0')) / 75.0f;
}
