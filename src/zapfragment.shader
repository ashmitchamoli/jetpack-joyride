#version 330 core

out vec4 FragColor;
in vec3 pos;

void main()
{
    float distx = pos.x / 0.25;
    float disty = pos.y / 0.02f;
    if(distx < 0)
        distx = -1 * distx;
    if(disty < 0)
        disty = -1 * disty;
    float dist = sqrt(distx * distx + disty * disty);
    float c = smoothstep(sqrt(2)+0.2, 0.0, dist);
    
    FragColor = vec4(c, c, 0.0, c);
}