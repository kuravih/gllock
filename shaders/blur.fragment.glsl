//https://www.shadertoy.com/view/XdfGDH
#version 330 core

// ---- gllock required fields -----------------------------------------------------------------------------------------
#define RATE 0.75

uniform float time;
uniform float end;
uniform sampler2D imageData;
uniform vec2 screenSize;
// ---------------------------------------------------------------------------------------------------------------------

#define MIN_SIZE 0.0f
#define MAX_SIZE 7.0f //odd value required

float normpdf(in float x, in float sigma) {
  return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main(void) {

  vec3 c = texture(imageData, vec2(1,-1)*gl_FragCoord.xy/screenSize).rgb;

  //declare stuff
  const int mSize = int(MAX_SIZE+4);
  const int kSize = (mSize-1)/2;
  float kernel[mSize];
  vec3 final_colour = vec3(0.0);
  
  float shaderTime = smoothstep(0.0,1.0,(time-end)*RATE);
  shaderTime = (end==0)?shaderTime:(1.0-shaderTime);

  float radius = mix(MIN_SIZE, MAX_SIZE, shaderTime); //Returns the linear blend of MIN_SIZE and MAX_SIZE

  //create the 1-D kernel
  float Z = 0.0;
  for (int j = 0; j <= kSize; ++j) {
    kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), radius);
  }
  
  //get the normalization factor (as the gaussian has been clamped)
  for (int j = 0; j < mSize; ++j) {
    Z += kernel[j];
  }
  
  //read out the texels
  for (int i=-kSize; i <= kSize; ++i) {
    for (int j=-kSize; j <= kSize; ++j) {
      final_colour += kernel[kSize+j]*kernel[kSize+i]*texture(imageData, vec2(1,-1)*(gl_FragCoord.xy+vec2(float(i),float(j))) / screenSize).rgb;
    }
  }

  gl_FragColor = vec4(final_colour/(Z*Z), 1.0);

}