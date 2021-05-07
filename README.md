# ParticleSim_2007

This Win32 C++ project was the 5th iteration of a particle simulation I was playing with. The goal was to provide an interactive simulation of particles that behaved according to simple electromagnetic forces (the force between any two charged bodies being directly proportional to their charge divided by the square of their distance). This version also served as a learning project for doing field rendering via shaders.

The code is clean and simple. In order to run the released build, you will need to run the PhysX installer included in the "install" folder in the zip file.

### Features
- Pausable simulation
- any object can be grabbed and flung
- field visualization can either be continuous (per pixel) or lined (shows discrete field values in the way contour lines on a map indicate altitude)
- two difference field calculations
  - Subtract, uses user-defined colors to represent positive and negative charge potential across the window's client space
  - Real, uses red (x axis) and green (y axis) to indicate field vector alignment per pixel
