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

int cmd_count(int argc, const char **argv, const char *prefix);
int cmd_create(int argc, const char **argv, const char *prefix);
int cmd_delete(int argc, const char **argv, const char *prefix);
int cmd_help(int argc, const char **argv, const char *prefix);
int cmd_init(int argc, const char **argv, const char *prefix);
int cmd_read(int argc, const char **argv, const char *prefix);
int cmd_update(int argc, const char **argv, const char *prefix);
int cmd_version(int argc, const char **argv, const char *prefix);

const char *const cmd_pk_usages[] = {
	"usage: %s", "pk <command> [<args>]",
	NULL,
};

const struct option cmd_pk_options[] = {
	OPTION_END(),
};

struct command_info
{
	const char *name;
	int (*handle)(int argc, const char **argv, const char *prefix);
	const char *help;
};

static struct command_info commands[] = {
	{ "count",	cmd_count,	"Count the number of records" },
	{ "create",	cmd_create,	"Create a record" },
	{ "delete",	cmd_delete,	"Delete a record" },
	{ "help",	cmd_help,	"Display help information about PassKeeper" },
	{ "init",	cmd_init,	"Initialize database files for storing credentials" },
	{ "read",	cmd_read,	"Read a record" },
	{ "update",	cmd_update,	"Update a record" },
	{ "version",	cmd_version,	"Display version information about PassKeeper" },
	{ NULL },
};

static struct command_info *find_command(const char *cmdname)
{
	struct command_info *cmditer;

	cmditer = commands;
	while (cmditer->name != NULL)
	{
		if (!strcmp(cmdname, cmditer++->name))
		{
			return cmditer - 1;
		}
	}

	return NULL;
}

bool is_command(const char *cmdname)
{
	return find_command(cmdname);
}

static char *resolve_working_dir(void)
{
	size_t size = 96;
	char *buf;

	while (39)
	{
		buf = xmalloc(size);

		if (getcwd(buf, size) == buf)
		{
			return buf;
		}

		if (errno != ERANGE)
		{
			die("permission denied while getting working directory");
		}

		free(buf);
		size = fixed_growth(size);
	}

	return buf;
}

static void execute_command(struct command_info *command, int argc, const char **argv)
{
	char *prefix;

	if (command->handle == NULL)
	{
		bug("command '%s' has no handler", command->name);
	}

	prefix = resolve_working_dir();
	exit(command->handle(argc, argv, prefix));
}

static const char *fallback_command[] = { "help", "pk", NULL };

/**
 * do not use '__' as parameter prefix
 * program name should NOT be in the argv
 */
static void calibrate_argv(int *argc, const char ***argv)
{
	/* turn "pk cmd --help" into "pk help cmd" */
	if (*argc > 1 && !strcmp((*argv)[1], "--help"))
	{
		(*argv)[1] = (*argv)[0];
		(*argv)[0] = "help";
	}
	/**
	 * - no command passed, set argv to "pk help pk"
	 * - turn "pk help" into "pk help pk"
	 */
	else if (*argc == 0 || (*argc == 1 && !strcmp(**argv, "help")))
	{
		*argc = 2;
		*argv = fallback_command;
	}
	else if (!is_command(**argv))
	{
		/**
		 * turn "pk dummycmd" into "pk help dummycmd" so that
		 * command 'help' can generate messages
		 */
		fallback_command[1] = cmdname;
		*argc = 2;
		*argv = fallback_command;
	}
}

static const char *get_option_value(int *argc, const char ***argv, const char *option)
{
	if (*option == '=')
	{
		return option + 1;
	}
	else
	{
		if (*argc == 0)
		{
			return NULL;
		}

		(*argc)--;
		(*argv)++;
		return **argv;
	}
}

static void handle_parse_global_option_error(
	int *argc, const char ***argv, const char *option)
{
	error("no database path given for --db-path\n");
	*argc = 2;
	*argv = fallback_command;
}

static void handle_options(int *argc, const char ***argv)
{
	const char *option;
	const char *argval;
	while (*argc && *(option = **argv) == '-')
	{
		(*argc)--;
		(*argv)++;

		if (skip_prefix(option, "--db-path", &option))
		{
			if ((argval = get_option_value(argc, argv, option)) == NULL)
			{
				handle_parse_global_option_error();
				break;
			}
		}
		else if (!skip_prefix(option, "--key-path", &option))
		{
			//
		}
		else
		{
			//
		}
	}
}

int main(int argc, const char **argv)
{
	struct command_info *command;

	argv++;
	argc--;

	handle_options(&argc, &argv);

	calibrate_argv(&argc, &argv);

	command = find_command(*argv);
	if (command == NULL)
	{
		bug("your calibrate_command() broken");
	}

	argc--;
	argv++;

	execute_command(command, argc, argv);

	return EXIT_SUCCESS;
}

extern int option_usage_width;

void list_passkeeper_commands(void)
{
	const struct command_info *iter;
	option_usage_width = 13;

	iter = commands;
	while (iter->name)
	{
		size_t pos;

		pos = printf("   %s", iter->name);
		print_help(iter->help, pos, stdout);
		putchar('\n');

		iter++;
	}
}