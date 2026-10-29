#include <SFML/Graphics.hpp>
#include<vector>
#include<iostream>
#include<cmath>
#include<algorithm>

#define PI 3.1415

// grid represents the 3d map, it is fixed to 10x01 as of now, 1 represents wall and 0 represents nothing.
int grid[10][10] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,0,0,0,1},
    {1,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1},

};

float deg_to_rad(float degrees) {
    // c++ trig funcs accept in radians only so this thing.

    return  degrees * (3.1415 / 180);
}

float mod360(float degrees) {
    // keeping degrees in 0 to 360 range.

    float result = fmod(degrees, 360);
    return result < 0 ? result + 360 : result;

}
class Pl {
    public:
        // X,Y are player positions , degrees is the direction of player.
        Pl(float X, float Y):X(X), Y(Y), pointerX(X+15), pointerY(0), degrees(0) {}

		bool grid_bound(float X, float Y) {
			return (X >= 0 && X <= 200 && Y >= 0 && Y <= 200);
		}

        std::tuple<int, int> get_grid_coordinates(float X, float Y) {
            // gives grid coordinates if wihting the grid.

            if (X >= 0 && X < 200 && Y >= 0 && Y < 200) {
                int gridX = static_cast<int>(std::floor(X / 20));
                int gridY = static_cast<int>(std::floor(Y / 20));
                return std::tuple<int, int>(gridX, gridY);
            }
            return std::tuple<int, int>(-1, -1);
        }

        bool through_wall(float X, float Y) {
            // checking in grid array if certain coordinate is a wall coordinate.

            auto [ grid_X,  grid_Y] = get_grid_coordinates(X, Y);

            if (grid_X == -1 || grid_Y == -1)
                return false;
            else {
                if (grid[grid_Y][grid_X] == 1)
                    return true;
                else
                    return false;

            }

        }

        void move(float delta) {
            // movement variables updation

            if (grid_bound( X+ (pointerX - X) * delta, Y+ (pointerY - Y) * delta)) {
                Y += (pointerY - Y) * delta * 0.3;
                X += (pointerX - X) * delta * 0.3;
                pointerX = X + 15 * cos(deg_to_rad(degrees));
                pointerY = Y + 15 * sin(deg_to_rad(degrees));


            }

        }

        std::tuple<float, float> getStepXStepY(float degrees) {
            // direction of players also decides how the casted ray will move along grid so step variables.

            float stepX = 0,stepY=0;
            if ((degrees > 0) && (degrees < 90)) {
                stepX = 1;
                stepY = 1;
            }
            else if ((degrees > 90) && (degrees < 180)) {
                stepX = -1;
                stepY = 1;
            }
            else if ((degrees > 180) && (degrees < 270)) {
                stepX = -1;
                stepY = -1;
            }
            else if ((degrees > 270) && (degrees < 360)) {
                stepX = 1;
                stepY = -1;
            }
            else if (degrees == 0) {
                stepX = 1;
                stepY = 0;
            }
            else if (degrees == 90) {
                stepX = 0;
                stepY = 1;
            }
            else if (degrees == 180) {
                stepX = -1;
                stepY = 0;
            }
            else {
                stepX = 0;
                stepY = -1;
            }

            return std::tuple<float, float>(stepX, stepY);

        }

        void rotate(float deltaDegree) {
            // rotation func

            degrees += deltaDegree;
			degrees = mod360(degrees);
			pointerX = X + 15 * cos(deg_to_rad(degrees));
			pointerY = Y + 15 * sin(deg_to_rad(degrees));
        }

        std::tuple<float , float > getNextXY(int X, int Y,int stepX, int stepY) {

            float temp_X, temp_Y;
            if (stepX ==1)
                temp_X = (std::round(X / 20) + 1) * 20.0f;
            if(stepY == 1)
                temp_Y = (std::round(Y / 20) + 1) * 20.0f;
            if (stepX == -1)
                if (X % 20 == 0) // if on grid line itself
                    temp_X = (std::round(X / 20) - 1) * 20.0f;
                else
                    temp_X = std::round(X / 20) * 20.0f;

            if (stepY == -1)
                if (Y % 20 == 0) // if on grid line itself
                    temp_Y = (std::round(Y / 20) - 1) * 20.0f; 
                else
                    temp_Y = std::round(Y / 20) * 20.0f;
            if (stepX == 0)
                temp_X = 9999; // to ignore moving in X direction .. check drawRay to get complete idea
            if (stepY == 0)
                temp_Y = 9999; // to ignore moving in Y direction .. check drawRay to get complete idea
            return std::tuple(temp_X, temp_Y);
        }   
        
