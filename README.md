**Introduction:**

This code implements a real-time Cafe / Restaurant Order Management System using C++, Crow (web framework), SQLite, Mustache templates, and WebSockets.

It supports:

- Admin product management
- Order placement
- Subgroup-based kitchen sections
- Live order status updates for customers and staff

**Tools & Technologies used:**

- Core Programming Language: C++
- Web Framework: Crow (C++ web framework similar to flask)
- Database: Sqlite3
- Templating Engine: Mustache
- Live updates: Websockets (server code written in crow cpp)
- Views: HTML, CSS and js (for interfacing with websockets etc)

**System Architecture - 3 Tier Web Architecture**

**Presentation Layer**

- HTML templates (clientOrder.html, adminView.html etc)
- Mustache rendering (provided by crow)

**Application Layer**

- HTTP routes (Crow)
- Placing Orders
- Products (add, update, remove and edit)
- Authentication (admin only)
- WebSockets for live updates

**Data Layer**

SQLite database which includes the following tables:

- Products
- Orders
- OrderItems

**Modules**

**main.cpp**

This contains all the crow code at one place. It contains logic for handling request at all the routes defined in the application as well as the websocket code for updating the clients and sub-sections in the café when an order gets completed or a new one is added respectively.

Handles:

- Routing (admin, client, orders)
- Authentication via cookies (for admin with a hardcoded password)
- Websocket connections (freeing dynamic memory on connection close etc as well)
- Request validation and redirection (such as when a user tries to view admin page)

**sqlite_helper.cpp**

This provides all the functions for communicating with the sqlite database. The crow code therefore is simplified because it just has to call these functions with the expected data as input. Includes functions such as:

- Order getOrderById(sqlite3\* db, int order_id);
- void deleteProduct(sqlite3\* db, int id);

Handles:

- Database connection
- Table creation
- CRUD operations
- Status updates for orders, aggregation/sums
- Some core logic (fetching the earliest pending order)

**templates/\* (directory)**

This contains all the html mustache temples aka views for clients, admin as well as for sub-sections of café for processing orders. These pages are then rendered by the crow -> mustache using the data retrieved using sqlite_helper.cpp by passing in the corresponding inputs from request data.

Includes files:

- addData.html
- adminOrder.html
- adminView.html
- auth.html
- clientOrder.html
- …

**Implemented Features:**

**Admin Features**

- Login authentication with session cookies
- Add, edit, delete, and view products
- View orders by subgroup (e.g., kitchen sections)

**Product Management**

- Persistent storage in SQLite
- Subgroup-based classification
- Real-time availability updates

**Order Management**

- Create customer orders
- Assign products to orders
- Track quantities and preparation status
- FIFO (oldest-first) order handling (logic inside sqlite_helper.cpp)

**Real-Time Updates**

- WebSocket-based live communication

Separate channels (keys in the connection map):

- s:&lt;subgroup&gt; for kitchen section
- o:&lt;order_id&gt; for customer
- Order progress notifications (by updating the quantity prepared on client side)

**Customer Experience**

- Order placement
- Live order tracking page (where quantity count updates as the items are made)
- Automatic updates without page refresh (due to websockets)

**Key Concepts Applied**

- Dynamic memory allocation and deletion
- Type conversions
- Functions and arguments
- Pointers are everywhere
- Dictionary like structures (called maps in cpp)
- Prepared SQL statements
- Session management via cookies
- WebSockets for updating on events instead of polling (making requests again and again and wasting resources)
- FIFO queue logic for orders
- Aggregation queries (SUM, GROUP BY)

**Market Value**

**This system is suitable for:**

- Cafés
- University canteens
- Small restaurants

**It helps in various scenarios such as:**

- Order confusion where the chef doesn't know who to serve the order first. The person who ordered late sometimes get the order first which is unfair.
- It gets rid of manual coordination as order IDs, order date and time and who the order belongs to are handled by the system without doing anything manually.
- Clients don't have to wait for unnecessarily long periods because the system itself assigns who gets the order first (FIFO ordering just like a queue).

**Team Responsibilities**

**HAMMAD:**

- Crow routing (GET/POST requests, retrieving data from requests, parsing it and passing it to sqlite functions)

- Authentication logic, setting Cookies by hand, parsing them, and validating them.

- Websocket logic, creating connections, handling their lifecycle, and sending updates to the appropriate ones.

- Bridge between database and templates.

**HAMZA**

- SQLite schema, all three tables and definition of their attributes.

- All the query logic in isolation, error handling, safe input and includes logging as well.

- Database connection and functions for initializing db with the predefined schema.

- Interface (functions with parameters) for interacting with the database in abstraction.

**JAWAD**

- Mustache templates for rendering views.

- CSS for all the styling and appealing designs.

- Javascript code for connecting with websockets, listening to events etc.

- Dynamic UI changes as messages are received over the websocket connection

- UI integration (using placeholders such as {{name}} for real data)