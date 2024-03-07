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

#ifndef STRBUF_H
#define STRBUF_H

#ifdef PK_IS_DEBUG
extern unsigned sb_resize_count;
#endif

typedef struct
{
	char *data;
	size_t size;
	size_t capacity;

} stringbuffer;

stringbuffer *sballoc(size_t capacity);

void sbputc(stringbuffer *strbuf, char c);

void sbprint(stringbuffer *strbuf, const char *src);

void sbprintf(stringbuffer *strbuf, const char *format, ...);

void sbnprintf(stringbuffer *strbuf, size_t length, const char *format, ...);

void sbfree(stringbuffer *strbuf);

bool starts_with(const char *str, const char *prefix);

const char *trim_prefix(const char *str, const char *prefix);

const char *find_char(const char *s, char c);

#endif /* STRBUF_H */