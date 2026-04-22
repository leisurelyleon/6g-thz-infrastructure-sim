# Joint Communication and Sensing (JCAS) Mathematics

The 6G infrastructure acts as an active radar system, utilizing the communication carrier waves to simultaneously map the physical environment. This document details the mathematical models driving the `cellular_sensing` module.

## 1. Spatial Mapping via Time-of-Flight (ToF)

To generate a 3D point cloud of the environment, the engine measures the time it takes for a THz pulse to reflect off a surface and return to the Access Point. The one-way distance is calculated as:

$$d = \frac{c \cdot \Delta t}{2}$$

Where:
* $d$ is the distance to the target in meters.
* $c$ is the speed of light ($299,792,458$ m/s).
* $\Delta t$ is the total round-trip Time-of-Flight.

## 2. Material Density Estimation

The system differentiates between materials (e.g., glass vs. concrete) by measuring signal absorption against the theoretical Free-Space Path Loss (FSPL):

$$\text{FSPL (dB)} = 20 \log_{10}(d) + 20 \log_{10}(f) - 147.55$$

If the measured Return Signal Strength Indicator (RSSI) is significantly lower than the inverted FSPL, the system infers a highly absorptive or dense material at that coordinate.

## 3. Kinetic Tracking via Spatial Doppler

The `SpatialDoppler.cpp` module tracks moving entities by measuring micro-shifts in the carrier frequency caused by the Doppler effect. The radial velocity is derived using:

$$v = \frac{f_d \cdot \lambda}{2 \cos(\theta)}$$

Where:
* $v$ is the velocity of the object.
* $f_d$ is the measured Doppler frequency shift.
* $\lambda$ is the operational wavelength (e.g., 1mm for 300 GHz).
* $\theta$ is the angle between the object's trajectory and the line-of-sight.
