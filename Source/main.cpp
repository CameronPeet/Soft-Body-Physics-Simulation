// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name   : main.cpp
// Description : Main implementation file 
// Author	   : Cameron Peet
// Mail		   : Cameron.Pee6737@mediadesignschool.com

//Includes
#include <ctime>
#include <iostream>


//Local Includes
#include "Utility\ShaderLoader.h"
#include "Graphics\Camera.h"
#include "Graphics\SkyBox.h"
#include "Graphics\Light.h"
#include "Graphics\ObjModel.h"
#include "Graphics\Terrain.h"
#include "Graphics\GeometryModel.h"

#include "Physics\PhysicsWorld.h"

#include "Cloth.h"

#include "TextLabel.h"

struct MouseInfo
{
	//FPS movement
	float lastX = 600.0f;
	float lastY = 400.0f;

	float pitch = 0.0f;
	float yaw = 90.0f;
	bool firstMouse = true;
	bool lock = true;

	//used for physics

	//noramalised mouseX and Y coords
	//NDC
	float mouseX, mouseY;
	bool mouseDown = false;
};


std::vector<TextLabel*> m_pCurrentMenu;
TextLabel* m_tHooks;
TextLabel* m_tReset;

int hooks = 2;

MouseInfo g_MouseInfo;

//Raycasting Mouse info
void pickingPreTickCallback(btDynamicsWorld *world, btScalar timeStep);


//Local Functions
bool Init();
void Render();
void Update();
void KeyDown(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void Reshape(int width, int height);
void ResetCloth();
void ResetNodeMass();
void CreatePhysicsCloth(btScalar x, btScalar y);
void CreatePhysicsWorld();
void CreatePhysicsBox();
void ApplyFanForces();
void PassiveMotion(int x, int y);
void MouseButton(int button, int state, int x, int y);
void MouseMoveWhileClicked(int x, int y);
void DefineUniformBufferObjects();
void BindUBO();
bool g_WindowRunning = false;

//Global Timer for calculating deltaTime
std::clock_t g_PreviousTicks;
std::clock_t g_CurrentTicks;

//Uniform buffer objects for uniform mat4's across all shaders (Camera Projectiona nd View matrix)
GLuint g_uboMatrices;
//Global Shader Programs
ShaderLoader shaderLoader;
GLuint standardShader;
GLuint skyboxShader;
GLuint objModelShader;
GLuint terrainShader;
//Global geometry shaders
GLuint geometry_explode_shader;
GLuint geometry_show_normals;
GLuint geometry_model_star;

GLuint* AllShaders[] =
{
	&standardShader,
	&skyboxShader,
	&objModelShader,
	&terrainShader,
	&geometry_explode_shader,
	&geometry_show_normals,
	&geometry_model_star
};


//Global Rendering Components and CObjects
Camera g_Camera;
SkyBox g_SkyBox;
Light g_GlobalLight;


glm::vec3 g_Movement;



//Bullet physics entities
//Initialised in CreatePhysicsWorld();
btBroadphaseInterface*					broadphase;
btDefaultCollisionConfiguration*		collisionConfiguration;
btCollisionDispatcher*					dispatcher;
btSequentialImpulseConstraintSolver*	solver;
btSoftRigidDynamicsWorld*				dynamicsWorld;



btSoftBody* cloth;
PhysicsBody g_Cloth;
btRigidBody* box;
Model g_Box;
Model g_fanBox;
btVector3 g_btFanPosition;
btSoftBodyWorldInfo softBodyWorldInfo;

btScalar g_ClothScalarX = 4;
btScalar g_ClothScalarY = 4;
float g_ResizeTimer = 0.0f;

float g_fanSpeed = 0.3f;


btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color = btVector4(1, 0, 0, 1))
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(cInfo);

	body->setUserIndex(-1);
	dynamicsWorld->addRigidBody(body);
	return body;
}
float * tex_coords;

void CreatePhysicsWorld()
{
	//STEP 1:
	//Defines the broadphase algorithm
	broadphase = new btDbvtBroadphase();

	//Collision configuration allows us to fine tune the algorithms used for the full collision detection (not broadphase)
	collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	//If you introduce different types of collision objects later  on (eg meshes using btGimpactMeshShape) then we may need
	//to register a collision algorithm to get collision recognised
	//Register the collision dispatcher with an addative collison algorithm
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

	//Adding a solver, this causes objects to interact properly, taking into account gravity, game logic, supplied forces, collisons and hinge constrains.
	//Does well if not pushed, bottlenecks at high performance simulations. Parralel versions available for threading models
	solver = new btSequentialImpulseConstraintSolver();


	//Instantiate the dynamics world
	dynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//Set the Gravity!
	dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

	softBodyWorldInfo.m_broadphase = broadphase;
	softBodyWorldInfo.m_dispatcher = dispatcher;
	softBodyWorldInfo.m_gravity = dynamicsWorld->getGravity();
	softBodyWorldInfo.m_sparsesdf.Initialize();

	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

	btRigidBody::btRigidBodyConstructionInfo
	groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	dynamicsWorld->addRigidBody(groundRigidBody);
}

