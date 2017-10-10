#include <SFML\Graphics.hpp>
#include <time.h>
#include <vector>

typedef std::vector<std::vector<int>> grid;

const int VACANT = 0, MINE = 9, COVERED = 10, FLAG = 11;

// Prototyping
void generateGame(int, int, int, grid&, grid&);
void revealVacant(int, int, grid&, grid&);

int main() {
	// Define game variables
	int DIFFICULTY = 7; // Constant for how often bombs appear each game
	int scale = 32; // Length of square texture
	int rows = 12, cols = 12; // Game grid rows
	bool drawResetText = false;
	grid gameGrid(rows + 2, std::vector<int>(cols + 2)); // Excess of two to account for borders
	grid displayGrid(rows + 2, std::vector<int>(cols + 2));

	// Formatting window
	sf::RenderWindow game(sf::VideoMode((cols + 2) * 32, (rows + 4) * 32), "Minesweeper");
	game.setFramerateLimit(20);	// 20 frames per second

	// Load textures
	sf::Texture t;
	t.loadFromFile("resources/tiles.jpg");
	sf::Sprite tile(t);

	// Load text
	sf::Font font;
	font.loadFromFile("resources/arial.ttf");
	sf::Text text(sf::String("Press R to reset game"), font, 20);
	text.setFillColor(sf::Color::Black);
	text.setPosition(sf::Vector2f(
		game.getSize().x / 2.0f - text.getLocalBounds().width / 2,	// Center horizontally
		(rows + 2) * 32));

	// Generate initial game
	generateGame(DIFFICULTY, rows, cols, displayGrid, gameGrid);

	// Game Loop
	while (game.isOpen()) {
		sf::Vector2i position = sf::Mouse::getPosition(game);
		int mouseX = position.x / scale, mouseY = position.y / scale;
		bool inGrid = true;

		// Resolve out of bounds
		if (mouseX < 1 || mouseY < 1 || mouseX > rows || mouseY > cols)
			inGrid = false;

		// Catch user action
		sf::Event e;
		while (game.pollEvent(e)) {
			if (e.type == sf::Event::Closed) game.close();

			if (e.type == sf::Event::MouseButtonPressed) {
				if (!inGrid)
					continue;

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
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
				drawResetText = false;
				generateGame(DIFFICULTY, rows, cols, displayGrid, gameGrid);
			}
		}

		game.clear(sf::Color(200, 200, 200));	// Set board background to grey

		// Draw the game
		for (int i = 1; i <= rows; i++) {
			for (int j = 1; j <= cols; j++) {
				if (inGrid && displayGrid[mouseX][mouseY] == MINE) { // If mine was revealed
					displayGrid[i][j] = gameGrid[i][j];
					drawResetText = true;
				}
				tile.setTextureRect(sf::IntRect(displayGrid[i][j] * scale, 0, scale, scale));
				tile.setPosition(i * scale, j * scale);
				game.draw(tile);
			}
		}

		if (drawResetText) game.draw(text);
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