#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "..\include\math.h"
#include "..\include\game.h"
#include "..\include\graphics.h"
#include "..\include\world.h"

#include <SOIL.h>

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




SpriteInfo::SpriteInfo(int left, int top, int width, int height) : left(left), top(top), width(width), height(height)
{
	key = 0;
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

SpriteSheet::SpriteSheet() {}

SpriteSheet::SpriteSheet(const char* file)
{
	load_sprite_sheet(file);
}

bool SpriteSheet::sprites_loaded()
{
	return m_SpriteBuffer != 0;
}

void SpriteSheet::load_sprite_sheet(const char* file)
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

void SpriteSheet::load_sprites(std::vector<SpriteInfo*>& sprites)
{
	float* spriteData = new float[24 * sprites.size()];

	for (int k = sprites.size() - 1; k >= 0; --k)
	{
		SpriteInfo* info = sprites[k];

		float l = (float)info->left / width; // left texcoord
		float r = (float)(info->left + info->width) / width; // right texcoord
		float w = (float)info->width;

		float t = (float)info->top / height; // top texcoord
		float b = (float)(info->top + info->height) / height; // bottom texcoord
		float h = (float)info->height;

		// Calculate base index and starting pointer for sprite data
		int i = (24 * k);
		float* data = spriteData + i;

		// First triangle: bottom-left, bottom-right, top-right
		// Bottom-left corner, vertices
		data[0] = 0.0f;
		data[1] = 0.0f;
		// Bottom-left corner, tex coord
		data[2] = l;
		data[3] = b;
		// Bottom-right corner, vertices
		data[4] = w;
		data[5] = 0.0f;
		// Bottom-right corner, tex coord
		data[6] = r;
		data[7] = b;
		// Top-right corner, vertices
		data[8] = w;
		data[9] = h;
		// Top-right corner, tex coord
		data[10] = r;
		data[11] = t;

		// Second triangle: bottom-left, top-left, top-right
		// Bottom-left corner, vertices
		data[12] = 0.0f;
		data[13] = 0.0f;
		// Bottom-left corner, tex coord
		data[14] = l;
		data[15] = b;
		// Top-left corner, vertices
		data[16] = 0.0f;
		data[17] = h;
		// Top-left corner, tex coord
		data[18] = l;
		data[19] = t;
		// Top-right corner, vertices
		data[20] = w;
		data[21] = h;
		// Top-right corner, tex coord
		data[22] = r;
		data[23] = t;

		// Output sprite key
		info->key = 6 * k;
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
	glUniformMatrix4fv(m_SpriteShaderMVP, 1, GL_FALSE, mat_get_values());

	// Draw the sprite using information from buffer
	glDrawArrays(GL_TRIANGLES, sprite, 6);

	return;
}



std::unordered_map<CHUNK_KEY, Chunk*> Chunk::m_Chunks{};

Chunk::Chunk(const char* path) : m_ChunkFilepath(path)
{
	// Load tile sprites
	if (!m_TileSprites.sprites_loaded())
	{
		m_TileSprites.load_sprite_sheet("res/img/tiles.png");

		int imax = m_TileSprites.width / TILE_WIDTH;
		int jmax = m_TileSprites.height / TILE_HEIGHT;

		std::vector<SpriteInfo*> sprites;
		for (int i = 0; i < imax; ++i)
		{
			for (int j = 0; j < jmax; ++j)
			{
				sprites.push_back(new SpriteInfo(TILE_WIDTH * i, TILE_HEIGHT * j, TILE_WIDTH, TILE_HEIGHT));
			}
		}

		m_TileSprites.load_sprites(sprites);

		for (auto iter = sprites.begin(); iter != sprites.end(); ++iter)
		{
			delete *iter;
		}
	}

	m_Tiles = nullptr;
	m_Objects = nullptr;
}

void Chunk::load()
{
	if (!m_Objects) // Make sure chunk hasn't already been loaded.
	{
		int s = width * height; // Size of arrays

		// Initialize tiles and objects and set default values
		m_Tiles = new SPRITE_KEY[s];
		memset(m_Tiles, 0, s * sizeof(SPRITE_KEY));

		m_Objects = new Object*[s];
		memset(m_Objects, NULL, s * sizeof(Object*));
	}
}




World::World() :
	m_Transform(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, -1.f, 0.f,
		0.f, 1.f, 1.f, 0.f
	)
{
	// Load information about all chunks
	for (int k = MAXIMUM_NUMBER_OF_CHUNKS; k > 0; --k)
	{
		std::string filename_to_check("res/data/chunks/chunk");
		filename_to_check.append(to_string(k)).append(".dat");
		const char* filename_to_check_cstr = filename_to_check.c_str();

		std::ifstream file_to_check(filename_to_check_cstr);

		if (file_to_check.good())
		{
			Chunk* chunk = Chunk::set_chunk(k, filename_to_check_cstr);

			// TODO change this
			chunk->width = CHUNK_SIZE;
			chunk->height = CHUNK_SIZE;
		}
	}

	// TODO
	m_Chunk = Chunk::get_chunk(1);
	m_Chunk->load();

	// Set camera
	camera_set(330, 50);
}