const int numX = 20;
const int numY = 20;

void CreatePhysicsCloth(btScalar x, btScalar y)
{
	const btScalar s = 4;

	std::cout << "x = " << x << ", y = " << y << ", s = " << s << endl;



	const int fixed = 1 + 2;

	tex_coords = new float[(numX - 1)*(numY - 1) * 12];

	//This creates the cloth, and the UV extention tex_coords calculates the texture coords and places them in that array
	cloth = btSoftBodyHelpers::CreatePatchUV(softBodyWorldInfo,
		btVector3(-x / 2, y + 1, 0),
		btVector3(+x / 2, y + 1, 0),
		btVector3(-x / 2, s - y + 1, +s),
		btVector3(+x / 2, s - y + 1, +s),
		numX, numY,
		fixed, true, tex_coords);



	btSoftBody::Material* pm= cloth->appendMaterial();
	pm->m_kLST = 0.0f;
	pm->m_kAST = 0.0f;
	pm->m_kVST = 0.0f;

	cloth->getCollisionShape()->setMargin(0.01f);

	cloth->generateBendingConstraints(6, pm);

	cloth->m_cfg.citerations = 5;
	cloth->m_cfg.diterations = 5;

	cloth->m_cfg.piterations = 5;

	cloth->m_cfg.collisions = btSoftBody::fCollision::SDF_RS+btSoftBody::fCollision::CL_SS + btSoftBody::fCollision::CL_SELF;
	
	cloth->setTotalMass(10);
	
	//cloth->generateClusters(8);

	cloth->m_cfg.kDF = 0.8f;
	cloth->m_cfg.kDP = 0.005f;


	
	dynamicsWorld->addSoftBody(cloth);
}

void CreatePhysicsBox()
{
	btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(1.1), btScalar(1.1), btScalar(1.1)));
	btTransform groundTransform;
	groundTransform.setIdentity();
	
	glm::mat4 transform;
	transform = glm::translate(transform, glm::vec3(0, 1, 0));

	groundTransform.setFromOpenGLMatrix(glm::value_ptr(transform));

	box = createRigidBody(btScalar(0.), groundTransform, groundShape);
}

//What are your start up options
int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowPosition(GLUT_WINDOW_WIDTH / 2, GLUT_WINDOW_HEIGHT / 2);
	glutInitWindowSize(1200, 800);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glEnable(GL_MULTISAMPLE);
	glutCreateWindow("Adcanced Graphics - Week 9 - Height Maps");


	glClearColor(1.0f, 1.0f, 0.0f, 0.5f);               // Set background color to black and opaque
	glClearDepth(1.0f);                                 // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);                            // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);                             // Set the type of depth-test
	glShadeModel(GL_SMOOTH);                            // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections

	glewExperimental = GL_TRUE;

	glewInit();

	if (!Init())
		return EXIT_FAILURE;

	glutDisplayFunc(Render);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutIdleFunc(Update);
	glutReshapeFunc(Reshape);
	glutPassiveMotionFunc(PassiveMotion);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMoveWhileClicked);

	glutMainLoop();

	return EXIT_SUCCESS;
}

