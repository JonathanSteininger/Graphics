#shader vertex
#version 120  

attribute vec4 in_Position;  

void main()  
{  
    gl_Position = in_Position;  
}

#shader fragment
#version 120  
uniform vec2 WindowSize;  
void main()  
{  
    gl_FragColor = vec4(smoothstep(0, WindowSize.y * 3, gl_FragCoord.y) , 1.0 - smoothstep(0, WindowSize.x * 3, gl_FragCoord.x), smoothstep(0, WindowSize.x * 3, gl_FragCoord.x) , 1.0);  
}
