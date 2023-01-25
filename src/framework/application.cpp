#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
}

Application::~Application()
{
	SDL_DestroyWindow(window);
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;
    
    if (toolbar.LoadPNG("images/toolbar.png") == false)
    {
        std::cout << "Failed to load image" << std::endl;
    }
    std::cout << "Image loaded successfully" << std::endl;
    selected_color = Color::WHITE; // sets initial color
    selected_mode = Free; // sets initial mode
}

// Render one frame
void Application::Render(void)
{
    if(selected_mode == Particle)
    {
        // render particle system
        framebuffer.Fill(Color::BLACK); // placeholder
    } else { // render drawing view
        bool top = true;
        framebuffer.DrawImagePixels(toolbar, window_width, window_height, top);
        int buttonSize = 50;
        int margin = 5;
        int numButtons = 10;
        for (int i = 0; i<numButtons; i++) {
            framebuffer.DrawRect(margin + i * buttonSize, top ? (window_height-buttonSize-margin)-margin : margin, buttonSize, buttonSize, Color::RED);
        }
    }
	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{

}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
        case SDLK_1: selected_mode = Line; break;
        case SDLK_2: selected_mode = Circle; break;
        case SDLK_3: selected_mode = Free; break;
        case SDLK_4: selected_mode = Particle; break;
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {
        switch (selected_mode) {
            case Line: {
                if(previous_click.x != -1)
                {
                    framebuffer.DrawLineBresenham(previous_click.x, previous_click.y, mouse_position.x, mouse_position.y, selected_color);
                    previous_click.x = -1;
                } else {
                    previous_click = mouse_position; // remember the click position
                }
                break;
            }
            case Circle: {
                int radius = 10;
                bool fill = true;
                framebuffer.DrawCircle(mouse_position.x, mouse_position.y, radius, selected_color, fill);
                break;
            }
            case Free: {
                framebuffer.SetPixel(mouse_position.x, mouse_position.y, selected_color);
            }
            default:
                break;
        }
          
        
        // button selection
        if(selected_mode != Particle)
        {
            int buttonId = (event.x / (50)) % 10; // 50 for button width, 10 for amount of buttons
            if(mouse_position.x <= 10 * 50 + 5 && mouse_position.y > window_height - toolbar.height) // check we are not out of bounds to the right and on the toolbar
            {
                if (buttonId  == 0) // new project
                {
                    framebuffer.Fill(Color::BLACK);
                } else if(buttonId == 1) // save project
                {
                    if(framebuffer.SaveTGA("test.tga"))
                        std::cout << "Saved file successully" << std::endl;
                } else // color selection
                {
                    Color availableColors[8] = {Color::BLACK, Color::RED, Color::GREEN, Color::BLUE, Color::YELLOW, Color::PURPLE, Color::CYAN, Color::WHITE};
                    selected_color = availableColors[buttonId - 2];
                }
            }
        }
	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    if(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT) & selected_mode == Free)
    {
        framebuffer.SetPixel(mouse_position.x, mouse_position.y, selected_color);
    }
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}
