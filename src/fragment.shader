#version 330 core

out vec4 FragColor;
in vec3 vColor;
uniform bool spacePressed;

void main()
{
    float distx = (vColor.x - 0.05) / 0.05f;
    float disty = (vColor.y - 0.1) / 0.1;
    if(distx < 0)
        distx = -1 * distx;
    if(disty < 0)
        disty = -1 * disty;
    float dist = sqrt(distx * distx + disty * disty);
    float c = smoothstep(2, 0.0, dist);
    float alpha = c;
    if(spacePressed == false)
    {
        c = 0.5;
        alpha = 1.0;
    }
        
        
    FragColor = vec4(0.1f, c, c, alpha);
}