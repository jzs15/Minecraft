#version 330 core
layout (location = 0) in vec4 coord;
layout (location = 1) in float aNormal;
layout (location = 2) in float type;
uniform mat4 pv;
uniform mat4 model;
out vec4 texcoord;
out float textype;
out vec3 FragPos;
out float Normal;

bool checkType(int v)
{
	float bias = 0.001;
	return type >= v - bias && type <= v + bias;
}

void main(void) {
	// Just pass the original vertex coordinates to the fragment shader as texture coordinates
	texcoord = coord;
	textype = type;
	FragPos = vec3(model * vec4(coord.xyz, 1));
	Normal = aNormal;
	vec3 vect;
	if(checkType(1))
		vect = vec3(coord.x + 0.5, coord.y, coord.z);
	else if(checkType(2))
		vect = vec3(coord.x, coord.y, coord.z + 0.5);
	else if(checkType(3))
		vect = vec3(coord.x + 0.4375, coord.y, coord.z);
	else if(checkType(4))
		vect = vec3(coord.x - 0.4375, coord.y, coord.z);
	else if(checkType(5))
		vect = vec3(coord.x, coord.y - 0.375, coord.z);
	else if(checkType(6))
		vect = vec3(coord.x, coord.y, coord.z + 0.4375);
	else if(checkType(7))
		vect = vec3(coord.x, coord.y, coord.z - 0.4375);
	else
	{
		vect = coord.xyz;
	}
	// Apply the model-view-projection matrix to the xyz components of the vertex coordinates
	gl_Position = pv * model * vec4(vect, 1);
}
