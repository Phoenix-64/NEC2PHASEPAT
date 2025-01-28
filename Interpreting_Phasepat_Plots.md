# How to interpret the PHASEPAT plots:



## Feed Phase Plot:
*In the upper right side.*

This plot shows the phase of the radiated wave in relationship to the rotation angle around the estimated phase centre.

### But what does that mean?
The estimated phase centre is the point from which all the waves seem to emanate. This is estimated automatically by the program. If a more accurate result is needed one can run the `P` plot type.  
The rotation angle is just how far of we are from the centre of the dish, so the `theta` value is from our radiation pattern.  
And the Feed phase angle is just the phase at which the wave arrives on the dish.  
So out of this graph, we can gather what the phase does as we increase the illumination angle, `theta`.

### What is good and what is bad?
We want the red and blue lines to stay as close to 0 as possible. Because the farther they deviate the larger our loss due to phase cancellation will get.  
This can be used to judge up to what illumination angle, or f/D, the feed would be usable.  
So in this example, the feed is usable up to around 50 degrees of half illumination angle.  
![Image](example/feed_phase_example.png)

