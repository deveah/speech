
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

/*
 *  compare_audio_buffers() -- compares two given audio buffers, calculating
 *  the mean square error; please note that the buffers have to be of the same
 *  size; comparing two buffers of different sizes will result in NAN;
 *  @arg {struct audio_buffer *} a -- first audio buffer to compare;
 *  @arg {struct audio_buffer *} b -- second audio buffer to compare;
 *  @return {float}                -- the mean square error.
 */
float compare_audio_buffers(struct audio_buffer *a, struct audio_buffer *b)
{
  unsigned int i;
  float mse = 0.0f;

  if (a->length != b->length) {
    fprintf(stderr, "Audio buffers are of different lengths.\n");
    return NAN;
  }

  for (i = 0; i < a->length; i++) {
    if (a->data[i] > 1.0f ||
        a->data[i] < -1.0f) {
      printf("wat <%p>@%i\n", (void *)a, i);
    }
    if (b->data[i] > 1.0f ||
        b->data[i] < -1.0f) {
      printf("wat <%p>@%i\n", (void *)b, i);
    }

    mse += (a->data[i] - b->data[i]) * (a->data[i] - b->data[i]);
  }

  mse = mse / (float)a->length;

  return mse;
}

