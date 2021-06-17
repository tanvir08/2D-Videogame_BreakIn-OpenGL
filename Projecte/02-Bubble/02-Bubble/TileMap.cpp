#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"


using namespace std;


TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	TileMap *map = new TileMap(levelFile, minCoords, program);
	
	return map;
}


TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	loadLevel(levelFile);
	prepareArrays(minCoords, program);
}

TileMap::~TileMap()
{
	if(map != NULL)
		delete map;
}


void TileMap::render() const
{
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
}

/*en CPU: carga el nivel y construye el mapa*/
bool TileMap::loadLevel(const string &levelFile) {
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	string tile;

	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;		/*tilemap size*/
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;		/*16, 16; blockSize para encontrar siguiente tile (mirar abajo)*/
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);	/*cargo tilesheet correspondiente*/
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);	/*no hace GL_REPEAT*/
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);		/* no hace GL_LINEAR_MIPMAP_LINEAR*/
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;	/*numero de tiles en tilesheet (16,16)*/
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);

	/*leo el protocolo*/
	map = new int[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++) {
		getline(fin, line); sstream.str(line);
		for (int i = 0; i < mapSize.x; i++) {
			sstream >> tile;
			if (tile == "00") map[j*mapSize.x + i] = 0;
			else map[j*mapSize.x + i] = stoi(tile);
		}
		sstream.clear();
	}
#ifndef _WIN32
	fin.get(tile);
#endif
	fin.close();

	return true;
}

bool TileMap::empty_lastRoom()
{
	// Last room is 3 
	for (int j = 0; j < mapSize.y/3; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			int tile = map[j * mapSize.x + i];
			//check there is only wall,  doorbell or empty 
			if (tile != 0 && !((tile >= 33 && tile <= 36) || (tile >= 129 && tile <= 132)))
				return false;
		}
	}
	return true;
}


/*mapeo de coord de las pos de tiles con las coord de textura*/
void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program) {
	int tile, nTiles = 0;
	glm::vec2 posTile, texCoordTile[2], halfTexel;	/*texCoordTile va de (0,0) hasta (1,1)*/
	vector<float> vertices;

	/*la dimension de la textura blocks es 128px*128px, por tanto tilesheet.width() y tilesheet.height() seran 128*/
	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height()); /*mitad de 1px de la textura*/
	for (int j = 0; j<mapSize.y; j++) {
		for (int i = 0; i<mapSize.x; i++) {
			tile = map[j * mapSize.x + i];
			if (tile != 0) {
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);	/*obtengo la pos de tile respecto la window*/
																								/*no confundir tilesheetSize (que es 16) con tilesheet.width() ...*/
																								/*de la TEXTURA(tilesheet)  selecciono la coordenada de la imagene que me interesa, recuerda que tilesheet esta en coordenadas de (0,0) hasta (1,1)*/
				texCoordTile[0] = glm::vec2(float((tile - 1) % 16) / tilesheetSize.x, float((tile - 1) / 16) / tilesheetSize.y); /*analizando en la coord y, float(1/2)=0, si pones float(1.0/2)=0.5*/
				texCoordTile[1] = texCoordTile[0] + tileTexSize;	/*tileTexSize = (0.0625, 0.0625)*/
																	//texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;	/*no se que efecto tiene*/

												// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
		}
	} /*una vez hecho el mapeo y construido el vector de vertices, lo pasamos al shader para que despues podamos pintar*/

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);	/*en este vector vamos a tener position+texCoord*/
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);	/* 12vertice * 2coord * cada tile */
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}


// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is already intersecting a tile below.
bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size) const {
	int x, y0, y1;

	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;

	for (int y = y0; y <= y1; y++) {
		if (map[y*mapSize.x + x] != 0 && pos.x <= 16) {
			return true;
		}
	}

	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size) const
{
	int x, y0, y1;

	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		if (map[y*mapSize.x + x] != 0)
			return true;
	}

	return false;
}


bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY) const {
	int x0, x1, y;
	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;

	for (int x = x0; x <= x1; x++) {
		if (map[y*mapSize.x + x] != 0) {
			if (*posY - tileSize * y + size.y <= 4) {
				*posY = tileSize * y - size.y;
				return true;
			}
		}
	}
	return false;

}




























