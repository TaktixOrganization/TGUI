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


#include <TGUI/Renderers/LabelRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(LabelRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(LabelRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, BackgroundColor, Color::Transparent)
    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(LabelRenderer, TextStyle, TextStyle::Regular)

    TGUI_RENDERER_PROPERTY_NUMBER(LabelRenderer, TextOutlineThickness, 0)
    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, TextOutlineColor, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(LabelRenderer, TextureBackground)

    TGUI_RENDERER_PROPERTY_RENDERER(LabelRenderer, Scrollbar, "scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(LabelRenderer, ScrollbarWidth, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
