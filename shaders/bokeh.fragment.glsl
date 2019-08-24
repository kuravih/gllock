// https://www.shadertoy.com/view/XsfSRr
#version 330 core

// ---- gllock required fields -----------------------------------------------------------------------------------------
#define RATE 0.75

uniform float time;
uniform float end;
uniform sampler2D imageData;
uniform vec2 screenSize;
// ---------------------------------------------------------------------------------------------------------------------

#define MIN_SIZE 0.0f
#define MAX_SIZE 2.0f

vec2 pixel;

float factor;
float radius;

vec4 tex(vec2 uv) {
  return pow(texture(imageData, uv), vec4(2.2));
}

vec4 accumCol = vec4(0.0);
vec4 accumW = vec4(0.0);

const float mas = 3.0;

void add(vec2 uv, float i, float j) {
  vec2 offset = pixel * vec2(i, j);
  vec4 col = tex(uv + offset * radius);
  vec4 bokeh = vec4(1.0) + pow(col, vec4(4.0)) * vec4(factor);
  accumCol += col * bokeh;
  accumW += bokeh;
  }

vec4 blur(vec2 uv) {
  for (float j = -7.0 + mas; j <= 7.0 - mas; j += 1.0)
    for (float i = -7.0 + mas; i <=7.0 - mas; i += 1.0)
      add(uv, i, j);
    
  for (float i = -5.0 + mas; i <=5.0 - mas; i+=1.0) {
    add(uv, i, -8.0 + mas);
    add(uv, i, 8.0 - mas);
  }
  for (float j = -5.0 + mas; j <=5.0 - mas; j+=1.0) {
    add(uv, -8.0 + mas, j);
    add(uv, 8.0 - mas, j);
  }

  for (float i = -3.0 +mas; i <=3.0 -  mas; i+=1.0) {
    add(uv, i, -9.0 + mas);
    add(uv, i, 9.0 - mas);
  }
  for (float j = -3.0 + mas; j <=3.0 - mas; j+=1.0) {
    add(uv, -9.0 + mas, j);
    add(uv, 9.0 - mas, j);
  }

  return accumCol/accumW;
}

void main(void) {

  pixel = 1.0/screenSize;

  float shaderTime = smoothstep(0.0,1.0,(time-end)*RATE);
  shaderTime = (end==0)?shaderTime:(1.0-shaderTime);

  radius = mix(MIN_SIZE, MAX_SIZE, shaderTime); //Returns the linear blend of MIN_SIZE and MAX_SIZE
  factor = radius*16.0;  

  vec2 uv = gl_FragCoord.xy / screenSize;
  gl_FragColor = pow(blur(vec2(0.0, 1.0) + uv * vec2(1.0, -1.0)),1.0/vec4(2.2));
}