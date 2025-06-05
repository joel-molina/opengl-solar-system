

#include "engine.h"
#include "glm/ext.hpp"

//global variable to track scroll amount
static double scrollY = 0.0;

Engine::Engine(const char* name, int width, int height)
{
  m_WINDOW_NAME = name;
  m_WINDOW_WIDTH = width;
  m_WINDOW_HEIGHT = height;

}


Engine::~Engine()
{
  delete m_window;
  delete m_graphics;
  m_window = NULL;
  m_graphics = NULL;
}

bool Engine::Initialize()
{
  // Start a window
  m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
  if(!m_window->Initialize())
  {
    printf("The window failed to initialize.\n");
    return false;
  }

  // Start the graphics
  m_graphics = new Graphics();
  if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
  {
    printf("The graphics failed to initialize.\n");
    return false;
  }

  glfwSetCursorPosCallback(m_window->getWindow(), cursorPositionCallBack);
  glfwSetScrollCallback(m_window->getWindow(), mouse_scroll_callback); //new callback

  // No errors
  return true;
}

void Engine::Run()
{
  m_running = true;

  while (!glfwWindowShouldClose(m_window->getWindow()))
  {
      ProcessInput();
      Display(m_window->getWindow(), glfwGetTime());
      glfwPollEvents();
  }
  m_running = false;

}

void Engine::ProcessInput()
{
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window->getWindow(), true);

    char key = ' ';
    //WASD key control to pan camera up, down, left, right
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        key = 'w';

    else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        key = 'a';

    else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        key = 's';

    else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        key = 'd';
    //'t' toggle game modes
    else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_T) == GLFW_PRESS)
        key = 't';
    //'r' reset camera view
    else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_R) == GLFW_PRESS)  
        key = 'r';
    //' ', spacebar set velocity to 0, brake
    else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
        key = '~';

    //mouse movement 
    double xpos, ypos;
    glfwGetCursorPos(m_window->getWindow(), &xpos, &ypos);

    //mouse scroll wheel
    double yOffset = scrollY; //pass in scroll offset to increase or decrease to FoV.
    scrollY = 0.0; //reset amount of scroll

    m_graphics->GetCamera()->Update(xpos, ypos, key, yOffset);
}

void Engine::mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    scrollY += yOffset;
}

void Engine::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
 
    glfwGetCursorPos(window, &xpos, &ypos);
    std::cout << "Position: (" << xpos << ":" << ypos << ")";
}

unsigned int Engine::getDT()
{
  //long long TimeNowMillis = GetCurrentTimeMillis();
  //assert(TimeNowMillis >= m_currentTimeMillis);
  //unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
  //m_currentTimeMillis = TimeNowMillis;
  //return DeltaTimeMillis;
    return glfwGetTime();
}

long long Engine::GetCurrentTimeMillis()
{
  //timeval t;
  //gettimeofday(&t, NULL);
  //long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
  //return ret;
    return (long long) glfwGetTime();
}

void Engine::Display(GLFWwindow* window, double time) {

    m_graphics->Render();
    m_window->Swap();
    m_graphics->HierarchicalUpdate2(time);
}

static void cursorPositionCallBack(GLFWwindow* window, double xpos, double ypos) {
    //cout << xpos << " " << ypos << endl;
}
