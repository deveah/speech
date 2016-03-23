
/*
 *  genetic.c ~ speech synthesis toy project
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

#include "audiobuffer.h"
#include "genetic.h"
#include "speech.h"
#include "synth.h"

/*
 *  alloc_phenotype() -- allocates a phenotype structure and fills it with
 *  default data;
 *  @return {struct phenotype *} -- the allocated phenotype structure.
 */
struct phenotype *alloc_phenotype(void)
{
  unsigned int i;
  struct phenotype *p = (struct phenotype*) malloc(sizeof(struct phenotype));
  
  for (i = 0; i < PHENOTYPE_CHROMOSOME_COUNT; i++) {
    p->coefficient[i] = 0.0f;
  }

  p->fitness = 0.0f;

  return p;
}

/*
 *  free_phenotype() -- frees a given phenotype structure;
 *  @arg {struct phenotype *} p -- the phenotype structure in question.
 */
void free_phenotype(struct phenotype *p)
{
  free(p);
}

/*
 *  calculate_phenotype_fitness() -- calculates the fitness of a given phenotype
 *  by synthesizing a signal, passing it through a filter whose coefficients are
 *  taken from the phenotype's genes, and then calculating the mean square error
 *  between the reference buffer and the synthesized buffer;
 *  @arg {struct phenotype *} p -- the phenotype in question;
 *  @return {float}             -- the phenotype's fitness.
 */
float calculate_phenotype_fitness(struct phenotype *p)
{
  float fitness = 0.0f;
  struct audio_buffer *buf = generate_base_speech_signal(400.0f, SAMPLE_RATE);

  process_filter_from_phenotype(p, buf, 0, SAMPLE_RATE);
  fitness = compare_audio_buffers(buf, reference_buffer);

  free_buffer(buf);
  return fitness;
}

/*
 *  fill_population_fitness() -- traverses an entire population of phenotypes
 *  and calculates each individual's fitness, filling it in its respective
 *  field;
 *  @arg {struct phenotype **} population -- the array of individuals;
 *  @arg {unsigned int} population_count  -- number of individuals in array;
 *  @return {void}.
 */
void fill_population_fitness(struct phenotype **population,
                             unsigned int population_count)
{
  unsigned int i;

  for (i = 0; i < population_count; i++) {
    population[i]->fitness = calculate_phenotype_fitness(population[i]);
  }
}

/*
 *  compare_fitness() -- compares two phenotypes by their fitness; this
 *  function is only used internally by `sort_population_by_fitness';
 *  @arg {const void *} a -- first phenotype to compare;
 *  @arg {const void *} b -- second phenotype to compare;
 *  @return {int}         -- comparison result (less than, equal to, or greater
 *                           than zero).
 */
int compare_fitness(const void *a, const void *b)
{
  struct phenotype *p_a = (struct phenotype *)a,
                   *p_b = (struct phenotype *)b;

  float res = p_b->fitness - p_a->fitness;

  if (res > 0.0001f)
    return 1;

  if (res < -0.0001f)
    return -1;

  return 0;
}

/*
 *  sort_population_by_fitness() -- sorts a population array by its individuals'
 *  fitness, in descending order;
 *  @arg {struct phenotype **} population -- the population array to be sorted;
 *  @arg {unsigned int} population_count  -- number of individuals in
 *                                           population array;
 *  @return {void}.
 */
void sort_population_by_fitness(struct phenotype **population,
                                unsigned int population_count)
{
  qsort(population, population_count, sizeof(struct phenotype *), compare_fitness);
}

/*
 *  get_best_of_random_two() -- randomly picks two phenotypes from the
 *  population, and returns the best of those two picked;
 *  @arg {struct phenotype **} population -- population to choose from;
 *  @arg {unsigned int} population_count  -- number of phenotypes in population;
 *  @return {struct phenotype *}          -- the best of two phenotypes.
 */
struct phenotype *get_best_of_random_two(struct phenotype **population,
                                         unsigned int population_count)
{
  unsigned int a, b;
  struct phenotype *p_a, *p_b;

  do {
    a = rand() % population_count;
    b = rand() % population_count;
  } while (a == b);

  p_a = population[a];
  p_b = population[b];

  if (p_a->fitness > p_b->fitness) {
    return p_b;
  }

  return p_a;
}

/*
 *  combine_phenotypes() -- combines two phenotypes by randomly taking
 *  chromosomes from each, and creates a new phenotype; please note that this
 *  uses the `rand' function, thus raising the need for setting a random seed
 *  on program initialization; also, please note that the created phenotype
 *  is allocated here, thus it is necessary to handle its destruction separately;
 *  @arg {struct phenotype *} a  -- first phenotype to combine;
 *  @arg {struct phenotype *} b  -- second phenotype to combine;
 *  @result {struct phenotype *} -- resulting phenotype.
 */
struct phenotype *combine_phenotypes(struct phenotype *a, struct phenotype *b)
{
  unsigned int i;

  struct phenotype *result = alloc_phenotype();

  for (i = 0; i < PHENOTYPE_CHROMOSOME_COUNT; i++) {
    if (rand() % 100 > 50) {
      result->coefficient[i] = a->coefficient[i];
    } else {
      result->coefficient[i] = b->coefficient[i];
    }
  }

  return result;
}

/*
 *  create_generation() -- creates a generation of phenotypes;
 *  @arg {unsigned int} phenotype_count -- number of phenotypes in generation;
 *  @return {struct phenotype **}       -- the generated array of phenotypes.
 */
struct phenotype **create_generation(unsigned int phenotype_count)
{
  unsigned int i;

  struct phenotype **generation = (struct phenotype **)malloc(
    sizeof(struct phenotype *) * phenotype_count);

  for (i = 0; i < phenotype_count; i++) {
    generation[i] = create_random_phenotype();
  }

  return generation;
}

/*
 *  create_random_phenotype() -- creates a phenotype whose chromosomes are
 *  filled with random (uniformly distributed in the [-2.0f, 2.0f) range) data;
 *  @return {struct phenotype *} -- the created phenotype.
 */
struct phenotype *create_random_phenotype(void)
{
  unsigned int i;

  struct phenotype *p = alloc_phenotype();

  for (i = 0; i < PHENOTYPE_CHROMOSOME_COUNT; i++) {
    p->coefficient[i] = 4.0f * ((float)(rand() % 10000) / 10000.0f) - 2.0f;
  }

  return p;
}

void run_generation(void) {
  unsigned int i;

  struct phenotype **generation = create_generation(100);
  struct phenotype *tournament_winners[20];

  fill_population_fitness(generation, 100);
  for (i = 0; i < 20; i++) {
    tournament_winners[i] = get_best_of_random_two(generation, 100);
    printf("%i: %f\n", i, tournament_winners[i]->fitness);
  }

  for (i = 0; i < 100; i++) {
    free_phenotype(generation[i]);
  }
  free(generation);
}