bool Init()
{
	CreatePhysicsWorld();

	if (dynamicsWorld == nullptr)
	{
		std::cout << "Unknown error in creating Physics dynamicWorld... exiting" << std::endl;
		int wait;
		std::cin >> wait;
		return false;
	}

	CreatePhysicsCloth(g_ClothScalarX, g_ClothScalarY);
	CreatePhysicsBox();


	//Load and create shader files
	standardShader = shaderLoader.CreateProgram(
		"Assets/shaders/shader.vert",
		"Assets/shaders/shader.frag");

	skyboxShader = shaderLoader.CreateProgram(
		"Assets/shaders/Skybox.vs",
		"Assets/shaders/Skybox.frag");

	objModelShader = shaderLoader.CreateProgram(
		"Assets/shaders/3D_Model.vert",
		"Assets/shaders/3D_Model.frag");

	terrainShader = shaderLoader.CreateProgram(
		"Assets/shaders/heightmap.vs",
		"Assets/shaders/heightmap.fs");

	geometry_explode_shader = shaderLoader.CreateProgram(
		"Assets/shaders/geometry_explode.vs",
		"Assets/shaders/geometry_explode.frag",
		"Assets/shaders/geometry_explode.gs");

	geometry_show_normals = shaderLoader.CreateProgram(
		"Assets/shaders/geometry_show_normals.vs",
		"Assets/shaders/geometry_show_normals.frag",
		"Assets/shaders/geometry_show_normals.gs");

	geometry_model_star = shaderLoader.CreateProgram(
		"Assets/shaders/geometry_model_star.vert",
		"Assets/shaders/geometry_model_star.frag",
		"Assets/shaders/geometry_model_star.gs");

	//Camera creation
	g_Camera.SetPosition(glm::vec3(0.0f, 3, -7));
	g_Camera.SetViewPort(0, 0, 1200.0f, 800.0f);
	g_Camera.SetProjection(45.0f, (float)(1200.0f / 800.0f), 0.1f, 1000.0f);
	g_Camera.SetRotation(glm::rotate(g_Camera.GetRotation(), glm::radians(170.0f), glm::vec3(0, 1, 0)));

	g_GlobalLight.Position.y += 10.0f;

	//Skybox creation
	g_SkyBox = SkyBox(CUBE,
		"Assets/skybox/right.jpg",  //Each tile is labeled according to its mapped position on a box
		"Assets/skybox/left.jpg",
		"Assets/skybox/top.jpg",
		"Assets/skybox/bottom.jpg",
		"Assets/skybox/back.jpg",
		"Assets/skybox/front.jpg");
	g_SkyBox.Initialise();

	
	g_Cloth = PhysicsBody();
	g_Cloth.DynamicDraw = true;
	g_Cloth.texturePath = "assets/textures/Cloth.jpg";
	g_Cloth.Initialise();
	g_Cloth.m_Position = glm::vec3(0, 0, 0);
	g_Cloth.ObjectColor = glm::vec3(1.0f, 1.0f, 1.0f);


	g_Box = Model(CUBE, "assets/textures/ball.jpg", true);
	g_Box.Initialise();
	g_Box.m_Position = glm::vec3(0, 1, 0);
	g_Box.ObjectColor = glm::vec3(1.0f, 1.0f, 1.0f);

	g_fanBox = Model(CUBE, "assets/textures/ball.jpg", false);
	g_fanBox.Initialise();
	g_fanBox.m_Position = glm::vec3(0, 3, -2);
	g_fanBox.ObjectColor = glm::vec3(1.0f, 1.0f, 1.0f);
	g_fanBox.m_Scale = glm::vec3(0.2f, 0.2f, 0.2f);


	m_tHooks = (new TextLabel("Hooks : 5", "Assets/Fonts/waltographUI.ttf"));
	m_tHooks->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	m_tHooks->setPosition(glm::vec3(0.0f, 100.0f, 0.0f));
	m_tHooks->setScale(0.5f);
	
	TextLabel* pText = new TextLabel("UNLOCK MOUSE : SPACE_BAR ", "Assets/Fonts/waltographUI.ttf");
	pText->setColor(glm::vec3(1.0f, 1.0f, 0.0f));
	pText->setPosition(glm::vec3(0.0f, 00.0f, 0.0f));
	pText->setScale(0.75f);
	m_pCurrentMenu.push_back(pText);

	pText = new TextLabel("Release Hooks", "Assets/Fonts/waltographUI.ttf");
	pText->setColor(glm::vec3(1.0f, 1.0f, 0.0f));
	pText->setPosition(glm::vec3(0.0f, 200.0f, 0.0f));
	pText->setScale(0.5f);
	m_pCurrentMenu.push_back(pText);

	pText = new TextLabel("Fan : Off", "Assets/Fonts/waltographUI.ttf");
	pText->setColor(glm::vec3(1.0f, 1.0f, 0.0f));
	pText->setPosition(glm::vec3(0.0f, 300.0f, 0.0f));
	pText->setScale(0.5f);
	m_pCurrentMenu.push_back(pText);

	m_pCurrentMenu.push_back(m_tHooks);

	DefineUniformBufferObjects();
	BindUBO();
	return true;
}