        sf::RectangleShape drawWall(float ray_degrees, float ray_len, bool wall_hit) {
            float camera_depth = 150;
            float camera_angle = 30;

            sf::RectangleShape wall;
            if (!wall_hit)
                return wall;
            float plane = 1; // this variable is to understand wheteher ray on player's left or right side.
            if (ray_degrees < 180) 
                plane = -1; // ray on right side :->
            float wall_x_dist = std::abs(std::tanf(deg_to_rad(ray_degrees)) * camera_depth);
            float wall_y_dist = std::abs(std::cosf(deg_to_rad(ray_degrees)) * ray_len);
            float wall_height = (20.f / (ray_len * std::cos(deg_to_rad(ray_degrees)))) * 400.f;
            if (wall_x_dist > std::abs(std::tanf(deg_to_rad(camera_angle)) * camera_depth))
                return wall; // returning no wall to draw here

            if (wall_y_dist > camera_depth)
                return wall; // returning no wall to draw here
            
            wall_height = std::min(wall_height, 800.f);

            float coor_x = 300 + std::min(wall_x_dist,camera_depth) * plane * (300/std::abs(camera_depth*std::tanf(deg_to_rad(camera_angle))));


            wall.setSize(sf::Vector2f(3, wall_height));
            wall.setPosition({200+ coor_x, 400 - wall_height / 2 });
            wall.setOutlineThickness(1);
            wall.setFillColor(sf::Color::Green);
            wall.setOutlineColor(sf::Color::Red);

            
            return wall;
        }
        std::tuple<std::vector<sf::Vertex>, sf::RectangleShape, float> drawRay(float ray_degrees) {

            float ray_degrees_relative = ray_degrees;
            ray_degrees = degrees + ray_degrees;

            std::vector<sf::Vertex> ray_points;
            float rayX = X;
            float rayY = Y;
            ray_degrees = mod360(ray_degrees);
            
            auto [stepX, stepY] = getStepXStepY(ray_degrees);

            float nextX=0;
            float nextY=0;
            int iter = 0;
            float XDiff, YDiff;
            float coor1_x, coor1_y, coor2_x, coor2_y,prev_x,prev_y, coor1_y_diff, coor2_x_diff, ray_len,prev_ray_len,coor1_len,coor2_len;
            ray_len = 0;
            bool wall_hit = false;
            while (((rayX >= 0) && (rayX <= 200)) && ((rayY >= 0) && (rayY <= 200))) {
                prev_x = rayX;
                prev_y = rayY;
                prev_ray_len = ray_len;
                ray_points.push_back(sf::Vertex{ sf::Vector2(rayX, rayY) });

                if (rayX == 0 || rayY == 0 || rayX == 200 || rayY == 200)
                    break;
                iter++;
                if (iter > 20)
                    break;
                auto [nextX,nextY] = getNextXY(rayX, rayY, stepX, stepY);

                XDiff = abs(nextX - rayX);
                YDiff = abs(nextY - rayY);
                coor1_y_diff = abs(std::tanf(deg_to_rad(ray_degrees))) * XDiff * stepY;
                coor2_x_diff = abs((1 / std::tanf(deg_to_rad(ray_degrees)))) * YDiff * stepX;

                coor1_x = nextX;
                coor1_y = rayY + coor1_y_diff;
                coor1_len = std::abs(coor1_y_diff / std::sinf(deg_to_rad(ray_degrees)));

                coor2_y = nextY;
                coor2_x = rayX + coor2_x_diff;
                coor2_len = std::abs(coor2_x_diff / std::cosf(deg_to_rad(ray_degrees)));


                if (grid_bound(coor1_x, coor1_y) && grid_bound(coor2_x, coor2_y)) {
                    // jsut pick the coordinate which is closer
                    if (coor1_len < coor2_len) { 
                        rayX = coor1_x;
                        rayY = coor1_y;
                        ray_len += coor1_len;
                    }
                    else {
                        rayX = coor2_x;
                        rayY = coor2_y;
                        ray_len += coor2_len;
                    }
                }
                else
                {
                    // one of the coordinate is out of bounds pick the valid one
                    if (grid_bound(coor1_x, coor1_y)) {
                        rayX = coor1_x;
                        rayY = coor1_y;
                        ray_len += coor1_len;

                    }
                    else if (grid_bound(coor2_x, coor2_y))
                    {
                        rayX = coor2_x;
                        rayY = coor2_y;
                        ray_len += coor2_len;
                    }
                }
                // the logic lets it pass through wall on purpose
                // its midpoint is check prev + new / 2 if those coordiantes in a wall index in grid
                // wall hit so stop at prev coordinates !
                if (through_wall((rayX+prev_x)/2, (rayY+prev_y)/2)) { 
                    wall_hit = true;
                    break;
                }

            }
            
            return std::tuple(ray_points,drawWall(ray_degrees_relative, prev_ray_len,wall_hit), prev_ray_len);
        }
    
