/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "Tests.hpp"
#include <TGUI/Widgets/Button.hpp>

TEST_CASE("[Signal]")
{
    tgui::Widget::Ptr widget = tgui::ClickableWidget::create();

    SECTION("connect")
    {
        unsigned int id = widget->connect("PositionChanged", [](){});
        REQUIRE(widget->connect("PositionChanged", [](tgui::Vector2f){}) == ++id);
        REQUIRE(widget->connect("PositionChanged", [](tgui::Widget::Ptr, tgui::String){}) == ++id);
        REQUIRE(widget->connect("PositionChanged", [](tgui::Widget::Ptr, tgui::String, tgui::Vector2f){}) == ++id);

        REQUIRE(widget->connect("SizeChanged", [](){}) == ++id);
        REQUIRE(widget->connect("SizeChanged", [](tgui::Vector2f){}) == ++id);
        REQUIRE(widget->connect("SizeChanged", [](tgui::Widget::Ptr, tgui::String){}) == ++id);
        REQUIRE(widget->connect("SizeChanged", [](tgui::Widget::Ptr, tgui::String, tgui::Vector2f){}) == ++id);

        REQUIRE(widget->connect("Focused", [](){}) == ++id);
        REQUIRE(widget->connect("Focused", [](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->connect("Unfocused", [](){}) == ++id);
        REQUIRE(widget->connect("Unfocused", [](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->connect("MouseEntered", [](){}) == ++id);
        REQUIRE(widget->connect("MouseEntered", [](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->connect("MouseLeft", [](){}) == ++id);
        REQUIRE(widget->connect("MouseLeft", [](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->connect("AnimationFinished", [](){}) == ++id);
        REQUIRE(widget->connect("AnimationFinished", [](tgui::ShowAnimationType){}) == ++id);
        REQUIRE(widget->connect("AnimationFinished", [](bool){}) == ++id);
        REQUIRE(widget->connect("AnimationFinished", [](tgui::ShowAnimationType, bool){}) == ++id);

        REQUIRE_THROWS_AS(widget->connect("SomeInvalidSignal", [](){}), tgui::Exception);

        tgui::Widget::Ptr widget2 = tgui::Button::create();
        REQUIRE(widget2->connect("Pressed", [](){}) == ++id);
        REQUIRE(widget2->connect("Pressed", [](int){}, 5) == ++id);
        REQUIRE(widget2->connect("Pressed", [](auto){}, 10.f) == ++id);
        REQUIRE(widget2->connect("Pressed", [](tgui::Widget::Ptr, tgui::String){}) == ++id);
        REQUIRE(widget2->connect("Pressed", [](auto, auto, auto, tgui::Widget::Ptr, tgui::String){}, "Hey", 15, 3.f) == ++id);
        REQUIRE(widget2->connect("Pressed", std::bind([](int){}, 5)) == ++id);

        REQUIRE(widget2->connect({"MouseEntered", "MouseLeft"}, [](){}) == id+2);
        REQUIRE(widget2->connect({"PositionChanged", "SizeChanged", "MouseEntered", "MouseLeft", "Pressed"}, [](const tgui::Widget::Ptr&, const tgui::String&){}) == id+7);
        id += 7;

        struct Class
        {
            void signalHandler1() {}
            void signalHandler2(tgui::Widget::Ptr, const tgui::String&) {}
            void signalHandler3(int, float, tgui::Widget::Ptr, const tgui::String&) {}
            void signalHandler4(int&, tgui::Widget::Ptr, const tgui::String&) {}
            void signalHandler5(int&, const tgui::String&) {}
            void signalHandler6(int&, tgui::Widget::Ptr, const tgui::String&, tgui::String) {}

            void signalHandler7() const {}
            void signalHandler8(tgui::Widget::Ptr, const tgui::String&) const {}
            void signalHandler9(int, float, tgui::Widget::Ptr, const tgui::String&) const {}
            void signalHandler10(int&, tgui::Widget::Ptr, const tgui::String&) const {}
            void signalHandler11(int&, const tgui::String&) const {}
            void signalHandler12(int&, tgui::Widget::Ptr, const tgui::String&, tgui::String) const {}
        };

        int i;
        Class instance;
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler1, &instance) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler2, &instance) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler3, &instance, 0, 5.f) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler4, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler5, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler6, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler7, &instance) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler8, &instance) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler9, &instance, 0, 0.5f) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler10, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler11, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->connect("Pressed", &Class::signalHandler12, &instance, std::ref(i)) == ++id);

        REQUIRE(widget2->connect("Pressed", std::function<void(tgui::String)>([](tgui::String){})) == ++id);
    }

    SECTION("disconnect")
    {
        unsigned int i = 0;
        unsigned int id = widget->connect("PositionChanged", [&](){ i++; });
        widget->setPosition(10, 10);
        REQUIRE(i == 1);

        widget->disconnect(id);
        widget->setPosition(20, 20);
        REQUIRE(i == 1);

        widget->connect("PositionChanged", [&](){ i++; });
        widget->connect("SizeChanged", [&](){ i++; });
        widget->disconnectAll("PositionChanged");
        widget->setPosition(30, 30);
        widget->setSize(100, 25);
        REQUIRE(i == 2);

        widget->disconnectAll("PositionChanged");
        widget->disconnectAll("SizeChanged");
        widget->connect("PositionChanged", [&](){ i++; });
        widget->connect("SizeChanged", [&](){ i++; });
        widget->setPosition(50, 50);
        widget->setSize(300, 75);
        REQUIRE(i == 4);

        widget->disconnectAll();
        widget->setPosition(60, 60);
        widget->setSize(400, 100);
        REQUIRE(i == 4);
    }
}