void UpdateSoftBodyVertices()
{
	if (cloth == nullptr)
		return;
	/* Each soft body contain an array of vertices (nodes/particles_mass)   */
	btSoftBody::tNodeArray&   _nodes(cloth->m_nodes);

	/* And edges (links/distances constraints)                        */
	btSoftBody::tLinkArray&   _links(cloth->m_links);

	/* And finally, faces (triangles)                                 */
	btSoftBody::tFaceArray&   _faces(cloth->m_faces);


	g_Cloth.vertices.clear();
	g_Cloth.indices.clear();

	/* Then, you can draw vertices...    */
	/* Node::m_x => position             */
	/* Node::m_n => normal				 */
	int ndoesize = _nodes.size();
	int tex = 0;

	for (int j = 0; j< _nodes.size(); ++j)
	{
		Vertex2 v = Vertex2();
		btVector3 p = _nodes[j].m_x;
		btVector3 n = _nodes[j].m_n;

		v.pos = glm::vec3(p[0], p[1], p[2]);
		v.n = glm::vec3(n[0], n[1], n[2]);

		g_Cloth.vertices.push_back(v);
	}

	/* Or edges (for ropes)               */
	/* Link::m_n[2] => pointers to nodes   */
	/*if (node_0->m_v.length >= 0.1f)
	{
		int node1 = (int(node_0 - &_nodes[0]));
		int node2 = (int(node_1 - &_nodes[0]));

	}*/


	for (int j = 0; j<_links.size(); ++j)
	{
		btSoftBody::Node*   node_0 = _links[j].m_n[0];
		btSoftBody::Node*   node_1 = _links[j].m_n[1];


			
		btVector3 p1 = node_0->m_x;
		btVector3 n1 = node_0->m_n;
		btVector3 p2 = node_1->m_x;
		btVector3 n2 = node_1->m_n;

		Vertex2 v0 = Vertex2();
		Vertex2 v1 = Vertex2();


		v0.pos = glm::vec3(p1[0], p1[1], p1[2]);
		v0.n = glm::vec3(n1[0], n1[1], n1[2]);
		v0.uv = glm::vec2(tex_coords[tex + 0], tex_coords[tex + 1]);

		v1.pos = glm::vec3(p2[0], p2[1], p2[2]);
		v1.n = glm::vec3(n2[0], n2[1], n2[2]);
		v1.uv = glm::vec2(tex_coords[tex + 2], tex_coords[tex + 3]);

		g_Cloth.vertices.push_back(v0);
		g_Cloth.vertices.push_back(v1);

	}

	/* And faces							*/
	/* Face::m_n[3] -> pointers to nodes	*/
	for (int j = 0; j< _faces.size(); ++j)
	{
		btSoftBody::Node*   node_0 = _faces[j].m_n[0];
		btSoftBody::Node*   node_1 = _faces[j].m_n[1];
		btSoftBody::Node*   node_2 = _faces[j].m_n[2];

		btVector3 p1 = node_0->m_x;
		btVector3 n1 = node_0->m_n;
		btVector3 p2 = node_1->m_x;
		btVector3 n2 = node_1->m_n;
		btVector3 p3 = node_2->m_x;
		btVector3 n3 = node_2->m_n;

		Vertex2 v = Vertex2();
		Vertex2 v1 = Vertex2();
		Vertex2 v2 = Vertex2();

		v.pos = glm::vec3(p1[0], p1[1], p1[2]);
		v.n = glm::vec3(n1[0], n1[1], n1[2]);
		v.uv = glm::vec2(tex_coords[tex + 0], tex_coords[tex + 1]);

		v1.pos = glm::vec3(p2[0], p2[1], p2[2]);
		v1.n = glm::vec3(n2[0], n2[1], n2[2]);
		v1.uv = glm::vec2(tex_coords[tex + 2], tex_coords[tex + 3]);
		
		v2.pos = glm::vec3(p3[0], p3[1], p3[2]);
		v2.n = glm::vec3(n3[0], n3[1], n3[2]);
		v2.uv = glm::vec2(tex_coords[tex + 4], tex_coords[tex + 5]);


		g_Cloth.vertices.push_back(v);
		g_Cloth.vertices.push_back(v1);
		g_Cloth.vertices.push_back(v2);

		g_Cloth.indices.push_back(int(node_0 - &_nodes[0]));
		g_Cloth.indices.push_back(int(node_1 - &_nodes[0]));
		g_Cloth.indices.push_back(int(node_2 - &_nodes[0]));

		tex += 6;
	}

}


void Render()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update the uniform buffer objects in the shaders with the latest proj and view matrix
	BindUBO();
	g_SkyBox.Render(skyboxShader, g_Camera);

	g_fanBox.Render(standardShader, g_Camera);

	if(cloth != nullptr)
		UpdateSoftBodyVertices();

	g_Cloth.Render(standardShader, g_Camera);

	btTransform t = box->getWorldTransform();
	GLfloat* ModelMatrix = new GLfloat[16];
	t.getOpenGLMatrix(ModelMatrix);

	g_Box.modelMatrix = glm::make_mat4x4(ModelMatrix);
	g_Box.Render(standardShader, g_Camera);

	for (auto itr : m_pCurrentMenu)
	{
		itr->Render(g_Camera);
	}

	//g_Sphere.Render(standardShader, g_Camera);
	//RenderOld();

	glBindVertexArray(0);
	glutSwapBuffers();
}

int MoveHinge = 0 ;


void Update()
{
	g_CurrentTicks = std::clock();
	float deltaTicks = (float)(g_CurrentTicks - g_PreviousTicks);

	float fDeltaTime = deltaTicks / (float)CLOCKS_PER_SEC;

	//Physics Update Stuff


	//If dragging a body / soft body
	if(g_MouseInfo.mouseDown)
		pickingPreTickCallback(dynamicsWorld, fDeltaTime);


	if (MoveHinge != 0)
	{
		btSoftBody::tNodeArray& _nodes(cloth->m_nodes);

		//Get node
		for (int i = 0; i < numX; i++)
		{
			if ((cloth->getMass(i)) <= 0.09f)
			{
				//Get distance
				float distance = 0.0f;
				btVector3 a = _nodes[i].m_x;
				btVector3 b = _nodes[19].m_x;

				distance = (a - b).length() * MoveHinge;
				if (i == 0)
				{
					cout << distance << endl;
				}
				_nodes[i].m_v = btVector3(distance, 0, 0) * fDeltaTime;
			}
		}
	}

	//World Simulation
	dynamicsWorld->stepSimulation(fDeltaTime);

	g_ResizeTimer += fDeltaTime;

	g_PreviousTicks = g_CurrentTicks;

	g_btFanPosition = btVector3(g_fanBox.m_Position.x, g_fanBox.m_Position.y, g_fanBox.m_Position.z);
	//ApplyFanForces();
	
	glUseProgram(0);
	g_Camera.Translate(g_Movement * 10.0f * fDeltaTime);
	g_WindowRunning = true;
	Sleep(10);
	glutPostRedisplay();
}

