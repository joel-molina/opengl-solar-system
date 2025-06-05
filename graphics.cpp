#include "graphics.h"

Graphics::Graphics()
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int width, int height)
{
	// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
  // cout << glewGetString(GLEW_VERSION) << endl;
	glewExperimental = GL_TRUE;

	auto status = glewInit();

	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();

	//Check for error
	if (status != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}
#endif



	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height))
	{
		printf("Camera Failed to Initialize\n");
		return false;
	}

	// Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize())
	{
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize())
	{
		printf("Program to Finalize\n");
		return false;
	}

	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}


	//Light
	m_light = new Light(m_camera->GetView());


	//Galaxy
	m_galaxy = new Sphere(256, "assets\\Galaxy.jpg");

	//Comet
	m_halley_comet = new Mesh(glm::vec3(45.0f, 0.0f, 45.0f), "assets\\Halley_Comet.obj");

	// Starship
	m_starship = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f), "assets\\SpaceShip-1.obj", "assets\\SpaceShip-1.png");
	m_starship_fire = new Mesh(glm::vec3(0, 0, 0), "assets\\Fire.obj");

	//Asteroid
	m_inner_asteroid = new Mesh(glm::vec3(20.0f, 0.0f, 20.0f), "assets\\Asteroid.obj");
	m_outer_asteroid = new Mesh(glm::vec3(70.0f, 0.0f, 70.0f), "assets\\Asteroid.obj");

	//m_inner_asteroid_transforms;
	//m_outer_asteroid_transforms;
	// m_inner_asteroid_angles;
	// m_outer_asteroid_angles;

	srand(glfwGetTime());
	float radius = 16.0f;
	float offset = 2.5f;

	m_inner_asteroid_transforms.clear();
	for (int i = 0; i < m_numInnerAsteroids; ++i) {
		glm::mat4 transform = glm::mat4(1.0f);
		float angle = (float)i / (float)m_numInnerAsteroids * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		transform = glm::translate(transform, glm::vec3(x, y, z));

		float scale = (rand() % 20) / 10000.0f + 0.003;
		transform = glm::scale(transform, glm::vec3(scale));

		float rotAngle = (rand() % 360);
		transform = glm::rotate(transform, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		m_inner_asteroid_transforms.push_back(transform);
	}

	radius = 45.0f;
	offset = 3.0f;
	m_outer_asteroid_transforms.clear();
	for (int i = 0; i < m_numOuterAsteroids; ++i) {
		glm::mat4 transform = glm::mat4(1.0f);
		float angle = (float)i / (float)m_numOuterAsteroids * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		transform = glm::translate(transform, glm::vec3(x, y, z));

		float scale = (rand() % 20) / 10000.0f + 0.005;
		transform = glm::scale(transform, glm::vec3(scale));

		float rotAngle = (rand() % 360);
		transform = glm::rotate(transform, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		m_outer_asteroid_transforms.push_back(transform);
	}



	// The Sun
	m_sun = new Sphere(64, "assets\\2k_sun.jpg");
	
	// mercury
	m_mercury = new Sphere(48);
	if (!m_mercury->loadTexture("assets\\Mercury.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Mercury diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_mercury->loadTexture("assets\\Mercury-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Mercury normal" << endl;
		exit(EXIT_FAILURE);
	}
	// venus
	m_venus = new Sphere(48);
	if (!m_venus->loadTexture("assets\\Venus.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Venus diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_venus->loadTexture("assets\\Venus-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Venus normal" << endl;
		exit(EXIT_FAILURE);
	}
	// The Earth
	m_earth = new Sphere(48);
	if (!m_earth->loadTexture("assets\\2k_earth_daymap.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load earth diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_earth->loadTexture("assets\\2k_earth_daymap-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load earth normal" << endl;
		exit(EXIT_FAILURE);
	}
	
	// The moon
	m_moon = new Sphere(48);
	if (!m_moon->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_moon->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}


	// mars
	m_mars = new Sphere(48);
	if (!m_mars->loadTexture("assets\\Mars.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Mars diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_mars->loadTexture("assets\\Mars-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Mars normal" << endl;
		exit(EXIT_FAILURE);
	}
	//mars moons
	m_mars_moon1 = new Sphere(48);
	if (!m_mars_moon1->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_mars_moon1->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}
	m_mars_moon2 = new Sphere(48);
	if (!m_mars_moon2->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_mars_moon2->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}


	//jupiter
	m_jupiter = new Sphere(48);
	if (!m_jupiter->loadTexture("assets\\Jupiter.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Jupiter diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_jupiter->loadTexture("assets\\Jupiter-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Jupiter normal" << endl;
		exit(EXIT_FAILURE);
	}

	//jupiter moons
	m_jupiter_moon1 = new Sphere(48);
	if (!m_jupiter_moon1->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_jupiter_moon1->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}
	m_jupiter_moon2 = new Sphere(48);
	if (!m_jupiter_moon2->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_jupiter_moon2->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}
	m_jupiter_moon3 = new Sphere(48);
	if (!m_jupiter_moon3->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_jupiter_moon3->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}



	//saturn
	m_saturn = new Sphere(48);
	if (!m_saturn->loadTexture("assets\\Saturn.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Saturn diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	//saturn has no normal
	//if (!m_saturn->loadTexture("assets\\Saturn-n.jpg", "NORMAL_TEXTURE")) {
	//	cout << "Failed to load Saturn normal" << endl;
	//	exit(EXIT_FAILURE);
	//}
	m_saturn_ring = new Sphere(48);
	if (!m_saturn_ring->loadTexture("assets\\Saturn_ring.png", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Saturn ring diffuse" << endl;	
		exit(EXIT_FAILURE);
	}

	//saturn moons
	m_saturn_moon1 = new Sphere(48);
	if (!m_saturn_moon1->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_saturn_moon1->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}
	m_saturn_moon2 = new Sphere(48);
	if (!m_saturn_moon2->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_saturn_moon2->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}
	m_saturn_moon3 = new Sphere(48);
	if (!m_saturn_moon3->loadTexture("assets\\2k_moon.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load moon diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_saturn_moon3->loadTexture("assets\\2k_moon-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load moon normal" << endl;
		exit(EXIT_FAILURE);
	}



	// uranus
	m_uranus = new Sphere(48);
	if (!m_uranus->loadTexture("assets\\Uranus.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Uranus diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_uranus->loadTexture("assets\\Uranus-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Uranus normal" << endl;
		exit(EXIT_FAILURE);
	}
	// neptune
	m_neptune = new Sphere(48);
	if (!m_neptune->loadTexture("assets\\Neptune.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Neptune diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_neptune->loadTexture("assets\\Neptune-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Neptune normal" << endl;
		exit(EXIT_FAILURE);
	}


	//dwarves
	// ceres
	m_ceres = new Sphere(48);
	if (!m_ceres->loadTexture("assets\\Ceres.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Ceres diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_ceres->loadTexture("assets\\Ceres-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Ceres normal" << endl;
		exit(EXIT_FAILURE);
	}
	// eris
	m_eris = new Sphere(48);
	if (!m_eris->loadTexture("assets\\Eris.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Eris diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_eris->loadTexture("assets\\Eris-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Eris normal" << endl;
		exit(EXIT_FAILURE);
	}
	// haumea
	m_haumea = new Sphere(48);
	if (!m_haumea->loadTexture("assets\\Haumea.jpg", "DIFFUSE_TEXTURE")) {
		cout << "Failed to load Haumea diffuse" << endl;
		exit(EXIT_FAILURE);
	}
	if (!m_haumea->loadTexture("assets\\Haumea-n.jpg", "NORMAL_TEXTURE")) {
		cout << "Failed to load Haumea normal" << endl;
		exit(EXIT_FAILURE);
	}


	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}

void Graphics::HierarchicalUpdate2(double dt) {
	// Update your animation for the solar system here.

	//variables for positions and rotations
	std::vector<float> speed, dist, rotSpeed, scale;
	glm::vec3 rotVector;
	glm::mat4 localTransform;
	
	//galaxy
	glm::mat4 galaxyTransform = glm::mat4(1.f) * glm::scale(glm::vec3(150.0f, 150.0f, 150.0f));
	m_galaxy->Update(galaxyTransform);

	//position of the starship
	if (m_starship != nullptr)
	{
		//get ship direction vectors
		glm::vec3 shipPos = m_camera->GetShipPos();
		glm::vec3 shipFront = m_camera->GetShipFront();
		glm::vec3 shipUp = m_camera->GetShipUp();
		glm::vec3 shipRight = glm::normalize(glm::cross(shipFront, shipUp));

		//create a transformation matrix for starship
		glm::mat4 model = glm::mat4(1.0f);
		model[0] = glm::vec4(shipRight, 0.0f); //x-axis
		model[1] = glm::vec4(shipUp, 0.0f);	//y-axis
		model[2] = glm::vec4(shipFront, 0.0f);//z-axis   
		model[3] = glm::vec4(shipPos, 1.0f);//position (translation column)     

		//scale starship down
		model = glm::scale(model, glm::vec3(0.01));

		m_starship->Update(model);
	}

	//position of the starship fire
	if (m_starship_fire != nullptr)
	{
		//get ship direction vectors
		glm::vec3 shipPos = m_camera->GetShipPos();
		glm::vec3 shipFront = m_camera->GetShipFront();
		glm::vec3 shipUp = m_camera->GetShipUp();
		glm::vec3 shipRight = glm::normalize(glm::cross(shipFront, shipUp));

		//create a transformation matrix for starship
		glm::mat4 model = glm::mat4(1.0f);
		model[0] = glm::vec4(shipRight, 0.0f); //x-axis
		model[1] = glm::vec4(shipUp, 0.0f);	//y-axis
		model[2] = glm::vec4(shipFront, 0.0f);//z-axis   
		model[3] = glm::vec4(shipPos, 1.0f);//position (translation column)     

		//scale starship down
		model = glm::translate(model, glm::vec3(0, 0, -0.2));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(0.1));

		if (m_starship_fire != NULL)
			m_starship_fire->Update(model);
	}


	
	// position of the sun (center of solar system rotates slowly)
	modelStack.push(glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0)));  // sun's coordinate
	localTransform = modelStack.top();		// The sun origin
	localTransform *= glm::rotate(glm::mat4(1.0f), (float) (dt * 0.1), glm::vec3(0.f, 1.f, 0.f));
	localTransform *= glm::scale(glm::vec3(5, 5, 5));
	if (m_sun != NULL)
		m_sun->Update(localTransform);

	// position of mercury
	speed = { -4., -4., -4. };
	dist = { 6, 0, 6 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { .25,.25,.25 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_mercury != NULL)
		m_mercury->Update(localTransform);
	modelStack.pop();

	// position of venus (orbit sun on x-z plane and rotate around its own y-axis)
	speed = { -1.7, -1.7, -1.7 };
	dist = { 8, 0, 8 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { .5,.5,.5 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_venus != NULL)
		m_venus->Update(localTransform);
	modelStack.pop();


	// position of the earth (orbit sun on x-z plane and rotate around its own y-axis)
	speed = { -1., -1., -1. };
	dist = { 10, 0, 10 };
	rotVector = { 0.,1.,0. }; 
	rotSpeed = { 1., 1., 1. };
	scale = { 0.5, 0.5, 0.5 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_earth != NULL)
		m_earth->Update(localTransform);

	// position of the  moon
	speed = { 6, 6, 6 };
	dist = { 1.25, 1.25, 0. };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .2f, .2f, .2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store moon-planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	if (m_moon != NULL)
		m_moon->Update(localTransform);


	modelStack.pop(); 	// back to the planet coordinate
	modelStack.pop(); //back to sun

	// mars
	speed = { -0.5, -0.5, -0.5 };
	dist = { 15, 0, 15 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { .4,.4,.4 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_mars != NULL)
		m_mars->Update(localTransform);
	
	//mars moons
	speed = { 4, 4, 4 };
	dist = { 1.25, 1.25, 0.0 };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .1f, .1f, .1f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	if (m_mars_moon1 != NULL)
		m_mars_moon1->Update(localTransform);

	speed = { 4, 4, 4 };
	dist = { 1.25, 0., 1.25 };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .1f, .1f, .1f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_mars_moon2 != NULL)
		m_mars_moon2->Update(localTransform);

	modelStack.pop(); //back to sun

	//ceres
	speed = { -0.2, -0.2, -0.2 };
	dist = { 20, 4, 20 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.3, 0.3, 0.3 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_ceres != NULL)
		m_ceres->Update(localTransform);
	modelStack.pop();



	// jupiter
	speed = { -0.1, -0.1, -0.1 };
	dist = { 27, 0, 27 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 3., 3., 3. };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_jupiter != NULL)
		m_jupiter->Update(localTransform);

	//jupiter moons
	speed = { 2, 1, 2 };
	dist = { 5, 5, 0. };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .2f, .2f, .2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	if (m_jupiter_moon1 != NULL)
		m_jupiter_moon1->Update(localTransform);

	speed = { 2, 2, 1.5 };
	dist = { 5, 0., 5 };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .2f, .2f, .2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_jupiter_moon2 != NULL)
		m_jupiter_moon2->Update(localTransform);

	speed = { 2, 2, 2 };
	dist = { 0.0, 5, 5 };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .2f, .2f, .2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_jupiter_moon3 != NULL)
		m_jupiter_moon3->Update(localTransform);

	modelStack.pop(); //back to sun


	//saturn
	speed = { -0.05, -0.05, -0.05 };
	dist = { 35, 0, 35 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 2.5, 2.5, 2.5 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_saturn != NULL)
		m_saturn->Update(localTransform);

	//saturn ring
	speed = { -0.1, -0.1, -0.1 };
	dist = { 0, 0, 0 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 6.0, 0.0, 6.0 };
	localTransform = modelStack.top();				// start with saturns's coordinate
	//localTransform *= glm::rotate(glm::mat4(1.f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));//start with a rotation to flatten the disk
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_saturn_ring != NULL)
		m_saturn_ring->Update(localTransform);

	modelStack.pop(); //pop ring


	//saturn moons
	speed = { 2, 1.5, 2 };
	dist = { 5, 5, 0.0 };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .2f, .2f, .2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));

	if (m_saturn_moon1 != NULL)
		m_saturn_moon1->Update(localTransform);

	speed = { 2, 0, 2.5 };
	dist = { 5, 0.5, 5 };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .2f, .2f, .2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_saturn_moon2 != NULL)
		m_saturn_moon2->Update(localTransform);

	speed = { 1, 2, 1 };
	dist = { 0, 5, 5 };
	rotVector = { 1.,0.,1. };
	rotSpeed = { .25, .25, .25 };
	scale = { .2f, .2f, .2f };
	localTransform = modelStack.top();
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_saturn_moon3 != NULL)
		m_saturn_moon3->Update(localTransform);

	modelStack.pop(); //back to sun

	// uranus
	speed = { -0.25, -0.25, -0.25 };
	dist = { 45, 0, 45 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 1.5, 1.5, 1.5 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_uranus != NULL)
		m_uranus->Update(localTransform);
	modelStack.pop();

	// neptune
	speed = { -0.125, -0.125, -0.125 };
	dist = { 55, 0, 55 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 1.5, 1.5, 1.5 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_neptune != NULL)
		m_neptune->Update(localTransform);
	modelStack.pop();
	
	
	// haumea
	speed = { -0.2, -0.2, -0.2 };
	dist = { 75, 0, 75 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.7, 0.7, 0.7 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_haumea != NULL)
		m_haumea->Update(localTransform);
	modelStack.pop();
	
	// eris
	speed = { -0.1, -0.1, -0.1 };
	dist = { 75, 0, 75 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.5, 0.5, 0.5 };
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_eris != NULL)
		m_eris->Update(localTransform);
	modelStack.pop();

	//halley's comet
	speed = { -.2, -.2, -.2 };
	dist = { 80, -5, 15 };
	rotVector = { 0.,1.,0. };
	rotSpeed = { 1., 1., 1. };
	scale = { 0.01, 0.01, 0.01};
	localTransform = modelStack.top();				// start with sun's coordinate
	localTransform *= glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2]));
	modelStack.push(localTransform);			// store planet-sun coordinate
	localTransform *= glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	localTransform *= glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	if (m_halley_comet != NULL)
		m_halley_comet->Update(localTransform);
	modelStack.pop();

	modelStack.pop(); //removing sun


}


void Graphics::ComputeTransforms(double dt, std::vector<float> speed, std::vector<float> dist, 
	std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat) {
	tmat = glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2])
	);
	rmat = glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	smat = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
}

