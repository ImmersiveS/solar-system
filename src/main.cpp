#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <MMSystem.h>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL/SOIL.h>

// Properties
GLuint screenWidth = 1600, screenHeight = 900;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
void showText(vector<GLuint>& texture, GLuint VAO, Shader* textShader);
GLuint loadCubemap(vector<const GLchar*> faces);

// Camer
Camera camera(glm::vec3(0.0f, 1.0f, 23.0f));
bool keys[1024];

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLfloat lastX = 600, lastY = 400;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Solar system", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);

	// Build and compile our shader program
	Shader textShader(	"C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/shaders/textVertex.vs",
						"C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/shaders/textFragment.fs");


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions          // Colors           // Texture Coords
		1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
		1.0f, 0.4f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
		0.4f, 0.4f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
		0.4f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
	};
	GLfloat loadingScreenVertices[] = {
		// Positions          // Colors           // Texture Coords
		1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
		1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	GLuint VBO0, VAO0, EBO0;
	glGenVertexArrays(1, &VAO0);
	glGenBuffers(1, &VBO0);
	glGenBuffers(1, &EBO0);

	glBindVertexArray(VAO0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(loadingScreenVertices), loadingScreenVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	GLuint loading;
	glGenTextures(1, &loading);
	glBindTexture(GL_TEXTURE_2D, loading); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
												   // Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("..\\resources\\textures\\textLoading.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glBindTexture(GL_TEXTURE_2D, loading);

	textShader.use();

	// Draw container
	glBindVertexArray(VAO0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glfwSwapBuffers(window);

	glDeleteVertexArrays(1, &VAO0);
	glDeleteBuffers(1, &VBO0);
	glDeleteBuffers(1, &EBO0);

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Load and create a texture
	string texturesPath = "C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/textures/";
	vector<string> fileNamesOfPlanets = {"textSun.jpg", 
										"textEarth.jpg", 
										"textMercury.jpg", 
										"textMars.jpg", 
										"textVenus.jpg", 
										"textJupiter.jpg", 
										"textSaturn.jpg", 
										"textNeptune.jpg", 
										"textUranus.jpg"};
	vector<GLuint> textTextures(9);
	for (int i = 0; i < 9; i++)
	{
		glGenTextures(1, &textTextures[i]);
		glBindTexture(GL_TEXTURE_2D, textTextures[i]); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
											// Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load image, create texture and generate mipmaps
		int width, height;
		unsigned char* image = SOIL_load_image((texturesPath + fileNamesOfPlanets[i]).c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	}


	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// Set OpenGL options

    // Setup and compile other shaders
	Shader instanceShader("../shaders/instancing.vs", "../shaders/instancing.fs");
    Shader shader(	"C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/shaders/vertex.vs", 
					"C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/shaders/fragment.fs");
	Shader skyboxShader("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/shaders/cubemap.vs", 
						"C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/shaders/cubemap.fs");

    // Load models
	Model sun_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/Sun/sun2.obj");
	Model mercury_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/mercury/mercury.obj");
	Model mars_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/mars/Mars_normal-strong.obj");
	Model earth_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/earth/earth.obj");
	Model venus_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/Venus/venus.obj");
	Model saturn_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/Saturn/saturn3.obj");
	Model neptun_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/Neptune/neptune.obj");
	Model uranus_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/Uranus/uranus.obj");
	Model jupiter_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/jupiter/jupiter1.obj");
	Model moon_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/moon/moon1.obj");
	Model asteroid_model("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/objects/asteroid/rock.obj");

	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Cubemap
	vector<const GLchar*> faces;
	faces.push_back("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/textures/skybox/right.jpg");
	faces.push_back("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/textures/skybox/left.jpg");
	faces.push_back("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/textures/skybox/top.jpg");
	faces.push_back("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/textures/skybox/bottom.jpg");
	faces.push_back("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/textures/skybox/back.jpg");
	faces.push_back("C:/Users/user/Documents/Visual Studio 2015/Projects/SecondSemester/Cubemap/resources/textures/skybox/front.jpg");
	GLuint cubemapTexture = loadCubemap(faces);

	//Play theme sound
	PlaySound("..\\resources\\sounds\\spacetheme.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

	// Generate a large list of semi-random model transformation matrices
	GLuint amount = 200;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	GLfloat radius = 38.5;
	GLfloat offset = 3.0f;
	for (GLuint i = 0; i < amount; i++)
	{
		glm::mat4 model;
		// 1. Translation: displace along circle with 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat y = displacement * 0.4f; // Keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. Scale: Scale between 0.05 and 0.1f
		GLfloat scale = (rand() % 10) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector
		GLfloat rotAngle = (rand() % 360);
		model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. Now add to list of matrices
		modelMatrices[i] = model;
	}

	// forward declare the instancing buffer
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	// Set transformation matrices as an instance vertex attribute (with divisor 1)
	// NOTE: We're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// Normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	for (GLuint i = 0; i < asteroid_model.getMeshes().size(); i++)
	{
		GLuint VAO = asteroid_model.getMeshes()[i].getVAO();
		glBindVertexArray(VAO);
		// Set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = 2*(currentFrame - lastFrame);
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        Do_Movement();
		
        // Clear the colorbuffer
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		showText(textTextures, VAO, &textShader);

        shader.use();   // <-- Don't forget this one!
        // Transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));


        // Draw the loaded model
        glm::mat4 model1, model2, model3, model4, model5, model6, model7, model8, model9, model10;
        model1 = glm::translate(model1, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model1 = glm::rotate(model1, glm::radians((GLfloat)glfwGetTime() * 8.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model1 = glm::scale(model1, glm::vec3(0.1f, 0.1f, 0.1f));	// It's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model1));
        sun_model.Draw(shader);

		model2 = glm::translate(model2, glm::vec3(18.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model2 = glm::rotate(model2, glm::radians((GLfloat)glfwGetTime() * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model2 = glm::scale(model2, glm::vec3(0.01f, 0.01f, 0.01f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model2));
		mercury_model.Draw(shader);

		model3 = glm::translate(model3, glm::vec3(-20.0f, -1.75f, -6.0f)); // Translate it down a bit so it's at the center of the scene
		model3 = glm::rotate(model3, glm::radians((GLfloat)glfwGetTime() * 18.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model3 = glm::scale(model3, glm::vec3(0.01f, 0.01f, 0.01f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model3));
		venus_model.Draw(shader);

		model4 = glm::translate(model4, glm::vec3(-27.0f, -1.0f, 10.0f)); // Translate it down a bit so it's at the center of the scene
		model4 = glm::rotate(model4, glm::radians((GLfloat)glfwGetTime() * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model4 = glm::scale(model4, glm::vec3(0.003f, 0.003f, 0.003f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model4));
		earth_model.Draw(shader);

		model10 = glm::translate(model10, glm::vec3(-27.0f, 0.8f, 11.0f)); // Translate it down a bit so it's at the center of the scene
		model10 = glm::scale(model10, glm::vec3(0.005f, 0.005f, 0.005f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model10));
		moon_model.Draw(shader);

		model5 = glm::translate(model5, glm::vec3(30.0f, -1.75f, -9.0f)); // Translate it down a bit so it's at the center of the scene
		model5 = glm::rotate(model5, glm::radians((GLfloat)glfwGetTime() * 25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model5 = glm::scale(model5, glm::vec3(0.1f, 0.1f, 0.1f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model5));
		mars_model.Draw(shader);

		model6 = glm::translate(model6, glm::vec3(-43.5f, -1.75f, 25.0f)); // Translate it down a bit so it's at the center of the scene
		model6 = glm::rotate(model6, glm::radians((GLfloat)glfwGetTime() * 25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model6 = glm::scale(model6, glm::vec3(0.03f, 0.03f, 0.03f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model6));
		saturn_model.Draw(shader);

		model7 = glm::translate(model7, glm::vec3(-47.0f, -1.75f, -40.0f)); // Translate it down a bit so it's at the center of the scene
		model7 = glm::rotate(model7, glm::radians((GLfloat)glfwGetTime() * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model7 = glm::scale(model7, glm::vec3(0.03f, 0.03f, 0.03f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model7));
		neptun_model.Draw(shader);

		model8 = glm::translate(model8, glm::vec3(45.0f, -1.75f, -40.0f)); // Translate it down a bit so it's at the center of the scene
		model8 = glm::rotate(model8, glm::radians((GLfloat)glfwGetTime() * 15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model8 = glm::scale(model8, glm::vec3(0.03f, 0.03f, 0.03f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model8));
		uranus_model.Draw(shader);

		model9 = glm::translate(model9, glm::vec3(36.0f, -1.75f, 37.0f)); // Translate it down a bit so it's at the center of the scene
		model9 = glm::rotate(model9, glm::radians((GLfloat)glfwGetTime() * 17.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model9 = glm::scale(model9, glm::vec3(0.05f, 0.05f, 0.05f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model9));
		jupiter_model.Draw(shader);

		instanceShader.use();
		glUniformMatrix4fv(glGetUniformLocation(instanceShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(instanceShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glBindTexture(GL_TEXTURE_2D, asteroid_model.getLoadedTextures()[0].id); // Note we also made the textures_loaded vector public (instead of private) from the model class.
		for (GLuint i = 0; i < asteroid_model.getMeshes().size(); i++)
		{
			glBindVertexArray(asteroid_model.getMeshes()[i].getVAO());
			glDrawElementsInstanced(GL_TRIANGLES, asteroid_model.getMeshes()[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		// Draw skybox as lasl
		// Bind Texture
		
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default


        // Swap the buffers
        glfwSwapBuffers(window);
    }
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	delete[] modelMatrices;
    glfwTerminate();
    return 0;
}


// Loads a cubemap texture from 6 individual texture faces
// Order should be:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;
}

void showText(vector<GLuint>& textTextures, GLuint VAO, Shader* textShader)
{
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	auto drawText = [&](int index) {
		glBindTexture(GL_TEXTURE_2D, textTextures[index]);

		textShader->use();

		// Draw container
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};
	if (keys[GLFW_KEY_Q])
		drawText(0);
	else if (keys[GLFW_KEY_E])
		drawText(1);
	else if (keys[GLFW_KEY_R])
		drawText(2);
	else if (keys[GLFW_KEY_Z])
		drawText(3);
	else if (keys[GLFW_KEY_F])
		drawText(4);
	else if (keys[GLFW_KEY_X])
		drawText(5);
	else if (keys[GLFW_KEY_C])
		drawText(6);
	else if (keys[GLFW_KEY_V])
		drawText(7);
	else if (keys[GLFW_KEY_B])
		drawText(8);
}
// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}