#include "sphere.h"

Sphere::Sphere()
{
    init(48);
    setupVertices();
    setupBuffers();
    //setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
}

Sphere::Sphere(int prec) { // prec is precision, or number of slices

    init(prec);
    setupVertices();
    setupBuffers();
    //setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
    hasTex = false;
}

Sphere::Sphere(int prec, const char* fname) { // prec is precision, or number of slices

    init(prec);
    setupVertices();
    setupBuffers();
    //setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

        // load texture from file
    m_texture = new Texture(fname);
    if (m_texture) {
        hasTex = true;
        m_TextureID = m_texture->getTextureID();
    }
    else{
        hasTex = false;
     }
}


void Sphere::Render(GLint posAttribLoc, GLint normalAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc)
{

    glBindVertexArray(vao);
    
    // Set vertex attribute pointers to the load correct data. Update here to load the correct attributes.
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, TB);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(tcAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(texCoords[0]), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, NB);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(normals[0]), 0);

    

    // If has texture, set up texture unit(s): update here for texture rendering
    if (m_TextureID != NULL) {
        glUniform1i(hasTextureLoc, true);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }
    else {
        glUniform1i(hasTextureLoc, false);
    }

    // Bind your Element Array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    // Render
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

    // Disable vertex arrays
    glDisableVertexAttribArray(posAttribLoc);
    glDisableVertexAttribArray(normalAttribLoc);
    glDisableVertexAttribArray(tcAttribLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Sphere::setupVertices() {
    std::vector<int> ind = getIndices();
    std::vector<glm::vec3> vert = getVertices();
    std::vector<glm::vec2> tex = getTexCoords();
    std::vector<glm::vec3> norm = getNormals();


    int numVertices = getNumVertices();
    for (int i = 0; i < numVertices; i++) {
        Vertices.push_back(Vertex(vert[i], norm[i], tex[i]));
    }

    int numIndices = getNumIndices();
    for (int i = 0; i < numIndices; i++) {
        int index = ind[i];
        Indices.push_back(index);
    }
}


void Sphere::setupBuffers() {
    // For OpenGL 3
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &TB);
    glBindBuffer(GL_ARRAY_BUFFER, TB);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);

    glGenBuffers(1, &NB);
    glBindBuffer(GL_ARRAY_BUFFER, NB);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Sphere::setupModelMatrix(glm::vec3 pivot, float angle, float scale) {
    pivotLocation = pivot;
    model = glm::translate(glm::mat4(1.0f), pivotLocation);
    model *= glm::rotate(glm::mat4(1.f), angle, glm::vec3(0., 1., 0));
    model *= glm::scale(glm::vec3(scale, scale, scale));
}

void Sphere::Update(glm::mat4 matModel) {

    model = matModel;

}

bool Sphere::loadTexture(const char* texFile, const char * textureType) {
    if (textureType == "DIFFUSE_TEXTURE") {
        m_TextureID = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
        if (!m_TextureID) {
            std::cout << "Failed to open diffuse texture file" << std::endl;
            return false;
        }
        hasTex = true;
    }
    else if (textureType == "NORMAL_TEXTURE") {
        m_NormalTextureID = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
        if (!m_NormalTextureID) {
            std::cout << "Failed to open normal texture file" << std::endl;
            return false;
        }
        hasTex = true;
    }
    return true;
}

GLuint Sphere::getTextureID(const char* textureType) {
    if (textureType == "DIFFUSE_TEXTURE") {
        return m_TextureID;
    }
    else if (textureType == "NORMAL_TEXTURE") {
        return m_NormalTextureID;
    }
}


float Sphere::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void Sphere::init(int prec) {
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;
    for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
    for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
    for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

    for (int i = 0; i <= prec; i++) {
        for (int j = 0; j <= prec; j++) {
            float y = (float)cos(toRadians(180.f - i * 180.f / prec));
            float x = -(float)cos(toRadians(j * 360.f / prec)) * (float)abs(cos(asin(y)));
            float z = (float)sin(toRadians(j * 360.f / prec)) * (float)abs(cos(asin(y)));
            vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
            texCoords[i * (prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
            normals[i * (prec + 1) + j] = glm::vec3(x, y, z);
        }
    }

    // calculate triangles indices
    for (int i = 0; i < prec; i++) {
        for (int j = 0; j < prec; j++) {
            indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
            indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
            indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
        }
    }
}

// accessors
int Sphere::getNumVertices() { return numVertices; }
int Sphere::getNumIndices() { return numIndices; }
std::vector<int> Sphere::getIndices() { return indices; }
std::vector<glm::vec3> Sphere::getVertices() { return vertices; }
std::vector<glm::vec2> Sphere::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Sphere::getNormals() { return normals; }