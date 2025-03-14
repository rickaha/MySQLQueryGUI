# MySQL Query Tool

A simple GUI application to query MySQL databases using Qt5/6 and MySQL
Connector/C++.

## Prerequisites

- Qt5/6 (https://www.qt.io/download)
  - You'll need to install a version of Qt that supports C++17 or later for this
    application.
- MySQL Connector/C++ (https://dev.mysql.com/downloads/connector/)
  - Download the binary package matching your distribution.

## Installation

1. Clone this repository:

```
   git clone https://github.com/rickaha/MySQLQueryGUI.git
   cd MySQLQueryGUI
```

2. Make sure you have a MySQL server running with a database created for testing
purposes.

3. Build and run the application using Qt's qmake and make tools:

```
   mkdir build
   cd build
   cmake ..
   make
   ./MySQLQueryTool
```

- Enter the connection details (host, port, username, password) and optionally
provide a database name to use.
- Click on "Connect" to establish a connection with your MySQL server.

## Features

- Connect/disconnect from MySQL servers.
- Execute SELECT, SHOW queries and display results in tabular format.
- Execute non-SELECT (INSERT, UPDATE, DELETE) queries and show affected rows
  count.
- Clear output when executing new queries or connecting to a new database.
