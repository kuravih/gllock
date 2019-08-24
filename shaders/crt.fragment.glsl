// http://www.shadertoy.com/view/lt3yz7
#version 330 core

// ---- gllock required fields -----------------------------------------------------------------------------------------
#define RATE 0.75

uniform float time;
uniform float end;
uniform sampler2D imageData;
uniform vec2 screenSize;
// ---------------------------------------------------------------------------------------------------------------------

float rand(float seed){
  return fract(sin(dot(vec2(seed) ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 displace(vec2 co, float seed, float seed2) {
  vec2 shift = vec2(0);
  if (rand(seed) > 0.5) {
    shift += 0.1 * vec2(2. * (0.5 - rand(seed2)));
  }
  if (rand(seed2) > 0.6) {
    if (co.y > 0.5) {
      shift.x *= rand(seed2 * seed);
    }
  }
  return shift;
}

vec4 interlace(vec2 co, vec4 col) {
  float shaderTime = time*RATE;
  if (int(co.y) % 3 == 0) {
    return col * ((sin(shaderTime * 4.) * 0.1) + 0.75) + (rand(shaderTime) * 0.05);
  }
  return col;
}

void main(void) {

  float shaderTime = time*RATE;

  // Normalized pixel coordinates (from 0 to 1)
  vec2 uv = vec2(1,-1)*gl_FragCoord.xy / screenSize;

  vec2 rDisplace = vec2(0);
  vec2 gDisplace = vec2(0);
  vec2 bDisplace = vec2(0);
  
  if (rand(shaderTime) > 0.95) {
    rDisplace = displace(uv, shaderTime * 2., 2. + shaderTime);
    gDisplace = displace(uv, shaderTime * 3., 3. + shaderTime);
    bDisplace = displace(uv, shaderTime * 5., 5. + shaderTime);
  }
  
  rDisplace.x += 0.005 * (0.5 - rand(shaderTime * 37. * uv.y));
  gDisplace.x += 0.007 * (0.5 - rand(shaderTime * 41. * uv.y));
  bDisplace.x += 0.0011 * (0.5 - rand(shaderTime * 53. * uv.y));

  rDisplace.y += 0.001 * (0.5 - rand(shaderTime * 37. * uv.x));
  gDisplace.y += 0.001 * (0.5 - rand(shaderTime * 41. * uv.x));
  bDisplace.y += 0.001 * (0.5 - rand(shaderTime * 53. * uv.x));
  
  // Output to screen
  float rcolor = texture(imageData, uv.xy + rDisplace).r;
  float gcolor = texture(imageData, uv.xy + gDisplace).g;
  float bcolor = texture(imageData, uv.xy + bDisplace).b;

  gl_FragColor = interlace(gl_FragCoord.xy, vec4(rcolor, gcolor, bcolor, 1));
    
}
//