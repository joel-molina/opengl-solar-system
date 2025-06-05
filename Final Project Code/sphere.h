#include "object.h"
#include "Texture.h"

class Sphere :
    public Object
{
public:
    Sphere();
    Sphere(int prec);
    Sphere(int prec, const char* fname);

    void Render(GLint positionAttribLoc, GLint colorAttribLoc, GLint tcAttribLoc, GLint hasTex);
    void Update(glm::mat4 matModel);

    glm::mat4 GetModel() { return model; }
    int getNumVertices();
    int getNumIndices();

    std::vector<int> getIndices();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getTexCoords();
    std::vector<glm::vec3> getNormals();

    GLuint getTextureID(const char* textureType);
    
    bool hasTex;
    bool loadTexture(const char* texFile, const char* textureType);

private:
    glm::vec3 pivotLocation;
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;
    GLuint TB;
    GLuint NB;
    Texture* m_texture;

    GLuint m_TextureID = 0;
    GLuint m_NormalTextureID = 0;

    GLuint vao;

    float angle;

    void setupVertices();
    void setupBuffers();
    void setupModelMatrix(glm::vec3 pivotLoc, float angle, float scale);

    int numVertices;
    int numIndices;
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    void init(int);
    float toRadians(float degrees);

    std::vector<float> pvalues; // vertex positions
    std::vector<float> tvalues; // texture coordinates
    std::vector<float> nvalues; // normal vectors
};