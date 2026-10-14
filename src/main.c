#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mp_neuron.h"

#define MAX_INPUTS 1024

enum line_result { LINE_OK, LINE_TOO_LONG, LINE_EOF };

static enum line_result read_line(char *buf, size_t size)
{
	int c;

	if (!fgets(buf, size, stdin))
		return LINE_EOF;

	if (strchr(buf, '\n'))
		return LINE_OK;

	/* No newline: last line of the input, or a line that was too long */
	c = getchar();
	if (c == EOF)
		return LINE_OK;

	while (c != '\n' && c != EOF)
		c = getchar();
	return LINE_TOO_LONG;
}

static bool parse_int(const char *s, long *out)
{
	char *end;

	errno = 0;
	*out = strtol(s, &end, 10);
	if (end == s || errno == ERANGE)
		return false;

	while (isspace((unsigned char)*end))
		end++;
	return *end == '\0';
}

static bool read_int(const char *prompt, int min, int max, int *out)
{
	char buf[64];
	long val;

	for (;;) {
		printf("%s", prompt);

		switch (read_line(buf, sizeof(buf))) {
		case LINE_EOF:
			return false;
		case LINE_TOO_LONG:
			printf("Input too long.\n");
			continue;
		case LINE_OK:
			break;
		}

		if (!parse_int(buf, &val)) {
			printf("Please enter an integer.\n");
			continue;
		}
		if (val < min || val > max) {
			printf("Please enter a value from %d to %d.\n",
			       min, max);
			continue;
		}

		*out = (int)val;
		return true;
	}
}

enum menu_choice {
	MENU_CUSTOM = 1,
	MENU_AND,
	MENU_OR,
	MENU_NOT,
	MENU_NOR,
};

static void print_menu(void)
{
	printf("McCulloch-Pitts neuron\n"
	       "  1) Custom neuron\n"
	       "  2) AND gate\n"
	       "  3) OR gate\n"
	       "  4) NOT gate\n"
	       "  5) NOR gate\n");
}

static struct mp_neuron *build_custom(void)
{
	struct mp_neuron *neuron;
	int n, threshold;

	if (!read_int("Number of inputs: ", 1, MAX_INPUTS, &n))
		return NULL;
	if (!read_int("Threshold (0 to number of inputs): ", 0, n,
		      &threshold))
		return NULL;

	neuron = mp_neuron_create(n, threshold);
	if (!neuron) {
		fprintf(stderr, "Memory allocation failed!\n");
		return NULL;
	}

	for (size_t i = 0; i < neuron->n; i++) {
		char prompt[64];
		int flag;

		snprintf(prompt, sizeof(prompt),
			 "x(%zu) 0: excitatory, 1: inhibitory: ", i + 1);
		if (!read_int(prompt, 0, 1, &flag)) {
			mp_neuron_destroy(neuron);
			return NULL;
		}
		mp_neuron_set_inhibitory(neuron, i, flag == 1);
	}

	return neuron;
}

static struct mp_neuron *build_gate(enum menu_choice choice)
{
	struct mp_neuron *neuron = NULL;
	int n;

	if (choice == MENU_NOT)		/* NOT always has one input */
		neuron = mp_gate_not();
	else if (read_int("Number of inputs: ", 1, MAX_INPUTS, &n)) {
		switch (choice) {
		case MENU_AND:
			neuron = mp_gate_and(n);
			break;
		case MENU_OR:
			neuron = mp_gate_or(n);
			break;
		case MENU_NOR:
			neuron = mp_gate_nor(n);
			break;
		default:
			break;
		}
	} else {
		return NULL;
	}

	if (!neuron)
		fprintf(stderr, "Memory allocation failed!\n");

	return neuron;
}

static bool read_inputs(const struct mp_neuron *neuron, int *inputs)
{
	for (size_t i = 0; i < neuron->n; i++) {
		char prompt[64];

		snprintf(prompt, sizeof(prompt), "Input x(%zu) [0/1]: ",
			 i + 1);
		if (!read_int(prompt, 0, 1, &inputs[i]))
			return false;
	}

	return true;
}

int main(void)
{
	struct mp_neuron *neuron = NULL;
	int *inputs = NULL;
	int status = EXIT_FAILURE;
	int choice;
	int again;

	print_menu();
	if (!read_int("Choose [1-5]: ", MENU_CUSTOM, MENU_NOR, &choice))
		goto out;

	if (choice == MENU_CUSTOM)
		neuron = build_custom();
	else
		neuron = build_gate(choice);
	if (!neuron)
		goto out;

	printf("Neuron ready: %zu input(s), threshold %d\n",
	       neuron->n, neuron->threshold);

	inputs = calloc(neuron->n, sizeof(*inputs));
	if (!inputs) {
		fprintf(stderr, "Memory allocation failed!\n");
		goto out;
	}

	do {
		if (!read_inputs(neuron, inputs))
			goto out;

		printf("Output: %d\n", mp_neuron_fire(neuron, inputs));

		/* EOF here just means the user is done */
		if (!read_int("Evaluate again? [0: no, 1: yes]: ", 0, 1,
			      &again))
			again = 0;
	} while (again);

	status = EXIT_SUCCESS;

out:
	if (status != EXIT_SUCCESS)
		fprintf(stderr, "\nStopped before a result was produced.\n");

	free(inputs);
	mp_neuron_destroy(neuron);

	return status;
}
