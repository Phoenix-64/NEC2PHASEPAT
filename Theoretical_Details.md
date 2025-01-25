
# Guide to using PHASEPAT32

PHASEPAT32 is a program to compute dish illumination efficiency based on NEC data



## Choosing the correct input data:


### What are the E and H fields

In Nec2 a dipole oriented along the Y axis has its magnetic field revolving around this axis on the XZ Plane  E(Phi)
And the Electric field 90 degrees to that revolving around the X axis on the YZ Plane E(Theta)


Theta: is the angle up or down from the YX plane, Describing one Horizontal plane
Phi: is the angle around the Z-Axis describing one Vertical plane

The values to be chosen are in the *NORMALIZED GAIN* section of the NEC output.

For the H field file use the values of the PHI 0 column

For the E field File use the values of the PHI 90 column


It is important that the normalized values are used and that they are db below the pattern maximum, so no negative sign in front.

*I am not sure if it would be better to use the RADIATION PATTERN values because one can separate the horizontal and vertical gain patterns but would need to do the normalization themselves. This has to be tested. For basic examples where the other pattern does not contribute it should be fine*

### What Phase values to choose:

The Phase values need to be extracted out of the *RADIATION PATTERNS* section of the NEC output file

For the H field file use the PHI 0  E(PHI) Phase values, so the ones at the far rigth.

And for the E field file us the PHI 90, at the end, E(THETA) values


## Now for an example:

With the basic_radiator example, PHASEPAT32 uses the following axes for the files
Use overall normalized gain values and remove negative sing 

### Values 
For the H file DB data, the vertical plane horizontal gain of Phi 0 is used.
- Select the vertical plane
- Pattern horizontal gain
- PHI 0

So from the NEC out file in the normalized gain section the THETA 0 to 180 PHI 0 gain values.
For the basic_radiator 0 dB for all points


For the E field DB data, the Vertical Plane Vertical Gain of Phi 90 is used.
- Select the vertical plane
- Pattern vertical gain
- PHI 90

So from the Nec out file in the normalized gain section the THETA 0 to 180 PHI 90 gain values.




## Running PHASEPAT32


PHASEPAT32 test run:

    Dish diameter         --->     10  
    Feed support diameter --->     0.1  
    E field file    	  --->     basic_radiator_e.dat   
    H field file	      --->     basic_radiator_h.dat  
    Plot type         	  --->     E  
    Output file			  --->     basic_radiator.eps  
    Phase centre shift	  --->     0


*The file names can also be input using command line arguments*  

`PHASEPAT32.exe basic_radiator_e.dat basic_radiator_h.dat`  




## Phase centre determination:




## Important notes 

If posible orient the radiator along the Y axis with the main beam pointing into the positive Z direction.  
Also, pay attention to the plane selection and verify that the E and H planes are selected appropriately with the radiator orientation in the NEC file.


