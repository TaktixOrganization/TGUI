/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Tests.hpp"
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Gui.hpp>

TEST_CASE("[Picture]")
{
    tgui::Picture::Ptr picture = std::make_shared<tgui::Picture>();

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(picture->connect("DoubleClicked", [](){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(picture->getWidgetType() == "Picture");
    }

    SECTION("constructor")
    {
        sf::Texture texture;
        texture.loadFromFile("resources/image.png");

        SECTION("from file")
        {
            REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>("resources/image.png"));
            REQUIRE(picture->getLoadedFilename() == "resources/image.png");
        }

        SECTION("from texture")
        {
            REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>(tgui::Texture{"resources/image.png"}));
            REQUIRE(picture->getLoadedFilename() == "");
        }

        SECTION("from sf::Texture")
        {
            REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>(texture));
            REQUIRE(picture->getLoadedFilename() == "");
        }

        REQUIRE(picture->getSize() == sf::Vector2f(texture.getSize()));
    }

    SECTION("setTexture")
    {
        picture->setSize(50, 50);

        sf::Texture texture;
        texture.loadFromFile("resources/image.png");

        SECTION("from file")
        {
            REQUIRE_NOTHROW(picture->setTexture("resources/image.png"));
            REQUIRE(picture->getLoadedFilename() == "resources/image.png");
        }

        SECTION("from texture")
        {
            REQUIRE_NOTHROW(picture->setTexture(tgui::Texture{"resources/image.png"}));
            REQUIRE(picture->getLoadedFilename() == "");
        }

        SECTION("from sf::Texture")
        {
            REQUIRE_NOTHROW(picture->setTexture(texture));
            REQUIRE(picture->getLoadedFilename() == "");
        }

        REQUIRE(picture->getSize() == sf::Vector2f(50, 50));
    }

    SECTION("Smooth")
    {
        // Calling isSmooth has no effect when not loaded
        REQUIRE(!picture->isSmooth());
        picture->setSmooth(true);
        REQUIRE(!picture->isSmooth());

        picture = std::make_shared<tgui::Picture>("resources/image.png");
        REQUIRE(!picture->isSmooth());
        picture->setSmooth(true);
        REQUIRE(picture->isSmooth());
        picture->setSmooth(false);
        REQUIRE(!picture->isSmooth());
    }

    SECTION("events")
    {
        unsigned int mousePressedCount = 0;
        unsigned int mouseReleasedCount = 0;
        unsigned int clickedCount = 0;
        unsigned int doubleClickedCount = 0;

        picture->setTexture("resources/image.png");
        picture->setPosition(40, 30);
        picture->setSize(150, 100);

        picture->connect("MousePressed", mousePressed, std::ref(mousePressedCount));
        picture->connect("MouseReleased", mouseReleased, std::ref(mouseReleasedCount));
        picture->connect("Clicked", mouseClicked, std::ref(clickedCount));
        picture->connect("DoubleClicked", doubleClicked, std::ref(doubleClickedCount));

        SECTION("mouseOnWidget")
        {
            picture->setTexture("resources/TransparentParts.png", true);

            REQUIRE(!picture->mouseOnWidget(10, 15));
            REQUIRE(picture->mouseOnWidget(40, 30));
            REQUIRE(picture->mouseOnWidget(115, 80));
            REQUIRE(picture->mouseOnWidget(189, 129));
            REQUIRE(!picture->mouseOnWidget(190, 130));

            picture->setTexture("resources/TransparentParts.png", false);
            REQUIRE(!picture->mouseOnWidget(115, 80));

            REQUIRE(mousePressedCount == 0);
            REQUIRE(mouseReleasedCount == 0);
            REQUIRE(clickedCount == 0);
            REQUIRE(doubleClickedCount == 0);
        }

        SECTION("mouse click")
        {
            picture->leftMouseReleased(115, 80);

            REQUIRE(mouseReleasedCount == 1);
            REQUIRE(clickedCount == 0);

            SECTION("mouse press")
            {
                picture->leftMousePressed(115, 80);

                REQUIRE(mousePressedCount == 1);
                REQUIRE(mouseReleasedCount == 1);
                REQUIRE(clickedCount == 0);
            }

            picture->leftMouseReleased(115, 80);

            REQUIRE(mousePressedCount == 1);
            REQUIRE(mouseReleasedCount == 2);
            REQUIRE(clickedCount == 1);
            REQUIRE(doubleClickedCount == 0);
        }

        SECTION("double click")
        {
            picture->leftMousePressed(115, 80);
            picture->leftMouseReleased(115, 80);

            tgui::Gui gui;
            gui.add(picture);
            gui.updateTime(doubleClickTimeout);

            picture->leftMousePressed(115, 80);
            picture->leftMouseReleased(115, 80);

            REQUIRE(mousePressedCount == 2);
            REQUIRE(mouseReleasedCount == 2);
            REQUIRE(clickedCount == 2);
            REQUIRE(doubleClickedCount == 0);

            gui.updateTime(doubleClickTimeout / 2.f);

            picture->leftMousePressed(115, 80);
            picture->leftMouseReleased(115, 80);

            REQUIRE(doubleClickedCount == 1);
        }
    }

    SECTION("Renderer")
    {
        auto renderer = picture->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
        }

        SECTION("functions")
        {
            renderer->setOpacity(0.8f);

            SECTION("getPropertyValuePairs")
            {
                auto pairs = renderer->getPropertyValuePairs();
                REQUIRE(pairs.size() == 1);
                REQUIRE(pairs["opacity"].getNumber() == 0.8f);
            }
        }

        REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);
    }

    SECTION("Saving and loading from file")
    {
        REQUIRE_NOTHROW(picture = std::make_shared<tgui::Picture>("resources/Black.png"));

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(picture);

        picture->setSmooth();
        picture->getRenderer()->setOpacity(0.8f);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePicture1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFilePicture1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePicture2.txt"));
        REQUIRE(compareFiles("WidgetFilePicture1.txt", "WidgetFilePicture2.txt"));

        SECTION("Copying widget")
        {
            copy(parent, picture);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePicture2.txt"));
            REQUIRE(compareFiles("WidgetFilePicture1.txt", "WidgetFilePicture2.txt"));
        }
    }
}