
uniform sampler2D diffuse;
varying vec2 texCoordVar;

uniform float healthTintAmount;

void main() 
{
    vec4 texColor = texture2D(diffuse, texCoordVar);
    
    // Blend original color with red, based on healthTintAmount (e.g. 0.0 to 1.0)
    vec3 tintedColor = mix(texColor.rgb, vec3(1.0, 0.0, 0.0), healthTintAmount);
    
    gl_FragColor = vec4(tintedColor, texColor.a);
}