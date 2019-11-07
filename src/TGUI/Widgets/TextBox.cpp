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


#include <TGUI/Clipboard.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Clipping.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox()
    {
        m_type = "TextBox";
        m_draggableWidget = true;
        m_textBeforeSelection.setFont(m_fontCached);
        m_textSelection1.setFont(m_fontCached);
        m_textSelection2.setFont(m_fontCached);
        m_textAfterSelection1.setFont(m_fontCached);
        m_textAfterSelection2.setFont(m_fontCached);

        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);
        m_horizontalScrollbar->setVisible(false);

        m_renderer = aurora::makeCopied<TextBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_fontCached, m_textSize) * 18,
                 10 * m_fontCached.getLineSpacing(m_textSize) + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize) + Text::getExtraVerticalPadding(m_textSize)
                 + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::Ptr TextBox::create()
    {
        return std::make_shared<TextBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::Ptr TextBox::copy(TextBox::ConstPtr textBox)
    {
        if (textBox)
            return std::static_pointer_cast<TextBox>(textBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBoxRenderer* TextBox::getSharedRenderer()
    {
        return aurora::downcast<TextBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextBoxRenderer* TextBox::getSharedRenderer() const
    {
        return aurora::downcast<const TextBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBoxRenderer* TextBox::getRenderer()
    {
        return aurora::downcast<TextBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextBoxRenderer* TextBox::getRenderer() const
    {
        return aurora::downcast<const TextBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        updateScrollbars();

        // The size of the text box has changed, update the text
        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setText(const String& text)
    {
        // Remove all the excess characters when a character limit is set
        if ((m_maxChars > 0) && (text.length() > m_maxChars))
            m_text = text.toUtf32().substr(0, m_maxChars);
        else
            m_text = text.toUtf32();

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::addText(const String& text)
    {
        setText(m_text + text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String TextBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectedText(std::size_t selectionStartIndex, std::size_t selectionEndIndex)
    {
        setCaretPosition(selectionEndIndex);
        auto selEnd = m_selEnd;
        setCaretPosition(selectionStartIndex);
        m_selEnd = selEnd;
        updateSelectionTexts();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String TextBox::getSelectedText() const
    {
        const std::size_t selStart = getSelectionStart();
        const std::size_t selEnd = getSelectionEnd();
        if (selStart <= selEnd)
            return m_text.substr(selStart, selEnd - selStart);
        else
            return m_text.substr(selEnd, selStart - selEnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getSelectionStart() const
    {
        return getIndexOfSelectionPos(m_selStart);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getSelectionEnd() const
    {
        return getIndexOfSelectionPos(m_selEnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_textSize = size;
        if (m_textSize < 1)
            m_textSize = 1;

        // Change the text size
        m_textBeforeSelection.setCharacterSize(m_textSize);
        m_textSelection1.setCharacterSize(m_textSize);
        m_textSelection2.setCharacterSize(m_textSize);
        m_textAfterSelection1.setCharacterSize(m_textSize);
        m_textAfterSelection2.setCharacterSize(m_textSize);

        // Calculate the height of one line
        m_lineHeight = static_cast<unsigned int>(m_fontCached.getLineSpacing(m_textSize));

        m_verticalScrollbar->setScrollAmount(m_lineHeight);
        m_horizontalScrollbar->setScrollAmount(m_textSize);

        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setMaximumCharacters(std::size_t maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exceeded
        if ((m_maxChars > 0) && (m_text.length() > m_maxChars))
        {
            // Remove all the excess characters
            m_text.erase(m_maxChars, std::u32string::npos);
            rearrangeText(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setVerticalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_verticalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_verticalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(true);
        }

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy TextBox::getVerticalScrollbarPolicy() const
    {
        return m_verticalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setHorizontalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_horizontalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_horizontalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(true);
        }

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy TextBox::getHorizontalScrollbarPolicy() const
    {
        return m_horizontalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setCaretPosition(std::size_t charactersBeforeCaret)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.length())
            charactersBeforeCaret = m_text.length();

        // Find the line and position on that line on which the caret is located
        std::size_t count = 0;
        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            if (count + m_lines[i].length() < charactersBeforeCaret)
            {
                count += m_lines[i].length();
                if ((count < m_text.length()) && (m_text[count] == U'\n'))
                    count += 1;
            }
            else
            {
                m_selStart.y = i;
                m_selStart.x = charactersBeforeCaret - count;

                m_selEnd = m_selStart;
                updateSelectionTexts();
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getCaretPosition() const
    {
        return getSelectionEnd();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::isReadOnly() const
    {
        return m_readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getLinesCount() const
    {
        return m_lines.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setFocused(bool focused)
    {
        if (focused)
        {
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else // Unfocusing
        {
            // If there is a selection then undo it now
            if (m_selStart != m_selEnd)
            {
                m_selStart = m_selEnd;
                updateSelectionTexts();
            }
        }

#if defined (TGUI_SYSTEM_ANDROID) || defined (TGUI_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(focused);
#endif

        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::enableMonospacedFontOptimization(bool enable)
    {
        m_monospacedFontOptimizationEnabled = enable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setVerticalScrollbarValue(unsigned int value)
    {
        m_verticalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getVerticalScrollbarValue() const
    {
        return m_verticalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setHorizontalScrollbarValue(unsigned int value)
    {
        m_horizontalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getHorizontalScrollbarValue() const
    {
        return m_horizontalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        // Set the mouse down flag
        m_mouseDown = true;

        // If there is a scrollbar then pass the event
        if ((m_verticalScrollbar->isShown()) && (m_verticalScrollbar->mouseOnWidget(pos)))
        {
            m_verticalScrollbar->leftMousePressed(pos);
            recalculateVisibleLines();
        }
        else if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->mouseOnWidget(pos))
        {
            m_horizontalScrollbar->leftMousePressed(pos);
        }
        else // The click occurred on the text box
        {
            // Don't continue when line height is 0
            if (m_lineHeight == 0)
                return;

            const auto caretPosition = findCaretPosition(pos);

            // Check if this is a double click
            if ((m_possibleDoubleClick) && (m_selStart == m_selEnd) && (caretPosition == m_selEnd))
            {
                // The next click is going to be a normal one again
                m_possibleDoubleClick = false;

                // If the click was to the right of the end of line then make sure to select the word on the left
                if (m_lines[m_selStart.y].length() > 1 && (m_selStart.x == (m_lines[m_selStart.y].length()-1) || m_selStart.x == m_lines[m_selStart.y].length()))
                {
                    m_selStart.x--;
                    m_selEnd.x = m_selStart.x;
                }

                bool selectingWhitespace;
                if (isWhitespace(m_lines[m_selStart.y][m_selStart.x]))
                    selectingWhitespace = true;
                else
                    selectingWhitespace = false;

                // Move start pointer to the beginning of the word/whitespace
                for (std::size_t i = m_selStart.x; i > 0; --i)
                {
                    if (selectingWhitespace != isWhitespace(m_lines[m_selStart.y][i-1]))
                    {
                        m_selStart.x = i;
                        break;
                    }
                    else
                        m_selStart.x = 0;
                }

                // Move end pointer to the end of the word/whitespace
                for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].length(); ++i)
                {
                    if (selectingWhitespace != isWhitespace(m_lines[m_selEnd.y][i]))
                    {
                        m_selEnd.x = i;
                        break;
                    }
                    else
                        m_selEnd.x = m_lines[m_selEnd.y].length();
                }
            }
            else // No double clicking
            {
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
                    m_selStart = caretPosition;

                m_selEnd = caretPosition;

                // If the next click comes soon enough then it will be a double click
                m_possibleDoubleClick = true;
            }

            // Update the texts
            updateSelectionTexts();

            // The caret should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMouseReleased(Vector2f pos)
    {
        // If there is a scrollbar then pass it the event
        if (m_verticalScrollbar->isShown())
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_verticalScrollbar->isMouseDown())
            {
                m_verticalScrollbar->leftMouseReleased(pos - getPosition());
                recalculateVisibleLines();
            }
        }

        if (m_horizontalScrollbar->isShown())
        {
            if (m_horizontalScrollbar->isMouseDown())
                m_horizontalScrollbar->leftMouseReleased(pos - getPosition());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse event should go to the vertical scrollbar
        if (m_verticalScrollbar->isShown() && ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->mouseOnWidget(pos)))
        {
            m_verticalScrollbar->mouseMoved(pos);
            recalculateVisibleLines();
        }

        // Check if the mouse event should go to the horizontal scrollbar
        else if (m_horizontalScrollbar->isShown() && ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->mouseOnWidget(pos)))
        {
            m_horizontalScrollbar->mouseMoved(pos);
        }

        // If the mouse is held down then you are selecting text
        else if (m_mouseDown)
        {
            auto caretPosition = findCaretPosition(pos);
            if (caretPosition != m_selEnd)
            {
                m_selEnd = caretPosition;
                updateSelectionTexts();
            }

            // Check if the caret is located above or below the view
            if (m_verticalScrollbar->isShown())
            {
                if (m_selEnd.y <= m_topLine)
                    m_verticalScrollbar->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
                else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                    m_verticalScrollbar->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight) - m_verticalScrollbar->getViewportSize()));

                recalculateVisibleLines();
            }
        }

        // Inform the scrollbars that the mouse is not on them
        else
        {
            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->mouseNoLongerOnWidget();

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();

        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->leftMouseButtonNoLongerDown();

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        switch (event.code)
        {
            case sf::Keyboard::Up:
            {
                m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y - (m_lineHeight / 2.f) - m_verticalScrollbar->getValue()});

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Down:
            {
                m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y + (m_lineHeight * 1.5f) - m_verticalScrollbar->getValue()});

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Left:
            {
                if (event.control)
                {
                    // Move to the beginning of the word (or to the beginning of the previous word when already at the beginning)
                    bool skippedWhitespace = false;
                    bool done = false;
                    for (std::size_t j = m_selEnd.y + 1; j > 0; --j)
                    {
                        for (std::size_t i = m_selEnd.x; i > 0; --i)
                        {
                            if (skippedWhitespace)
                            {
                                if (isWhitespace(m_lines[m_selEnd.y][i-1]))
                                {
                                    m_selEnd.x = i;
                                    done = true;
                                    break;
                                }
                            }
                            else
                            {
                                if (!isWhitespace(m_lines[m_selEnd.y][i-1]))
                                    skippedWhitespace = true;
                            }
                        }

                        if (!done)
                        {
                            if (!skippedWhitespace)
                            {
                                if (m_selEnd.y > 0)
                                {
                                    m_selEnd.y--;
                                    m_selEnd.x = m_lines[m_selEnd.y].length();
                                }
                            }
                            else
                            {
                                m_selEnd.x = 0;
                                break;
                            }
                        }
                        else
                            break;
                    }
                }
                else // Control key is not being pressed
                {
                    if ((m_selStart != m_selEnd) && !event.shift)
                    {
                        if ((m_selStart.y < m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x < m_selEnd.x)))
                            m_selEnd = m_selStart;
                    }
                    else if (m_selEnd.x > 0)
                        m_selEnd.x--;
                    else
                    {
                        // You are at the left side of a line so move up
                        if (m_selEnd.y > 0)
                        {
                            m_selEnd.y--;
                            m_selEnd.x = m_lines[m_selEnd.y].length();
                        }
                    }
                }

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Right:
            {
                if (event.control)
                {
                    // Move to the end of the word (or to the end of the next word when already at the end)
                    bool skippedWhitespace = false;
                    bool done = false;
                    for (std::size_t j = m_selEnd.y; j < m_lines.size(); ++j)
                    {
                        for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].length(); ++i)
                        {
                            if (skippedWhitespace)
                            {
                                if (isWhitespace(m_lines[m_selEnd.y][i]))
                                {
                                    m_selEnd.x = i;
                                    done = true;
                                    break;
                                }
                            }
                            else
                            {
                                if (!isWhitespace(m_lines[m_selEnd.y][i]))
                                    skippedWhitespace = true;
                            }
                        }

                        if (!done)
                        {
                            if (!skippedWhitespace)
                            {
                                if (m_selEnd.y + 1 < m_lines.size())
                                {
                                    m_selEnd.y++;
                                    m_selEnd.x = 0;
                                }
                            }
                            else
                            {
                                m_selEnd.x = m_lines[m_selEnd.y].length();
                                break;
                            }
                        }
                        else
                            break;
                    }
                }
                else // Control key is not being pressed
                {
                    if ((m_selStart != m_selEnd) && !event.shift)
                    {
                        if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                            m_selEnd = m_selStart;
                    }
                    else
                    {
                        // Move to the next line if you are at the end of the line
                        if (m_selEnd.x == m_lines[m_selEnd.y].length())
                        {
                            if (m_selEnd.y + 1 < m_lines.size())
                            {
                                m_selEnd.y++;
                                m_selEnd.x = 0;
                            }
                        }
                        else
                            m_selEnd.x++;
                    }
                }

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Home:
            {
                if (event.control)
                    m_selEnd = {0, 0};
                else
                    m_selEnd.x = 0;

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::End:
            {
                if (event.control)
                    m_selEnd = {m_lines[m_lines.size()-1].length(), m_lines.size()-1};
                else
                    m_selEnd.x = m_lines[m_selEnd.y].length();

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::PageUp:
            {
                // Move to the top line when not there already
                if (m_selEnd.y != m_topLine)
                    m_selEnd.y = m_topLine;
                else
                {
                    // Scroll up when we already where at the top line
                    const auto visibleLines = static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight);
                    if (m_topLine < visibleLines - 1)
                        m_selEnd.y = 0;
                    else
                        m_selEnd.y = m_topLine - visibleLines + 1;
                }

                m_selEnd.x = 0;

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::PageDown:
            {
                // Move to the bottom line when not there already
                if (m_topLine + m_visibleLines > m_lines.size())
                    m_selEnd.y = m_lines.size() - 1;
                else if (m_selEnd.y != m_topLine + m_visibleLines - 1)
                    m_selEnd.y = m_topLine + m_visibleLines - 1;
                else
                {
                    // Scroll down when we already where at the bottom line
                    const auto visibleLines = static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight);
                    if (m_selEnd.y + visibleLines >= m_lines.size() + 2)
                        m_selEnd.y = m_lines.size() - 1;
                    else
                        m_selEnd.y = m_selEnd.y + visibleLines - 2;
                }

                m_selEnd.x = m_lines[m_selEnd.y].length();

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Tab:
            {
                textEntered(U'\t');
                break;
            }

            case sf::Keyboard::Return:
            {
                textEntered(U'\n');
                break;
            }

            case sf::Keyboard::BackSpace:
            {
                if (m_readOnly)
                    break;

                // Check that we did not select any characters
                if (m_selStart == m_selEnd)
                {
                    const std::size_t pos = getSelectionEnd();
                    if (pos > 0)
                    {
                        if (m_selEnd.x > 0)
                        {
                            // There is a specific case that we have to watch out for. When we are removing the last character on
                            // a line which was placed there by word wrap and a newline follows this character then the caret
                            // has to be placed at the line above (before the newline) instead of at the same line (after the newline)
                            if ((m_lines[m_selEnd.y].length() == 1) && (pos > 1) && (pos < m_text.length()) && (m_text[pos-2] != U'\n') && (m_text[pos] == U'\n') && (m_selEnd.y > 0))
                            {
                                m_selEnd.y--;
                                m_selEnd.x = m_lines[m_selEnd.y].length();
                            }
                            else // Just remove the character normally
                                --m_selEnd.x;
                        }
                        else // At the beginning of the line
                        {
                            if (m_selEnd.y > 0)
                            {
                                --m_selEnd.y;
                                m_selEnd.x = m_lines[m_selEnd.y].length();

                                if ((m_text[pos-1] != U'\n') && m_selEnd.x > 0)
                                    --m_selEnd.x;
                            }
                        }

                        m_selStart = m_selEnd;

                        m_text.erase(pos - 1, 1);
                        rearrangeText(true);
                    }
                }
                else // When you did select some characters then delete them
                    deleteSelectedCharacters();

                // The caret should be visible again
                m_caretVisible = true;
                m_animationTimeElapsed = {};

                onTextChange.emit(this, m_text);
                break;
            }

            case sf::Keyboard::Delete:
            {
                if (m_readOnly)
                    break;

                // Check that we did not select any characters
                if (m_selStart == m_selEnd)
                {
                    m_text.erase(getSelectionEnd(), 1);
                    rearrangeText(true);
                }
                else // You did select some characters, so remove them
                    deleteSelectedCharacters();

                onTextChange.emit(this, m_text);
                break;
            }

            case sf::Keyboard::A:
            {
                if (event.control && !event.alt && !event.shift && !event.system)
                {
                    m_selStart = {0, 0};
                    m_selEnd = Vector2<std::size_t>(m_lines[m_lines.size()-1].length(), m_lines.size()-1);
                    updateSelectionTexts();
                }

                break;
            }

            case sf::Keyboard::C:
            {
                if (event.control && !event.alt && !event.shift && !event.system)
                {
                    const std::size_t selStart = getSelectionStart();
                    const std::size_t selEnd = getSelectionEnd();
                    if (selStart <= selEnd)
                        Clipboard::set(m_text.substr(selStart, selEnd - selStart));
                    else
                        Clipboard::set(m_text.substr(selEnd, selStart - selEnd));
                }
                break;
            }

            case sf::Keyboard::X:
            {
                if (event.control && !event.alt && !event.shift && !event.system && !m_readOnly)
                {
                    const std::size_t selStart = getSelectionStart();
                    const std::size_t selEnd = getSelectionEnd();
                    if (selStart <= selEnd)
                        Clipboard::set(m_text.substr(selStart, selEnd - selStart));
                    else
                        Clipboard::set(m_text.substr(selEnd, selStart - selEnd));

                    deleteSelectedCharacters();
                }
                break;
            }

            case sf::Keyboard::V:
            {
                if (event.control && !event.alt && !event.shift && !event.system && !m_readOnly)
                {
                    const std::u32string clipboardContents = Clipboard::get().toUtf32();

                    // Only continue pasting if you actually have to do something
                    if ((m_selStart != m_selEnd) || (clipboardContents != ""))
                    {
                        deleteSelectedCharacters();

                        m_text.insert(getSelectionEnd(), clipboardContents);
                        m_lines[m_selEnd.y].insert(m_selEnd.x, clipboardContents);

                        m_selEnd.x += clipboardContents.length();
                        m_selStart = m_selEnd;
                        rearrangeText(true);

                        onTextChange.emit(this, m_text);
                    }
                }

                break;
            }

            default:
                break;
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::textEntered(char32_t key)
    {
        if (m_readOnly)
            return;

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.length() + 1 > m_maxChars))
            return;

        auto insert = TGUI_LAMBDA_CAPTURE_EQ_THIS()
        {
            deleteSelectedCharacters();

            const std::size_t caretPosition = getSelectionEnd();

            m_text.insert(m_text.begin() + caretPosition, key);
            m_lines[m_selEnd.y].insert(m_lines[m_selEnd.y].begin() + m_selEnd.x, key);

            // Increment the caret position, unless you type a newline at the start of a line while that line only existed due to word wrapping
            if ((key != U'\n') || (m_selEnd.x > 0) || (m_selEnd.y == 0) || m_lines[m_selEnd.y-1].empty() || (m_text[caretPosition-1] == U'\n'))
            {
                m_selStart.x++;
                m_selEnd.x++;
            }

            rearrangeText(true);
        };

        // If there is a scrollbar then inserting can't go wrong
        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Never)
        {
            insert();
        }
        else // There is no scrollbar, the text may not fit
        {
            // Store the data so that it can be reverted
            const auto oldText = m_text;
            const auto oldSelStart = m_selStart;
            const auto oldSelEnd = m_selEnd;

            // Try to insert the character
            insert();

            // Undo the insert if the text does not fit
            if (m_lines.size() > getInnerSize().y / m_lineHeight)
            {
                m_text = oldText;
                m_selStart = oldSelStart;
                m_selEnd = oldSelEnd;

                rearrangeText(true);
            }
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_horizontalScrollbar->isShown()
            && (!m_verticalScrollbar->isShown()
                || m_horizontalScrollbar->mouseOnWidget(pos - getPosition())
                || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)))
        {
            m_horizontalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            recalculateVisibleLines();
        }
        else if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            recalculateVisibleLines();
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2<std::size_t> TextBox::findCaretPosition(Vector2f position) const
    {
        position.x -= m_bordersCached.getLeft() + m_paddingCached.getLeft();
        position.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_fontCached == nullptr))
            return Vector2<std::size_t>(m_lines[m_lines.size()-1].size(), m_lines.size()-1);

        // Find on which line the mouse is
        std::size_t lineNumber;
        if (m_verticalScrollbar->isShown())
        {
            if (position.y + m_verticalScrollbar->getValue() < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor((position.y + m_verticalScrollbar->getValue()) / m_lineHeight));
        }
        else
        {
            if (position.y < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor(position.y / m_lineHeight));
        }

        // Check if you clicked behind everything
        if (lineNumber + 1 > m_lines.size())
            return Vector2<std::size_t>(m_lines[m_lines.size()-1].size(), m_lines.size()-1);

        // Find between which character the mouse is standing
        float width = Text::getExtraHorizontalPadding(m_fontCached, m_textSize) - m_horizontalScrollbar->getValue();
        char32_t prevChar = 0;
        for (std::size_t i = 0; i < m_lines[lineNumber].size(); ++i)
        {
            float charWidth;
            const char32_t curChar = m_lines[lineNumber][i];
            //if (curChar == U'\n')
            //    return Vector2<std::size_t>(m_lines[lineNumber].getSize() - 1, lineNumber); // TextBox strips newlines but this code is kept for when this function is generalized
            //else
            if (curChar == U'\t')
                charWidth = static_cast<float>(m_fontCached.getGlyph(' ', getTextSize(), false).advance) * 4;
            else
                charWidth = static_cast<float>(m_fontCached.getGlyph(curChar, getTextSize(), false).advance);

            const float kerning = m_fontCached.getKerning(prevChar, curChar, getTextSize());
            if (width + charWidth + kerning <= position.x)
                width += charWidth + kerning;
            else
            {
                if (position.x < width + kerning + (charWidth / 2.0f))
                    return {i, lineNumber};
                else
                    return {i + 1, lineNumber};
            }

            prevChar = curChar;
        }

        // You clicked behind the last character
        return Vector2<std::size_t>(m_lines[lineNumber].length(), lineNumber);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getIndexOfSelectionPos(Vector2<std::size_t> selectionPos) const
    {
        auto findIndex = [this](std::size_t line)
        {
            std::size_t counter = 0;
            for (std::size_t i = 0; i < line; ++i)
            {
                counter += m_lines[i].length();
                if ((counter < m_text.length()) && (m_text[counter] == U'\n'))
                    counter += 1;
            }

            return counter;
        };

        return findIndex(selectionPos.y) + selectionPos.x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::deleteSelectedCharacters()
    {
        if (m_selStart != m_selEnd)
        {
            const std::size_t selStart = getSelectionStart();
            const std::size_t selEnd = getSelectionEnd();
            if (selStart <= selEnd)
            {
                m_text.erase(selStart, selEnd - selStart);
                m_selEnd = m_selStart;
            }
            else
            {
                m_text.erase(selEnd, selStart - selEnd);
                m_selStart = m_selEnd;
            }

            rearrangeText(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::rearrangeText(bool keepSelection)
    {
        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_fontCached == nullptr))
            return;

        std::u32string string;
        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Never)
            string = m_text;
        else
        {
            // Find the maximum width of one line
            const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);
            float maxLineWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - 2 * textOffset;
            if (m_verticalScrollbar->isShown())
                maxLineWidth -= m_verticalScrollbar->getSize().x;

            // Don't do anything when there is no room for the text
            if (maxLineWidth <= 0)
                return;

            string = Text::wordWrap(maxLineWidth, m_text, m_fontCached, m_textSize, false, false);
        }

        // Store the current selection position when we are keeping the selection
        const std::size_t selStart = keepSelection ? getSelectionStart() : 0;
        const std::size_t selEnd = keepSelection ? getSelectionEnd() : 0;

        // Split the string in multiple lines
        m_maxLineWidth = 0;
        m_lines.clear();
        std::size_t searchPosStart = 0;
        std::size_t newLinePos = 0;
        std::size_t longestLineCharCount = 0;
        std::size_t longestLineIndex = 0;
        while (newLinePos != std::u32string::npos)
        {
            newLinePos = string.find('\n', searchPosStart);

            if (newLinePos != std::u32string::npos)
                m_lines.push_back(string.substr(searchPosStart, newLinePos - searchPosStart));
            else
                m_lines.push_back(string.substr(searchPosStart));

            if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Never)
            {
                if (m_monospacedFontOptimizationEnabled)
                {
                    if (m_lines.back().length() > longestLineCharCount)
                    {
                        longestLineCharCount = m_lines.back().length();
                        longestLineIndex = m_lines.size() - 1;
                    }
                }
                else // Not using optimization for monospaced font, so really calculate the width of every line
                {
                    float lineWidth = Text::getLineWidth(m_lines.back(), m_fontCached, m_textSize);
                    if (lineWidth > m_maxLineWidth)
                        m_maxLineWidth = lineWidth;
                }
            }

            searchPosStart = newLinePos + 1;
        }

        if ((m_horizontalScrollbarPolicy != Scrollbar::Policy::Never) && m_monospacedFontOptimizationEnabled)
            m_maxLineWidth = Text::getLineWidth(m_lines[longestLineIndex], m_fontCached, m_textSize);

        // Check if we should try to keep our selection
        if (keepSelection)
        {
            std::size_t index = 0;
            Vector2<std::size_t> newSelStart;
            Vector2<std::size_t> newSelEnd;
            bool newSelStartFound = false;
            bool newSelEndFound = false;

            // Look for the new locations of our selection
            for (std::size_t i = 0; i < m_lines.size(); ++i)
            {
                index += m_lines[i].length();

                if (!newSelStartFound && (index >= selStart))
                {
                    newSelStart = Vector2<std::size_t>(m_lines[i].length() - (index - selStart), i);

                    newSelStartFound = true;
                    if (newSelEndFound)
                        break;
                }

                if (!newSelEndFound && (index >= selEnd))
                {
                    newSelEnd = Vector2<std::size_t>(m_lines[i].length() - (index - selEnd), i);

                    newSelEndFound = true;
                    if (newSelStartFound)
                        break;
                }

                // Skip newlines in the text
                if (m_text[index] == U'\n')
                    ++index;
            }

            // Keep the selection when possible
            if (newSelStartFound && newSelEndFound)
            {
                m_selStart = newSelStart;
                m_selEnd = newSelEnd;
            }
            else // The text has changed too much, the selection can't be kept
            {
                m_selStart = Vector2<std::size_t>(m_lines[m_lines.size()-1].length(), m_lines.size()-1);
                m_selEnd = m_selStart;
            }
        }
        else // Set the caret at the back of the text
        {
            m_selStart = Vector2<std::size_t>(m_lines[m_lines.size()-1].length(), m_lines.size()-1);
            m_selEnd = m_selStart;
        }

        updateScrollbars();

        // Tell the scrollbars how many pixels the text contains
        const bool verticalScrollbarShown = m_verticalScrollbar->isShown();
        const bool horizontalScrollbarShown = m_horizontalScrollbar->isShown();

        m_verticalScrollbar->setMaximum(static_cast<unsigned int>(m_lines.size() * m_lineHeight
                                                                  + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)
                                                                  + Text::getExtraVerticalPadding(m_textSize)));

        m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_maxLineWidth
                                                                    + Text::getExtraHorizontalPadding(m_fontCached, m_textSize) * 2));

        if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Never)
        {
            // Word-wrap will have to be done again if the vertical scrollbar just appeared or disappeared
            if (verticalScrollbarShown != m_verticalScrollbar->isShown())
            {
                rearrangeText(true);
                return;
            }
            else
                updateScrollbars();
        }
        else // Horizontal scrollbar is enabled
        {
            if ((verticalScrollbarShown != m_verticalScrollbar->isShown()) || (horizontalScrollbarShown != m_horizontalScrollbar->isShown()))
                updateScrollbars();
        }

        updateSelectionTexts();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::updateScrollbars()
    {
        if (m_horizontalScrollbar->isShown())
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, getInnerSize().y - m_horizontalScrollbar->getSize().y});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_horizontalScrollbar->getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }
        else
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, getInnerSize().y});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }

        if (m_verticalScrollbar->isShown())
        {
            m_horizontalScrollbar->setSize({getInnerSize().x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_verticalScrollbar->getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }
        else
        {
            m_horizontalScrollbar->setSize({getInnerSize().x, m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::updateSelectionTexts()
    {
        // If there is no selection then just put the whole text in m_textBeforeSelection
        if (m_selStart == m_selEnd)
        {
            String displayedText;
            for (std::size_t i = 0; i < m_lines.size(); ++i)
                displayedText += m_lines[i] + "\n";

            m_textBeforeSelection.setString(displayedText);
            m_textSelection1.setString("");
            m_textSelection2.setString("");
            m_textAfterSelection1.setString("");
            m_textAfterSelection2.setString("");
        }
        else // Some text is selected
        {
            auto selectionStart = m_selStart;
            auto selectionEnd = m_selEnd;

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                std::swap(selectionStart, selectionEnd);

            // Set the text before the selection
            if (selectionStart.y > 0)
            {
                String string;
                for (std::size_t i = 0; i < selectionStart.y; ++i)
                    string += m_lines[i] + "\n";

                string += m_lines[selectionStart.y].substr(0, selectionStart.x);
                m_textBeforeSelection.setString(string);
            }
            else
                m_textBeforeSelection.setString(m_lines[0].substr(0, selectionStart.x));

            // Set the selected text
            if (m_selStart.y == m_selEnd.y)
            {
                m_textSelection1.setString(m_lines[selectionStart.y].substr(selectionStart.x, selectionEnd.x - selectionStart.x));
                m_textSelection2.setString("");
            }
            else
            {
                m_textSelection1.setString(m_lines[selectionStart.y].substr(selectionStart.x, m_lines[selectionStart.y].length() - selectionStart.x));

                String string;
                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                    string += m_lines[i] + "\n";

                string += m_lines[selectionEnd.y].substr(0, selectionEnd.x);

                m_textSelection2.setString(string);
            }

            // Set the text after the selection
            {
                m_textAfterSelection1.setString(m_lines[selectionEnd.y].substr(selectionEnd.x, m_lines[selectionEnd.y].length() - selectionEnd.x));

                String string;
                for (std::size_t i = selectionEnd.y + 1; i < m_lines.size(); ++i)
                    string += m_lines[i] + "\n";

                m_textAfterSelection2.setString(string);
            }
        }

        // Check if the caret is located above or below the view
        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Never)
        {
            if (m_selEnd.y <= m_topLine)
                m_verticalScrollbar->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
            else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                m_verticalScrollbar->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight)
                                                                        + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)
                                                                        + Text::getExtraVerticalPadding(m_textSize)
                                                                        - m_verticalScrollbar->getViewportSize()));
        }

        // Position the caret
        {
            /// TODO: We should be able to do this without creating sf::Text object

            const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);
            sf::Text tempText{sf::String(reinterpret_cast<const sf::Uint32*>(m_lines[m_selEnd.y].substr(0, m_selEnd.x).c_str())), *m_fontCached.getFont(), getTextSize()};

            float kerning = 0;
            if ((m_selEnd.x > 0) && (m_selEnd.x < m_lines[m_selEnd.y].length()))
                kerning = m_fontCached.getKerning(m_lines[m_selEnd.y][m_selEnd.x - 1], m_lines[m_selEnd.y][m_selEnd.x], m_textSize);

            m_caretPosition = {textOffset + tempText.findCharacterPos(tempText.getString().getSize()).x + kerning, static_cast<float>(m_selEnd.y * m_lineHeight)};
        }

        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Never)
        {
            const unsigned int left = m_horizontalScrollbar->getValue();
            if (m_caretPosition.x <= left)
            {
                unsigned int newValue =
                    static_cast<unsigned int>(std::max(0, static_cast<int>(m_caretPosition.x
                                                                           - (Text::getExtraHorizontalPadding(m_fontCached, m_textSize) * 2))));
                m_horizontalScrollbar->setValue(newValue);
            }
            else if (m_caretPosition.x > (left + m_horizontalScrollbar->getViewportSize()))
            {
                unsigned int newValue = static_cast<unsigned int>(m_caretPosition.x
                                                                  + (Text::getExtraHorizontalPadding(m_fontCached, m_textSize) * 2)
                                                                  - m_horizontalScrollbar->getViewportSize());
                m_horizontalScrollbar->setValue(newValue);
            }
        }

        recalculatePositions();

        // Send an event when the selection changed
        if ((m_selStart != m_lastSelection.first) || (m_selEnd != m_lastSelection.second))
        {
            // Only send the event when there is an actual change, not when the caret position moved
            if ((m_selStart != m_selEnd) || (m_lastSelection.first != m_lastSelection.second))
                onSelectionChange.emit(this);

            m_lastSelection.first = m_selStart;
            m_lastSelection.second = m_selEnd;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TextBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::update(Duration elapsedTime)
    {
        Widget::update(elapsedTime);

        // Only show/hide the caret every half second
        if (m_animationTimeElapsed >= std::chrono::milliseconds(500))
        {
            // Reset the elapsed time
            m_animationTimeElapsed = {};

            // Switch the value of the visible flag
            m_caretVisible = !m_caretVisible;

            // Too slow for double clicking
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::recalculatePositions()
    {
        if (!m_fontCached)
            return;

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);

        // Calculate the position of the text objects
        m_selectionRects.clear();
        m_textBeforeSelection.setPosition({textOffset, 0});
        if (m_selStart != m_selEnd)
        {
            auto selectionStart = m_selStart;
            auto selectionEnd = m_selEnd;

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                std::swap(selectionStart, selectionEnd);

            float kerningSelectionStart = 0;
            if ((selectionStart.x > 0) && (selectionStart.x < m_lines[selectionStart.y].length()))
                kerningSelectionStart = m_fontCached.getKerning(m_lines[selectionStart.y][selectionStart.x-1], m_lines[selectionStart.y][selectionStart.x], m_textSize);

            float kerningSelectionEnd = 0;
            if ((selectionEnd.x > 0) && (selectionEnd.x < m_lines[selectionEnd.y].length()))
                kerningSelectionEnd = m_fontCached.getKerning(m_lines[selectionEnd.y][selectionEnd.x-1], m_lines[selectionEnd.y][selectionEnd.x], m_textSize);

            if (selectionStart.x > 0)
            {
                m_textSelection1.setPosition({textOffset + m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().length()).x + kerningSelectionStart,
                                              m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});
            }
            else
                m_textSelection1.setPosition({textOffset, m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});

            m_textSelection2.setPosition({textOffset, static_cast<float>((selectionStart.y + 1) * m_lineHeight)});

            if (!m_textSelection2.getString().empty() || (selectionEnd.x == 0))
            {
                m_textAfterSelection1.setPosition({textOffset + m_textSelection2.findCharacterPos(m_textSelection2.getString().length()).x + kerningSelectionEnd,
                                                   m_textSelection2.getPosition().y + ((selectionEnd.y - selectionStart.y - 1) * m_lineHeight)});
            }
            else
                m_textAfterSelection1.setPosition({m_textSelection1.getPosition().x + m_textSelection1.findCharacterPos(m_textSelection1.getString().length()).x + kerningSelectionEnd,
                                                   m_textSelection1.getPosition().y});

            m_textAfterSelection2.setPosition({textOffset, static_cast<float>((selectionEnd.y + 1) * m_lineHeight)});

            // Recalculate the selection rectangles
            {
                m_selectionRects.push_back({m_textSelection1.getPosition().x, static_cast<float>(selectionStart.y * m_lineHeight), 0, static_cast<float>(m_lineHeight)});

                if (!m_lines[selectionStart.y].empty())
                {
                    m_selectionRects.back().width = m_textSelection1.findCharacterPos(m_textSelection1.getString().length()).x;

                    // There is kerning when the selection is on just this line
                    if (selectionStart.y == selectionEnd.y)
                        m_selectionRects.back().width += kerningSelectionEnd;
                }

                /// TODO: We should be able to do this without creating sf::Text object

                sf::Text tempText{"", *m_fontCached.getFont(), getTextSize()};
                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                {
                    m_selectionRects.back().width += textOffset;
                    m_selectionRects.push_back({m_textSelection2.getPosition().x - textOffset, static_cast<float>(i * m_lineHeight), textOffset, static_cast<float>(m_lineHeight)});

                    if (!m_lines[i].empty())
                    {
                        tempText.setString(sf::String(reinterpret_cast<const sf::Uint32*>(m_lines[i].c_str())));
                        m_selectionRects.back().width += tempText.findCharacterPos(tempText.getString().getSize()).x;
                    }
                }

                if (selectionStart.y != selectionEnd.y)
                {
                    m_selectionRects.back().width += textOffset;

                    if (m_textSelection2.getString() != U"")
                    {
                        tempText.setString(sf::String(reinterpret_cast<const sf::Uint32*>(m_lines[selectionEnd.y].substr(0, selectionEnd.x).c_str())));
                        m_selectionRects.push_back({m_textSelection2.getPosition().x - textOffset, static_cast<float>(selectionEnd.y * m_lineHeight),
                                                    textOffset + tempText.findCharacterPos(tempText.getString().getSize()).x + kerningSelectionEnd, static_cast<float>(m_lineHeight)});
                    }
                    else
                        m_selectionRects.push_back({0, static_cast<float>(selectionEnd.y * m_lineHeight), textOffset, static_cast<float>(m_lineHeight)});
                }
            }
        }

        recalculateVisibleLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::recalculateVisibleLines()
    {
        if (m_lineHeight == 0)
            return;

        float horiScrollOffset = 0.0f;
        if (m_horizontalScrollbar->isShown())
        {
            horiScrollOffset = m_horizontalScrollbar->getSize().y;
            m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), getSize().y - m_bordersCached.getBottom() - m_horizontalScrollbar->getSize().y);
        }

        m_visibleLines = std::min(static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - horiScrollOffset) / m_lineHeight), m_lines.size());

        // Store which area is visible
        if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->setPosition({getSize().x - m_bordersCached.getRight() - m_verticalScrollbar->getSize().x, m_bordersCached.getTop()});

            m_topLine = m_verticalScrollbar->getValue() / m_lineHeight;

            // The scrollbar may be standing between lines in which case one more line is visible
            if (((static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - horiScrollOffset) % m_lineHeight) != 0) || ((m_verticalScrollbar->getValue() % m_lineHeight) != 0))
                m_visibleLines++;
        }
        else // There is no scrollbar
        {
            m_topLine = 0;
            m_visibleLines = std::min(static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - horiScrollOffset) / m_lineHeight), m_lines.size());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TextBox::getSignal(String signalName)
    {
        if (signalName == onTextChange.getName().toLower())
            return onTextChange;
        else if (signalName == onSelectionChange.getName().toLower())
            return onSelectionChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::rendererChanged(const String& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "textcolor")
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection1.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection2.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "selectedtextcolor")
        {
            m_textSelection1.setColor(getSharedRenderer()->getSelectedTextColor());
            m_textSelection2.setColor(getSharedRenderer()->getSelectedTextColor());
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (!getSharedRenderer()->getScrollbarWidth())
            {
                m_verticalScrollbar->setSize({m_verticalScrollbar->getDefaultWidth(), m_verticalScrollbar->getSize().y});
                m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, m_horizontalScrollbar->getDefaultWidth()});
                setSize(m_size);
            }
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
            setSize(m_size);
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "selectedtextbackgroundcolor")
        {
            m_selectedTextBackgroundColorCached = getSharedRenderer()->getSelectedTextBackgroundColor();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "caretcolor")
        {
            m_caretColorCached = getSharedRenderer()->getCaretColor();
        }
        else if (property == "caretwidth")
        {
            m_caretWidthCached = getSharedRenderer()->getCaretWidth();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_verticalScrollbar->setInheritedOpacity(m_opacityCached);
            m_horizontalScrollbar->setInheritedOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
            m_textBeforeSelection.setOpacity(m_opacityCached);
            m_textAfterSelection1.setOpacity(m_opacityCached);
            m_textAfterSelection2.setOpacity(m_opacityCached);
            m_textSelection1.setOpacity(m_opacityCached);
            m_textSelection2.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setFont(m_fontCached);
            m_textSelection1.setFont(m_fontCached);
            m_textSelection2.setFont(m_fontCached);
            m_textAfterSelection1.setFont(m_fontCached);
            m_textAfterSelection2.setFont(m_fontCached);
            setTextSize(getTextSize());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TextBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(String(m_text)));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_textSize));
        node->propertyValuePairs["MaximumCharacters"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maxChars));

        if (m_readOnly)
            node->propertyValuePairs["ReadOnly"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_verticalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_verticalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }
        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(node->propertyValuePairs["textsize"]->value.toInt());
        if (node->propertyValuePairs["maximumcharacters"])
            setMaximumCharacters(node->propertyValuePairs["maximumcharacters"]->value.toInt());
        if (node->propertyValuePairs["readonly"])
            setReadOnly(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["readonly"]->value).getBool());

        if (node->propertyValuePairs["verticalscrollbarpolicy"])
        {
            String policy = node->propertyValuePairs["verticalscrollbarpolicy"]->value.trim().toLower();
            if (policy == "automatic")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == "always")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == "never")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{"Failed to parse VerticalScrollbarPolicy property, found unknown value."};
        }

        if (node->propertyValuePairs["horizontalscrollbarpolicy"])
        {
            String policy = node->propertyValuePairs["horizontalscrollbarpolicy"]->value.trim().toLower();
            if (policy == "automatic")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == "always")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == "never")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{"Failed to parse HorizontalScrollbarPolicy property, found unknown value."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const sf::RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Draw the contents of the text box
        {
            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

            float clipWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                clipWidth -= m_verticalScrollbar->getSize().x;

            float clipHeight = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();
            if (m_horizontalScrollbar->isShown())
                clipHeight -= m_horizontalScrollbar->getSize().y;

            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            const Clipping clipping{target, states, {}, {clipWidth, clipHeight}};

            // Move the text according to the scrollars
            states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -static_cast<float>(m_verticalScrollbar->getValue())});

            // Draw the background of the selected text
            for (const auto& selectionRect : m_selectionRects)
            {
                states.transform.translate({selectionRect.left, selectionRect.top});
                drawRectangleShape(target, states, {selectionRect.width, selectionRect.height + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)}, m_selectedTextBackgroundColorCached);
                states.transform.translate({-selectionRect.left, -selectionRect.top});
            }

            // Draw the text
            m_textBeforeSelection.draw(target, states);
            if (m_selStart != m_selEnd)
            {
                m_textSelection1.draw(target, states);
                m_textSelection2.draw(target, states);
                m_textAfterSelection1.draw(target, states);
                m_textAfterSelection2.draw(target, states);
            }

            // Only draw the caret when needed
            if (m_focused && m_caretVisible && (m_caretWidthCached > 0))
            {
                const float caretHeight = m_lineHeight + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize);
                states.transform.translate({std::ceil(m_caretPosition.x - (m_caretWidthCached / 2.f)), m_caretPosition.y});
                drawRectangleShape(target, states, {m_caretWidthCached, caretHeight}, m_caretColorCached);
            }
        }

        // Draw the scrollbars if needed
        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->draw(target, statesForScrollbar);

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->draw(target, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
