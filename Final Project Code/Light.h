#pragma once
#include "graphics_headers.h"
#include <SOIL2/SOIL2.h>

class Light
{
	public:
		Light(glm::mat4 viewMatrix);

		glm::vec4 m_globalAmbient;
		glm::vec4 m_lightAmbient;
		glm::vec4 m_lightDiffuse;
		glm::vec4 m_lightSpecular;

		glm::vec3 m_lightPosition = glm::vec3(0.0, 0.0, 0.0);
		glm::vec3 m_lightPositionViewSpace;

	private:
		glm::vec4 lightAmb = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 lightDiff = glm::vec4(1.0f, 1.0f, 0.7f, 1.0f);
		glm::vec4 lightSpec = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 gAmb = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);

};

