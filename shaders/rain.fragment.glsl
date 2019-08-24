// https://www.shadertoy.com/view/ltffzl
#version 330 core

// ---- gllock required fields -----------------------------------------------------------------------------------------
#define RATE 0.5

uniform float time;
uniform float end;
uniform sampler2D imageData;
uniform vec2 screenSize;
// ---------------------------------------------------------------------------------------------------------------------

#define MIN_AMOUNT 0.0f
#define MAX_AMOUNT 1.0f //odd value required

// #define CHEAP_NORMALS
// #define USE_POST_PROCESSING

vec3 N13(float p) {
  //  from DAVE HOSKINS
  vec3 p3 = fract(vec3(p) * vec3(.1031,.11369,.13787));
  p3 += dot(p3, p3.yzx + 19.19);
  return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

vec4 N14(float t) {
  return fract(sin(t*vec4(123., 1024., 1456., 264.))*vec4(6547., 345., 8799., 1564.));
}

float N(float t) {
  return fract(sin(t*12345.564)*7658.76);
}

float Saw(float b, float t) {
  return smoothstep(0., b, t)*smoothstep(1., b, t);
}

vec2 DropLayer2(vec2 uv, float t) {
  vec2 UV = uv;
  
  uv.y += t*0.75;
  vec2 a = vec2(6., 1.);
  vec2 grid = a*2.;
  vec2 id = floor(uv*grid);
  
  float colShift = N(id.x); 
  uv.y += colShift;
  
  id = floor(uv*grid);
  vec3 n = N13(id.x*35.2+id.y*2376.1);
  vec2 st = fract(uv*grid)-vec2(.5, 0);
  
  float x = n.x-.5;
  
  float y = UV.y*20.;
  float wiggle = sin(y+sin(y));
  x += wiggle*(.5-abs(x))*(n.z-.5);
  x *= .7;
  float ti = fract(t+n.z);
  y = (Saw(.85, ti)-.5)*.9+.5;
  vec2 p = vec2(x, y);
  
  float d = length((st-p)*a.yx);
  
  float mainDrop = smoothstep(.4, .0, d);
  
  float r = sqrt(smoothstep(1., y, st.y));
  float cd = abs(st.x-x);
  float trail = smoothstep(.23*r, .15*r*r, cd);
  float trailFront = smoothstep(-.02, .02, st.y-y);
  trail *= trailFront*r*r;
  
  y = UV.y;
  float trail2 = smoothstep(.2*r, .0, cd);
  float droplets = max(0., (sin(y*(1.-y)*120.)-st.y))*trail2*trailFront*n.z;
  y = fract(y*10.)+(st.y-.5);
  float dd = length(st-vec2(x, y));
  droplets = smoothstep(.3, 0., dd);
  float m = mainDrop+droplets*r*trailFront;

  //m += st.x>a.y*.45 || st.y>a.x*.165 ? 1.2 : 0.;
  return vec2(m, trail);
}

float StaticDrops(vec2 uv, float t) {
  uv *= 40.;
    
  vec2 id = floor(uv);
  uv = fract(uv)-.5;
  vec3 n = N13(id.x*107.45+id.y*3543.654);
  vec2 p = (n.xy-.5)*.7;
  float d = length(uv-p);
  
  float fade = Saw(.025, fract(t+n.z));
  float c = smoothstep(.3, 0., d)*fract(n.z*10.)*fade;
  return c;
}

vec2 Drops(vec2 uv, float t, float l0, float l1, float l2) {
  float s = StaticDrops(uv, t)*l0; 
  vec2 m1 = DropLayer2(uv, t)*l1;
  vec2 m2 = DropLayer2(uv*1.85, t)*l2;

  float c = s+m1.x+m2.x;
  c = smoothstep(.3, 1., c);

  return vec2(c, max(m1.y*l0, m2.y*l1));
}

void main(void) {

  float shaderTime = time*RATE;

  vec2 uv = (gl_FragCoord.xy-.5*screenSize) / screenSize.y;
  vec2 UV = vec2(1,-1)*gl_FragCoord.xy/screenSize;
  
  float amountTime = smoothstep(0.0,1.0,(time-end)*RATE);
  amountTime = (end==0)?amountTime:(1.0-amountTime);

  float amount = mix(MIN_AMOUNT, MAX_AMOUNT, amountTime); //Returns the linear blend of MIN_SIZE and MAX_SIZE

  float rainAmount = amount*(0.3*sin(shaderTime*0.05)+0.7);
  
  float maxBlur = mix(3.0, 6.0, rainAmount);
  float minBlur = 2.0;
    
  UV = (UV-0.5)+0.5;
  
  float staticDrops = smoothstep(-.5, 1., rainAmount)*2.;
  float layer1 = smoothstep(.25, .75, rainAmount);
  float layer2 = smoothstep(.0, .5, rainAmount);
  
  vec2 c = Drops(uv, shaderTime, staticDrops, layer1, layer2);

  vec2 e = vec2(.001, 0.);
  float cx = Drops(uv+e, shaderTime, staticDrops, layer1, layer2).x;
  float cy = Drops(uv+e.yx, shaderTime, staticDrops, layer1, layer2).x;
  vec2 n = vec2(cx-c.x, cy-c.x); // expensive normals
  
  float focus = mix(maxBlur, minBlur, smoothstep(.1, .2, c.x));
  vec3 col = textureLod(imageData, UV+n, focus).rgb;
    
  gl_FragColor = vec4(col, 1.);
}
//