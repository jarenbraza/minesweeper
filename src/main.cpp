#include <SFML\Graphics.hpp>
#include <time.h>
#include <vector>

typedef std::vector<std::vector<int>> grid;

const int VACANT = 0, MINE = 9, COVERED = 10, FLAG = 11;

// Prototyping
void generateGame(int, int, int, grid&, grid&);
void revealVacant(int, int, grid&, grid&);

int main() {
	sf::RenderWindow game(sf::VideoMode(400, 400), "Minesweeper");

	int DIFFICULTY = 5;
	int rows = 10, cols = 10;
	int width = 32, height = 32; // Values based on texture dimensions
	grid gameGrid(rows + 2, std::vector<int>(cols + 2));
	grid displayGrid(rows + 2, std::vector<int>(cols + 2));

	// Load textures
	sf::Texture t;
	t.loadFromFile("tiles.jpg");
	sf::Sprite tile(t);

	generateGame(DIFFICULTY, rows, cols, displayGrid, gameGrid);

	while (game.isOpen()) {
		sf::Vector2i position = sf::Mouse::getPosition(game);
		int mouseX = position.x / width, mouseY = position.y / height;

		// Resolve out of bounds
		if (mouseX < 1 || mouseY < 1 || mouseX > rows + 1 || mouseY > cols + 1)
			continue;

		sf::Event e;
		while (game.pollEvent(e)) {
			if (e.type == sf::Event::Closed) game.close();

			if (e.type == sf::Event::MouseButtonPressed) {
				// Reveal an unflagged tile on left click; Chain reveal vacant locations
				if (e.key.code == sf::Mouse::Left) {
					if (displayGrid[mouseX][mouseY] != FLAG) {
						if (gameGrid[mouseX][mouseY] == VACANT) {
							revealVacant(mouseX, mouseY, displayGrid, gameGrid);
						}
						else {
							displayGrid[mouseX][mouseY] = gameGrid[mouseX][mouseY];
						}
					}
				}
				// Flag / Unflag on right click
				else if (e.key.code == sf::Mouse::Right) {
					if (displayGrid[mouseX][mouseY] == COVERED)
						displayGrid[mouseX][mouseY] = FLAG;
					else if (displayGrid[mouseX][mouseY] == FLAG)
						displayGrid[mouseX][mouseY] = COVERED;
				}
			}

			// Generate a new game if R is pressed
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
				generateGame(DIFFICULTY, rows, cols, displayGrid, gameGrid);
		}

		game.clear(sf::Color::White);

		for (int i = 1; i <= rows; i++) {
			for (int j = 1; j <= cols; j++) {
				if (displayGrid[mouseX][mouseY] == MINE) // If mine was revealed
					displayGrid[i][j] = gameGrid[i][j];
				tile.setTextureRect(sf::IntRect(displayGrid[i][j] * width, 0, width, height));
				tile.setPosition(i * width, j * height);
				game.draw(tile);
			}
		}

		game.display();
	}

    return 0;
}

void generateGame(int difficulty, int rows, int cols, grid& displayGrid, grid& gameGrid) {
	for (int i = 1; i <= rows; i++) {
		for (int j = 1; j <= cols; j++) {
			displayGrid[i][j] = COVERED;

			// Randomly set mines and vacant spots
			if (rand() % difficulty == 0)
				gameGrid[i][j] = MINE;
			else
				gameGrid[i][j] = VACANT;
		}
	}

	for (int i = 1; i <= rows; i++) {
		for (int j = 1; j <= cols; j++) {
			int numMines = 0;
			if (gameGrid[i][j] == MINE) continue;

			// Find number of adjacent mines about (i, j)
			for (int x = i - 1; x <= i + 1; x++)
				for (int y = j - 1; y <= j + 1; y++)
					if (gameGrid[x][y] == MINE) numMines++;

			gameGrid[i][j] = numMines;
		}
	}
}

void revealVacant(int x, int y, grid& displayGrid, grid& gameGrid) {
	// Reveal adjacent numbers
	for (int i = x - 1; i <= x + 1; i++)
		for (int j = y - 1; j <= y + 1; j++)
			if (gameGrid[i][j] != VACANT)
				displayGrid[i][j] = gameGrid[i][j];

	// Reveal current vacant spot
	displayGrid[x][y] = VACANT;

	// Chain search for adjacent vacant spots
	if (gameGrid[x - 1][y] == VACANT && displayGrid[x - 1][y] == COVERED) revealVacant(x - 1, y, displayGrid, gameGrid);
	if (gameGrid[x + 1][y] == VACANT && displayGrid[x + 1][y] == COVERED) revealVacant(x + 1, y, displayGrid, gameGrid);
	if (gameGrid[x][y - 1] == VACANT && displayGrid[x][y - 1] == COVERED) revealVacant(x, y - 1, displayGrid, gameGrid);
	if (gameGrid[x][y + 1] == VACANT && displayGrid[x][y + 1] == COVERED) revealVacant(x, y + 1, displayGrid, gameGrid);
}
