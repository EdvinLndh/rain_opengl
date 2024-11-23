#version 330 core

out vec4 FragColor;

uniform vec2 iResolution; // viewport resolution (in pixels)

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = fragCoord / iResolution.xy;

    vec3 col = vec3(uv.x, uv.y, uv.x); 

    FragColor = vec4(col, 1.0);
}