void ApplyFanForces()
{
	btSoftBody::tNodeArray&   _nodes(cloth->m_nodes);

	for (int j = 0; j< _nodes.size() / 2; ++j)
	{
		// Reset force
	//	_nodes[j].m_f = btVector3(0, 0, 0);
		
		btVector3 nodeToFan = _nodes[j].m_x - g_btFanPosition;

		// Convert from btVector3 to glm::vec3
		glm::vec3 NodeToFan = glm::vec3(nodeToFan.m_floats[0], nodeToFan.m_floats[1], nodeToFan.m_floats[2]);
		glm::vec3 nodeNormal = glm::vec3(_nodes[j].m_n.m_floats[0], _nodes[j].m_n.m_floats[1], _nodes[j].m_n.m_floats[2]);

		float dotProduct = glm::dot(NodeToFan, nodeNormal);

		if (dotProduct == 0)
			dotProduct = 0.0001f;

		// Apply force based on dot product
		_nodes[j].m_f = btVector3(1, 1, 1) * 0.2 / dotProduct;
	//	_nodes[j].m_f = -nodeToFan * (0.01f / dotProduct);
	}
}

void KeyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'W':
		g_Movement.z = -1.0f;
		break;
	case 'a':
	case 'A':
		g_Movement.x = -1.0f;
		break;
	case 's':
	case 'S':
		g_Movement.z = 1.0f;
		break;
	case 'd':
	case 'D':
		g_Movement.x = 1.0f;
		break;
	case 'q':
	case 'Q':
		g_Movement.y = -1.0f;
		break;
	case 'e':
	case 'E':
		g_Movement.y = 1.0f;
		break;

	case VK_ESCAPE:
		glutLeaveMainLoop();
		break;

	case 'z':
	case 'Z':
		g_ClothScalarX = 4;
		g_ClothScalarY = 4;
		ResetCloth();
		break;

	case 'i':
	case 'I':

		if ((g_ClothScalarY < 9) && (g_ResizeTimer > 0.5f))
		{
			g_ClothScalarY += 1;
			ResetCloth();
		}	
		break;

	case 'k':
	case 'K':
		if ((g_ClothScalarY > 2) && (g_ResizeTimer > 0.5f))
		{
			g_ClothScalarY -= 1;
			ResetCloth();
		}

		break;

	case 'j':
	case 'J':
		if ((g_ClothScalarX > 2) && (g_ResizeTimer > 0.5f))
		{
			g_ClothScalarX -= 1;
			ResetCloth();
		}
		
		break;

	case 'l':
	case 'L':
		if ((g_ClothScalarX < 9) && (g_ResizeTimer > 0.5f))
		{
			g_ClothScalarX += 1;
			ResetCloth();
		}
		break;
	case VK_SPACE:
	{
		g_MouseInfo.lock = !g_MouseInfo.lock;
//		ResetNodeMass();
	} 
		break;
	case 't':
	case 'T':
		g_fanBox.m_Position += glm::vec3(0.0f, 0.0f, 1.0f) * g_fanSpeed;
		break;
	case 'f':
	case 'F':
		g_fanBox.m_Position += glm::vec3(1.0f, 0.0f, 0.0f) * g_fanSpeed;
		break;
	case 'g':
	case 'G':
		g_fanBox.m_Position -= glm::vec3(0.0f, 0.0f, 1.0f) * g_fanSpeed;
		break;
	case 'h':
	case 'H':
		g_fanBox.m_Position -= glm::vec3(1.0f, 0.0f, 0.0f) * g_fanSpeed;
		break;
	case 'y':
	case 'Y':
		g_fanBox.m_Position += glm::vec3(0.0f, 1.0f, 0.0f) * g_fanSpeed;
		break;
	case 'r':
	case 'R':
		g_fanBox.m_Position -= glm::vec3(0.0f, 1.0f, 0.0f) * g_fanSpeed;
		break;

	case '7':
		MoveHinge = -1;
		break;
	case '6':
		MoveHinge = 1;
		break;
	default:
		break;
	}
}

void ResetNodeMass()
{
	for (int i = 0; i < numX; i++)
	{
		cloth->setMass(i, 0.1f);
	}
}