void Graphics::Render()
{
	//clear the screen
	glClearColor(0.5, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the correct program
	m_shader->Enable();

	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	// sending light values
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_globalAmbLoc, 1, glm::value_ptr(m_light->m_globalAmbient));
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightALoc, 1, glm::value_ptr(m_light->m_lightAmbient));
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightDLoc, 1, glm::value_ptr(m_light->m_lightDiffuse));
	glProgramUniform4fv(m_shader->GetShaderProgram(), m_lightSLoc, 1, glm::value_ptr(m_light->m_lightSpecular));
	glProgramUniform3fv(m_shader->GetShaderProgram(), m_lightPosLoc, 1, glm::value_ptr(m_light->m_lightPositionViewSpace));

	
	// Render the objects
	/*if (m_cube != NULL){
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
		m_cube->Render(m_positionAttrib,m_colorAttrib);
	}*/

	//galaxy

	if (m_galaxy != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matSpec[4] = { 0.0, 0.0, 0.0, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_galaxy->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_galaxy->GetModel()));


		if (m_galaxy->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_galaxy->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
			m_galaxy->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
		}
	}
		
	//starship
	if (m_starship != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.5, 0.5, 0.5, 1.0 };
		float matDiff[4] = { 0.6, 0.6, 0.6, 1.0 };
		float matSpec[4] = { 0.9, 0.9, 0.9, 1.0 };
		float matShininess = 100.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_starship->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_starship->GetModel()));

		if (m_starship->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_starship->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
		}
		m_starship->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}
	//starship fire
	if (m_starship_fire != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 1.0, 1.0, 1.0, 1.0 };
		float matDiff[4] = { 0.6, 0.6, 0.6, 1.0 };
		float matSpec[4] = { 0.9, 0.9, 0.9, 1.0 };
		float matShininess = 100.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_starship_fire->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_starship_fire->GetModel()));

		if (m_starship_fire->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_starship_fire->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
		}
		
		//emissive
		glUniform1i(m_isEmissive, 1);
		float normalizedSpeed = glm::clamp(abs(m_camera->GetVelocity()) / 0.3f, 0.0f, 1.0f);
		glUniform3fv(m_emissiveColor, 1, glm::value_ptr(glm::vec3(0.9, 0.1, 0.1) * normalizedSpeed));
		m_starship_fire->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
		glUniform1i(m_isEmissive, 0);
	}

	

	//sun
	if (m_sun != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 1.0, 1.0, 0.8, 1.0 };
		float matDiff[4] = { 1.0, 1.0, 0.3, 1.0 };
		float matSpec[4] = { 0.8, 0.8, 0.3, 1.0 };
		float matShininess = 90.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_sun->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sun->GetModel()));
		
		if (m_sun->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		m_sun->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}
	//mercury
	if (m_mercury != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.5, 0.7, 1.0 };
		float matSpec[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_mercury->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mercury->GetModel()));


		if (m_mercury->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mercury->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_mercury->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_mercury->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_mercury->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}
	//venus
	if (m_venus != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.5, 0.7, 1.0 };
		float matSpec[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_venus->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_venus->GetModel()));



		if (m_venus->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_venus->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_venus->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_venus->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_venus->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}
	

	//earth
	if (m_earth != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.5, 0.7, 1.0 };
		float matSpec[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_earth->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_earth->GetModel()));

		if (m_earth->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_earth->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_earth->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_earth->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_earth->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	
	}

	//moon
	if (m_moon != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView()* m_moon->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon->GetModel()));



		if (m_moon->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_moon->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_moon->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_moon->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}

	//mars
	if (m_mars != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.5, 0.7, 1.0 };
		float matSpec[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_mars->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars->GetModel()));



		if (m_mars->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mars->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_mars->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_mars->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_mars->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}
	//mars moons 
	if (m_mars_moon1 != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_mars_moon1->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars_moon1->GetModel()));



		if (m_mars_moon1->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mars_moon1->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_mars_moon1->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_mars_moon1->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_mars_moon1->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}
	if (m_mars_moon2 != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_mars_moon2->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mars_moon2->GetModel()));



		if (m_mars_moon2->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mars_moon2->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_mars_moon2->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_mars_moon1->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_mars_moon2->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}

	//ceres
	if (m_ceres != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.4, 0.4, 0.4, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

	//	//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_ceres->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_ceres->GetModel()));


		if (m_ceres->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_ceres->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_ceres->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_ceres->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_ceres->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}

	//inner asteroid belt
	if (m_inner_asteroid != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.3, 0.3, 1.0 };
		float matSpec[4] = { 0.01, 0.01, 0.01, 1.0 };
		float matShininess = 1.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_inner_asteroid->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_inner_asteroid->GetModel()));


		if (m_instanceVBO == 0) {
			glGenBuffers(1, &m_instanceVBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, m_inner_asteroid_transforms.size() * sizeof(glm::mat4), &m_inner_asteroid_transforms[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//setup instance attribute pointers on asteroid VAO
			m_inner_asteroid->SetupInstanceAttributes(m_instanceVBO, m_shader->GetShaderProgram());
		}

		//bind texture
		if (m_inner_asteroid->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_inner_asteroid->getTextureID());

			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 0);
		}

		//rendering
		glUniform1i(m_isInstanced, 1);
		m_inner_asteroid->RenderInstancing(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture,m_instanceVBO,static_cast<GLuint>(m_inner_asteroid_transforms.size()),m_shader->GetShaderProgram());
		glUniform1i(m_isInstanced, 0);
	}




	//jupiter
	if (m_jupiter != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.5, 0.7, 1.0 };
		float matSpec[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_jupiter->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter->GetModel()));


		if (m_jupiter->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_jupiter->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_jupiter->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_jupiter->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_jupiter->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}
	//jupiter moons
	if (m_jupiter_moon1 != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_jupiter_moon1->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter_moon1->GetModel()));



		if (m_jupiter_moon1->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_jupiter_moon1->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_jupiter_moon1->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_jupiter_moon1->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_jupiter_moon1->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}
	if (m_jupiter_moon2 != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_jupiter_moon2->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter_moon2->GetModel()));



		if (m_jupiter_moon2->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_jupiter_moon2->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_jupiter_moon2->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_jupiter_moon2->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_jupiter_moon2->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}
	if (m_jupiter_moon3 != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_jupiter_moon3->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_jupiter_moon3->GetModel()));



		if (m_jupiter_moon3->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_jupiter_moon3->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_jupiter_moon3->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_jupiter_moon3->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_jupiter_moon3->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}



	//saturn
	if (m_saturn != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.5, 0.7, 1.0 };
		float matSpec[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_saturn->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn->GetModel()));


		if (m_saturn->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_saturn->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_saturn->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_saturn->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_saturn->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}
	//saturn ring
	if (m_saturn_ring != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.5, 0.5, 0.5, 1.0 };
		float matShininess = 60.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_saturn_ring->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn_ring->GetModel()));



		if (m_saturn_ring->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_saturn_ring->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_saturn_ring->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_saturn_ring->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_saturn_ring->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}
	//saturn moons
	if (m_saturn_moon1 != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_saturn_moon1->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn_moon1->GetModel()));



		if (m_saturn_moon1->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_saturn_moon1->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_saturn_moon1->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_saturn_moon1->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_saturn_moon1->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}
	if (m_saturn_moon2 != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_saturn_moon2->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn_moon2->GetModel()));



		if (m_saturn_moon2->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_saturn_moon2->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_saturn_moon2->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_saturn_moon2->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_saturn_moon2->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}
	if (m_saturn_moon3 != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 5.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_saturn_moon3->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_saturn_moon3->GetModel()));



		if (m_saturn_moon3->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_saturn_moon3->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_saturn_moon3->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_saturn_moon3->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_saturn_moon3->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}





	//uranus
	if (m_uranus != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.5, 0.7, 1.0 };
		float matSpec[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_uranus->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_uranus->GetModel()));



		if (m_uranus->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_uranus->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_uranus->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_uranus->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_uranus->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}
	//neptune
	if (m_neptune != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.3, 0.5, 0.7, 1.0 };
		float matSpec[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_neptune->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_neptune->GetModel()));


		if (m_neptune->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_neptune->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_neptune->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_neptune->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_neptune->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}


	//outer asteroid belt
	if (m_outer_asteroid != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.3, 0.3, 0.3, 1.0 };
		float matDiff[4] = { 0.3, 0.3, 0.3, 1.0 };
		float matSpec[4] = { 0.01, 0.01, 0.01, 1.0 };
		float matShininess = 1.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_outer_asteroid->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_outer_asteroid->GetModel()));


		if (m_instanceVBO2 == 0) {
			glGenBuffers(1, &m_instanceVBO2);
			glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO2);
			glBufferData(GL_ARRAY_BUFFER, m_outer_asteroid_transforms.size() * sizeof(glm::mat4),
				&m_outer_asteroid_transforms[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// Also setup instance attribute pointers on asteroid mesh VAO
			m_outer_asteroid->SetupInstanceAttributes(m_instanceVBO2, m_shader->GetShaderProgram());
		}

		// --- Bind texture if present ---
		if (m_outer_asteroid->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_outer_asteroid->getTextureID());

			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION) {
				printf("Sampler not found\n");
			}
			glUniform1i(sampler, 0);
		}

		// --- Render ---
		glUniform1i(m_isInstanced, 1);
		m_outer_asteroid->RenderInstancing(
			m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture,
			m_instanceVBO2,
			static_cast<GLuint>(m_outer_asteroid_transforms.size()),
			m_shader->GetShaderProgram()
		);
		glUniform1i(m_isInstanced, 0);
	}

	//haumea
	if (m_haumea != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.4, 0.4, 0.4, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_haumea->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_haumea->GetModel()));


		if (m_haumea->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_haumea->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_haumea->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_haumea->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_haumea->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}

	//eris
	if (m_eris != NULL) {
		//sending material values for each object5
		float matAmbient[4] = { 0.2, 0.2, 0.2, 1.0 };
		float matDiff[4] = { 0.4, 0.4, 0.4, 1.0 };
		float matSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
		float matShininess = 10.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_eris->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_eris->GetModel()));


		if (m_eris->getTextureID("DIFFUSE_TEXTURE")) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_eris->getTextureID("DIFFUSE_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, false);
		}
		if (m_eris->getTextureID("NORMAL_TEXTURE")) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_eris->getTextureID("NORMAL_TEXTURE"));
			GLuint sampler = m_shader->GetUniformLocation("samp1");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUseProgram(m_shader->GetShaderProgram());
			glUniform1i(sampler, 0);
			glUniform1i(m_hasNormalMap, true);
		}
		m_eris->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);

	}

	//halley's comet
	if (m_halley_comet != NULL) {
		//sending material values for each object
		float matAmbient[4] = { 0.7, 0.7, 0.7, 1.0 };
		float matDiff[4] = { 0.6, 0.6, 0.6, 1.0 };
		float matSpec[4] = { 0.3, 0.3, 0.3, 1.0 };
		float matShininess = 20.0;
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mAmbLoc, 1, matAmbient);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mDiffLoc, 1, matDiff);
		glProgramUniform4fv(m_shader->GetShaderProgram(), m_mSpecLoc, 1, matSpec);
		glProgramUniform1f(m_shader->GetShaderProgram(), m_mShineLoc, matShininess);

		//create inverse transpose matrix for normal in shader
		glUniformMatrix3fv(m_normalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(m_camera->GetView() * m_halley_comet->GetModel())))));
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_halley_comet->GetModel()));


		if (m_halley_comet->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_halley_comet->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("samp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
		}
		m_halley_comet->Render(m_positionAttrib, m_normalAttrib, m_tcAttrib, m_hasTexture);
	}


	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}


