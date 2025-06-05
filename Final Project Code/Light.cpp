#include "Light.h"

Light::Light(glm::mat4 viewMatrix) {
	m_lightPositionViewSpace[0] = glm::vec4((viewMatrix * glm::vec4(m_lightPosition, 1.0f))).x;
	m_lightPositionViewSpace[1] = glm::vec4((viewMatrix * glm::vec4(m_lightPosition, 1.0f))).y;
	m_lightPositionViewSpace[2] = glm::vec4((viewMatrix * glm::vec4(m_lightPosition, 1.0f))).z;

	m_lightAmbient[0] = lightAmb.r;
	m_lightAmbient[1] = lightAmb.g;
	m_lightAmbient[2] = lightAmb.b;
	m_lightAmbient[3] = lightAmb.a;

	m_lightDiffuse[0] = lightDiff.r;
	m_lightDiffuse[1] = lightDiff.g;
	m_lightDiffuse[2] = lightDiff.b;
	m_lightDiffuse[3] = lightDiff.a;

	m_lightSpecular[0] = lightSpec.r;
	m_lightSpecular[1] = lightSpec.g;
	m_lightSpecular[2] = lightSpec.b;
	m_lightSpecular[3] = lightSpec.a;

	m_globalAmbient[0] = gAmb.r;
	m_globalAmbient[1] = gAmb.g;
	m_globalAmbient[2] = gAmb.b;
	m_globalAmbient[3] = gAmb.a;
}

