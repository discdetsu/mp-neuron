#include <stdlib.h>

#include "mp_neuron.h"

struct mp_neuron *mp_neuron_create(size_t n, int threshold)
{
	struct mp_neuron *neuron;

	if (n == 0)
		return NULL;

	neuron = malloc(sizeof(*neuron));
	if (!neuron)
		return NULL;

	neuron->n = n;
	neuron->threshold = threshold;
	neuron->inhibitory = calloc(n, sizeof(*neuron->inhibitory));
	if (!neuron->inhibitory) {
		free(neuron);
		return NULL;
	}

	return neuron;
}

void mp_neuron_destroy(struct mp_neuron *neuron)
{
	if (!neuron)
		return;

	free(neuron->inhibitory);
	free(neuron);
}

void mp_neuron_set_inhibitory(struct mp_neuron *neuron, size_t i,
			      bool inhibitory)
{
	if (i < neuron->n)
		neuron->inhibitory[i] = inhibitory;
}

bool mp_neuron_fire(const struct mp_neuron *neuron, const int *inputs)
{
	int sum = 0;

	for (size_t i = 0; i < neuron->n; i++) {
		if (neuron->inhibitory[i] && inputs[i] == 1)
			return false;
		sum += inputs[i];
	}

	return sum >= neuron->threshold;
}

struct mp_neuron *mp_gate_and(size_t n)
{
	return mp_neuron_create(n, (int)n);
}

struct mp_neuron *mp_gate_or(size_t n)
{
	return mp_neuron_create(n, 1);
}

struct mp_neuron *mp_gate_not(void)
{
	struct mp_neuron *neuron = mp_neuron_create(1, 0);

	if (neuron)
		mp_neuron_set_inhibitory(neuron, 0, true);

	return neuron;
}

struct mp_neuron *mp_gate_nor(size_t n)
{
	struct mp_neuron *neuron = mp_neuron_create(n, 0);

	if (!neuron)
		return NULL;

	for (size_t i = 0; i < neuron->n; i++)
		mp_neuron_set_inhibitory(neuron, i, true);

	return neuron;
}
