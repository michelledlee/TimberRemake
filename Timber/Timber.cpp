/*** Simple Timber game (see Steam store for play details) built using C++ and SFML ***/

// Include important C++ libraries here
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// Identify the namespace
using namespace sf;

// Function declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Player position enum class
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES]; // Array of positions for branches

int main() {

	// Create a video mode object
	VideoMode vm(1920, 1080);

	// Create and open a window for the game
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);
	
	// Create a texture to hold a graphic
	Texture textureBackground;

	// Load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");
	Sprite spriteBackground;
	// Attach texture to the sprite
	spriteBackground.setTexture(textureBackground);
	spriteBackground.setPosition(0, 0);

	// Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	// Prepare the bee sprite
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	// Is the bee moving?
	bool beeActive = false;
	// How fast can the bee fly?
	float beeSpeed = 0.0f;

	// Make cloud sprites
	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");
	Sprite spriteCloud1, spriteCloud2, spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	// Position the clouds off screen
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);
	// Are the clouds on screen?
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	// How fast is each cloud?
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	// Initialize a clock object
	Clock clock;

	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	// Track whether the game is running
	bool paused = true;

	// Text for the game window
	int score = 0;
	sf::Text messageText;
	sf::Text scoreText;

	// Select font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	// Set font to message
	messageText.setFont(font);
	scoreText.setFont(font);

	// Assign message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");

	// Adjust string size
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	// Set string colour
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	// Position the text
	FloatRect textRect = messageText.getLocalBounds();
	// Set origin to the center of itself
	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	// Prepare branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 20); // Set sprite origin to center
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	// Player starts on left
	side playerSide = side::LEFT;

	// Prepare gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);
	// Line up axe with tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Prepare flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);
	// Log variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// Control player inout
	bool acceptInput = false;

	// Loading sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);
	
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	// Game loop
	while (window.isOpen()) {
		/* Handle player input */

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				// Listen for key presses again
				acceptInput = true;

				// Hide the axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// Reset the time and score
			score = 0;
			timeRemaining = 5;

			// Make all the branches disappear
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			// Move the player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		// Wrap player controls to accept input
		if (acceptInput) {
			// First right key press
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				playerSide = side::RIGHT;
				score++;

				// Add to amount of time remaining
				timeRemaining += (2 / score) + .15; // The higher the score, the less time is added

				// Set position of player and axe to the right
				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);

				// Update the branches
				updateBranches(score);

				// Set the log to fly left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000; // Move to the left
				logActive = true;

				acceptInput = false;

				// Play chop sound
				chop.play();
			}

			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				// Set player to left side
				playerSide = side::LEFT;
				score++;

				// Add to time remaining
				timeRemaining += (2 / score) + .15;

				// Set axe and player position
				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);

				// Update the branches
				updateBranches(score);

				// Update the log movement
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000; // Move to right
				logActive = true;

				acceptInput = false;

				// Play chop sound
				chop.play();
			}
		}

		/* Update the scene */

		if (!paused) {
			// Measure time
			Time deltaTime = clock.restart();

			// Subtract from time remaining
			timeRemaining -= deltaTime.asSeconds();
			// Size the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * 
				timeRemaining, timeBarHeight));

			// Update the player when time runs out
			if (timeRemaining <= 0.0f) {
				// Pause the game
				paused = true;

				// Change message shown
				messageText.setString("Out of time!!");

				// Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// Play oot sound
				outOfTime.play();
			}

			// Setup the bee movement
			if (!beeActive) {
				// How fast is the bee
				srand((int)time(0) * 10); // Seed number generator
				beeSpeed = (rand() % 200) + 200; // Ensure speed between 199 and 399

				// How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500; // Ensure height between 499 and 999
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			else {
				spriteBee.setPosition(
					spriteBee.getPosition().x -
					(beeSpeed * deltaTime.asSeconds()),
					spriteBee.getPosition().y);

				// Has the bee reached the right hand edge of the screen
				if (spriteBee.getPosition().x < -100) {
					beeActive = false;
				}
			}

			// Set up the cloud movement
			// Cloud 1
			if (!cloud1Active) {
				// How fat is the cloud
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);

				// How high is the cloud
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}
			else {
				spriteCloud1.setPosition(
					spriteCloud1.getPosition().x +
					(cloud1Speed * deltaTime.asSeconds()),
					spriteCloud1.getPosition().y);

				// Has the cloud moved off the screen?
				if (spriteCloud1.getPosition().x > 1920) {
					cloud1Active = false;
				}
			}

			// Cloud 2
			if (!cloud2Active) {
				// How fat is the cloud
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);

				// How high is the cloud
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}
			else {
				spriteCloud2.setPosition(
					spriteCloud2.getPosition().x +
					(cloud2Speed * deltaTime.asSeconds()),
					spriteCloud2.getPosition().y);

				// Has the cloud moved off the screen?
				if (spriteCloud2.getPosition().x > 1920) {
					cloud2Active = false;
				}
			}

			// Cloud 3
			if (!cloud3Active) {
				// How fat is the cloud
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200);

				// How high is the cloud
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-200, height);
				cloud3Active = true;
			}
			else {
				spriteCloud3.setPosition(
					spriteCloud3.getPosition().x +
					(cloud3Speed * deltaTime.asSeconds()),
					spriteCloud3.getPosition().y);

				// Has the cloud moved off the screen?
				if (spriteCloud3.getPosition().x > 1920) {
					cloud3Active = false;
				}
			}

			// Update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// Update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPositions[i] == side::LEFT) {
					// Move sprite to leftside
					branches[i].setPosition(610, height);
					// Flip sprite
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT) {
					// Move sprite to right side
					branches[i].setPosition(1330, height);
					// Set rotation to normal
					branches[i].setRotation(0);
				}
				else {
					// Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			// Handle the flying log
			if (logActive) {
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * deltaTime.asSeconds()),
					spriteLog.getPosition().y +
					(logSpeedY * deltaTime.asSeconds()));
			
				// Log has reached right hand edge
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x > 2000) {
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// Handle squished player
			if (branchPositions[5] == playerSide) {
				// Handle death
				paused = true;
				acceptInput = false;

				// Draw gravestone
				spriteRIP.setPosition(525, 760);

				// Hide player
				spritePlayer.setPosition(2000, 660);

				// Change message text
				messageText.setString("SQUISHED!!");

				// Center text on screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f); 
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
			
				// Play squished sound
				death.play();
			}

		} // End if (!paused)

		/* Draw the scene */

		// Clear everything from the last frame
		window.clear();

		// Draw game scene
		window.draw(spriteBackground);

		// Draw the clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		// Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		// Draw the tree
		window.draw(spriteTree);

		// Draw the player
		window.draw(spritePlayer);

		// Draw the axe
		window.draw(spriteAxe);

		// Draw the flying log
		window.draw(spriteLog);

		// Draw the gravestone
		window.draw(spriteRIP);

		// Draw the bee
		window.draw(spriteBee);

		// Draw the time bar
		window.draw(timeBar);

		// Draw the score
		window.draw(scoreText);
		if (paused) {
			// Draw message only when paused
			window.draw(messageText);
		}

		// Show everything
		window.display();
	}

	return 0;
}

// Function definition 
void updateBranches(int seed) {
	// Move all branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}

	// Spawn new branch at position 0
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;		
	}
}
