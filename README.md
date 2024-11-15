# install opencv in beagle bone and host computer
# Build commands:
```bash
# Regenerate build/ folder and makefiles:
rm -rf build/               # Wipes temporary build folder
cmake -S . -B build         # Generate makefiles in build

# Build (compile & link) the project
cmake --build build
```


Inside the folder, there are two subfolders - host and target. Each contains OpenCV libraries.

## host (host computer):
Running cmake (the same way we did for assignments 1 and 2) will generate an executable file ('project_host') in the 'build/app' directory. 
```bash
# Change to the correct directory (otherwise it will have issues finding file paths):
# assuming we are currently inside the "...project/host"
cd build/app
sudo ./project_host
```

The program creates a window to display the video feed received from a camera. The video appears somewhat stuttering, which I suspect is due to the BeagleBone's processor not being powerful enough. Additionally, the program outputs distance information of the detected objects (it can only detect persons and cars). Also, the program sorts the distances and sends the shortest distance to the 'project_target'.


## target (Beagle Bone):
Running cmake will generate an executable file (project_target) in the mnt/remote/myapps directory. 
'project_target' can only run when the BeagleBone is connected to a camera. It is executed with sudo ./project_target (I couldn't run it without 'sudo'. I think it is because there are some commands in the code require sudo privileges).
```bash
# run this on target (bbg) inside directory /mnt/remote/myApps:
sudo ./project_target
```

=================================================================================================

# Modules

## Joystick: 
pressed upward(hold for about 1s): will turn on the info and hightlight box display on the image (by default, it is enabled)
pressed downward (hold for about 1s): will turn off the info and hightlight box display on the image
pressed middle (hold for about 1s): terminate the program (botn project_host and project_target)

## segment display:
Show the nearest distance of the deteced objects in the image. It will work in red zen cape. 

## buzzer:
BY default, it is off. When the nearest distance is smaller than 2.5m, the buzzer will turn on. When the nearest distance is larger than 2.5m, it will turn off.

## LED:
BY default, it is green. When the nearest distance is smaller than 2.5m, the LED is red. When the nearest distance is larger than 2.5m, it will be green again. 

## UDP Socket (camera.cpp, joystick.cpp, rec_dst.cpp):
Send video (received from the camera frame) to the 'project_host'. Send messasge (joystick direction info) to the 'project_host'. And receive message (nearest distance) from the 'project_host'.

