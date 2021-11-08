// Task 5 - Draw an origin with 3 axis and an animated cube rotating around one of the axis
//        - Encapsulate the Cube using a class, it will be used for the rotating object and the axis
//        - Use color_vert/frag shader to display the polygon
//        - Animate the rotation of the all 3 axis and the cube.

#include <cmath>
#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

const unsigned int SIZE = 512;

// Object to represent a 3D cube
class Cube {
private:
    // 2D vectors define points/vertices of the shape
	 // TODO: Define cube vertices
    std::vector<glm::vec3> vertices = {
            {0, 0, 0},             // 0 pred vlavo dole
            {0.5, 0 ,0},           // 1 pred vpravo dole
            {0.5, 0.5 ,0},         // 2 pred vpravo hore
            {0, 0.5 ,0},           // 3 pred vlavo hore
            {0, 0, 0.5},           // 4 vzad vlavo dole
            {0.5, 0 ,0.5},         // 5 vzad vpravo dole
            {0.5, 0.5 ,0.5},       // 6 vzad vpravo hore
            {0, 0.5 ,0.5},         // 7 vzad vlavo hore
    };

    // Structure representing a triangular face
    struct Face {
        GLuint v0, v1, v2;
    };

    // Indices define triangles that index into vertices
	 // TODO: Define cube indices
    std::vector<Face> indices = {

            // face
            {0, 1, 2},
            {0, 2, 3},

            // back
            {4, 5, 6},
            {4, 5, 7},

            // left
            {0, 4, 3},
            {4, 7, 3},

            // right
            {1, 5, 2},
            {5, 6, 2},

            // top
            {3, 2, 6},
            {3, 6, 7},

            // button
            {0, 1, 5},
            {1, 5, 4},
    };

    // Program to associate with the object
    ppgso::Shader program = {color_vert_glsl, color_frag_glsl};

    // These will hold the data and object buffers
    GLuint vao, vbo, cbo, ibo;
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;

public:
    // Public attributes that define position, color ..
    glm::vec3 position{0,0,0};
    glm::vec3 rotation{0,0,0};
    glm::vec3 scale{1,1,1};
    glm::vec3 color{1,0,0};


    // Initialize object data buffers
    Cube() {
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

        // Copy indices to gpu
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Face), indices.data(), GL_STATIC_DRAW);

        // Set projection matrices to identity
        program.setUniform("ProjectionMatrix", glm::perspective((ppgso::PI / 180.f) * 60.0f, 1.0f, 0.1f, 100.0f));

        program.setUniform("ViewMatrix", viewMatrix);
    };
    // Clean up
    ~Cube() {
        // Delete data from OpenGL
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &cbo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    // Set the object transformation matrix
    void updateModelMatrix() {
        // Compute transformation by scaling, rotating and then translating the shape
		 // TODO: Update model matrix: modelMatrix = ... use position, rotation and scale
		 modelMatrix =  translate(glm::mat4{1}, position) *
//		                rotate(glm::mat4{1}, rotation.x, {1, 0, 0}) *
//                        rotate(glm::mat4{1}, rotation.y, {0, 1, 0}) *
                        rotate(glm::mat4{1}, rotation.z, {0, 0, 1}) *
                        glm::scale(glm::mat4{1}, scale);
    }

    void updateViewMatrix(glm::vec3 viewRotation) {
        // Compute transformation by scaling, rotating and then translating the shape
		// TODO: Update view matrix: modelMatrix = ... use translation -20 in Z and viewRotation
        viewMatrix =    translate(glm::mat4{1}, {0, 0, -10}) *
                        rotate(glm::mat4{1}, viewRotation.x, {1, 0, 0}) *
                        rotate(glm::mat4{1}, viewRotation.y, {0, 1, 0}) *
                        rotate(glm::mat4{1}, viewRotation.z, {0, 0, 1});
      }

    // Draw polygons
    void render(){
        // Update transformation and color uniforms in the shader
        program.use();
        program.setUniform("OverallColor", color);
        program.setUniform("ModelMatrix", modelMatrix);
        program.setUniform("ViewMatrix", viewMatrix);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size() * 3, GL_UNSIGNED_INT, 0);
    };
};

class OriginWindow : public ppgso::Window {
private:
    Cube axisX, axisY, axisZ;
    Cube cube;

    glm::vec3 viewRotation{0,0,0};
public:
    OriginWindow() : Window{"task5_3d_origin", SIZE, SIZE} {
//        glEnable(GL_DEPTH_TEST);
//        glDepthFunc(GL_LESS);

		// TODO: Set axis colors to red,green and blue...and cube color to grey
        axisX.color = {1, 0, 0};
        axisY.color = {0, 1, 0};
        axisZ.color = {0, 0, 1};
        cube.color = {0.5, 0.5, 0.5};

        const float scaleMin = 0.03f;
        const float scaleMax = 10.00f;

		// TODO: Set axis scaling in X,Y,Z directions...hint use scaleMin in tangent directions and scaleMax in the axis direction
        axisX.scale = {scaleMax, scaleMin, scaleMin};
        axisY.scale = {scaleMin, scaleMax, scaleMin};
        axisZ.scale = {scaleMin, scaleMin, scaleMax};
    }

    void onIdle() {
        // Set gray background
        glClearColor(.1f,.1f,.1f,1.0f);
        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move and Render shape\    // Get time for animation
        float t = (float) glfwGetTime();

        // Set rotation and scale
//        cube.position = axisZ.position;
        cube.rotation.z = t * 2.0f;


        viewRotation.x = t * 0.1f;
        viewRotation.y = t * 0.1f;
        viewRotation.z = t * 0.1f;

		// TODO: update view matrix of X,Y,Z axis and cube
		cube.updateViewMatrix(viewRotation);

		axisX.updateViewMatrix(viewRotation);
        axisY.updateViewMatrix(viewRotation);
        axisZ.updateViewMatrix(viewRotation);

		// TODO: update model matrix
		cube.updateModelMatrix();

        axisX.updateModelMatrix();
        axisY.updateModelMatrix();
        axisZ.updateModelMatrix();

        cube.render();

        axisX.render();
        axisY.render();
        axisZ.render();
     }
};

int main() {
    // Create our window
    auto window = OriginWindow{};

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}
