// http://www.shadertoy.com/view/4dsXWs
#version 330 core

// ---- gllock required fields -----------------------------------------------------------------------------------------
#define RATE 1.0

uniform float time;
uniform float end;
uniform sampler2D imageData;
uniform vec2 screenSize;
// ---------------------------------------------------------------------------------------------------------------------

#define MIN_SIZE 2.0f
#define MAX_SIZE 18.0f

// Compute the relative distance to the circle, where < 0.0 is outside the feathered border, and > 1.0 is inside the feathered border.
float ComputeCircle(vec2 pos, vec2 center, float radius, float feather) {
  // Determine the distance to the center of the circle.
  float dist = length(center - pos);
  
  // Use the distance and the specified feather factor to determine where the distance lies relative to the circle border.
  float start = radius - feather;
  float end   = radius + feather;
  return smoothstep(start, end, dist);
}

// The main function, which is executed once per pixel.
void main(void) {

  float shaderTime = smoothstep(0.0,1.0,(time-end)*RATE);
  shaderTime = (end==0)?shaderTime:(1.0-shaderTime);

  float diameter = mix(MIN_SIZE, MAX_SIZE, shaderTime); //Returns the linear blend of MIN_SIZE and MAX_SIZE

  float radius = diameter / 2.0;
  vec2  center = vec2(0.0);
  
  // Compute the relative distance to the circle, using mod() to repeat the circle across the display.
  // A feather value (in pixels) is used to reduce aliasing artifacts when the circles are small.
  // The position is adjusted so that a circle is in the center of the display.
  vec2 screenPos = gl_FragCoord.xy - (screenSize / 2.0) - vec2(radius);
  vec2 pos = mod(screenPos, vec2(diameter)) - vec2(radius);
  float d = ComputeCircle(pos, center, radius, 0.5);
  
  // Compute "pixelated" (stepped) texture coordinates using the floor() function.
  // The position is adjusted to match the circles, i.e. so a pixelated block is at the center of the display.
  vec2 count = screenSize / diameter;
  vec2 shift = vec2(0.5) - fract(count / 2.0);
  vec2 uv = floor(count * gl_FragCoord.xy / screenSize + shift) / count;
  
  // Sample the texture, using an offset to the center of the pixelated block.
  // NOTE: Use a large negative bias to effectively disable mipmapping, which would otherwise lead to sampling artifacts where the UVs change abruptly at the pixelated block boundaries.
  uv += vec2(0.5) / count;
  uv = clamp(uv, 0.0, 1.0);
  uv.y = 1.0 - uv.y;
  vec3 texColor = texture(imageData, uv, -32.0).rgb;  
  
  // Calculate the color based on the circle shape, mixing between that color and a background color.
  // NOTE: Set the mix factor to 0.0 to see the pixelating effect directly, without the circles.
  vec3 bg  = vec3(0.0, 0.0, 0.0);
  vec3 col = mix(texColor, bg, d);
    
  // Set the final fragment color.
  gl_FragColor = vec4(col, 1.0);
}
//