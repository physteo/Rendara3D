#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	TexCoords = aTexCoords;
}

#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D hdrBuffer;
uniform float exposure;


void main()
{
	// TODO: URGENT	remove this if want hdr
	//FragColor = vec4(texture(hdrBuffer, TexCoords).rgb, 1.0);

	// TODO URGENT: activate all this if you want hdr, and comment out: glEnable(GL_FRAMEBUFFER_SRGB) in Window's constructor;
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	
	// exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	// gamma correction
	mapped = pow(mapped, vec3(1.0/gamma));
	
	FragColor = vec4(mapped, 1.0);

}

