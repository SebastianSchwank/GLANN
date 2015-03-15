# GLANN


A Artificial Neural Network running on GPU(GLSL) written in C++ in the Framework Qt

- The current IO is hardcoded in the void GLANN::paintGL() routine and just (tries to) modulate a sine function depending on the activated Input Neuron.

- ATM. the values are between [0..1( and [-1..1(.

- note that the programm hard exits if you input values over [0..1( for the input and [-1..1( for the error.
- 


- Some Images:
- 

![alt tag](https://github.com/schwenk/GLANN/blob/master/learningToModulateASineFunction.png)
![alt tag](https://github.com/schwenk/GLANN/blob/master/learningToModulateASineFunction1.png)
![alt tag](https://github.com/schwenk/GLANN/blob/master/learningToModulateASineFunction2.png)


