#version 330  

// Specific incoming primitive type is a triangle
layout (triangles) in;
// Output is essentially the same but also specifies the number of vertices expected with it. If you output more than the max, they won’t be rendered.
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightMatrices[6];

out vec4 FragPos;

void main()
{
	for(int face = 0; face < 6; face++)
	{
		// Set the value of gl_Layer to determine which one to write to when calling EmitVertex.
		gl_Layer = face;
		for(int i = 0; i < 6; i++)
		{
			// gl_in stores data for each vertex passed from Vertex Shader.
			FragPos = gl_in[i].gl_Position;
			gl_Position = lightMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}