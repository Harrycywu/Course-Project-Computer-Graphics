# Course-Project-Computer-Graphics
Course Project Computer Graphics (C++, OpenGL)

Course: CS 450 - Introduction to Computer Graphics

Term: Fall 2021

**Course Description:**

Theoretical and practical treatment of 3D computer graphics using OpenGL: geometric modeling, transformations, viewing, lighting, texture mapping, shading, rendering, and animation.  

Course Website: http://web.engr.oregonstate.edu/~mjb/cs550

Course Instructor: Mike Bailey

My Grade: A (Got 1049 out of 1060 points)

# Final Project - Animated 3D Graphics
**Introduction**

The goal of this assignment is to give you a chance to apply all that you have learned to a project of your own choosing.

* Created a scene with several animated 3D graphics with OpenGL C++

I developed my own galaxy with a partial solar system (i.e., Sun-Moon-Earth) and several orbiting and spinning textured stars. A distorted textured sun and a cat-living star are stationary, and others are moving and rotating. For a fixed star, several planets are orbiting it. A textured car is transferring back and forth between two systems through textured torus and peculiar wormholes (i.e., Animated Bézier curves). And animal objects are living inside the stars. Also, I enabled several keys (0~5: lights and f: animation) to control the lights and animation, and I added nine viewing options from different positions.

Note: 

Texture images are from the following website: https://www.solarsystemscope.com/textures/

**How to run the program**

Use the Makefile provided to compile the main program: fp.cpp

`g++ -framework OpenGL -framework GLUT fp.cpp -o fp -Wno-deprecated`
```
$ make
$ ./fp
```

**Demo Video Link**

https://media.oregonstate.edu/media/t/1_401hn5i3

**Other Projects Demo Video Link**

Project requirements can refer to the course website.

Project #1 - Draw Something Cool in 3D!: https://media.oregonstate.edu/media/t/1_odv7vl3s

Project #2 - Animate a Helicopter!: https://media.oregonstate.edu/media/t/1_f0gnf3ii

Project #3 - Texture Mapping: https://media.oregonstate.edu/media/t/1_2q3p7tux

Project #4 - Lighting: https://media.oregonstate.edu/media/t/1_bppirbxl

Project #5 - Shaders: https://media.oregonstate.edu/media/t/1_mx19yz6o

Project #6 - Geometric Modeling: https://media.oregonstate.edu/media/t/1_ho35fx33