void KeyUp(unsigned char key, int x, int y)
{
	btSoftBody::tNodeArray& _nodes(cloth->m_nodes);

	switch (key)
	{
	case 'w':
	case 'W':
		g_Movement.z = 0.f;
		break;
	case 'a':
	case 'A':
		g_Movement.x = 0.f;
		break;
	case 's':
	case 'S':
		g_Movement.z = 0.f;
		break;
	case 'd':
	case 'D':
		g_Movement.x = 0.f;
		break;
	case 'q':
	case 'Q':
		g_Movement.y = 0.f;
		break;
	case 'e':
	case 'E':
		g_Movement.y = 0.f;
		break;

	case '1':
	{
		ResetCloth();
		cloth->setMass(0, 0.0f);
		cloth->setMass(19, 0.0f);
	}
	break;
	case '2':
	{
		ResetCloth();
		for (int i = 0; i < numX; i += 5)
			cloth->setMass(i, 0.0f);
	}
	break;
	case '3':
	{
		ResetCloth();
		for (int i = 0; i < numX; i += 3)
			cloth->setMass(i, 0.0f);
	}
	break;

	case '4':
	{
		ResetCloth();
		for (int i = 0; i < numX; i += 2)
			cloth->setMass(i, 0.0f);
	}
	break;

	case '5':
	{
		ResetCloth();
		for (int i = 0; i < numX; i++)
			cloth->setMass(i, 0.0f);
	}
	break;

	case '6':
	case '7':
		MoveHinge = 0;
	break;

	case '8':
	{
	} break;

	default:
		break;
	}
}

void Reshape(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	g_Camera.SetViewPort(0, 0, width, height);
	g_Camera.SetProjection(45.0f, (float)(width / height), 0.1f, 1000.0f);
}

void ResetCloth()
{
	cloth->releaseClusters();
	cloth->~btSoftBody();
	dynamicsWorld->removeSoftBody(cloth);
	delete cloth;
	cloth = nullptr;
	//g_Cloth.~PhysicsBody();

//	std::cout << g_ClothScalar << endl;
	CreatePhysicsCloth(g_ClothScalarX, g_ClothScalarY);

	//g_Cloth = PhysicsBody();
	//g_Cloth.DynamicDraw = true;
	//g_Cloth.texturePath = "assets/textures/Cloth.jpg";
	//g_Cloth.Initialise();
	//g_Cloth.m_Position = glm::vec3(0, 0, 0);
	//g_Cloth.ObjectColor = glm::vec3(1.0f, 1.0f, 1.0f);

	g_ResizeTimer = 0.0f;
}


void ResetPointer()
{
	if (!g_MouseInfo.lock)
		return;
	glutWarpPointer(600, 400);
	g_MouseInfo.lastX = 600.0f;
	g_MouseInfo.lastY = 400.0f;
}





//Current constraint being held / manipulated;
btPoint2PointConstraint* m_pickConstraint;
bool								m_cutting = true;
bool								m_raycast;
int									m_lastmousepos[2];
btVector3							m_impact;
btSoftBody::sRayCast				m_results;
btSoftBody::Node*					m_node;
btVector3							m_goal;
bool								m_drag;

//
struct	ImplicitSphere : btSoftBody::ImplicitFn
{
	btVector3	center;
	btScalar	sqradius;
	ImplicitSphere() {}
	ImplicitSphere(const btVector3& c, btScalar r) : center(c), sqradius(r*r) {}
	btScalar	Eval(const btVector3& x)
	{
		return((x - center).length2() - sqradius);
	}
};

struct ImplicitPlane : btSoftBody::ImplicitFn
{
	btVector3	point;
	btVector3	normal;
	ImplicitPlane() {}
	ImplicitPlane(btVector3& point, btVector3& normal) : point(point), normal(normal) {}
	btScalar	Eval(const btVector3& x)
	{
		return normal.dot((x - point));
	}
};


btSoftBody::Face* m_faceHit;
/* btMakeVector3 
@Author : Cameron Peet
@Desc   : Take in a glm::vec3 and return a btVector3
*/
btVector3 btMakeVector3(const glm::vec3& _kr)
{
	return btVector3(_kr.x, _kr.y, _kr.z);
}

