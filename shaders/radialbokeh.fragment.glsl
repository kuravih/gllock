//https://www.shadertoy.com/view/XtjGWm
// Bokeh disc.
// by David Hoskins.
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
#version 330 core

// ---- gllock required fields -----------------------------------------------------------------------------------------
#define RATE 0.75

uniform float time;
uniform float end;
uniform sampler2D imageData;
uniform vec2 screenSize;
// ---------------------------------------------------------------------------------------------------------------------

#define MIN_SIZE 0.0f
#define MAX_SIZE 0.5f

#define PI 3.141596
// This is (3.-sqrt(5.0))*PI radians, which doesn't precompiled for some reason.
// The compiler is a dunce I tells-ya!!
#define GOLDEN_ANGLE 2.39996323
#define NUMBER 150.0
#define ITERATIONS (GOLDEN_ANGLE * NUMBER)

//-------------------------------------------------------------------------------------------
// This creates the 2D offset for the next point.
// (r-1.0) is the equivalent to sqrt(0, 1, 2, 3...)
vec2 Sample(in float theta, inout float r) {
  r += 1.0 / r;
  return (r-1.0) * vec2(cos(theta), sin(theta));
}

//-------------------------------------------------------------------------------------------
vec3 Bokeh(sampler2D tex, vec2 uv, float radius, float amount) {
  vec3 acc = vec3(0.0);
  vec3 div = vec3(0.0);
  vec2 pixel = vec2(screenSize.y/screenSize.x, 1.0) * radius * .006;
  float r = 1.0;
  for (float j = 0.0; j < ITERATIONS; j += GOLDEN_ANGLE) {
    vec3 col = texture(tex, uv + pixel * Sample(j, r), radius*1.5).xyz;
    vec3 bokeh = vec3(5.0) + pow(col, vec3(9.0)) * amount;
    acc += col * bokeh;
    div += bokeh;
  }
  return acc / div;
}

void main(void) {
  float shaderTime = smoothstep(0.0,1.0,(time-end)*RATE);
  shaderTime = (end==0)?shaderTime:(1.0-shaderTime);

  float radius = mix(MIN_SIZE, MAX_SIZE, shaderTime); //Returns the linear blend of MIN_SIZE and MAX_SIZE

  vec2 uv = gl_FragCoord.xy / screenSize;
  float a = 10.0;
  vec2 xy = vec2(uv.x,1.0-uv.y);
  float dis = distance(gl_FragCoord.xy, screenSize/2.0);
  gl_FragColor = vec4(Bokeh(imageData, xy, radius*dis*0.001, a), 1.0);
}
//