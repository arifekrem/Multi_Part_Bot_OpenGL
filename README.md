# CPS511 Assignment1

To compile the program you will need Visual Studio (used 2022) for Windows, the files Robot3D.cpp, QuadMesh.cpp, QuadMesh.h, and VECTOR3D.h.
Because we are using VS you will also need the .sln and .vcxproj makefiles. There are no extra libraries/dependencies used other than
the ones used in the Windows setup provided in class (freeglut, GLEW).

User inputs:
"W" key to start the walking animation and then to stop/reset joint angles used
"C" key to toggle the cannon spinning animation
"1" Default isometric camera angle (bonus)
"2" Front view camera angle (bonus)
"3" Side view camera angle (bonus) 
"4" Top-down view camera angle (bonus)

User inputs to select one of 6 joints, then use arrow keys to increment and decrement the selected joint angles:
"K" to select the upper left leg joint
"L" to select the lower left leg joint
"H" to select the left hip joint
"N" to select the neck, spinning the head
"B" to select the upper body
"A" to select the left ankle joint
