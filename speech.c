
/*
 *  speech.c ~ speech synthesis toy project
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
#include <time.h>

#include <sndfile.h>

#include "audiobuffer.h"
#include "synth.h"
#include "genetic.h"
#include "speech.h"

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  srand(time(NULL));

  SF_INFO sfinfo;
  SNDFILE *out;
  struct audio_buffer *buf = NULL;

  sfinfo.samplerate = SAMPLE_RATE;
  sfinfo.channels = 1;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

  buf = generate_base_speech_signal(200.0f, SAMPLE_RATE * 5);
  /* a */ process_formant_filter(buf, 700.0f, 1300.0f,               0,     SAMPLE_RATE);
  /* e */ process_formant_filter(buf, 400.0f, 1950.0f,     SAMPLE_RATE, 2 * SAMPLE_RATE);
  /* i */ process_formant_filter(buf, 300.0f, 2200.0f, 2 * SAMPLE_RATE, 3 * SAMPLE_RATE);
  /* o */ process_formant_filter(buf, 400.0f, 1000.0f, 3 * SAMPLE_RATE, 4 * SAMPLE_RATE);
  /* u */ process_formant_filter(buf, 350.0f,  850.0f, 4 * SAMPLE_RATE, 5 * SAMPLE_RATE);

  out = sf_open("out.wav", SFM_WRITE, &sfinfo);
  sf_write_float(out, buf->data, buf->length);

  free_buffer(buf);
  sf_close(out);
  return 0;
}