        float X, Y, pointerX, pointerY, degrees;
};

int main()
{
    int gridSize = 10;
    int startX = 0;
    int startY = 0;
    std::vector<sf::RectangleShape> rectangles;
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SFML works!");

	float playerX = 5;
	float playerY = 5;

    float pointerX = playerX + 5;
    float pointerY = 0;
    float pointer_angle = 0;
    
	int square_size = 20;
	sf::CircleShape player(2.f);  
    Pl player_pos(25, 25);

    for (int i = 0; i < gridSize;i++) {

        startY = 0;
        // the grid in top left corner
        for (int j = 0; j < gridSize;j++) {
            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(square_size, square_size));
            rectangle.setPosition(sf::Vector2f(startX, startY));
            rectangle.setOutlineThickness(1.f);
            rectangle.setOutlineColor(sf::Color::Green);
            if (grid[j][i] == 1)
                rectangle.setFillColor(sf::Color::Green);
            else
                rectangle.setFillColor(sf::Color::Transparent);

            rectangles.push_back(rectangle);

            startY += square_size;
        }
        startX += square_size;
    }

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        {
            // left key is pressed: move our character
            player_pos.rotate(-0.5);

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        {
            // left key is pressed: move our character
            player_pos.rotate(0.5);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        {
            // left key is pressed: move our character
            player_pos.move(0.02);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            // left key is pressed: move our character
            player_pos.move(-0.02);
        }
        window.clear();

        for (auto r : rectangles)
            window.draw(r);
        player.setPosition(sf::Vector2f(player_pos.X, player_pos.Y));
        std::array line =
        {
            sf::Vertex{sf::Vector2f(player_pos.X, player_pos.Y)},
            sf::Vertex{sf::Vector2f(player_pos.pointerX, player_pos.pointerY)}
        };
        window.draw(line.data(), 2, sf::PrimitiveType::LineStrip);


        auto [ray1, ray1_wall,ray1_len] = player_pos.drawRay(0);
        window.draw(ray1.data(), ray1.size(), sf::PrimitiveType::LineStrip);
        window.draw(ray1_wall);

        auto [ray2, ray2_wall,ray2_len] = player_pos.drawRay(330);
        window.draw(ray2.data(), ray2.size(), sf::PrimitiveType::LineStrip);
        window.draw(ray2_wall);

        std::vector<sf::RectangleShape> wall_rectangles_list;
        std::vector<std::vector<sf::Vertex>> ray_vertices_list;

        for (float i = 0;i < 100;i+=1) {

            // this trig stuff vertical pixel line to left of mid screen to player's casted ray degree.
            auto [ray1, ray1_wall, ray1_len] = player_pos.drawRay(std::asin(i/100) * (180.0 / PI)); 
            
            wall_rectangles_list.push_back(ray1_wall);
            ray_vertices_list.push_back(ray1);

            // this trig stuff vertical pixel line to left of mid screen to player's casted ray degree.
            auto [ray2, ray2_wall, ray2_len] = player_pos.drawRay(360-(std::asin(i / 100) * (180.0 / PI)));
            
            wall_rectangles_list.push_back(ray2_wall);
            ray_vertices_list.push_back(ray2);

        }

        for (auto i : wall_rectangles_list) // drawing the rectangles for final 3d effect
            window.draw(i);

        for (auto i : ray_vertices_list) // 2d casted rays visualization on top left.
            window.draw(i.data(), i.size(), sf::PrimitiveType::LineStrip);

        window.draw(player);
        window.display();
    }
}