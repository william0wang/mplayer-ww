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

#include <stdlib.h>
#include "osdep/timer.h"
#include "input/input.h"
#include "input/mouse.h"
#include "mp_fifo.h"

int key_fifo_size = 7;
static int *key_fifo_data;
static unsigned key_fifo_read;
static unsigned key_fifo_write;
static int previous_down_key;
unsigned doubleclick_time = 300;

void mplayer_put_key(int code){
  int fifo_free = key_fifo_read + key_fifo_size - key_fifo_write;
  if (key_fifo_data == NULL)
    key_fifo_data = malloc(key_fifo_size * sizeof(int));
  if(!fifo_free) return; // FIFO FULL!!
  // reserve some space for key release events to avoid stuck keys
  // Make sure we do not reset key state because of a down event
  if((code & MP_KEY_DOWN) && fifo_free <= (key_fifo_size >> 1))
    return;
  // in the worst case, just reset key state
  if (fifo_free == 1) {
    // ensure we do not only create MP_KEY_RELEASE_ALL events
    if (previous_down_key & MP_KEY_RELEASE_ALL)
      return;
    // HACK: this ensures that a fifo size of 2 does
    // not queue any key presses while still allowing
    // the mouse wheel to work (which sends down and up
    // at nearly the same time
    if (code != previous_down_key)
      code = 0;
    code |= MP_KEY_RELEASE_ALL;
  }
  key_fifo_data[key_fifo_write % key_fifo_size]=code;
  key_fifo_write++;
  if (code & MP_KEY_DOWN)
    previous_down_key = code & ~MP_KEY_DOWN;
  else
    previous_down_key = code & MP_KEY_RELEASE_ALL;
}

int mplayer_get_key(int fd){
  int key;
  if (key_fifo_data == NULL)
    return MP_INPUT_NOTHING;
  if(key_fifo_write==key_fifo_read) return MP_INPUT_NOTHING;
  key=key_fifo_data[key_fifo_read % key_fifo_size];
  key_fifo_read++;
  return key;
}

