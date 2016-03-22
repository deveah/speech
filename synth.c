
/*
 *  synth.c ~ speech synthesis toy project
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
#include <math.h>

#include "audiobuffer.h"
#include "genetic.h"
#include "synth.h"

/*
 *  generate_base_speech_signal() -- creates a buffer of `frame_count' frames,
 *  containing a 25% width pulse wave of `frequency' Hz.
 *  @arg {float} frequency          -- the frequency of the signal;
 *  @arg {unsigned int} frame_count -- buffer size;
 *  @return {struct audio_buffer *} -- the filled audio buffer structure.
 */
struct audio_buffer *generate_base_speech_signal(float frequency,
                                                 unsigned int frame_count)
{
  unsigned int i;
  unsigned int period = SAMPLE_RATE / frequency;

  struct audio_buffer *buf = alloc_buffer(frame_count);

  for (i = 0; i < frame_count; i++) {
      buf->data[i] = (float)((i % period) < (period / 4)) - 0.5f;
  }

  return buf;
}

/*
 *  process_formant_filter() -- processes a given audio buffer structure,
 *  passing it through two two-pole low pass filters, of frequencies `f1'
 *  and `f2';
 *
 *  code was partially taken from:
 *      http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
 *      Author: Robert Bristow-Johnson <rbj@audioimagination.com>
 *
 *  @arg {struct audio_buffer *} buf -- the source audio buffer structure;
 *  @arg {float} f1                  -- first formant frequency;
 *  @arg {float} f2                  -- second formant frequency;
 *  @arg {unsigned int} start_frame  -- first frame to be processed;
 *  @arg {unsigned int} end_frame    -- last frame to be processed;
 *  @return {void}.
 */
void process_formant_filter(struct audio_buffer *buf, float f1, float f2,
                            unsigned int start_frame, unsigned int end_frame)
{
  unsigned int i;
  float y0 = 0.0f,    /* current value of the filter's output */
        y1 = 0.0f,    /* previous value of the filter's output */
        y2 = 0.0f;    /* second previous value of the filter's output */
  float w0 = 0.0f;    /* the filter's angular frequency */
  float alpha = 0.0f; /* intermediate parameter; see below */
  float a0 = 0.0f,    /* filter parameters */
        a1 = 0.0f,
        a2 = 0.0f,
        b0 = 0.0f;

    /*  first pass -- formant f1 */
  w0 = 2.0 * M_PI * (f1 / (float)SAMPLE_RATE);
  alpha = sinf(w0) * 0.1f;
  b0 = (1.0f - cosf(w0)) / 2.0f;
  a0 = 1.0f + alpha;
  a1 = -2.0 * cosf(w0);
  a2 = 1.0f - alpha;

  printf("   w0 = %f\n", w0);
  printf("alpha = %f\n", alpha);
  printf("   b0 = %f\n", b0);
  printf("   a0 = %f\n", a0);
  printf("   a1 = %f\n", a1);
  printf("   a2 = %f\n", a2);
  printf("b0/a0 = %f\n", b0 / a0);
  printf("a1/a0 = %f\n", a1 / a0);
  printf("a2/a0 = %f\n", a2 / a0);
  printf("-----\n");

  for (i = start_frame; i < end_frame; i++) {
      y0 = (b0 / a0) * buf->data[i]
         - (a1 / a0) * y1
         - (a2 / a0) * y2;
      y2 = y1;
      y1 = y0;

      buf->data[i] = y0;
  }


    /*  second pass -- formant f2 */
  w0 = 2.0 * M_PI * (f2 / (float)SAMPLE_RATE);
  alpha = sinf(w0) * 0.001f;
  b0 = (1.0f - cosf(w0)) / 2.0f;
  a0 = 1.0f + alpha;
  a1 = -2.0 * cosf(w0);
  a2 = 1.0f - alpha;

  for (i = start_frame; i < end_frame; i++) {
      y0 = (b0 / a0) * buf->data[i]
         - (a1 / a0) * y1
         - (a2 / a0) * y2;
      y2 = y1;
      y1 = y0;

      buf->data[i] = y0;
  }
}

/*
 *  process_filter_from_phenotype() -- passws a given audio buffer through a
 *  filter whose coefficients are taken from a given phenotype;
 *  @arg {struct phenotype *} phenotype -- the phenotype from which to take the
 *                                         filter coefficients;
 *  @arg {struct audio_buffer *} buf    -- the audio buffer to be processed;
 *  @arg {unsigned int} start_frame     -- the frame index from which the
 *                                         processing begins;
 *  @arg {unsigned int} end_frame       -- the last frame index to be processed;
 *  @return {void}.
 */
void process_filter_from_phenotype(struct phenotype *p,
                                   struct audio_buffer *buf,
                                   unsigned int start_frame,
                                   unsigned int end_frame)
{
  unsigned int i, j, k;
  float memory[PHENOTYPE_CHROMOSOME_COUNT] = {0.0f};
  float temp = 0.0f;

  for (i = start_frame; i < end_frame; i++) {
    temp = p->coefficient[0] * buf->data[i];

    for (j = 1; j < PHENOTYPE_CHROMOSOME_COUNT; j++) {
      temp += p->coefficient[i] * memory[i];

      buf->data[i] = temp;

      for (k = PHENOTYPE_CHROMOSOME_COUNT; k > 0; k--) {
        memory[k] = memory[k - 1];
      }

      memory[0] = temp;
    }
  }
}

