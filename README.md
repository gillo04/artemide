# Artemide
Artemide is a program for simplifying electronic circuits

# Controls
To change the selected component, press the number key associated with it shown in the top left legend. At the moment, only three components are supported:
1. Wires
2. Resistors
3. Terminals
To draw a component, hold and drag the left mouse button. Both component's ends will snap to nearby nodes (the snap node is marked with a green dot).
To simplify the circuit, press **space**. Currently, the only simplifications supported are resistors in series and in parallel.

# Building and running
To build the project, simply run the command
    
    make
    
This will also start the program upon finishing compilation. To run the program afterwards, simply call it with:
    
    ./artemide
    
If you want to open a circuit file, add the file name as an argument to the program:
    
    ./artemide ./circuits/parallel.circ
    

