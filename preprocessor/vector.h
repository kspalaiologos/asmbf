
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <assert.h>
#include <stdlib.h>

#define vector(type) type *

#define _internal_veccap(vec, size)         \
	do {                                    \
		if (vec) {                          \
			((size_t *)(vec))[-1] = (size); \
		}                                   \
	} while (0)

#define _internal_vecsiz(vec, size)         \
	do {                                    \
		if (vec) {                          \
			((size_t *)(vec))[-2] = (size); \
		}                                   \
	} while (0)

#define vector_capacity(vec) \
	((vec) ? ((size_t *)(vec))[-1] : (size_t)0)

#define vector_size(vec) \
	((vec) ? ((size_t *)(vec))[-2] : (size_t)0)

#define vector_empty(vec) \
	(vector_size(vec) == 0)

#define vector_grow(vec, count)                                               \
	do {                                                                      \
		const size_t cv_sz = (count) * sizeof(*(vec)) + (sizeof(size_t) * 2); \
		if (!(vec)) {                                                         \
			size_t *cv_p = malloc(cv_sz);                                     \
			assert(cv_p);                                                     \
			(vec) = (void *)(&cv_p[2]);                                       \
			_internal_veccap((vec), (count));                                 \
			_internal_vecsiz((vec), 0);                                       \
		} else {                                                              \
			size_t *cv_p1 = &((size_t *)(vec))[-2];                           \
			size_t *cv_p2 = realloc(cv_p1, (cv_sz));                          \
			assert(cv_p2);                                                    \
			(vec) = (void *)(&cv_p2[2]);                                      \
			_internal_veccap((vec), (count));                                 \
		}                                                                     \
	} while (0)

#define vector_pop_back(vec)                           \
	do {                                               \
		_internal_vecsiz((vec), vector_size(vec) - 1); \
	} while (0)

#define vector_erase(vec, i)                                   \
	do {                                                       \
		if (vec) {                                             \
			const size_t cv_sz = vector_size(vec);             \
			if ((i) < cv_sz) {                                 \
				_internal_vecsiz((vec), cv_sz - 1);            \
				size_t cv_x;                                   \
				for (cv_x = (i); cv_x < (cv_sz - 1); ++cv_x) { \
					(vec)[cv_x] = (vec)[cv_x + 1];             \
				}                                              \
			}                                                  \
		}                                                      \
	} while (0)

#define vector_free(vec)                         \
	do {                                         \
		if (vec) {                               \
			size_t *p1 = &((size_t *)(vec))[-2]; \
			free(p1);                            \
		}                                        \
	} while (0)

#define vector_begin(vec) \
	(vec)

#define vector_end(vec) \
	((vec) ? &((vec)[vector_size(vec)]) : NULL)

#define vector_push_back(vec, value)                    \
	do {                                                \
		size_t cv_cap = vector_capacity(vec);           \
		if (cv_cap <= vector_size(vec)) {               \
			vector_grow((vec), cv_cap + 16);            \
		}                                               \
		(vec)[vector_size(vec)] = (value);              \
		_internal_vecsiz((vec), vector_size(vec) + 1);  \
	} while (0)


#endif
