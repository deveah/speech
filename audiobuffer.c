
/*
 *  audiobuffer.c ~ speech synthesis toy project
 *
 *  Copyright (c) 2016, Vlad Dumitru <dalv.urtimud@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>

#include "audiobuffer.h"

/*
 *  alloc_buffer() -- allocates an audio_buffer structure, given an initial
 *  number of frames it contains.
 *  @arg {unsigned int} length      -- buffer length (in frames);
 *  @return {struct audio_buffer *} -- the allocated audio buffer.
 */
struct audio_buffer *alloc_buffer(unsigned int length)
{
  struct audio_buffer *buf = (struct audio_buffer*) malloc(sizeof(struct audio_buffer));
  buf->data = (float*) malloc(sizeof(float) * length);
  buf->length = length;

  return buf;
}

/*
 *  free_buffer() -- frees a given buffer and its contents;
 *  @arg {struct audio_buffer *} -- the buffer in question;
 *  @return {void}.
 */
void free_buffer(struct audio_buffer *buf)
{
  free(buf->data);
  free(buf);
}