void MouseButton(int button, int state, int x, int y)
{
	//Dp Nothing
	int yOld = y;
	y = glutGet(GLUT_WINDOW_HEIGHT) - y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			for (auto itr : m_pCurrentMenu)
			{
				if (itr->isActive() && itr->isButton())
				{

					if (x > itr->position.x && x < itr->position.x + itr->width && y > itr->position.y && y < itr->position.y + itr->height)
					{
						std::string text = itr->getText();
						if (text.length() >= 5)
						{
							std::string sub = text.substr(0, 5);
							if (sub == "Hooks")
							{
								if (hooks == 2)
									hooks = 3;
								else if (hooks == 3)
									hooks = 5;
								else if (hooks == 5)
									hooks = 19;
								else if (hooks == 19)
									hooks = 2;
								ResetCloth();
								for (int i = 0; i < numX; i += hooks)
									cloth->setMass(i, 0.0f);
							}
						}


						if (text.length() >= 7)
						{
							std::string sub = text.substr(0, 7);
							if (sub == "Release")
							{
								ResetNodeMass();
							}
						}

						itr->setHighlighted(false);
						break;
					}
				}
			}
		}
	}
	y = yOld;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			g_MouseInfo.mouseDown = true;
			btVector3 rayFrom = btMakeVector3(g_Camera.GetPosition());
			btVector3 rayTo = btMakeVector3(g_Camera.GetRayTo(x, y, 100.0f));

			//If we are not already holding a constraint
			if (!m_pickConstraint)
			{
				const btVector3			rayDir = (rayTo - rayFrom).normalized();

				//get a raycast result
				btSoftBody::sRayCast	res;
				cloth->rayTest(rayFrom, rayTo, res);
				m_results = res;

				//Picking threshold 
				if (m_results.fraction < 1.f)
				{
					//Calculate impact direction
					m_impact = rayFrom + (rayTo - rayFrom) * m_results.fraction;

					//if cutting then draggin = false. If not cutting then dragging = true;
					m_drag = m_cutting ? false : true;


				    //this  <- if then (true statement ) : (false statement)
					//store last mousepos for calculating in the update picking function
					m_lastmousepos[0] = x;
					m_lastmousepos[1] = y;

					m_node = 0;
					//Switch the body type
					switch (m_results.feature)
					{
						case btSoftBody::eFeature::Tetra:
						{
							btSoftBody::Tetra&	tet = m_results.body->m_tetras[m_results.index];
							m_node = tet.m_n[0];
							for (int i = 1; i<4; ++i)
							{
								if ((m_node->m_x - m_impact).length2()>
									(tet.m_n[i]->m_x - m_impact).length2())
								{
									m_node = tet.m_n[i];
								}
							}
							break;
						}
						case	btSoftBody::eFeature::Face:
						{
							m_faceHit = &m_results.body->m_faces[m_results.index];
							m_node = m_faceHit->m_n[0];

							for (int i = 1; i<3; ++i)
							{
								if ((m_node->m_x - m_impact).length2()>
									(m_faceHit->m_n[i]->m_x - m_impact).length2())
								{
									m_node = m_faceHit->m_n[i];
								}
							}
						}
						break;
					}
					if (m_node) m_goal = m_node->m_x;
					return;
				}
			}
		}

		if (state == GLUT_UP)
		{
			if ((!m_drag) && m_cutting && (m_results.fraction<1.f))
			{

				//dy, -dx).
				float dx = g_MouseInfo.mouseX - g_MouseInfo.lastX;
				float dy = g_MouseInfo.mouseY - g_MouseInfo.lastY;

				ImplicitPlane plane(m_impact, btVector3(0, 1, 0));
				cloth->refine(&plane, 1, true);

			}

			m_results.fraction = 1.f;
			m_drag = false;

			ResetPointer();
			g_MouseInfo.mouseDown = false;
		}
	}
}


void MouseMoveWhileClicked(int x, int y)
{
	if (m_node && (m_results.fraction < 1.f))
	{
		if (!m_drag)
		{
			#define SQ(_x_) (_x_)*(_x_)
			if ((SQ(x - m_lastmousepos[0]) + SQ(y - m_lastmousepos[1]))>6)
			{
				m_drag = true;
			}
			#undef SQ	
		}
		if (m_drag)
		{
			m_lastmousepos[0] = x;
			m_lastmousepos[1] = y;
		}
	}
}

void PassiveMotion(int x, int y)
{

	int yOld = y;
	y = glutGet(GLUT_WINDOW_HEIGHT) - y;

	for (auto itr : m_pCurrentMenu)
	{
		if (itr->isActive() && itr->isButton())
		{
			if (x > itr->position.x && x < itr->position.x + itr->width && y > itr->position.y && y < itr->position.y + itr->height)
			{
				itr->setHighlighted(true);
				itr->setHighlight(glm::vec3(0, 0, 1));
			}
			else
			{
				if (itr->isHighlighted())
				{
					itr->setHighlighted(false);
				}
			}
		}
	}

	if (g_MouseInfo.mouseDown)
	{
		g_MouseInfo.mouseX = x;
		g_MouseInfo.mouseY = y;
		return;
	}

	if (!g_MouseInfo.lock)
		return;
	static bool sJustWarped = false;

	if (sJustWarped) {
		sJustWarped = false;
		return;
	}

	y = yOld;
	GLfloat xoffset = x - g_MouseInfo.lastX;
	GLfloat yoffset = g_MouseInfo.lastY - y;

	g_MouseInfo.lastX = static_cast<float>(x); 
	g_MouseInfo.lastY = static_cast<float>(y);

	GLfloat sensitivity = 0.005f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	g_MouseInfo.yaw += xoffset;
	g_MouseInfo.pitch += yoffset;

	if (g_MouseInfo.pitch > 89.0f)
		g_MouseInfo.pitch = 89.0f;
	if (g_MouseInfo.pitch < -89.0f)
		g_MouseInfo.pitch = -89.0f;



	g_Camera.SetRotation(glm::quat());
	g_Camera.Rotate(glm::angleAxis(-g_MouseInfo.yaw, glm::vec3(0, 1, 0)));
	g_Camera.Rotate(glm::angleAxis(g_MouseInfo.pitch, glm::vec3(1, 0, 0)));


	ResetPointer();
	sJustWarped = true;
}



