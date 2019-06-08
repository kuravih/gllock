# gllock
OpenGL extension to the simple screen locker [slock](http://github.com/anekos/slock)

Inspired by this [reddit post](https://www.reddit.com/r/unixporn/comments/3358vu/i3lock_unixpornworthy_lock_screen/) this lock screen was implemented using the simplest lock program I could find.
**gllock** will obscure the screen using an opengl texture shader.

## few examples

#### [circles](http://www.shadertoy.com/view/4dsXWs)

<p align="center">
  <img width="460" height="300" src="images/circle_shader.gif"></img>
</p>


#### [squares](http://www.shadertoy.com/view/MtfXRN)

<p align="center">
  <img width="460" height="300" src="images/square_shader.gif"></img>
</p>


#### [ascii](http://www.shadertoy.com/view/lssGDj)

<p align="center">
  <img width="460" height="300" src="images/ascii_shader.gif"></img>
</p>


#### [crt](http://www.shadertoy.com/view/lt3yz7)

<p align="center">
  <img width="460" height="300" src="images/crt_shader.gif"></img>
</p>


#### [glitch](http://www.shadertoy.com/view/MlVSD3)

<p align="center">
  <img width="460" height="300" src="images/glitch_shader.gif"></img>
</p>


## Requirements
In order to build gllock you need the following packages:
  - _x11proto-core-dev_ for `#include <X11/keysym.h>`
  - _libx11-dev_ for `#include <X11/Xlib.h>`
  - _libglew-dev_ for `#include <GL/glew.h>`

The other requirement will be fulfilled automatically when installing the above.

## Installation
Edit `config.mk` to match your local setup (**gllock** is installed into the `/usr/local` namespace by default).

- `SHADER_LOCATION` - location of the shader files (default `~/.gllock/` which is a symlink to the shader folder of the repository)
- `FRGMNT_SHADER` -  the shader file to use (default circle.fragment.glsl)

The following command builds and installs **gllock**:
    
    > sudo make clean install

## Running gllock
Simply invoke the 'gllock' command. To get out of it, enter your password.
However the typical setup involves using gllock with **xautolock**.
For example in order to run **gllock** after N minutes of inactivity the following command may be used.

    > sudo xautolock -time N -locker "gllock" &

### Note
when using a particular fragment shader you may want to figure out the supported glsl version for your system using 

    > glxinfo | grep 'version'

as explained [here](https://askubuntu.com/questions/47062/what-is-terminal-command-that-can-show-opengl-version) and then make the necessary modifications to the shader files.

Most opengl texture shaders from [Shadertoy](www.shadertoy.com) may be adapted to gllock with minimal modifications.
You may use the following substitutions as a starting point.

* use `uniform vec2 screenSize` for `uniform vec3 iResolution` : viewport resolution *pixels*. (i.e. `iResolution.xy` -> `screenSize`).
* use `uniform float time` for `uniform float iTime` : shader playback time *seconds*. (i.e. `iTime` -> `time`);
* use `uniform sampler2D imageData` for `uniform samplerXX iChannel0..3` : input texture channel. (i.e. `iChannel0` -> `imageData`);

* use `void main(void)` for `void mainImage( out vec4 fragColor, in vec2 fragCoord )` for the main function (i.e. `void mainImage(out vec4 fragColor, in vec2 fragCoord)` -> `void main(void)`);
`fragCoord` -> `gl_FragCoord` implicit input
`fragColor` -> `gl_FragColor` implicit output
