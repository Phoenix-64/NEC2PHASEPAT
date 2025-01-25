
# Guide to using PHASEPAT32

PHASEPAT32 is a programm to compute dish illumination efficency based on NEC data



## Chosing the correct input data:


### What are the E and H fields

In Nec2 a dipole oriented along the Y axis has its magnetic field revolving around this axis on the XZ Plane  E(Phi)
And the Electric field 90 degrees to that revolving around the X axis on the YZ Plane E(Theta)


Theta: is the angle up or down from the YX plane, Describing one Horizontal plane
Phi: is the angle around the Z Axis describing one Vertical plane

The values to be chosen are in the *NORMALIZED GAIN* section of the NEC output.

For the H field file use the values of the PHI 0 column

And for the E field File use the values of the PHI 90 column


It is importand that the normalized values are used and that thy are db below pattern maximum, so no negativ sing infront.

*I am not sure if it would be better to use the RADIATION PATTERN values becasue ther one couldl seperate the oriyontal and vertical gain patterns out but would need to do the normaliyation themselfs. This has to be tested. For basic examples where the other pattern does not contribute it should be fine*

### What Phase values to chose:

The Phase values need to be extracted out of the *RADIATON PATTERNS* section of the nec output file

For the H field file use the PHI 0  E(PHI) Phase values, so the ones at the far rigth.

And for the E field file us the PHI 90, at the end, E(THETA) values


## Now for an example:

With the basic_radiator examle PHASEPAT32 uses these axis for the files
Use Overal normalized gain values and remove negativ sing 

### Values 
For the H file DB data the Vertical Plane Horizontal Gain of Phi 0 is used.
So select Vertical Plane 
Pattern Horizontal Gain
Phi 0

So from the Nec out file in the normaliyed gain section the THETA 0 to 180 PHI 0 gain values.
For the basic_radiator 0 dB for all points


For the E field DB data the Vertical Plane Vertical Gain of Phi 90 is used.
So select Vertical Pplane
Pattern Vertical Gain
Phi 90

So from the Nec out file in the normaliyed gain section the THETA 0 to 180 PHI 90 gain values.




## Running PHASEPAT32


PHASEPAT32 test run:

>Dish diameter   		                10\
>Feed support diameter 	                0.1\
>E field file    		                basic_radiator_e.dat \
>H field file			                basic_radiator_h.dat\
>Plot type				                E\
>output file			                basic_radiator.eps\
>phase center shift		                0


*The file names can also be input using command line arguments*
> PHASEPAT32.exe basic_radiator_e.dat basic_radiator_h.dat



## Phase center finding:




## Important notes 

If posible orient the radiator along the Y axis

And the main beam should point into the positiv Z direction

!! Pay attention with these normalized gain values they are the total pattern values and not the vertical and horizontal ones. In the basic_radiator example it works but not with others.

!! And also pay attention to the plane selection and verify that the E and H planes are selected aprorpiatly with the radiator orientation in the nec file.


