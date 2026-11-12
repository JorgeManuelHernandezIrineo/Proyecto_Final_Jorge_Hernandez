// Proyecto Final - Noche de Ánimas en Janitzio
// Hernández Irineo Jorge Manuel
// Fecha: 11 - noviembre - 2026
// No. de cuenta : 423045291
#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
//Camera  camera(glm::vec3(0.0f, 3.0f, 6.0f));
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;
glm::vec3 Light1 = glm::vec3(0);

// Positions of the point lights (para velas/veladoras del panteón)
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 2.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f)
};

// Variables para la animación del Xoloescuincle
bool animXolo = false;          // Interruptor con la tecla X
float xoloZ = 0.20f;            // Posición inicial en Z (entrada del panteón)
float xoloRotPatas = 0.0f;      // Ángulo de oscilación para las patas
bool xoloAvanzando = true;      // Control de ida y vuelta

// Variables para la animación de la canoa
bool animCanoa = false;         // Interruptor con la tecla V
float canoaPos = 0.0f;         // Posición inicial en el eje X o Z de la orilla
bool canoaIda = true;           // Control de ida y vuelta

// Variables mariposa
bool animMariposas = false;     // Interruptor con la tecla M
float mariposaAngle = 0.0f;     // Ángulo de giro para el vuelo circular

//Interaccion cultural
bool modoCultural = false; // Interruptor con la tecla I
bool infoUbicacion = false;     // Con la tecla O

// Variables para Animaciones y KeyFrames (Conservadas para usarlas después)
float rotDog = 0.0f;
float FLegs = 0.0f;
float FR_Legs = 0.0f;
float RLegs = 0.0f;
float head = 0.0f;
float tail = 0.0f;
float dogRotX = 0.0f;
float dogPosX, dogPosY, dogPosZ;

#define MAX_FRAMES 15
int i_max_steps = 400;
int i_curr_steps = 0;

