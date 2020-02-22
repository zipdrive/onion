#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL.h>

#include "..\include\math.h"
#include "..\include\game.h"
#include "..\include\graphics.h"

using namespace std;


// The main Application object.
Application* g_Application = NULL;


void load_settings()
{
	g_Application = new Application();

	string line;
	ifstream settings("settings.ini");

	if (settings.is_open())
	{
		regex eqmatcher("(\\S+)\\s*=\\s*(.*\\S)\\s*(//.*)?");
		smatch m;

		while (getline(settings, line))
		{
			if (regex_match(line, m, eqmatcher))
			{
				if (m[1].compare("title") == 0)
				{
					g_Application->title = m[2].str();
				}
				else if (m[1].compare("width") == 0)
				{
					g_Application->width = stoi(m[2].str());
				}
				else if (m[1].compare("height") == 0)
				{
					g_Application->height = stoi(m[2].str());
				}
				else if (m[1].compare("fullscreen") == 0)
				{
					g_Application->fullscreen = (m[2].compare("true") == 0);
				}
			}
		}

		settings.close();
	}
}

void save_settings()
{
	ofstream settings("settings.ini", ios::out | ios::trunc);

	if (settings.is_open())
	{
		settings << "title = " << g_Application->title;
		settings << "\nwidth = " << g_Application->width;
		settings << "\nheight = " << g_Application->height;
		settings << "\nfullscreen = " << (g_Application->fullscreen ? "true" : "false");

		settings.close();
	}
}


Application* get_application_settings()
{
	if (!g_Application)
	{
		load_settings();
	}

	return g_Application;
}

void set_application_settings(Application* app)
{
	g_Application = app;
	g_Application->display();

	save_settings();
}

Application::Application() : title("NO_TITLE_SET")
{
	width = 640;
	height = 400;
	fullscreen = false;
}

Application::Application(Application* other) : title(other->title)
{
	width = other->width;
	height = other->height;
	fullscreen = other->fullscreen;
}




// The raw text of the vertex shader for sprites.
const char* spriteVertexShaderText =
"#version 330 core\n"
"\n"
"layout(location = 0) in vec2 vertexPosition;\n"
"layout(location = 1) in vec2 vertexUV;\n"
"\n"
"uniform mat4 MVP;\n"
"\n"
"out vec2 UV;\n"
"\n"
"void main() {\n"
"	gl_Position = MVP * vec4(vertexPosition, 0, 1);\n"
"	UV = vertexUV;\n"
"}";

// The raw text of the fragment shader for sprites.
const char* spriteFragmentShaderText =
"#version 330 core\n"
"\n"
"in vec2 UV;\n"
"\n"
"uniform sampler2D tex2D;\n"
"\n"
"void main() {\n"
"	gl_FragColor = texture(tex2D, UV);\n"
"}";



SPRITE_SHEET_KEY SpriteSheet::m_Enabled{ UINT16_MAX };
SPRITE_SHEET_KEY SpriteSheet::m_NextAvailableID{ 0 };
GLuint SpriteSheet::m_SpriteShader{ 0 };
GLuint SpriteSheet::m_SpriteShaderMVP{ 0 };

SpriteSheet::SpriteSheet(const char* file)
{
	// Load data from file using SOIL.
	unsigned char* data;
	int channels;

	data = SOIL_load_image(file, &width, &height, &channels, SOIL_LOAD_RGBA);

	// Bind data to texture.
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Generate an ID for the sprite sheet.
	m_ID = m_NextAvailableID++;

	// If it hasn't been done yet, generate the shader program.
	if (m_SpriteShader == 0)
	{
		// Vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &spriteVertexShaderText, NULL);
		glCompileShader(vertexShader);

		// Fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &spriteFragmentShaderText, NULL);
		glCompileShader(fragmentShader);

		// Create shader program
		m_SpriteShader = glCreateProgram();
		glAttachShader(m_SpriteShader, vertexShader);
		glAttachShader(m_SpriteShader, fragmentShader);
		glLinkProgram(m_SpriteShader);

		// Determine the location of the MVP uniform
		m_SpriteShaderMVP = glGetUniformLocation(m_SpriteShader, "MVP");
	}
}

void SpriteSheet::add_sprites(std::vector<Sprite>& sprites, std::vector<SPRITE_KEY>& sprite_keys)
{
	float* spriteData = new float[24 * sprites.size()];
	sprite_keys.resize(sprites.size());

	for (int k = sprites.size() - 1; k >= 0; --k)
	{
		float l = (float)sprites[k].left / width; // left texcoord
		float r = (float)(sprites[k].left + sprites[k].width) / width; // right texcoord
		float w = (float)sprites[k].width;

		float t = (float)sprites[k].top / height; // top texcoord
		float b = (float)(sprites[k].top + sprites[k].height) / height; // bottom texcoord
		float h = (float)sprites[k].height;

		// Base index
		int i = (24 * k);

		// First triangle: bottom-left, bottom-right, top-right
		// Bottom-left corner, vertices
		spriteData[k] = 0.0f;
		spriteData[k + 1] = 0.0f;
		// Bottom-left corner, tex coord
		spriteData[k + 2] = l;
		spriteData[k + 3] = b;
		// Bottom-right corner, vertices
		spriteData[k + 4] = w;
		spriteData[k + 5] = 0.0f;
		// Bottom-right corner, tex coord
		spriteData[k + 6] = r;
		spriteData[k + 7] = b;
		// Top-right corner, vertices
		spriteData[k + 8] = w;
		spriteData[k + 9] = h;
		// Top-right corner, tex coord
		spriteData[k + 10] = r;
		spriteData[k + 11] = t;

		// Second triangle: bottom-left, top-left, top-right
		// Bottom-left corner, vertices
		spriteData[k + 12] = 0.0f;
		spriteData[k + 13] = 0.0f;
		// Bottom-left corner, tex coord
		spriteData[k + 14] = l;
		spriteData[k + 15] = b;
		// Top-left corner, vertices
		spriteData[k + 16] = 0.0f;
		spriteData[k + 17] = h;
		// Top-left corner, tex coord
		spriteData[k + 18] = l;
		spriteData[k + 19] = t;
		// Top-right corner, vertices
		spriteData[k + 20] = w;
		spriteData[k + 21] = h;
		// Top-right corner, tex coord
		spriteData[k + 22] = r;
		spriteData[k + 23] = t;

		// Output sprite key
		sprite_keys[k] = i;
	}

	// Bind the sprite data to a buffer
	glGenBuffers(1, &m_SpriteBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_SpriteBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * sprites.size(), spriteData, GL_STATIC_DRAW);
}



void SpriteSheet::display(SPRITE_KEY sprite)
{
	if (m_ID != m_Enabled)
	{
		m_Enabled = m_ID;

		// Change the texture being drawn from.
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		// Change the buffer being used.
		glBindBuffer(GL_ARRAY_BUFFER, m_SpriteBuffer);

		// Set attrib array for position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);

		// Set attrib array for tex coord
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	}

	// Set the shader program
	glUseProgram(m_SpriteShader);

	// Set the current transformation
	glUniformMatrix4fv(m_SpriteShaderMVP, 1, GL_FALSE, mat_get().mat);

	// Draw the sprite using information from buffer
	glDrawArrays(GL_TRIANGLES, sprite, 6);

	return;
}