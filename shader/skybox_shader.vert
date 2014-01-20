varying vec3 Pos;

void main(void)
{
	Pos = gl_Vertex.xyz;
	gl_Position = ftransform();
}