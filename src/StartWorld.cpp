#include "StartWorld.hpp"

#include <fstream>
#include <Tank/System/Game.hpp>
#include <Tank/System/Keyboard.hpp>
#include "MainWorld.hpp"

tank::Font StartWorld::titleFont;
tank::Font StartWorld::font;

StartWorld::StartWorld()
{
    // Load fonts
    titleFont.loadFromFile("res/Anonymous Pro.ttf");
    font.loadFromFile("res/Consola.ttf");

    // Make title text
    auto gui = makeEntity<tank::Entity>();
    auto title = gui->makeGraphic<tank::Text>(titleFont);
    title->setColor({255,255,255});
    title->setText("dango");
    title->setFontSize(72);
    title->setOrigin(tank::Vectori(title->getSize() / 2));
    title->setPos(tank::Vectorf(tank::Game::window()->getSize().x / 2, 50));

    errorText_ = gui->makeGraphic<tank::Text>(titleFont);
    errorText_->setColor({200,0,0});
    errorText_->setFontSize(14);
    errorText_->setPos(tank::Vectorf(tank::Game::window()->getSize().x / 2, 400));

    // Create error text

    std::function<void()> callback = std::bind(&StartWorld::startGame,this);
    // Make input boxes
    hostname_ = makeEntity<Input>(tank::Vectorf{250,300},
                                  tank::Vectoru{200,16},
                                  "Hostname:",
                                  callback);
    port_ = makeEntity<Input>(tank::Vectorf{250,330},
                              tank::Vectoru{200,16},
                              "Port:",
                              callback);

    button_ = makeEntity<Button>(tank::Vectorf{250,360},
                                 tank::Vectoru{200,30},
                                 "Connect",
                                 callback);
    // Set default text
    std::string hostname = "127.0.0.1";
    std::string port = "8037";
    std::ifstream defaultServer {"default_server.txt"};
    defaultServer >> hostname >> port;
    hostname_->setText(hostname);
    port_->setText(port);

    // Key bindings
    using K = tank::Keyboard;
    using Key = tank::Key;
    connect(K::KeyPress(Key::Tab) && []{return not K::control();},
            [this](){ switchFocus(1);} );
    connect(K::KeyPress(Key::Tab) && K::Control,
            [this](){ switchFocus(menuItems - 1);});
    //connect(K::KeyPress(Key::Return), &StartWorld::switchFocus);

    // Give focus to hostname
    menuItem_ = 0;
    hostname_->gainFocus();
}

void StartWorld::switchFocus(unsigned shift)
{
    menuItem_ += shift;
    menuItem_ %= menuItems;

    if (menuItem_ == 0) {
        hostname_->gainFocus();
        port_->loseFocus();
        button_ -> loseFocus();
    } else if (menuItem_ == 1) {
        hostname_->loseFocus();
        port_->gainFocus();
        button_ -> loseFocus();
    } else {
        hostname_->loseFocus();
        port_->loseFocus();
        button_ -> gainFocus();
    }
}

void StartWorld::startGame()
{
    using IO = boost::asio::io_service;
    try {
        std::shared_ptr<IO> io {new IO};
        Connection connection {io};
        std::string hostname = hostname_->getText();
        std::string port = port_->getText();

        errorText_->setText("Connecting...");
        connection.connect(hostname, port);
        tank::Game::makeWorld<MainWorld>(io, std::move(connection));
        errorText_->setText("");

        std::ofstream defaultServer {"default_server.txt"};
        defaultServer << hostname << std::endl << port << std::endl;
    }
    catch (std::exception const& e) {
        tank::Game::log << "Exception while connecting to server: "
                        << e.what() << std::endl;
        errorText_->setText(e.what());
        errorText_->setOrigin(tank::Vectori(errorText_->getSize() / 2));
    }
}
