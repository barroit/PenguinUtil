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

#include "parse-options.h"
#include "strbuf.h"

static struct
{
	const char *sitename;
	const char *siteurl;
	const char *username;
	const char *password;
	const char *authtext;
	const char *bakcode;
	const char *comment;
	// const char *sqltime
	// const char *modtime,
} record;

const char *const cmd_create_usages[] = {
	"pk create <fields>",
	NULL,
};

const struct option cmd_create_options[] = {
	OPTION_STRING(0, "sitename", &record.sitename, "name", "human readable name of a website"),
	OPTION_STRING(0, "siteurl ",  &record.siteurl, "url",  "url that used for disambiguation"),
	OPTION_STRING(0, "username", &record.username, "name", "identification that can be used to login"),
	OPTION_STRING(0, "password", &record.password, "pwd",  "secret phrase that can be used to login"),
	OPTION_STRING(0, "authtext", &record.authtext, "ans",  "predefined question selected during account creation"),
	OPTION_STRING(0, "bakcode ",  &record.bakcode, "code", "backup code used for account recovery purposes"),
	OPTION_STRING(0, "comment ",  &record.comment, "text", "you just write what the fuck you want to"),
	OPTION_END(),
};

int cmd_create(int argc, const char **argv, UNUSED const char *prefix)
{
	argc = parse_options(argc, argv, prefix, cmd_create_options, cmd_create_usages, PARSER_ABORT_NON_OPTION);
	// re

	return 0;
}