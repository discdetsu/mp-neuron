#ifndef MP_NEURON_H
#define MP_NEURON_H

#include <stdbool.h>
#include <stddef.h>

struct mp_neuron {
	size_t n;
	int threshold;
	bool *inhibitory;
};

/*
 * mp_neuron_create - allocate a neuron with n excitatory inputs
 *
 * Returns NULL if n is 0 or allocation fails. Release the result with
 * mp_neuron_destroy().
 */
struct mp_neuron *mp_neuron_create(size_t n, int threshold);

/* mp_neuron_destroy - free a neuron; NULL is allowed. */
void mp_neuron_destroy(struct mp_neuron *neuron);

/* mp_neuron_set_inhibitory - mark input i; an index >= n is ignored. */
void mp_neuron_set_inhibitory(struct mp_neuron *neuron, size_t i,
			      bool inhibitory);

/*
 * mp_neuron_fire - evaluate the neuron
 * @inputs: neuron->n values, each 0 or 1
 *
 * Returns true if no inhibitory input is 1 and sum(inputs) >= threshold.
 */
bool mp_neuron_fire(const struct mp_neuron *neuron, const int *inputs);

/*
 * Logic gates, built as in the McCulloch-Pitts note. Each returns NULL on
 * failure and must be released with mp_neuron_destroy().
 */
struct mp_neuron *mp_gate_and(size_t n);	/* theta = n */
struct mp_neuron *mp_gate_or(size_t n);		/* theta = 1 */
struct mp_neuron *mp_gate_not(void);		/* theta = 0, inhibitory */
struct mp_neuron *mp_gate_nor(size_t n);	/* theta = 0, all inhib. */

#endif /* MP_NEURON_H */
