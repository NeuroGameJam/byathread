#version 100

precision lowp float;

varying vec2 UV;

void main()
{		
    const vec3 darker = vec3(0.16, 0.13, 0.18);
    const vec3 lighter = vec3(0.30, 0.33, 0.37);

    vec3 color = mix(lighter, darker, UV.y);

    gl_FragColor = vec4(color, .25);
}
