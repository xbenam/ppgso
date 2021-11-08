// Task 6 - Generate a Bezier surface of variable density with UV coordinates.
//        - Confine the Bezier data and associated methods into a reusable class.
//        - Define a modelMatrix that uses position, rotation and scale.
//        - Render the generated mesh with texturing applied.
//        - Animate rotation.

#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

const unsigned int SIZE = 512;

// Object to represent Bezier patch
class BezierPatch {
private:
  // 3D vectors define points/vertices of the shape
  std::vector<glm::vec3> vertices;

  // Texture coordinates
  std::vector<glm::vec2> texCoords;

  // Define our face using indexes to 3 vertices
  struct face {
    GLuint v0, v1, v2;
  };

  // Define our mesh as collection of faces
  std::vector<face> mesh;

  // These will hold the data and object buffers
  GLuint vao, vbo, tbo, ibo;
  glm::mat4 modelMatrix{1.0f};

  glm::vec3 bezierPoint(const glm::vec3 controlPoints[4], float t) {
      glm::vec3 p01 = glm::lerp(controlPoints[0], controlPoints[1], t);
      glm::vec3 p12 = glm::lerp(controlPoints[1], controlPoints[2], t);
      glm::vec3 p23 = glm::lerp(controlPoints[2], controlPoints[3], t);

      glm::vec3 p0112 = glm::lerp(p01, p12, t);
      glm::vec3 p1223 = glm::lerp(p12, p23, t);

      glm::vec3 p01121223 = glm::lerp(p0112, p1223, t);
    return p01121223;
  }

  ppgso::Shader program{texture_vert_glsl, texture_frag_glsl};
  ppgso::Texture texture{ppgso::image::loadBMP("lena.bmp")};
public:
  // Public attributes that define position, color ..
  glm::vec3 position{0,0,0};
  glm::vec3 rotation{0,0,0};
  glm::vec3 scale{1,1,1};

  // Initialize object data buffers
  BezierPatch(const glm::vec3 controlPoints[4][4]) {
    // Generate Bezier patch points and incidences
    unsigned int PATCH_SIZE = 10;
    for(unsigned int i = 0; i < PATCH_SIZE ; i++) {
      for (unsigned int j = 0; j < PATCH_SIZE; j++) {
        // TODO: Compute points on the bezier patch
        // HINT: Compute u, v coordinates
          glm::vec3 newBez[4];
          for(unsigned int k = 0; k < 4; k++)
              newBez[k] = bezierPoint(controlPoints[k],(float)i/(float)PATCH_SIZE);
          vertices.push_back(bezierPoint(newBez,(float)j/(float)PATCH_SIZE));

          texCoords.emplace_back((float)i/(float)PATCH_SIZE,1.0f - (float)j/(float)PATCH_SIZE);
        // vertices.push_back(??);
        // texCoords.push_back(??);
      }
    }
    // Generate indices
    for(unsigned int i = 1; i < PATCH_SIZE; i++) {
      for (unsigned int j = 1; j < PATCH_SIZE; j++) {
        // TODO: Compute indices for triangle 1
        // indices.push_back(??);
        // indices.push_back(??);
        // indices.push_back(??);

          mesh.push_back({i + PATCH_SIZE * j ,i + PATCH_SIZE * (j-1), (i-1) + PATCH_SIZE * j});
          mesh.push_back({(i - 1) + PATCH_SIZE * j ,i + PATCH_SIZE * (j-1), (i-1) + PATCH_SIZE * (j-1)});
        // TODO: Compute indices for triangle 2
        // indices.push_back(??);
        // indices.push_back(??);
        // indices.push_back(??);
      }
    }

    // Copy data to OpenGL
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Copy positions to gpu
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Set vertex program inputs
    auto position_attrib = program.getAttribLocation("Position");
    glEnableVertexAttribArray(position_attrib);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy texture positions to gpu
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);

