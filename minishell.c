#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include "/usr/local/opt/readline/include/readline/readline.h"
//#include "/usr/local/opt/readline/include/readline/history.h"
#include <signal.h>

#include "libft/libft.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include "expander/expander.h"
#include "execute/execute.h"
#include "utils/get_next_line.h"

//	rl_on_new_line();
//	rl_replace_line("", 0); // <- 現状、何故かこの関数だけがコンパイルエラーとなり動かない
//	rl_redisplay();

// 何らかのSIGNAL(Ctrl-C(SIGINT), Ctrl-\(SIGQUIT))を受け取った時の挙動を定義する
static void	signal_handler(int signo)
{
//	printf("signo: %d\n", signo);
	if (signo == 2)
	{
		rl_on_new_line();
		rl_redisplay();
	}
}

void	set_signal_handler(void)
{
	signal(SIGINT, &signal_handler);
	// signal(SIGQUIT, &signal_handler);
}

int minishell(char *line)
{
	t_env_var	*env_vars;
	int 		exit_status;
	int on = 1;

	env_vars = init_env_lst();

	// todo: null check
	if (register_env_var(ft_strdup("?"), ft_strdup("0"), &env_vars) == MALLOC_ERROR)
		exit(delete_env_lst(env_vars, NULL, NULL));
	if (line)
		exit(execute(parse(lex(line)), &env_vars));
	ioctl(STDIN_FILENO, FIONBIO, &on);
	get_next_line(STDIN_FILENO, &line);
	if (line[0])
		exit(execute(parse(lex(line)), &env_vars));
	exit_status = EXIT_SUCCESS;
	set_signal_handler();
	while (1)
	{
		line = readline("minishell> ");
		if (!line)
			break ;
		// line can be NULL when Ctrl+d
		t_token *token = lex(line);
		t_ast_node *node = parse(token);
		if (!node)
		{
			free(line);
			continue;
		}
		// todo: No need to pass arguments `char **env`?
		exit_status = execute(node, &env_vars);
		add_history(line);
		free(line);
	}
	ft_putstr_fd("exit\n", STDERR_FILENO);
	return (exit_status);
}

int	main(int argc, char **argv)
{
	if (argc == 3 && !ft_strcmp(argv[1], "-c"))
		return (minishell(argv[2]));
	return (minishell(NULL));
}
