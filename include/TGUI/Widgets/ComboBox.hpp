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


#ifndef TGUI_COMBO_BOX_HPP
#define TGUI_COMBO_BOX_HPP


#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Renderers/ComboBoxRenderer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Combo box widget
    ///
    /// Signals:
    ///     - ItemSelected (a new item was selected)
    ///         * Optional parameter sf::String: Name of the item (the text that is visible)
    ///         * Optional parameters sf::String and sf::String: Name and id of the item
    ///         * Uses Callback member 'text' and 'itemId'
    ///
    ///     - Inherited signals from Widget
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API ComboBox : public Widget
    {
    public:

        typedef std::shared_ptr<ComboBox> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const ComboBox> ConstPtr; ///< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBox();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBox(const ComboBox& other);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Move constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBox(ComboBox&& other);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBox& operator= (const ComboBox& other);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Move assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBox& operator= (ComboBox&& other);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another combo box
        ///
        /// @param comboBox  The other combo box
        ///
        /// @return The new combo box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static ComboBox::Ptr copy(ComboBox::ConstPtr comboBox);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        ///
        /// @return Temporary pointer to the renderer
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ComboBoxRenderer* getRenderer() const
        {
            return aurora::downcast<ComboBoxRenderer*>(m_renderer.get());
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the combo box
        ///
        /// This size does not include the borders.
        ///
        /// @param size   The new size of the combo box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using Transformable::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the number of items that are displayed in the list
        ///
        /// @param nrOfItemsInListToDisplay  The maximum number of items to display when the list of items is shown
        ///
        /// When there is no scrollbar then this is the maximum number of items.
        /// If there is one, then it will only become visible when there are more items than this number.
        ///
        /// When set to zero then all items are shown (then there will never be a scrollbar).
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setItemsToDisplay(std::size_t nrOfItemsInListToDisplay);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the number of items that are displayed in the list
        ///
        /// @return The maximum number of items to display when the list of items is shown
        ///
        /// When there is no scrollbar then this is the maximum number of items.
        /// If there is one, then it will only become visible when there are more items than this number.
        ///
        /// When set to zero then all items are shown (then there will never be a scrollbar).
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::size_t getItemsToDisplay() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds an item to the list, so that it can be selected later
        ///
        /// @param itemName  The name of the item you want to add (this is the text that will be displayed inside the combo box)
        /// @param id        Optional unique id given to this item for the purpose to later identifying this item
        ///
        /// @return
        ///         - true when the item when it was successfully added
        ///         - false when the combo box wasn't loaded correctly
        ///         - false when the list is full (you have set a maximum item limit and you are trying to add more items)
        ///         - false when there is no scrollbar and you try to have more items than the number of items to display
        ///
        /// @see setMaximumItems
        /// @see setItemsToDisplay
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool addItem(const sf::String& itemName, const sf::String& id = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Selects an item from the list
        ///
        /// When adding items to the combo box with the addItem function, none of them will be selected.
        /// If you don't want the combo box to stay empty until the user selects something, but you want a default item instead,
        /// then you can use this function to select an item.
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
        /// @brief Selects an item from the list
        ///
        /// When adding items to the combo box with the addItem function, none of them will be selected.
        /// If you don't want the combo box to stay empty until the user selects something, but you want a default item instead,
        /// then you can use this function to select an item.
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
        /// @brief Selects an item in the list
        ///
        /// @param index  Index of the item in the list
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
        /// The combo box will be empty after this function is called.
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
        /// @brief Removes the item from the list
        ///
        /// @param index  Index of the item in the list
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
        /// @return The selected item.
        ///         When no item was selected then this function will return an empty string
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::String getSelectedItem() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the id of the selected item
        ///
        /// @return The id of the selected item, which was the optional id passed to the addItem function.
        ///         When no item was selected then this function returns 0
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
        /// @brief Returns the amount of items in the combo box
        ///
        /// @return Number of items inside the combo box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::size_t getItemCount() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a copy of the items in the combo box
        ///
        /// @return items
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<sf::String> getItems();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a copy of the item ids in the combo box
        ///
        /// @return item ids
        ///
        /// Items that were not given an id simply have an empty string as id.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const std::vector<sf::String>& getItemIds();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the maximum items that the combo box can contain
        ///
        /// @param maximumItems  The maximum items inside the combo box.
        ///                      When the maximum is set to 0 then the limit will be disabled
        ///
        /// If no scrollbar was loaded then there is always a limitation because there will be a limited space for the items.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMaximumItems(std::size_t maximumItems = 0);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the maximum items that the combo box can contain
        ///
        /// @return The maximum items inside the list.
        ///         If the function returns 0 then there is no limit
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::size_t getMaximumItems() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the text size of the items
        ///
        /// @param textSize  The size size of the text
        ///
        /// By default (or when passing 0 to this function) the text will be auto-sized to nicely fit inside the combo box.
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
        /// @internal
        /// This function is called when the widget is added to a container.
        /// You should not call this function yourself.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setParent(Container* parent) override;


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
        virtual void mouseWheelMoved(int delta, int x, int y) override;


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
        // Update the height of the internal list box
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateListBoxHeight();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Shows the list of items.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void showListBox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Hides the list of items.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void hideListBox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Initialize the internal list box
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void initListBox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Called by the internal ListBox when a different item is selected.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void newItemSelectedCallbackFunction();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Called by the internal ListBox when it gets unfocused.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void listBoxUnfocusedCallbackFunction();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<ComboBox>(*this);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        // The number of items to display. If there is a scrollbar then you can scroll to see the other.
        // If there is no scrollbar then this will be the maximum amount of items.
        std::size_t m_nrOfItemsToDisplay = 0;

        // Internally a list box is used to store all items
        ListBox::Ptr m_listBox = std::make_shared<ListBox>();

        Text m_text;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_COMBO_BOX_HPP