void DefineUniformBufferObjects()
{

	int size = sizeof(AllShaders) / sizeof(AllShaders[0]);
	for (int i = 0; i < size; i++)
	{
		GLuint uniformBlockIndex = glGetUniformBlockIndex(*AllShaders[i], "Matrices");
		glUniformBlockBinding(*AllShaders[i], uniformBlockIndex, 0);
	}

	glGenBuffers(1, &g_uboMatrices);

	glBindBuffer(GL_UNIFORM_BUFFER, g_uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, g_uboMatrices, 0, 2 * sizeof(glm::mat4));
}

void BindUBO()
{
	glBindBuffer(GL_UNIFORM_BUFFER, g_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(g_Camera.GetProjectionMatrix()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, g_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(g_Camera.GetViewMatrix()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


//for mouse picking
void pickingPreTickCallback(btDynamicsWorld *world, btScalar timeStep)
{

	if (m_drag)
	{
		//Calculating the goal position
		const int	x = m_lastmousepos[0];
		const int	y = m_lastmousepos[1];
		float		rf[3];
		
		float target[3];
		const btVector3 rayFrom = btMakeVector3(g_Camera.GetPosition());
		const btVector3	rayTo = btMakeVector3(g_Camera.GetRayTo(x, y, 100.0f));
		const btVector3	rayDir = (rayTo - rayFrom).normalized();
		const btVector3	N = (m_node->m_x - rayFrom).normalized();

		const btScalar	O = btDot(m_impact, N);
		const btScalar	den = btDot(N, rayDir);

		if ((den * den) > 0)
		{
			const btScalar			num = O - btDot(N, rayFrom);
			const btScalar			hit = num / den;
			if ((hit>0) && (hit<1500))
			{
				m_goal = rayFrom + rayDir*hit;
			}
		}

		//Either cut the object if the force is too great
		//else
		// Apply force towards the m_goal position
		btVector3				delta = m_goal - m_node->m_x;
		static const btScalar	maxdrag = 5;
		if (delta.length2()>(maxdrag*maxdrag))
		{
			ImplicitPlane plane(m_impact, btVector3(0, 0, 1));
			m_results.body->refine(&plane, 0.5f, true);

	/*		printf("Mass after: %f\r\n", m_results.body->getTotalMass());*/

			m_results.fraction = 1.f;
			m_drag = false;

			ResetPointer();
			g_MouseInfo.mouseDown = false;
			delta = delta.normalized() * maxdrag;
		}
		else
		{
			m_node->m_v += delta / timeStep;
		}
	}
}


//btSoftBody* m_pickedBody;
//int m_savedState;
//btTypedConstraint* m_pickedConstraint;
//btVector3 m_oldPickingPos;
//btVector3 m_hitPos;
//btScalar m_oldPickingDist;
//
//bool pickBody(const btVector3& rayFromWorld, const btVector3& rayToWorld)
//{
//	if (dynamicsWorld == 0)
//		return false;
//
//	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFromWorld, rayToWorld);
//	dynamicsWorld->rayTest(rayFromWorld, rayToWorld, rayCallback);
//	if (rayCallback.hasHit()) {
//		btVector3 pickPos = rayCallback.m_hitPointWorld;
//		btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
//		if (body) {
//			if (!(body->isStaticObject() || body->isKinematicObject())) {
//				//m_pickedBody = body; //is a soft body for the cloth atm.
//				m_savedState = m_pickedBody->getActivationState();
//				m_pickedBody->setActivationState(DISABLE_DEACTIVATION);
//
//				btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
//				btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, localPivot);
//				dynamicsWorld->addConstraint(p2p, true);
//				m_pickedConstraint = p2p;
//				btScalar mousePickClamping = 30.f;
//				p2p->m_setting.m_impulseClamp = mousePickClamping;
//				p2p->m_setting.m_tau = 0.001f;
//			}
//		}
//		m_oldPickingPos = rayToWorld;
//		m_hitPos = pickPos;
//		m_oldPickingDist = (pickPos - rayFromWorld).length();
//	}
//	return true;
//}
//
//bool movePickedBody(const btVector3& rayFromWorld, const btVector3& rayToWorld) {
//
//	if (m_pickedBody  && m_pickedConstraint) {
//		btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickedConstraint);
//		if (pickCon) {
//			btVector3 newPivotB;
//			btVector3 dir = rayToWorld - rayFromWorld;
//			dir.normalize();
//			dir *= m_oldPickingDist;
//			newPivotB = rayFromWorld + dir;
//			pickCon->setPivotB(newPivotB);
//			return true;
//		}
//	}
//	return false;
//}