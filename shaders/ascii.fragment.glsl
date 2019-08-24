// http://www.shadertoy.com/view/lssGDj
#version 330 core

// ---- gllock required fields -----------------------------------------------------------------------------------------
#define RATE 1.0

uniform float time;
uniform float end;
uniform sampler2D imageData;
uniform vec2 screenSize;
// ---------------------------------------------------------------------------------------------------------------------

#define MIN_SIZE 0.0f
#define MAX_SIZE 16.0f

float character(int n, vec2 p)
{
  p = floor(p*vec2(4.0, -4.0) + 2.5);
    if (clamp(p.x, 0.0, 4.0) == p.x)
  {
        if (clamp(p.y, 0.0, 4.0) == p.y)	
    {
          int a = int(round(p.x) + 5.0 * round(p.y));
      if (((n >> a) & 1) == 1) return 1.0;
    }	
    }
  return 0.0;
}

void main(void) {

  float shaderTime = smoothstep(0.0,1.0,(time-end)*RATE);
  shaderTime = (end==0)?shaderTime:(1.0-shaderTime);

  float radius = mix(MIN_SIZE, MAX_SIZE, shaderTime); //Returns the linear blend of MIN_SIZE and MAX_SIZE

  vec2 pix =  vec2(1,-1)*gl_FragCoord.xy;
  vec3 col = texture(imageData, floor(pix/radius)*radius/screenSize.xy).rgb;	
  
  float gray = 0.3 * col.r + 0.59 * col.g + 0.11 * col.b;
  
  int n =  4096;                // .
  if (gray > 0.2) n = 65600;    // :
  if (gray > 0.3) n = 332772;   // *
  if (gray > 0.4) n = 15255086; // o 
  if (gray > 0.5) n = 23385164; // &
  if (gray > 0.6) n = 15252014; // 8
  if (gray > 0.7) n = 13199452; // @
  if (gray > 0.8) n = 11512810; // #
  
  vec2 p = mod(2*pix/radius, 2.0) - vec2(1.0);
    
  col = col*character(n, p);
  
  gl_FragColor = vec4(col, 1.0);
}
//