typedef struct _frame {
	float rotDog;
	float rotDogInc;
	float dogPosX;
	float dogPosY;
	float dogPosZ;
	float incX;
	float incY;
	float incZ;
	float dogRotX, dogRotXInc;
	float FLegs, FLegsInc;
	float FR_Legs, FR_LegsInc;
	float RLegs, RLegsInc;
	float head, headInc;
	float tail, tailInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;
bool play = false;
int playIndex = 0;

void saveFrame(void) {
	printf("frameindex %d\n", FrameIndex);
	KeyFrame[FrameIndex].dogPosX = dogPosX;
	KeyFrame[FrameIndex].dogPosY = dogPosY;
	KeyFrame[FrameIndex].dogPosZ = dogPosZ;
	KeyFrame[FrameIndex].rotDog = rotDog;
	KeyFrame[FrameIndex].dogRotX = dogRotX;
	KeyFrame[FrameIndex].FLegs = FLegs;
	KeyFrame[FrameIndex].FR_Legs = FR_Legs;
	KeyFrame[FrameIndex].RLegs = RLegs;
	KeyFrame[FrameIndex].head = head;
	KeyFrame[FrameIndex].tail = tail;
	FrameIndex++;
}

void resetElements(void) {
	dogPosX = KeyFrame[0].dogPosX;
	dogPosY = KeyFrame[0].dogPosY;
	dogPosZ = KeyFrame[0].dogPosZ;
	rotDog = KeyFrame[0].rotDog;
	dogRotX = KeyFrame[0].dogRotX;
	FLegs = KeyFrame[0].FLegs;
	FR_Legs = KeyFrame[0].FR_Legs;
	RLegs = KeyFrame[0].RLegs;
	head = KeyFrame[0].head;
	tail = KeyFrame[0].tail;
}

void interpolation(void) {
	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].dogPosX - KeyFrame[playIndex].dogPosX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].dogPosY - KeyFrame[playIndex].dogPosY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].dogPosZ - KeyFrame[playIndex].dogPosZ) / i_max_steps;
	KeyFrame[playIndex].rotDogInc = (KeyFrame[playIndex + 1].rotDog - KeyFrame[playIndex].rotDog) / i_max_steps;
	KeyFrame[playIndex].dogRotXInc = (KeyFrame[playIndex + 1].dogRotX - KeyFrame[playIndex].dogRotX) / i_max_steps;
	KeyFrame[playIndex].FLegsInc = (KeyFrame[playIndex + 1].FLegs - KeyFrame[playIndex].FLegs) / i_max_steps;
	KeyFrame[playIndex].FR_LegsInc = (KeyFrame[playIndex + 1].FR_Legs - KeyFrame[playIndex].FR_Legs) / i_max_steps;
	KeyFrame[playIndex].RLegsInc = (KeyFrame[playIndex + 1].RLegs - KeyFrame[playIndex].RLegs) / i_max_steps;
	KeyFrame[playIndex].headInc = (KeyFrame[playIndex + 1].head - KeyFrame[playIndex].head) / i_max_steps;
	KeyFrame[playIndex].tailInc = (KeyFrame[playIndex + 1].tail - KeyFrame[playIndex].tail) / i_max_steps;
}

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main() {
	// Init GLFW
	glfwInit();
	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hernandez Irineo Jorge Manuel - Proyecto Final ", nullptr, nullptr);

	//// Obtener el monitor principal y su resolución nativa actual
	//GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	//const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	//// Actualizar las variables de pantalla con los valores reales del monitor
	//SCREEN_WIDTH = mode->width;
	//SCREEN_HEIGHT = mode->height;

	//// Crear la ventana en modo pantalla completa abarcando toda la pantalla
	//GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hernandez Irineo Jorge Manuel - Proyecto Final ", primaryMonitor, nullptr);

	//GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hernandez Irineo Jorge Manuel - Proyecto Final ", nullptr, nullptr);
	if (nullptr == window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Shaders
	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader skyboxShader("Shader/SkyBox.vs", "Shader/SkyBox.frag");

	// Modelos del Proyecto (Descomenta conforme los vayas ocupando)
	Model isla((char*)"Models/Meshy_AI_cemetery_island_terrain_20261111_texture.obj");
	Model ofrenda1((char*)"Models/Meshy_AI_altar_offering_setup_20261111_texture.obj");
	//Model merlina((char*)"Models/Meshy_AI_calavera_girl_statue_20261111_texture.obj"); 
	Model enid((char*)"Models/Meshy_AI_DayOfTheDeadGirlStatue_20261111_texture.obj"); 
	Model puerta((char*)"Models/Meshy_AI_cemetery_stone_gateway_wall_3d_20261110_texture.obj");
	Model muro((char*)"Models/Meshy_AI_Cemetery_Wall_Segment_20261110_texture.obj");
	Model mariposa((char*)"Models/Meshy_AI_MonarchButterflyLowPoly_20261111_texture.obj");
	Model copalero((char*)"Models/Meshy_AI_Clay_Sahumador_20261111_texture.obj");
	Model vela((char*)"Models/Meshy_AI_Day_of_the_Dead_Candleholder_20261110_texture.obj");
	//Model anthony((char*)"Models/Meshy_AI_day_of_the_dead_rocker_3d_20261111_texture.obj");
	Model diablo((char*)"Models/Meshy_AI_Devil_Villain_NPC_20261111_texture.obj");
	Model ofrenda2((char*)"Models/Meshy_AI_dia_de_muertos_altar_offering_20261111_texture.obj");
	Model canoa((char*)"Models/Meshy_AI_dugout_canoe_20261111_texture.obj");
	//Model vela2((char*)"Models/Meshy_AI_Low_Poly_Pillar_Candle_20261110_texture.obj");
	Model tumba1((char*)"Models/Meshy_AI_mexican_cemetery_grave_20261110_texture.obj");
	Model iglesia((char*)"Models/Meshy_AI_Mexican_Church_Facade_20261110_texture.obj");
	Model ofrenda3((char*)"Models/Meshy_AI_Purepecha_Food_Offering_20261110_texture.obj");
	Model tumba2((char*)"Models/Meshy_AI_purepecha_stone_grave_20261110_texture.obj");
	Model arco((char*)"Models/Meshy_AI_purepecha_tzirukua_arch_20261110_texture.obj");
	Model tumba3((char*)"Models/Meshy_AI_stone_crypt_grave_20261110_texture.obj");
	//Model canserbero((char*)"Models/Meshy_AI_Stylized_Rap_Artist_NPC_20261111_texture.obj");

	Model DogBody((char*)"Models/xolo_cuerpo.obj");
	Model HeadDog((char*)"Models/xolo_cabeza.obj");
	Model F_RightLeg((char*)"Models/xolo_frontDer.obj");
	Model F_LeftLeg((char*)"Models/xolo_frontIzq.obj");
	Model B_RightLeg((char*)"Models/xolo_backDer.obj");
	Model B_LeftLeg((char*)"Models/xolo_backIzq.obj");

	// Inicializar KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++) {
		KeyFrame[i].dogPosX = 0;
		KeyFrame[i].dogPosY = 0;
		KeyFrame[i].dogPosZ = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotDog = 0;
		KeyFrame[i].rotDogInc = 0;
		KeyFrame[i].FLegs = 0; KeyFrame[i].FLegsInc = 0;
		KeyFrame[i].FR_Legs = 0; KeyFrame[i].FR_LegsInc = 0;
		KeyFrame[i].RLegs = 0; KeyFrame[i].RLegsInc = 0;
		KeyFrame[i].head = 0; KeyFrame[i].headInc = 0;
		KeyFrame[i].tail = 0; KeyFrame[i].tailInc = 0;
	}

	// SkyBox Vertices
	GLfloat skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f
	};

	// Skybox VAO/VBO setup
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Carga de texturas del skybox
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.jpg");
	faces.push_back("SkyBox/left.jpg");
	faces.push_back("SkyBox/top.jpg");
	faces.push_back("SkyBox/bottom.jpg");
	faces.push_back("SkyBox/back.jpg");
	faces.push_back("SkyBox/front.jpg");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::vec3 pointLightPositions[] = {
	glm::vec3(0.085f, 0.025f,  0.02f),  // Veladora Izquierda 1
	glm::vec3(0.085f, 0.025f, -0.10f),  // Veladora Izquierda 2
	glm::vec3(0.115f, 0.025f,  0.02f),  // Veladora Derecha 1
	glm::vec3(0.115f, 0.025f, -0.10f)   // Veladora Derecha 2
	};

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		DoMovement();
		Animation();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		// Configuración de Shaders e Iluminación
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

		

		// Point light 1 (Dinámica / Vela)
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		// SpotLight (Linterna de cámara)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		glm::mat4 view = camera.GetViewMatrix();
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Dibujar la Isla / Terreno base
		glm::mat4 model(1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		isla.Draw(lightingShader);

		// --- 2. Dibujar la Fachada de la Iglesia (Al fondo de la isla) ---
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.1f, 0.05f, -0.2f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		iglesia.Draw(lightingShader);

		// --- 3. Dibujar la Puerta / Entrada del Panteón ---
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.1f, 0.028f, 0.25f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta.Draw(lightingShader);

		// --- 4. Dibujar Primer Segmento de Barda Perimetral ---
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.145f, 0.025f, 0.25f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.055f, 0.025f, 0.25f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.015f, 0.025f, 0.25f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		//////////Derecho
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, 0.225f)); // Conectando con el extremo derecho frontal
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giramos 90° para alinear en profundidad
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, 0.19f)); // Tercer segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, 0.155f)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, 0.12f)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, 0.085f)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, 0.05f)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, 0.015f)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, -0.02f)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, -0.055)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, -0.09)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, -0.125)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, -0.16)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, -0.195)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.17f, 0.025f, -0.23)); // Siguiente segmento hacia atrás
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		//////////Izquierodo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, 0.23f)); // Conectando con el extremo izquierdo frontal
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giramos 90°
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, 0.195f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, 0.16f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, 0.125f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, 0.09f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, 0.055f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, 0.02f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, -0.015f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, -0.05f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, -0.085f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, -0.12f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, -0.155f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		//model = glm::mat4 / ai; // (Nota: usa glm::mat4(1.0f) aquí)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, -0.19f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.005f, 0.025f, -0.225f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		muro.Draw(lightingShader);

		// --- 7. Barda Trasera (Cerrando el panteón detrás de la iglesia) ---
		// Conectamos horizontalmente desde X = -0.005 hasta X = 0.17 en Z = -0.55 (sin rotación extra)
		float xStartBack = -0.005f;
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(xStartBack + (i * 0.035f), 0.025f, -0.25f));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			muro.Draw(lightingShader);
		}

		// --- Circuito de Arcos de Cempasúchil ---
		for (float zPos = 0.20f; zPos >= -0.15f; zPos -= 0.05f) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.1f, 0.03f, zPos));

			model = glm::scale(model, glm::vec3(0.025f, 0.025f, 0.025f)); // Misma escala probada
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			arco.Draw(lightingShader);
		}

		// --- FLANCO IZQUIERDO (Adentro del panteón, a la izquierda de los arcos) ---
		// Tumba 1
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.04f, 0.025f, 0.12f)); 
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tumba1.Draw(lightingShader);

		// Tumba 2
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.035f, 0.025f, 0.03f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tumba2.Draw(lightingShader);

		// Tumba 3
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.04f, 0.035f, -0.06f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tumba3.Draw(lightingShader);

		// Tumba 4
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.035f, 0.036f, -0.15f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tumba1.Draw(lightingShader);


		// --- FLANCO DERECHO (Adentro del panteón, a la derecha de los arcos) ---
		// Tumba 5
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.14f, 0.025f, 0.12f)); 
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tumba2.Draw(lightingShader);

		// Tumba 6
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.145f, 0.025f, 0.03f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tumba3.Draw(lightingShader);

		// Tumba 7
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.14f, 0.035f, -0.06f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tumba1.Draw(lightingShader);

		// Tumba 8
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.145f, 0.035f, -0.15f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tumba2.Draw(lightingShader);

		// Copalero Izquierdo 1 (Parte alta del camino)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.065f, 0.03f, 0.08f)); 
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f)); 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		copalero.Draw(lightingShader);

		// Copalero Izquierdo 2 (Parte baja del camino)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.065f, 0.03f, -0.05f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		copalero.Draw(lightingShader);

		// Copalero Derecho 1 (Parte alta del camino)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.135f, 0.015f, 0.08f)); 
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		copalero.Draw(lightingShader);

		// Copalero Derecho 2 (Parte baja del camino)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.135f, 0.03f, -0.05f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		copalero.Draw(lightingShader);


		// --- Dibujar las 4 Veladoras Físicas en la Escena ---
		// Veladora 1 (Izquierda Alta - Intercalada)
		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLightPositions[0]); 
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f)); 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		vela.Draw(lightingShader);

		// Veladora 2 (Izquierda Baja - Intercalada)
		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLightPositions[1]);
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		vela.Draw(lightingShader);

		// Veladora 3 (Derecha Alta - Intercalada)
		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLightPositions[2]);
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		vela.Draw(lightingShader);

		// Veladora 4 (Derecha Baja - Intercalada)
		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLightPositions[3]);
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		vela.Draw(lightingShader);

		// --- OFRENDAS FLANCO IZQUIERDO ---
		// Ofrenda 1 (Al lado de la Tumba 1)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.065f, 0.025f, 0.12f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ofrenda1.Draw(lightingShader);

		// Ofrenda 2 (Al lado de la Tumba 2)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.06f, 0.025f, 0.03f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ofrenda2.Draw(lightingShader);

		// Ofrenda 3 (Al lado de la Tumba 3 - Altura ajustada del terreno)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.065f, 0.035f, -0.06f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ofrenda3.Draw(lightingShader);

		// Ofrenda 4 (Al lado de la Tumba 4 - Altura ajustada del terreno)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.06f, 0.036f, -0.15f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ofrenda1.Draw(lightingShader);


		// --- OFRENDAS FLANCO DERECHO ---
		// Ofrenda 5 (Al lado de la Tumba 5)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.115f, 0.025f, 0.12f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ofrenda2.Draw(lightingShader);

		// Ofrenda 6 (Al lado de la Tumba 6)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.12f, 0.025f, 0.03f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ofrenda3.Draw(lightingShader);

		// Ofrenda 7 (Al lado de la Tumba 7 - Altura ajustada del terreno)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.115f, 0.035f, -0.06f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ofrenda1.Draw(lightingShader);

		// Ofrenda 8 (Al lado de la Tumba 8 - Altura ajustada del terreno)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.12f, 0.035f, -0.15f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ofrenda2.Draw(lightingShader);

		// Personajes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.1f, 0.03f, 0.27f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		diablo.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.1f, 0.036f, -0.16f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		enid.Draw(lightingShader);

		//Mariposas
		// Radio del pequeño círculo de vuelo
		float radioVuelo = 0.03f;

		// --- Mariposa 1 (Sobre la entrada) ---
		glm::vec3 origenMariposa1(0.1f, 0.08f, 0.22f); // Su posición original
		float m1X = animMariposas ? origenMariposa1.x + (radioVuelo * cos(mariposaAngle)) : origenMariposa1.x;
		float m1Z = animMariposas ? origenMariposa1.z + (radioVuelo * sin(mariposaAngle)) : origenMariposa1.z;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(m1X, origenMariposa1.y, m1Z));

		// Rota de manera fluida siguiendo el círculo (o estática si está apagada)
		float rotM1 = animMariposas ? -mariposaAngle : glm::radians(45.0f);
		model = glm::rotate(model, rotM1, glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mariposa.Draw(lightingShader);


		
		// --- Mariposa 4 iglesia
		glm::vec3 origenMariposa4(0.1f, 0.1f, -0.08f); // Su posición original
		float m4X = animMariposas ? origenMariposa4.x + (radioVuelo * cos(mariposaAngle + 1.0f)) : origenMariposa4.x; // Desfasadas un poco con '+ 1.0f' para que no vayan sincronizadas idénticas
		float m4Z = animMariposas ? origenMariposa4.z + (radioVuelo * sin(mariposaAngle + 1.0f)) : origenMariposa4.z;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(m4X, origenMariposa4.y, m4Z));

		float rotM4 = animMariposas ? -mariposaAngle : glm::radians(90.0f);
		model = glm::rotate(model, rotM4, glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mariposa.Draw(lightingShader);


		// --- Mariposa 2 (A mitad del sendero de arcos - lado izquierdo) ---
		glm::vec3 origenMariposa2(0.07f, 0.09f, 0.08f);
		float m2X = animMariposas ? origenMariposa2.x + (radioVuelo * cos(mariposaAngle + 2.0f)) : origenMariposa2.x;
		float m2Z = animMariposas ? origenMariposa2.z + (radioVuelo * sin(mariposaAngle + 2.0f)) : origenMariposa2.z;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(m2X, origenMariposa2.y, m2Z));
		float rotM2 = animMariposas ? -mariposaAngle : glm::radians(120.0f);
		model = glm::rotate(model, rotM2, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mariposa.Draw(lightingShader);

		// --- Mariposa 3 (A mitad del sendero de arcos - lado derecho) ---
		glm::vec3 origenMariposa3(0.13f, 0.095f, 0.02f);
		float m3X = animMariposas ? origenMariposa3.x + (radioVuelo * cos(mariposaAngle + 3.5f)) : origenMariposa3.x;
		float m3Z = animMariposas ? origenMariposa3.z + (radioVuelo * sin(mariposaAngle + 3.5f)) : origenMariposa3.z;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(m3X, origenMariposa3.y, m3Z));
		float rotM3 = animMariposas ? -mariposaAngle : glm::radians(-30.0f);
		model = glm::rotate(model, rotM3, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mariposa.Draw(lightingShader);

		// --- Mariposa 5 (Revoloteando por el flanco izquierdo de las tumbas)
		glm::vec3 origenMariposa5(0.04f, 0.085f, -0.02f);
		float m5X = animMariposas ? origenMariposa5.x + (radioVuelo * cos(mariposaAngle + 4.8f)) : origenMariposa5.x;
		float m5Z = animMariposas ? origenMariposa5.z + (radioVuelo * sin(mariposaAngle + 4.8f)) : origenMariposa5.z;

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(m5X, origenMariposa5.y, m5Z));
		float rotM5 = animMariposas ? -mariposaAngle : glm::radians(200.0f);
		model = glm::rotate(model, rotM5, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mariposa.Draw(lightingShader);

		//// Mariposa 2 (A mitad del sendero de arcos - lado izquierdo)
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.07f, 0.09f, 0.08f));
		//model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//mariposa.Draw(lightingShader);

		//// Mariposa 3 (A mitad del sendero de arcos - lado derecho)
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.13f, 0.095f, 0.02f));
		//model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//mariposa.Draw(lightingShader);

		//// Mariposa 5 (Revoloteando por el flanco izquierdo de las tumbas)
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.04f, 0.085f, -0.02f));
		//model = glm::rotate(model, glm::radians(200.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//mariposa.Draw(lightingShader);


		// Animación del Xoloescuincle
		glm::mat4 modelDog = glm::mat4(1.0f);
		modelDog = glm::translate(modelDog, glm::vec3(0.1f, 0.025f, xoloZ)); // Camino de arcos (X = 0.1)

		// Orientación según la dirección en la que camina
		if (xoloAvanzando) {
			modelDog = glm::rotate(modelDog, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Apunta hacia la iglesia
		}
		else {
			modelDog = glm::rotate(modelDog, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Apunta hacia la entrada
		}

		glm::mat4 modelDogScaled = glm::scale(modelDog, glm::vec3(0.01f, 0.01f, 0.01f)); // Escala del perro

		// 1. Dibujar Cuerpo (Raíz)
		model = modelDogScaled;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DogBody.Draw(lightingShader);

		// 2. Dibujar Cabeza
		model = modelDogScaled;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		HeadDog.Draw(lightingShader);

		// 3. Pata Delantera Derecha
		model = glm::translate(modelDogScaled, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(xoloRotPatas), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		F_RightLeg.Draw(lightingShader);

		// 4. Pata Delantera Izquierda (Paso alternado con valor negativo)
		model = glm::translate(modelDogScaled, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-xoloRotPatas), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		F_LeftLeg.Draw(lightingShader);

		// 5. Pata Trasera Derecha
		model = glm::translate(modelDogScaled, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-xoloRotPatas), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		B_RightLeg.Draw(lightingShader);

		// 6. Pata Trasera Izquierda
		model = glm::translate(modelDogScaled, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(xoloRotPatas), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		B_LeftLeg.Draw(lightingShader);

		// Canoa
		glm::mat4 modelCanoa = glm::mat4(1.0f);

		// Ajusta la Z fija según la franja de agua de la orilla que mostraste en la foto,
		// y 'canoaPos' se encargará de moverla de izquierda a derecha (o viceversa).
		modelCanoa = glm::translate(modelCanoa, glm::vec3(canoaPos, -0.021f, 0.46f));

		// Orientación: Si va de ida apunta hacia un lado (0°), si regresa rota 180°
		if (canoaIda) {
			// Cambia 90.0f por 0.0f (o 180.0f) para alinearla de lado según hacia dónde avance
			modelCanoa = glm::rotate(modelCanoa, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			// Cambia -90.0f por 180.0f (o 0.0f) para cuando vaya de regreso
			modelCanoa = glm::rotate(modelCanoa, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		modelCanoa = glm::scale(modelCanoa, glm::vec3(0.035f, 0.035f, 0.035f)); // Escala de la canoa

		model = modelCanoa;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		canoa.Draw(lightingShader);

		// --- Dibujar el Skybox al fondo ---
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Use();
		view = camera.GetViewMatrix();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);



		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// Control de movimiento de cámara y luces
void DoMovement() {
	// Reducimos la velocidad multiplicando deltaTime por un factor (ej. 0.5f para ir a la mitad de velocidad)
	GLfloat camSpeed = deltaTime * 0.15f;

	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, camSpeed);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, camSpeed);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, camSpeed);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, camSpeed);

	// Movimiento de la luz puntual con T, G, Y, H, U, J
	if (keys[GLFW_KEY_T]) pointLightPositions[0].x += 0.01f;
	if (keys[GLFW_KEY_G]) pointLightPositions[0].x -= 0.01f;
	if (keys[GLFW_KEY_Y]) pointLightPositions[0].y += 0.01f;
	if (keys[GLFW_KEY_H]) pointLightPositions[0].y -= 0.01f;
	if (keys[GLFW_KEY_U]) pointLightPositions[0].z -= 0.1f;
	if (keys[GLFW_KEY_J]) pointLightPositions[0].z += 0.01f;

	//if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, deltaTime);
	//if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, deltaTime);
	//if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, deltaTime);
	//if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

	//// Movimiento de la luz puntual con T, G, Y, H, U, J
	//if (keys[GLFW_KEY_T]) pointLightPositions[0].x += 0.01f;
	//if (keys[GLFW_KEY_G]) pointLightPositions[0].x -= 0.01f;
	//if (keys[GLFW_KEY_Y]) pointLightPositions[0].y += 0.01f;
	//if (keys[GLFW_KEY_H]) pointLightPositions[0].y -= 0.01f;
	//if (keys[GLFW_KEY_U]) pointLightPositions[0].z -= 0.1f;
	//if (keys[GLFW_KEY_J]) pointLightPositions[0].z += 0.01f;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (keys[GLFW_KEY_L]) {
		if (play == false && (FrameIndex > 1)) {
			resetElements();
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else {
			play = false;
		}
	}

	if (keys[GLFW_KEY_K]) {
		if (FrameIndex < MAX_FRAMES) saveFrame();
	}

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}

	// Activar/Desactivar luz ambiental con barra espaciadora
	if (keys[GLFW_KEY_SPACE]) {
		active = !active;
		if (active) Light1 = glm::vec3(0.2f, 0.8f, 1.0f);
		else Light1 = glm::vec3(0);
	}

	if (keys[GLFW_KEY_X])
	{
		animXolo = !animXolo;
	}

	if (keys[GLFW_KEY_V])
	{
		animCanoa = !animCanoa;
	}
	if (keys[GLFW_KEY_M])
	{
		animMariposas = !animMariposas;
	}
	if (keys[GLFW_KEY_I])
	{
		modoCultural = !modoCultural;
		if (modoCultural) {
			std::cout << "\n==================================================================\n";
			std::cout << "       TRADICION PUREPECHA: NOCHE DE ANIMAS (DIA DE MUERTOS)      \n";
			std::cout << "==================================================================\n";
			std::cout << "1. EL XOLOESCUINCLE: Guia a las animas a traves del rio hacia el Mictlan.\n";
			std::cout << "2. MARIPOSAS MONARCA: Representan las almas de los difuntos que visitan\n";
			std::cout << "   los hogares durante la temporada en Michoacan.\n";
			std::cout << "3. ARCOS Y FLORES DE CEMPASUCHIL: Su aroma guia a las almas hacia\n";
			std::cout << "   el altar y el panteon.\n";
			std::cout << "4. LAS OFRENDAS Y TUMBAS: Celebran el eterno retorno de los seres queridos.\n";
			std::cout << "==================================================================\n\n";
		}
	}
	if (keys[GLFW_KEY_O])
	{
		infoUbicacion = !infoUbicacion;
		if (infoUbicacion) {
			std::cout << "\n==================================================================\n";
			std::cout << "         UBICACION Y CARACTERISTICAS DE LA CELEBRACION            \n";
			std::cout << "==================================================================\n";
			std::cout << "1. UBICACION: Lago de Patzcuaro, en el estado de Michoacan, Mexico.\n";
			std::cout << "2. LA ISLA (Janitzio / Tancitaro / Yunuen): El panteon se encuentra\n";
			std::cout << "   enclavado en una comunidad insular rodeada por agua.\n";
			std::cout << "3. FECHA DE CELEBRACION: Se lleva a cabo la noche del 1 al 2 de\n";
			std::cout << "   noviembre, cuando las animas regresan del anexo mundo.\n";
			std::cout << "==================================================================\n\n";
		}
	}
}

void Animation() {
	if (animXolo)
	{
		
		if (xoloAvanzando) {
			xoloZ -= 0.00015f; // Avanza más despacio hacia la iglesia (-Z)
			if (xoloZ <= -0.15f) {
				xoloAvanzando = false;
			}
		}
		else {
			xoloZ += 0.00015f; // Regresa más despacio hacia la entrada (+Z)
			if (xoloZ >= 0.20f) {
				xoloAvanzando = true;
			}
		}

		// Oscilación de las patas más suave y pausada (bajamos de 12.0f a 4.0f)
		xoloRotPatas = sin(glfwGetTime() * 4.0f) * 15.0f;
	}
	else
	{
		xoloRotPatas = 0.0f;
	}

	if (animCanoa)
	{
		// Movimiento lineal de ida y vuelta sobre la zona de agua de la orilla
		if (canoaIda) {
			canoaPos += 0.0003f; // Se desplaza hacia un lado
			if (canoaPos >= 0.1f) {
				canoaIda = false; // Cambia de dirección
			}
		}
		else {
			canoaPos -= 0.0003f; // Regresa
			if (canoaPos <= -0.1f) {
				canoaIda = true;
			}
		}
	}
	if (animMariposas)
	{
		mariposaAngle += 0.015f; // Velocidad del vuelo circular
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;
	camera.ProcessMouseMovement(xOffset, yOffset);
}