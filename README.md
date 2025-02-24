
# 🍽️My Restaurant Manager

  

  **My Restaurant Manager** is an application that is designed to help restaurant owners streamline daily operations. It manages table reservations, orders, billing, stock, employees, and financial reports. All data is stored in JSON format for easy import/export.
  **My Restaurant Manager** uses C++, Qt 6(includes `QMultimedia` and `QChart`) and [nlohmann-json](https://github.com/nlohmann/json). 

  

## **✨Features**

-  **🗂️JSON Data File Configuration**: `Create` a new JSON File template for the program or `Open` an existing JSON file (must follow the template). Uses JSON to import/export all data in a single file.

-  **📋Table Management**: **View** and `manage` table availability and reservations.

-  **📲Reserve Table**: `Reserve` tables and `save` reservations in JSON file.

-  **🛒Order Processing**: `Track` customer orders and `update` their status from data.

-  **📃Billing System**: `Generate` customer bills and `store` billing details in JSON.

-  **📦Stock Management**: `Monitor` inventory and `update` stock levels.

-  **👥Employee Management**: `Store` employee data, roles, and schedules.

-  **📖Menu Editing**: `Add`, `update`, or `remove` menu items.

-  **🍽️Order Food**: `Process` food orders, **link** them to customer bills, and `update` stock accordingly.

-  **📊Financial Statements**: `Generate` daily, weekly, or monthly sales reports, including expenses.

-  **📈Business Analysis**: `Analyze` trends and performance to optimize operations.

---

## 🛠**Usage**
### ⚙  **First-Time Setup**
1. Launch `MyRestaurantManager.exe`.
2. The File Configuration window will pop up. If no `data.json` is detected, you can:

   -  Click `New` to create a new `data.json` file with default keys compatible with the program.

   -  Click `Open` to select an existing `data.json` file.

3. Once a valid `data.json` file is found (or created/opened), the `Continue` button appears to take you to the main UI.
   
---

### 🗂️ **JSON Data File Configuration**
This section opens the "File Configuration" window.

#### **Features**

- ##### **Status Text**: 
   - Displays whether the JSON file is present or missing at the default PATH.
     
     - If missing, it shows "Data not found".
       
     - If found, it shows "Data found at..." with the file location.

- ##### **Main Buttons**:
  - `New`: Creates a `data.json` file with default keys and updates the status text to "Data created at...".

  - `Open`: Lets you choose an existing `.json` file and updates the status text to "Data opened at...".

  - `Continue`: Appears after a data file is found, created, or opened. Clicking it leads to the main UI.

  - `Force Close`: Immediately exits the application when no `.json` file is found.

  - `Open File...`: Available in the main UI to reopen the File Configuration window.
 
---

### 📋 **Table Management**
The main UI displays 9 clickable table icons labeled "Table#1" to "Table#9" at the center.

#### **Features**

- ##### **Table Selection**:
  - Clicking a `Table` icon reveals more buttons at the bottom:
    - `Order Food`
      
    - `Open Table`
      
    - `Reserve`
      
    - `Check Bills` (located at the bottom right)

- ##### **Main Buttons**:
  - `Refresh`: Resets the UI, hiding the additional buttons.

  - `Open Table`: Opens the "Open Table" window.
    - Seats can be selected by clicking icons (1-4 seats) or manually entering a number if more than 4 seats are needed.
      
    - Clicking `OK` confirms the process, and the number of seats is shown on the table icon
      
  - `Change seat`: Opens the "Open Table" window.
    - Replaces `Open Table` once seats have been selected, allowing you to adjust the seating.

---

### 📲 **Reserve Table**
The `Reserve` button appears once a `Table` icon is selected.

#### **Features**

- ##### **Main Buttons**:
  - `Reserve`: Opens the "Reserve" window where you fill in:
    - `Table Number` (non-editable)
      
    - `Reservation Time`
      
    - `Customer Name`
      
    - `Phone Number`
      
    - Once confirmed, the reservation details (time, name, and phone number) appear on the table icon.
      
  - `Unreserve`: Replaces `Reserve` for a reserved table, allowing you to cancel the reservation.

---

### 📃 **Billing System**
The `Check Bills` button appears once a `Table` icon is selected.

#### **Features**

- ##### **Main Buttons**:
  - `Check Bills`: Displays a receipt for the selected table that includes:
    - Table number, current date and time
      
    - A list of orders with individual amounts, prices, and the total price.
      
  - `Confirm Payment`: Resets the table to an available state.
    - Replaces `Check Bills` after `Check Bills` is clicked.
   
---

### 📦 **Stock Management**
Accessed via the `Stocks` and `Order Stock` buttons in the main UI.

#### **Features**

- ##### **Main Buttons**:
  - `Stocks`: Opens the "Stock" window, which shows three columns:
    - `No.` (read-only)
      
    - `Name`
      
    - `Quantity`
      
    - Double clicking a cell to edit its content.
      
    - ###### **Feature Buttons**:
    
      - `Add`: Inserts a new row with an auto-incremented `No.`, default `Name` as "New Stock", and `Quantity` as "0".
        
      - `Delete`: Removes the selected row. Multiple rows can be deleted using standard selection methods (Ctrl/Shift).

      - `Save`: Saves the current table data to the JSON file. Data is saved when the window is reopened.

  - `Order Stock`: Opens the "Order Stock" window.
    - You can choose a stock item, set the `Quantity` and `Price` (using scroll or arrow controls).
 
    - ###### **Feature Buttons**:
      - `Add To Statement`: Applies and records the changes.
   
---

### 👥 **Employee Management**
Access this feature through the `Employee` button in the main UI.

#### **Features**

- ##### **Main Buttons**:
  - `Employee`: Opens the "Employee" window, displaying six columns:
    - `No.` (hidden header)
      
    - `Name`
      
    - `Age`
      
    - `Gender`
      
    - `Salary`
      
    - `Job`
      
    - Double clicking a cell to edit its content.
   
    - ###### **Feature Buttons**:
    
      - `Add`: Adds a new row with an auto-incremented `No.` and blank fields for the other columns.
    
      - `Delete`: Removes the row corresponding to the selected `No.` cell. Multiple rows can be deleted using (Ctrl/Shift).

      - `Save`: Saves all changes to the JSON file.

      - `Refresh`: Reverts the table to the last saved state.

---

### 📖 **Menu Editing**
This feature is available through the `Edit Menu` button in the main UI.

#### **Features**

- ##### **Main Buttons**:
  - `Edit Menu`: Opens the "Edit Menu" window.
    
    - `Refresh`: Reverts to the state of the last saved JSON file.
      
    - `Save`: Saves any changes made in the table.

- ##### **Edit Menu Window**
  This window is divided into two sections: one for the menu and one for the ingredients.
  
  - ###### **Edit Menu Table (Left Side)**:
    - Columns include:
      - `No.` (hidden header)
        
      - `Menu` (editable by double-clicking)
    
      - `Price` (editable via input, arrow buttons, or scrolling)

      - `Type` (selectable from categories such as `Dishes`, `Drinks`, or `Not Selected`)
        
      - `Ingredients` (read-only)
        
      - `Edit Ingredients` (opens the ingredients table)
          
    - ###### **Feature Buttons**:
  
      - `Add Menu`: Inserts a new row with default values: auto-incremented `No.`, blank `Menu`, `Price` and `Ingredients` as "0", and `Type` as "Not Selected."
          
      - `Delete Menu`: Deletes the selected row.

  - ###### **Edit Ingredients Table (Right Side)**:
    - Columns includes:
      - `No.` (hidden header)
        
      - `Ingredients` (editable; choose from a list)
        
      - `Quantity` (editable via input, arrow buttons, or scrolling)
        
      - `Del` (a delete icon to remove the row)
        
    - ###### **Feature Buttons**:
      - `Add Ingredients`: Adds a new row with an auto-incremented `No.`, default `Ingredients` as "-", and `Quantity` as "0.00".


---

### 🍽️ **Order Food**
`Order Food` button appears after a `table` icon is selected and is active only if the table is open.

#### **Features**
  
- ##### **Main Buttons**:
  - `Order Food`: Opens the `Order Food` window.
    - **First box**: Allows you to choose between `food` or `drink` items, with the `price` updating accordingly.
      
    - **Second box**: Lets you adjust the `quantity` using editing or scrolling.
      
    - **Third box**: Let you add an additional price for extra servings, such as additional spices or side dishes not included in the original menu.

---

### 📊 **Financial Statements**
Accessible via the `Statement` button in the main UI. Presents detailed financial data.

#### **Features**

- ##### **Main Buttons**:
  - `Statement`: Opens the `Statement` window, showing:
    - `Date`, `Time`, `Item`, `Quantity`, and `Price`
      
    - Prices are shown in **green** for `income` and **red** for `expenses`.
      
    - Two selection boxes above the "table" let you choose the `month` and `year`.
    
    - A toggle in the top-left allows you to view `all` entries or filter by `income` or `expense`.
      
---

### 📈 **Business Analysis**
Accessible via the `Analysis` button in the main UI.

#### **Features**
Shows the trend of `Income` and `Expenses` of the time range chosen. Includes summaries of the top `dishes` and `drinks`, `total income`, `expenses`, and `total order`.

- ##### **Main Buttons**:
  - `Analysis`: Opens the `Analysis` window.
    - Hover over any graph point/dot for detailed information.
      
    - Use the dual-calendar interface to select specific date ranges:
      - The left calendar is the primary selector.
        
      - The right calendar is used when viewing a specific `range`.
        
    - Change the displayed `month` and `year` by clicking on their names or using mouse scrolling, or arrow controls.
        
    - The leftmost column shows the week number of the year.
     
    - ###### **Two boxes allow you to adjust**:
      - **The first box** / `Detail Mode`: Choose between `Auto`, `Scale`, or `Not Scale`:
        - `Auto`: Automatically shows dots in `Scale` or `Not Scale` mode depending on how many days you are looking at. Normally is in `Not Scale` until there are more than 6 days.
          
        - `Scale`: Overview; Totals all dots in a day into one, which means all statements in a day are merged together.
          
        - `Not Scale`: More detailed; Manys dots in a day, which means each time a statement is made, it got a dot of its own. 
          
      - **The second box** / `Data Scope`: Filter data by `All`, `Range`, `Day`, `Week`, `Month`, `Year`:
          - `All`: Shows all entries (calendars are disabled).
            
          - `Range`: Displays entries within two selected dates. The left calendar is used as the starting point, the right is used as the ending point.
            
          - `Day`/`Week`/`Month`/`Year`: Limits the view to the specified time frame.
            
    - ###### **Feature Buttons**:
      - `Refresh`: Resets the selection to the current real-life date without changing the mode settings.
      
---