    // Set vertex program inputs
    auto texCoord_attrib = program.getAttribLocation("TexCoord");
    glEnableVertexAttribArray(texCoord_attrib);
    glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy indices to gpu
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size() * sizeof(face), mesh.data(), GL_STATIC_DRAW);

  };
  // Clean up
  ~BezierPatch() {
    // Delete data from OpenGL
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }
  // Set the object transformation matrix
  void update() {
    // TODO: Compute transformation by scaling, rotating and then translating the shape
     modelMatrix = translate(glm::mat4{1}, position) *
                          rotate(glm::mat4{1}, rotation.x, {1,0,0}) *
                          rotate(glm::mat4{1}, rotation.y, {0,1,0}) *
                          rotate(glm::mat4{1}, rotation.z, {0,0,1}) *
                          glm::scale(glm::mat4{1},scale);
  }

  // Draw polygons
  void render(){
    // Update transformation and color uniforms in the shader
    program.use();

    // Initialize projection
    // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
    // You can think of this as the camera objective settings
    auto projection = glm::perspective( (ppgso::PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f);
    program.setUniform("ProjectionMatrix", projection);

    // Create view matrix (translate camera a bit backwards, so we can see the geometry)
    // This can be seen as the camera position/rotation in space
    auto view = glm::translate(glm::mat4{}, {0.0f, 0.0f, -3.0f});
    program.setUniform("ViewMatrix", view);

    // Set model position
    program.setUniform("ModelMatrix", modelMatrix);

    // Bind texture
    program.setUniform("Texture", texture);

    glBindVertexArray(vao);
    // TODO: Use correct rendering mode to draw the result
    //glDrawElements(??);
      glDrawElements(GL_TRIANGLES, (GLsizei) mesh.size() * 3, GL_UNSIGNED_INT, 0);

  };
};

class BezierSurfaceWindow : public ppgso::Window {
private:
  // Define 16 control points
  glm::vec3 controlPoints[4][4]{
          { {-.5,-.5,0}, {0,-.5,0},{.5,-.5,0},{1,-.5,0} },
          { {-.5,0,0}, {0.5,0,0}, {1,0,0}, {1.5,0.5,0} },
          { {-.5,.5,0}, {0.5,0.5,0}, {1,0.5,0}, {1.5,0.5,0}, },
          { {-.5,1,0}, {0.5,1,0}, {1,1,0}, {1.5,1,0} },
//      { {-1,1,0}, {-0.5,1,0}, {.5,1,0}, {1,1,3} },
//      { {-1,.5,0}, {-0.5,.5,0}, {.5,.5,0}, {1,.5,0} },
//      { {-1,-.5,0}, {-0.5,-.5,0}, {.5,-.5,0}, {1,-.5,-1} },
//      { {-1,-1,3}, {-0.5,-1,0}, {.5,-1,0}, {1,-1,0} },
  };

  BezierPatch bezier{controlPoints};
public:
  BezierSurfaceWindow() : Window{"task6_bezier_surface", SIZE, SIZE} {
    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }

  void onIdle() final {
    // Set gray background
    glClearColor(.1f,.1f,.1f,1.0f);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Move and Render shape
    auto time = (float) glfwGetTime();

//    for(int i=1; i<=2; i++)
//    {
//        for(int j=1; j<=2; j++)
//        {
//            controlPoints[i][j][2] = static_cast<float>(std::sin(time + static_cast<float>(j) + static_cast<float>(i)));
//        }
//    }
//     waving flag
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            controlPoints[i][j][2] = static_cast<float>(std::sin(time + static_cast<float>(j) + static_cast<float>(i / 3.14)));
        }
    }


    BezierPatch a{controlPoints};
    bezier = a;
    bezier.rotation = {time*0.3,0, 0};
    bezier.update();
    bezier.render();
  }
};

int main() {
  // Create new window
  auto window = BezierSurfaceWindow{};

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
