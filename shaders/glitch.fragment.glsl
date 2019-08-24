// http://www.shadertoy.com/view/MlVSD3
#version 330 core

// ---- gllock required fields -----------------------------------------------------------------------------------------
#define RATE 0.75

uniform float time;
uniform float end;
uniform sampler2D imageData;
uniform vec2 screenSize;
// ---------------------------------------------------------------------------------------------------------------------

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453) * 2.0 - 1.0;
}

float offset(float blocks, vec2 uv) {
  float shaderTime = time*RATE;
  return rand(vec2(shaderTime, floor(uv.y * blocks)));
}

void main(void) {
  vec2 uv = vec2(1,-1)*gl_FragCoord.xy / screenSize;
  gl_FragColor = texture(imageData, uv);
  gl_FragColor.r = texture(imageData, uv + vec2(offset(64.0, uv) * 0.03, 0.0)).r;
  gl_FragColor.g = texture(imageData, uv + vec2(offset(64.0, uv) * 0.03 * 0.16666666, 0.0)).g;
  gl_FragColor.b = texture(imageData, uv + vec2(offset(64.0, uv) * 0.03, 0.0)).b;
}
//