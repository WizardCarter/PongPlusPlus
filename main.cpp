#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <string>
#include <SFML/Graphics.hpp>

//use Standard Namespace
using namespace std;

//use SFML's namespace
using namespace sf;

enum Gamestate {MENU, IN_GAME};

const double PI = 3.14159265358979;

/*
 TODO
  Add Win Screen and Handling to go back to main menu
  Make Collisions for ball/paddle more dynamic
  Add a proper one player mode (with AI on paddle #2)
  Make Help Screen (for controls)
  Custom colors, etc.
 */

//functions to help calculate angles and such
float dot(Vector2f a, Vector2f b) {
	return (a.x * b.x + a.y * b.y);
}

float magnitude(Vector2f a) {
	float ax2 = pow(a.x, 2);
	float ay2 = pow(a.y, 2);
	return sqrt(ax2 + ay2);
}

template<typename T>
T rad(T degrees) {
	return (degrees * PI / 180);
}

template<typename T>
T degrees(T rad) {
	return (rad * 180 / PI);
}

int main(int argc, char **argv) {
	//width and height for the window
	const int WIDTH = 960;
	const int HEIGHT = 720;
	
	//Variable to store the gamestate
	Gamestate state = MENU;
	
	//Create a new window
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Pong++");
	
	//Load in our font
	Font font;
	
	if (!font.loadFromFile("Data/Login.ttf")) {
		cout << "Could not load Data/Login.ttf!" << '\n';
		return -1;
	}
	
	//Text variable to display framerate
	Text framerate_text("0  FPS", font, 25);
	framerate_text.setFillColor(Color::Red);
	
	bool show_framerate = false;
	
	//Text to show if game is paused
	Text paused_text("PAUSED", font, 25);
	paused_text.setPosition(0, HEIGHT-25);
	
	//bool that determines whether or not the game is paused
	bool paused = false;
	
	//Objects for MENU
	unique_ptr<Text> play_one_text = make_unique<Text>("1 player", font);
	play_one_text->setPosition((WIDTH / 2) - 75, (HEIGHT / 2) - 100);
	
	unique_ptr<Text> play_two_text = make_unique<Text>("2 players", font);
	play_two_text->setPosition((WIDTH / 2) - 75, (HEIGHT / 2) - 50);
	
	unique_ptr<Text> help_text = make_unique<Text>("Help", font);
	help_text->setPosition((WIDTH / 2) - 75, HEIGHT / 2);
	
	int selected_option = 0;
	
	//bool two_players
	
	//Objects for IN_GAME
	
	//Players
	unique_ptr<RectangleShape> player1;
	
	unique_ptr<RectangleShape> player2;
	
	//Normal vectors for each player, to help with calculation
	const Vector2f PLAYER1_NORMAL(-1, 0);
	
	const Vector2f PLAYER2_NORMAL(1, 0);
	
	//Divider
	vector<RectangleShape> divider;
	
	//Ball
	unique_ptr<CircleShape> ball;
	
	//Text for score counters
	unique_ptr<Text> player1_score_text;
	unique_ptr<Text> player2_score_text;
	
	//actual score counter
	int scores[2] = {0, 0};
	
	//Text for win screen
	unique_ptr<Text> win_text;
	
	bool win = false;
	
	//Current direction for the ball
	Vector2f ball_dir(cos(rad(135)), -1 * sin(rad(135)));
	
	//clock for measuring time
	Clock clock;
	
	//Game loop, runs while window is open
	while (window.isOpen()) {
		
			//Event loop
			Event ev;
			while (window.pollEvent(ev)) {
					switch (ev.type) {
						case Event::Closed:
							window.close();
							break;
						case Event::KeyReleased:
							if (ev.key.code == Keyboard::Escape) {
									if (state == IN_GAME) {
											paused = !paused;
									} else {
											window.close();
									}
							} else if (ev.key.code == Keyboard::F10) {
								show_framerate = !show_framerate;
							//use up and down to select menu options
							} else if (ev.key.code == Keyboard::Up && state == MENU) {
									if (selected_option > 0) {
											selected_option--;
									}
							} else if (ev.key.code == Keyboard::Down && state == MENU) {
									if (selected_option < 2) {
											selected_option++;
									}
							} else if (ev.key.code == Keyboard::Enter) {
									if (state == MENU) {
											if (selected_option == 0 || selected_option == 1) {
												state = IN_GAME;
												
												//free memory assosicated with the menu text options
												play_one_text.reset();
												play_two_text.reset();
												help_text.reset();
												
												//and initialize game objects
												player1  = make_unique<RectangleShape>(Vector2f(20, 100));
												player1->setPosition(WIDTH * 0.0625, HEIGHT * 0.1);
												
												player2 = make_unique<RectangleShape>(Vector2f(20, 100));
												player2->setPosition((WIDTH * 0.9375) - player2->getSize().x, HEIGHT * 0.1);
												
												const int NUM_DIVIDERS = 10;
												const int DIV_HEIGHT = ((HEIGHT - NUM_DIVIDERS*20) / NUM_DIVIDERS);
	
												for (int i = 0; i < NUM_DIVIDERS; i++) {
													RectangleShape r(Vector2f(5, DIV_HEIGHT));
													r.setPosition((WIDTH / 2) - 2.5, (DIV_HEIGHT + 20) * i);
													divider.push_back(r);
												}
												
												ball = make_unique<CircleShape>(15, 50);
												ball->setOrigin(ball->getRadius(), ball->getRadius());
												ball->setPosition(WIDTH / 2, HEIGHT / 2);
												
												player1_score_text = make_unique<Text>("0", font, 60);
												player1_score_text->setPosition(WIDTH * 3 / 8, HEIGHT / 32);
												
												player2_score_text = make_unique<Text>("0", font, 60);
												player2_score_text->setPosition((WIDTH * 5 / 8) - (player2_score_text->getLocalBounds().width / 2), HEIGHT / 32);
											}
									}
							} else if (ev.key.code == Keyboard::Space) {
								if (state == IN_GAME && win) {
									//reset states
									state = MENU;
									win = false;
									paused = false;
									selected_option = 0;
									scores[0] = 0;
									scores[1] = 0;
									
									//free uneeded memory
									player1.reset();
									player2.reset();
									ball.reset();
									player1_score_text.reset();
									player2_score_text.reset();
									win_text.reset();
									divider.erase(divider.begin(), divider.end());
									
									//and re-initialize the menu items
									play_one_text = make_unique<Text>("1 player", font);
									play_one_text->setPosition((WIDTH / 2) - 75, (HEIGHT / 2) - 100);
	
									play_two_text = make_unique<Text>("2 players", font);
									play_two_text->setPosition((WIDTH / 2) - 75, (HEIGHT / 2) - 50);
	
									help_text = make_unique<Text>("Help", font);
									help_text->setPosition((WIDTH / 2) - 75, HEIGHT / 2);
								}
							}
							break;
					}
			}
			
			//how long has it been since last frame?
			const float dt = clock.restart().asSeconds();
			
			const int framerate = floor(1 / dt);
			
			//Logic
			if (state == MENU) {
				if (selected_option == 0) {
						play_one_text->setFillColor(Color::Red);
						play_two_text->setFillColor(Color::White);
						help_text->setFillColor(Color::White);
				} else if (selected_option == 1) {
						play_one_text->setFillColor(Color::White);
						play_two_text->setFillColor(Color::Red);
						help_text->setFillColor(Color::White);
				} else if (selected_option == 2) {
						play_one_text->setFillColor(Color::White);
						play_two_text->setFillColor(Color::White);
						help_text->setFillColor(Color::Red);
				}
			} else if (state == IN_GAME) {	
				const int PLAYER_SPEED = 300;
				
				if (!paused && !win) {
				//movement for player 1
					if (Keyboard::isKeyPressed(Keyboard::W)) {
						//if player is not at the top
						if (player1->getPosition().y > 0) {
							//move them up
							player1->move(0, PLAYER_SPEED * dt * -1);
						}
					}
					
					if (Keyboard::isKeyPressed(Keyboard::S)) {
						//if player is not at the bottom
						if (player1->getPosition().y + player1->getSize().y < HEIGHT) {
							//move them down
							player1->move(0, PLAYER_SPEED * dt);
						}
					}
				
					//movement for player 2 (ditto above)
					if (Keyboard::isKeyPressed(Keyboard::Up)) {
						if (player2->getPosition().y > 0) {
							player2->move(0, PLAYER_SPEED * dt * -1);
						}
					}
				
					if (Keyboard::isKeyPressed(Keyboard::Down)) {
						if (player2->getPosition().y + player2->getSize().y < HEIGHT) {
							player2->move(0, PLAYER_SPEED * dt);
						}
					}
					
					const int BALL_SPEED = 350;
				
					//bounce if it touches top or bottom
					if (ball->getPosition().y - ball->getRadius() <= 0 || ball->getPosition().y + ball->getRadius() >= HEIGHT) {
						ball_dir.y *= -1;
					}
				
					//reset if out of bounds
					if (ball->getPosition().x - ball->getRadius() < 0) {
						//TODO, RANDOM IF UP OR DOWN
						ball_dir = Vector2f(1, -1);
						ball->setPosition(WIDTH / 2, HEIGHT / 2);
						
						scores[1] += 1;
					}
				
					if (ball->getPosition().x + ball->getRadius() > WIDTH) {
						ball_dir = Vector2f(-1, 1);
						ball->setPosition(WIDTH / 2, HEIGHT / 2);
						
						scores[0] += 1;
					}
					
					if (scores[0] >= 10 | scores[1] >= 10) {
						win = true;
						
						string winner = (scores[0] >= 10) ? "Player 1" : "Player 2";
						win_text = make_unique<Text>(winner + " won the game! Press Space to Continue", font);
						
						float x = (WIDTH / 2) - (win_text->getLocalBounds().width / 2);
						float y = (HEIGHT / 2) - (win_text->getLocalBounds().height / 2);
						win_text->setPosition(x, y);
					}
					
					player1_score_text->setString(to_string(scores[0]));
					player2_score_text->setString(to_string(scores[1]));
					
					//collision with paddle 1
					//if ball isn't already partially past the paddle
					if (ball->getPosition().x >= player1->getPosition().x + player1->getSize().x) {
						if (ball->getGlobalBounds().intersects(player1->getGlobalBounds())) {
							/*float dp = dot(PLAYER1_NORMAL, ball_dir);
							float mg = magnitude(PLAYER1_NORMAL) * magnitude(ball_dir);
							float cos_theta = dp / mg;
							float theta = degrees(acos(cos_theta));*/
							
							ball_dir.x *= -1;
							ball_dir.y *= -1;
						}
					}
					
					//ditto
					if (ball->getPosition().x <= player2->getPosition().x) {
						if (ball->getGlobalBounds().intersects(player2->getGlobalBounds())) {
							ball_dir.x *= -1;
							ball_dir.y *= -1;
						}
					}
					
					ball->move(ball_dir.x * BALL_SPEED * dt, ball_dir.y * BALL_SPEED * dt);
				}
			}
			
			//clear the window
			window.clear(Color::Black);
			
			//if we need to show framerate
			if (show_framerate) {
					//set text
					framerate_text.setString(to_string(framerate));
					
					//and display
					window.draw(framerate_text);
			}
			
			//draw everything, depending on the state
			if (state == MENU) {
				window.draw(*play_one_text);
				window.draw(*play_two_text);
				window.draw(*help_text);
			} else if (state == IN_GAME) {
					window.draw(*player1);
					window.draw(*player2);
					window.draw(*player1_score_text);
					window.draw(*player2_score_text);
					
					if (!win) {
							window.draw(*ball);
					}
					
					//Draw all rectangles in the divider (Requires C++11)
					for (RectangleShape r : divider) {
							window.draw(r);
					}
					
					//if game is paused, let the player(s) know
					if (paused) {
							window.draw(paused_text);
					}
					
					if (win) {
							window.draw(*win_text);
					}
			}
			
			window.display();
	}
	
    return 0;
}
