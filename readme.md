# A bridge between NEC and PHASEPAT32
This program takes a NEC output file as input and produces the e and h field .dat files required by PHASEPAT32 and [PHASEPAT](http://www.w1ghz.org/10g/software.htm) writen by [W1GHZ](http://www.w1ghz.org/).  
For the details behind the data look at [Theoretical_Details.md](Theoretical_Details.md)

## How to setup the NEC sim:
It is important that the antenna be situated at the origin and that the main lobe point in the direction of the positive Z axis.  
Try to avoid cross polarization by aligning the radiation element to one of the axes, preferably the Y axis. 
The pattern should contain the Phi angles 0 and 90 for Theta from 0 to 180 in any step size not smaller than 1 degree.  
This can be done by RP cards, here with a step size of 1 degree:  

    RP	0	181	3	1500	0	0	1	45  

*To use this just add it at the end of the NEC file before the `EN` card and in 4nec2 in the generate window, `F7`, select `Use Original File`*  
*A basic example can be found in the [example](example) folder*

## Usage of NEC2PHASEPAT:
Compile [NEC2PHASEPAT.c](NEC2PHASEPAT.c) using `gcc`:

    gcc .\NEC2PHASEPAT.c -o .\NEC2PHASEPAT.exe   

And then either give the input file as a command-line argument:

    .\NEC2PHASEPAT.exe NEC.out

Or when quoted.

The two programms can be nicely commbinded in one command-line:

    .\NEC2PHASEPAT.exe NEC.out && .\PHASEPAT32.exe pars_e.dat pars_h.dat

*The output file nemes are `pars_e.dat` and `pars_h.dat` and can not currently be changed*