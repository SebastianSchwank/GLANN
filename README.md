# GLANN
A Artificial Neural Network running on GPU(GLSL) written in C++ in the Framework Qt

- The current IO is hardcoded in the void GLANN::paintGL() routine and just (tries to) map a sine function onto itself.

- Currentliy just 1/ONE Layer is supported and I get silly stripes while rendering the weights.

- ATM. only the red image channel is used for calculations so the stepsize between 0..1 and -1..1 is very limeted.
