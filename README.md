# mp_neuron

A small C implementation of the McCulloch-Pitts (M-P) neuron, the first
mathematical model of a biological neuron (1943), written as a learning
project.

The neuron sums boolean inputs and fires (outputs 1) when the sum reaches a
hand-set threshold `theta`. An active **inhibitory** input vetoes the output
(forces it to 0). Choosing `theta` and which inputs are inhibitory yields the
basic logic gates:

| Gate | Inputs | theta |
|---|---|---|
| AND | n excitatory | n |
| OR | n excitatory | 1 |
| NOT | 1 inhibitory | 0 |
| NOR | n inhibitory | 0 |
| x1 AND NOT x2 | x1 excitatory, x2 inhibitory | 1 |

A single neuron can only represent linearly separable functions, so XOR is out
of reach.

## Build and run

```sh
make          # builds build/mp_neuron
make run      # builds and runs it
make clean    # removes build/
```

The program shows a menu: build a custom neuron or one of the AND, OR, NOT and
NOR gates, then enter inputs and read the output as many times as you like.

Compiler flags can be overridden, for example:

```sh
make CFLAGS="-std=c11 -Wall -Wextra -O2"
```

## Layout

```
include/mp_neuron.h   public API (struct mp_neuron, create/destroy/fire, gates)
src/mp_neuron.c       implementation
src/main.c            interactive program
Makefile              build rules
```

## Using the library

```c
#include "mp_neuron.h"

struct mp_neuron *g = mp_gate_and(2);
int x[] = {1, 1};

bool y = mp_neuron_fire(g, x);   /* true */
mp_neuron_destroy(g);
```

Every constructor returns `NULL` on failure, and objects are released with
`mp_neuron_destroy()`.

## License

MIT, see `LICENSE`.
