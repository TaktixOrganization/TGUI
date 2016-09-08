/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_LIST_BOX_HPP
#define TGUI_LIST_BOX_HPP


#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Renderers/ListBoxRenderer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief List box widget
    ///
    /// Signals:
    ///     - ItemSelected (a new item was selected)
    ///         * Optional parameter sf::String: Name of the item (the text that is visible)
    ///         * Optional parameters sf::String and sf::String: Name and id of the item
    ///         * Uses Callback member 'text' and 'itemId'
    ///
    ///     - MousePressed (left mouse went down on top of an item
    ///         * Optional parameter sf::String: Name of the item (the text that is visible)
    ///         * Optional parameters sf::String and sf::String: Name and id of the item
    ///         * Uses Callback member 'text' and 'itemId'
    ///
    ///     - MouseReleased (left mouse went up on top of an item after it went down on of the items of the list box)
    ///         * Optional parameter sf::String: Name of the item (the text that is visible)
    ///         * Optional parameters sf::String and sf::String: Name and id of the item
    ///         * Uses Callback member 'text' and 'itemId'
    ///
    ///     - DoubleClicked (double clicked on an item with the left mouse button)
    ///         * Optional parameter sf::String: Name of the item (the text that is visible)
    ///         * Optional parameters sf::String and sf::String: Name and id of the item
    ///         * Uses Callback member 'text' and 'itemId'
    ///
    ///     - Inherited signals from Widget
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API ListBox : public Widget
    {
      public:

        typedef std::shared_ptr<ListBox> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const ListBox> ConstPtr; ///< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ListBox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another list box
        ///
        /// @param listBox  The other list box
        ///
        /// @return The new list box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static ListBox::Ptr copy(ListBox::ConstPtr listBox);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        ///
        /// @return Temporary pointer to the renderer
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ListBoxRenderer* getRenderer() const
        {
            return aurora::downcast<ListBoxRenderer*>(m_renderer.get());
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the position of the widget
        ///
        /// @param position  New position
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setPosition(const Layout2d& position) override;
        using Transformable::setPosition;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the list box
        ///
        /// @param size  The new size of the list box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using Transformable::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds an item to the list
        ///
        /// @param itemName  The name of the item you want to add (this is the text that will be displayed inside the list box)
        /// @param id        Optional unique id given to this item for the purpose to later identifying this item
        ///
        /// @return
        ///         - true when the item was successfully added
        ///         - false when the list box is full (you have set a maximum item limit and you are trying to add more items)
        ///         - false when there is no scrollbar and you try to have more items than fit inside the list box
        ///
        /// @see setMaximumItems
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool addItem(const sf::String& itemName, const sf::String& id = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Selects an item in the list box
        ///
        /// @param itemName  The item you want to select
        ///
        /// In case the names are not unique, the first item with that name will be selected.
        ///
        /// @return
        ///         - true on success
        ///         - false when none of the items matches the name
        ///
        /// @see setSelectedItemById
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setSelectedItem(const sf::String& itemName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Selects an item in the list box
        ///
        /// @param id  Unique id passed to addItem
        ///
        /// In case the id would not be unique, the first item with that id will be selected.
        ///
        /// @return
        ///         - true on success
        ///         - false when none of the items has the given id
        ///
        /// @see setSelectedItem
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setSelectedItemById(const sf::String& id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Selects an item in the list box
        ///
        /// @param index  Index of the item in the list box
        ///
        /// @return
        ///         - true on success
        ///         - false when the index was too high
        ///
        /// @see setSelectedItem
        /// @see setSelectedItemById
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setSelectedItemByIndex(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Deselects the selected item
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void deselectItem();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the item from the list with the given name
        ///
        /// @param itemName  The item to remove
        ///
        /// In case the names are not unique, only the first item with that name will be removed.
        ///
        /// @return
        ///        - true when the item was removed
        ///        - false when the name did not match any item
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeItem(const sf::String& itemName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the item that were added with the given id
        ///
        /// @param id  Id that was given to the addItem function
        ///
        /// In case the id is not unique, only the first item with that id will be removed.
        ///
        /// @return
        ///        - true when the item was removed
        ///        - false when there was no item with the given id
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeItemById(const sf::String& id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the item from the list box
        ///
        /// @param index  Index of the item in the list box
        ///
        /// @return
        ///        - true when the item was removed
        ///        - false when the index was too high
        ///
        /// @see removeItem
        /// @see removeItemById
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeItemByIndex(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes all items from the list
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllItems();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the item name of the item with the given id
        ///
        /// @param id  The id of the item that was given to it when it was added
        ///
        /// In case the id is not unique, the first item with that id will be returned.
        ///
        /// @return The requested item, or an empty string when no item matches the id
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::String getItemById(const sf::String& id) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the currently selected item
        ///
        /// @return The selected item
        ///         When no item was selected then this function will return an empty string.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::String getSelectedItem() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the id of the selected item
        ///
        /// @return The id of the selected item, which was the optional id passed to the addItem function.
        ///         When no item was selected then this function returns an empty string
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::String getSelectedItemId() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the index of the selected item
        ///
        /// @return The index of the selected item, or -1 when no item was selected
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int getSelectedItemIndex() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes an item with name originalValue to newValue
        ///
        /// @param originalValue The name of the item which you want to change
        /// @param newValue      The new name for that item
        ///
        /// In case the names are not unique, only the first item with that name will be changed.
        ///
        /// @return
        ///        - true when the item was changed
        ///        - false when none of the items had the given name
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool changeItem(const sf::String& originalValue, const sf::String& newValue);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the name of an item with the given id to newValue
        ///
        /// @param id       The unique id of the item which you want to change
        /// @param newValue The new name for that item
        ///
        /// In case the id is not unique, only the first item with that id will be changed.
        ///
        /// @return
        ///        - true when the item was changed
        ///        - false when none of the items had the given id
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool changeItemById(const sf::String& id, const sf::String& newValue);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the name of an item at the given index to newValue
        ///
        /// @param index    The index of the item which you want to change
        /// @param newValue The new name for that item
        ///
        /// @return
        ///        - true when the item was changed
        ///        - false when the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool changeItemByIndex(std::size_t index, const sf::String& newValue);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the amount of items in the list box
        ///
        /// @return Number of items inside the list box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::size_t getItemCount() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a copy of the items in the list box
        ///
        /// @return items
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<sf::String> getItems() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a copy of the item ids in the list box
        ///
        /// @return item ids
        ///
        /// Items that were not given an id simply have an empty string as id.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const std::vector<sf::String>& getItemIds() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the height of the items in the list box
        ///
        /// @param itemHeight  The size of a single item in the list
        ///
        /// @warning When there is no scrollbar then the items will be removed when they no longer fit inside the list box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setItemHeight(unsigned int itemHeight);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the height of the items in the list box
        ///
        /// @return The item height
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getItemHeight() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the text size of the items
        ///
        /// @param textSize  The size size of the text
        ///
        /// This will not change the height that each item has. By default (or when passing 0 to this function) the text will
        /// be auto-sized to nicely fit inside this item height.
        ///
        /// @see setItemHeight
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextSize(unsigned int textSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the text size of the items
        ///
        /// @return The text size
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getTextSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the maximum items that the list box can contain
        ///
        /// @param maximumItems  The maximum items inside the list box.
        ///                      When the maximum is set to 0 then the limit will be disabled
        ///
        /// If no scrollbar was loaded then there is always a limitation because there will be a limited space for the items.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMaximumItems(std::size_t maximumItems = 0);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the maximum items that the list box can contain
        ///
        /// @return The maximum items inside the list box.
        ///         If the function returns 0 then there is no limit
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::size_t getMaximumItems() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes whether the list box scrolls to the bottom when a new item is added
        ///
        /// @param autoScroll  Should list box scroll to the bottom when new items are added?
        ///
        /// Auto scrolling is enabled by default.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setAutoScroll(bool autoScroll);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the maximum items that the list box can contain
        ///
        /// @return The maximum items inside the list box
        ///         If the function returns 0 then there is no limit.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool getAutoScroll() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool mouseOnWidget(sf::Vector2f pos) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void leftMousePressed(sf::Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void leftMouseReleased(sf::Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void mouseMoved(sf::Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void mouseWheelMoved(int delta, int x, int y) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void mouseNoLongerOnWidget() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void mouseNoLongerDown() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function called when one of the properties of the renderer is changed
        ///
        /// @param property  Lowercase name of the property that was changed
        /// @param value     New value of the property
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void rendererChanged(const std::string& property, ObjectConverter& value) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Returns the size without the borders
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::Vector2f getInnerSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update the colors and text style of the selected and hovered items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateSelectedAndHoveringItemColorsAndStyle();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update the color and text style of all the items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateItemColorsAndStyle();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update on which item the mouse is standing
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateHoveringItem(int item);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update which item is selected
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateSelectedItem(int item);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // This function is called every frame with the time passed since the last frame.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void update(sf::Time elapsedTime) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<ListBox>(*this);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        // This contains the different items in the list box
        std::vector<Text>       m_items;
        std::vector<sf::String> m_itemIds;

        // What is the index of the selected item?
        // This is also used by combo box, so it can't just be changed to a pointer!
        int m_selectedItem = -1;

        int m_hoveringItem = -1;

        // The size must be stored
        unsigned int m_itemHeight = 22;
        unsigned int m_requestedTextSize = 0;
        unsigned int m_textSize = 18;

        // This will store the maximum number of items in the list box (zero by default, meaning that there is no limit)
        std::size_t m_maxItems = 0;

        // When there are too many items a scrollbar will be shown
        ScrollbarChildWidget m_scroll;

        // Will be set to true after the first click, but gets reset to false when the second click does not occur soon after
        bool m_possibleDoubleClick = false;

        bool m_autoScroll = true;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_LIST_BOX_HPP
