
/*
 *  genetic.h ~ speech synthesis toy project
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

#pragma once

#define PHENOTYPE_CHROMOSOME_COUNT 5

struct phenotype {
  float coefficient[PHENOTYPE_CHROMOSOME_COUNT];
  float fitness;
};

struct phenotype *alloc_phenotype(void);

void free_phenotype(struct phenotype *p);

float calculate_phenotype_fitness(struct phenotype *p);

void fill_population_fitness(struct phenotype **population,
                             unsigned int population_count);

int compare_fitness(const void *a, const void *b);

void sort_population_by_fitness(struct phenotype **population,
                                unsigned int population_count);

struct phenotype *combine_phenotypes(struct phenotype *a, struct phenotype *b);

struct phenotype **create_generation(unsigned int phenotype_count);
