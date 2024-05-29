/****************************************************************************
**
** Copyright 2023, 2024 Jiamu Sun
** Contact: barroit@linux.com
**
** This file is part of PassKeeper.
**
** PassKeeper is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation, either version 3 of the License, or (at your
** option) any later version.
**
** PassKeeper is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with PassKeeper. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef HELPER_H
#define HELPER_H

#define is_pow2(x)\
	( (x) != 0 && ( (x) & ( (x) - 1 ) ) == 0 )

/**
 * check if `x__` is in range `r1` and `r2` exclusive
 */
#define in_range_e(x, r1, r2)\
	( (x) > (r1) && (x) < (r2) )

/**
 * check if `x__` is in range `r1` and `r2` inclusive
 */
#define in_range_i(x, r1, r2)\
	( (x) >= (r1) && (x) <= (r2) )

static inline FORCEINLINE size_t st_add(size_t x, size_t y)
{
	if ((SIZE_MAX - x) < y)
	{
		die("size_t overflow (%"PRIuMAX" + %"PRIuMAX")", x, y);
	}

	return x + y;
}

#define st_add3(a, b, c) st_add(st_add(a, b), c)

static inline FORCEINLINE size_t st_mult(size_t x, size_t y)
{
	if ((SIZE_MAX / x) < y)
	{
		die("size_t overflow (%"PRIuMAX" * %"PRIuMAX")", x, y);
	}

	return x * y;
}

#define FLEX_ARRAY

#define fixed_growth(len)\
	( st_mult(st_add(len, 16), 3) / 2 )

#define MOVE_ARRAY(dest, src, size)\
	memmove(dest, src, st_mult(sizeof(*(src)), size))

#define REALLOC_ARRAY(ptr, size)\
	xrealloc(ptr, st_mult(sizeof(*(ptr)), size))

#define FLEX_ALLOC_ARRAY(obj, field, data, len)				\
	do								\
	{								\
		(obj) = xmalloc(st_add3(sizeof(*(obj)), len, 1));	\
		memcpy((obj)->field, data, len);			\
	}								\
	while (0)

#define CAPACITY_GROW(ptr, size, cap)					\
	do								\
	{								\
		if ((size) > (cap))					\
		{							\
			(cap) = fixed_growth(cap);			\
			(cap) = (cap) < (size) ? (size) : (cap);	\
			(ptr) = REALLOC_ARRAY(ptr, cap);		\
		}							\
	}								\
	while (0)

#ifdef PK_DEBUG
#define debug_run() if (1)
#else
#define debug_run() if (0)
#endif

#define require_success(rescode, succode)		\
	do						\
	{						\
		if ((rescode) != (succode))		\
		{					\
			exit(EXIT_FAILURE);		\
		}					\
	}						\
	while (0)

#define EOE(rescode) require_success(rescode, 0)

#endif /* HELPER_H */