bool Graphics::collectShPrLocs() {
	bool anyProblem = true;
	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_projectionMatrix not found\n");
		anyProblem = false;
	}

	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_viewMatrix not found\n");
		anyProblem = false;
	}

	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_modelMatrix not found\n");
		anyProblem = false;
	}

	//locate normal matrix
	m_normalMatrix = m_shader->GetUniformLocation("normMatrix");
	if (m_normalMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_normalMatrix not found\n");
		anyProblem = false;
	}

	//locate instance transofmration matrix
	m_instanceTransform = m_shader->GetUniformLocation("instance_transform");
	if (m_instanceTransform == INVALID_UNIFORM_LOCATION)
	{
		printf("m_instanceTransform not found\n");
		anyProblem = false;
	}

	//locate is instanced
	m_isInstanced = m_shader->GetUniformLocation("isInstanced");
	if (m_isInstanced == INVALID_UNIFORM_LOCATION)
	{
		printf("m_isInstanced not found\n");
		anyProblem = false;
	}
	// Locate the position vertex attribute
	m_positionAttrib = m_shader->GetAttribLocation("v_position");
	if (m_positionAttrib == -1)
	{
		printf("v_position attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_normalAttrib = m_shader->GetAttribLocation("v_normal");
	if (m_normalAttrib == -1)
	{
		printf("v_color attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_tcAttrib = m_shader->GetAttribLocation("v_texCoord");
	if (m_tcAttrib == -1)
	{
		printf("v_texcoord attribute not found\n");
		anyProblem = false;
	}

	m_hasTexture = m_shader->GetUniformLocation("hasTexture");
	if (m_hasTexture == INVALID_UNIFORM_LOCATION) {
		printf("hasTexture uniform not found\n");
		anyProblem = false;
	}

	m_hasNormalMap = m_shader->GetUniformLocation("hasNormalMap");
	if (m_hasNormalMap == INVALID_UNIFORM_LOCATION) {
		printf("m_hasNormalMap uniform not found\n");
		anyProblem = false;
	}

	// new light attributes
	m_globalAmbLoc = m_shader->GetUniformLocation("GlobalAmbient");
	if (m_globalAmbLoc == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	m_lightALoc = m_shader->GetUniformLocation("light.ambient");
	if (m_lightALoc == INVALID_UNIFORM_LOCATION) {
		printf("Shader LightAmbient element not found\n");
		anyProblem = false;
	}

	m_lightDLoc = m_shader->GetUniformLocation("light.diffuse");
	if (m_lightDLoc == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	m_lightSLoc = m_shader->GetUniformLocation("light.spec");
	if (m_lightSLoc == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	m_lightPosLoc = m_shader->GetUniformLocation("light.position");
	if (m_lightPosLoc == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	//material attributes

	m_mAmbLoc = m_shader->GetUniformLocation("material.ambient");
	if (m_mAmbLoc == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	m_mDiffLoc = m_shader->GetUniformLocation("material.diffuse");
	if (m_mDiffLoc == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	m_mSpecLoc = m_shader->GetUniformLocation("material.spec");
	if (m_mSpecLoc == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	m_mShineLoc = m_shader->GetUniformLocation("material.shininess");
	if (m_mShineLoc == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	//emmisive attributes
	m_isEmissive = m_shader->GetUniformLocation("isEmissive"); 
	if (m_isEmissive == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}
	m_emissiveColor = m_shader->GetUniformLocation("emissiveColor");
	if (m_emissiveColor == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}
	m_emissiveStrength = m_shader->GetUniformLocation("emmisiveStrength");
	if (m_emissiveStrength == INVALID_UNIFORM_LOCATION) {
		printf("globalAmbient element not found\n");
		anyProblem = false;
	}

	return anyProblem;
}

std::string Graphics::ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
}

