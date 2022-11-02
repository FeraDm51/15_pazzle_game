#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

sf::Font font;      //Шрифт для всего

const sf::Vector2f LEFT = sf::Vector2f(0, -1);
const sf::Vector2f RIGHT = sf::Vector2f(0, 1);
const sf::Vector2f UP = sf::Vector2f(-1, 0);
const sf::Vector2f DOWN = sf::Vector2f(1, 0);


class Cell: public sf::Drawable {
private:
    sf::RectangleShape rect;
    int value;

    const static int SIZE = 75;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const {
        target.draw(rect);
        if (value == 0) {
            return;
        }

        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(value).c_str());
        text.setOrigin(sf::Vector2f(text.getLocalBounds().width / 2.f, text.getLocalBounds().height));
        text.setPosition(rect.getPosition() + rect.getSize() / 2.f);
        text.setFillColor(sf::Color(48, 173, 227));
        target.draw(text);
    }

public:
    Cell(int value): value(value) {
        rect.setSize(sf::Vector2f(SIZE, SIZE));
        rect.setFillColor(sf::Color(172, 40, 250, 98));
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2.f);
        
        if (value == 0) {
            value = 16;
        }
        
        int row = (value - 1) / 4;
        int col = (value - 1) % 4;

        float x = col * SIZE;
        float y = row * SIZE;
        rect.setPosition(sf::Vector2f(x, y));
    }

    int getValue()const { return value; }
    void setValue(int value) { this->value = value; }

    bool contains(sf::Vector2f point) {
        return rect.getGlobalBounds().contains(point);
    }
};

class Board : public sf::Drawable {
private:
    std::vector<std::vector<Cell>> cells;
       
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const {
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                target.draw(cell);
            }
        }
    }

    bool outOfBounds(int i, int j) {
        return i < 0 || i >= 4 || j < 0 || i >= 4;
    }

    void swapCells(int i1, int j1, int i2, int j2) {
        int value = cells[i1][j1].getValue();
        cells[i1][j1].setValue(cells[i2][j2].getValue());
        cells[i2][j2].setValue(value);
    }

public:

    Board() {
        cells.assign(4, std::vector<Cell>());
        int value = 1;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                Cell cell(value % 16);
                ++value;
                cells[i].push_back(cell);
            }
        }
    }

    void move(int row, int col, sf::Vector2f direction) {

        sf::Vector2f emptyCellPos(-1, -1);

        for (int i = row, j = col; !outOfBounds(i, j); i += direction.x, j += direction.y) {
            if (cells[i][j].getValue() == 0) {
                emptyCellPos.x = i;
                emptyCellPos.y = j;
                break;
            }
        }

        if (emptyCellPos == sf::Vector2f(-1, -1)) {
            return;
        }

        for (int i = emptyCellPos.x, j = emptyCellPos.y; 
                    !(i == row && j == col); 
                    i -= direction.x, j -= direction.y) {
            swapCells(i, j, i - direction.x, j - direction.y);
        }
    }

    //Возвращает номер строки, номер столбца
    sf::Vector2i getCell(sf::Vector2f point) {
        for (int i = 0; i < cells.size(); ++i) {
            for (int j = 0; j < cells[i].size(); ++j) {
                if (cells[i][j].contains(point)) {
                    return sf::Vector2i(i, j);
                }
            }
        }
        return sf::Vector2i(-1, -1);        //Точка находится вне поля
    }

};

int main() {
    font.loadFromFile("fonts/ARLRDBD.TTF");
    sf::RenderWindow window(sf::VideoMode(600, 600), "SFML works!", sf::Style::Close);
    sf::Color backgroundColor(240, 94, 51, 94);

    Board board;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                auto mousePos = (sf::Vector2f) sf::Mouse::getPosition(window);
                auto cellCords = board.getCell(mousePos);

                std::cout << cellCords.x << ' ' << cellCords.y << '\n';
                

            }

            

            /*
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                board.move(i, j, LEFT);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                board.move(i, j, RIGHT);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                board.move(i, j, UP);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                board.move(i, j, DOWN);
            }
            */
        }
        
        window.clear(backgroundColor);
        window.draw(board);
        window.display();
    }

    return 0;
}