// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 430
struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};
//
uniform sampler2D sceneTex; // 0
uniform float rotateShader_t; 
uniform float shader;

uniform float rt_w;
uniform float rt_h;


uniform float pixel_w; // 15.0
uniform float pixel_h; // 10.0

float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
//

uniform MaterialInfo Material;
out vec4 vFragColor;

//lighting color
vec4    ambientColor = vec4(0.1,0.1,0.1,1);
vec4    diffuseColor = vec4(0.8,0.8,0.8,1);   
vec4    specularColor = vec4(1,1,1,1);

in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
in vec2 UV;
float Shininess = 128.0;//for material specular

void main(void)
{ 
    vec2 uv = vec2(UV.x, 1 - UV.y);

    if(shader == 2){
        // pixel effect
        float dx = pixel_w*(1./rt_w);
        float dy = pixel_h*(1./rt_h);
        uv = vec2(dx * floor(uv.x/dx), dy * floor(uv.y/dy));
    }

    if(shader == 3){
         // rotation effect
        vec2 center = vec2(0.5, 0.5);
        uv -= center;
        float dist = length(uv);

        if(dist < 0.5)
        {
            float percent = (0.5 - dist) / 0.5;
            float theta = percent * percent * rotateShader_t * 2 * 3.1415926;
            float s = sin(theta);
            float c = cos(theta);
            uv = vec2(dot(uv, vec2(c, -s)), dot(uv, vec2(s, c)));
        }
        uv += center;
    }

    if(shader == 4){
        
    }

    vec3 tc = texture2D(sceneTex, uv).rgb;
    vec4 texColor = vec4(tc, 1.0f);

    //-------------------------------//

    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vVaryingNormal),
					normalize(vVaryingLightDir)));

    // Multiply intensity by diffuse color, force alpha to 1.0
    vFragColor = diff * diffuseColor*vec4(Material.Kd,1);

    // Add in ambient light
    vFragColor += ambientColor;


    // Specular Light
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir),
								normalize(vVaryingNormal)));//反射角
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
    if(diff != 0) {
		spec = pow(spec, Shininess);
		vFragColor += specularColor*vec4(Material.Ka,1)*spec;
    }

    if(shader != 0) vFragColor *= texColor;

}
	
    