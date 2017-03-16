/*
 * Based on work by Rocky Bernstein <rocky@panix.com>, 2004,
 * adapted by Ingo Brückl.
 *
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

#ifndef MPLAYER_VCD_READ_LIBCDIO_H
#define MPLAYER_VCD_READ_LIBCDIO_H

#include <stdlib.h>
#include <string.h>

#if HAVE_CDIO_PARANOIA_H
#include <cdio/cdda.h>
#include <cdio/paranoia.h>
#elif HAVE_CDIO_PARANOIA_PARANOIA_H
#include <cdio/paranoia/cdda.h>
#include <cdio/paranoia/paranoia.h>
#endif

/** Private vcd data. */
typedef struct {
    track_t track;     /**< Current track being played. */
    lsn_t lsn;         /**< Current logical sector number. */
    CdIo *cdio;        /**< Pointer to internal libcdio data. */
} mp_vcd_priv_t;

/**
 * @brief Prepare reading of the Video CD.
 *
 * @param fd (unused)
 *
 * @return pointer to the newly allocated private vcd data
 */
static inline mp_vcd_priv_t *vcd_read_toc(int fd)
{
    mp_vcd_priv_t *vcd = malloc(sizeof(mp_vcd_priv_t));

    return vcd;
}

/**
 * @brief Set the track of the Video CD to be read next.
 *
 * @param vcd pointer to the private vcd data
 * @param track track to be read next
 *
 * @return sector offset in bytes or -1 (error)
 */
static int vcd_seek_to_track(mp_vcd_priv_t *vcd, int track)
{
    vcd->lsn = cdio_get_track_lsn(vcd->cdio, track);

    if (vcd->lsn == CDIO_INVALID_LSN)
        return -1;

    return M2F2_SECTOR_SIZE * vcd->lsn;
}

/**
 * @brief Determine the number of sectors of a Video CD track.
 *
 * @param vcd pointer to the private vcd data
 * @param track track to examine
 *
 * @return sector offset in bytes or -1 (error)
 */
static int vcd_get_track_end(mp_vcd_priv_t *vcd, int track)
{
    lsn_t end_lsn;

    if (vcd_seek_to_track(vcd, track) == -1)
        return -1;

    end_lsn = vcd->lsn + cdio_get_track_sec_count(vcd->cdio, track) - 1;

    if (end_lsn == CDIO_INVALID_LSN)
        return -1;

    return M2F2_SECTOR_SIZE * end_lsn;
}

/**
 * @brief Get last track number of the Video CD.
 *
 * @param vcd pointer to the private vcd data
 *
 * @return last track number
 */
static inline int vcd_end_track(mp_vcd_priv_t *vcd)
{
    return cdio_get_last_track_num(vcd->cdio);
}

/**
 * @brief Set the sector of the Video CD to be read next.
 *
 * @param vcd pointer to the private vcd data
 * @param sect sector to be read next
 */
static inline void vcd_set_msf(mp_vcd_priv_t *vcd, unsigned int sect)
{
    vcd->lsn = sect;
}

/**
 * @brief Read a sector of the Video CD.
 *
 * @note As a side effect, the sector pointer will be incremented.
 *
 * @param vcd pointer to the private vcd data
 * @param buffer pointer to a buffer suitable to store the data
 *
 * @return number of bytes read or 0 (error)
 */
static int vcd_read(mp_vcd_priv_t *vcd, char *buffer)
{
    struct {
        uint8_t subheader[CDIO_CD_SUBHEADER_SIZE];
        uint8_t data[M2F2_SECTOR_SIZE];
        uint8_t spare[4];
    } vcd_sector;

    if (cdio_read_mode2_sector(vcd->cdio, &vcd_sector, vcd->lsn, true) != 0)
        return 0;

    memcpy(buffer, vcd_sector.data, M2F2_SECTOR_SIZE);
    vcd->lsn++;

    return M2F2_SECTOR_SIZE;
}

#endif /* MPLAYER_VCD_READ_LIBCDIO_H */
