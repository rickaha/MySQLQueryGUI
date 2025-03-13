// A simple GUI application to query MySQL databases
// Requires: Qt5/6 and MySQL Connector/C++

#include <QtWidgets>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class MySQLQueryGUI : public QMainWindow {
private:
    // Database connection components
    sql::Driver *driver;
    sql::Connection *connection;
    bool isConnected;

    // Connection widgets
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QLineEdit *userLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *dbNameLineEdit;
    QPushButton *connectButton;
    QPushButton *disconnectButton;

    // Query widgets
    QTextEdit *queryTextEdit;
    QPushButton *executeButton;
    QTableView *resultTableView;
    QStandardItemModel *model;

public:
    MySQLQueryGUI() : isConnected(false), connection(nullptr) {
        setWindowTitle("MySQL Query Tool");
        resize(800, 600);

        // Create central widget and layout
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // Connection section
        QGroupBox *connectionGroup = new QGroupBox("Database Connection");
        QGridLayout *connectionLayout = new QGridLayout();

        connectionLayout->addWidget(new QLabel("Host:"), 0, 0);
        hostLineEdit = new QLineEdit("localhost");
        connectionLayout->addWidget(hostLineEdit, 0, 1);

        connectionLayout->addWidget(new QLabel("Port:"), 0, 2);
        portLineEdit = new QLineEdit("3306");
        connectionLayout->addWidget(portLineEdit, 0, 3);

        connectionLayout->addWidget(new QLabel("Username:"), 1, 0);
        userLineEdit = new QLineEdit("root");
        connectionLayout->addWidget(userLineEdit, 1, 1);

        connectionLayout->addWidget(new QLabel("Password:"), 1, 2);
        passwordLineEdit = new QLineEdit();
        passwordLineEdit->setEchoMode(QLineEdit::Password);
        connectionLayout->addWidget(passwordLineEdit, 1, 3);

        connectionLayout->addWidget(new QLabel("Database:"), 2, 0);
        dbNameLineEdit = new QLineEdit();
        connectionLayout->addWidget(dbNameLineEdit, 2, 1);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        connectButton = new QPushButton("Connect");
        disconnectButton = new QPushButton("Disconnect");
        disconnectButton->setEnabled(false);
        buttonLayout->addWidget(connectButton);
        buttonLayout->addWidget(disconnectButton);
        connectionLayout->addLayout(buttonLayout, 2, 2, 1, 2);

        connectionGroup->setLayout(connectionLayout);
        mainLayout->addWidget(connectionGroup);

        // Query section
        QGroupBox *queryGroup = new QGroupBox("Query");
        QVBoxLayout *queryLayout = new QVBoxLayout();

        queryTextEdit = new QTextEdit();
        queryTextEdit->setPlaceholderText("Enter your SQL query here...");
        queryLayout->addWidget(queryTextEdit);

        executeButton = new QPushButton("Execute Query");
        executeButton->setEnabled(false);
        queryLayout->addWidget(executeButton);

        queryGroup->setLayout(queryLayout);
        mainLayout->addWidget(queryGroup);

        // Results section
        QGroupBox *resultsGroup = new QGroupBox("Results");
        QVBoxLayout *resultsLayout = new QVBoxLayout();

        resultTableView = new QTableView();
        model = new QStandardItemModel(this);
        resultTableView->setModel(model);
        resultsLayout->addWidget(resultTableView);

        resultsGroup->setLayout(resultsLayout);
        mainLayout->addWidget(resultsGroup);

        // Connect signals to slots
        connect(connectButton, &QPushButton::clicked, this, &MySQLQueryGUI::connectToDatabase);
        connect(disconnectButton, &QPushButton::clicked, this, &MySQLQueryGUI::disconnectFromDatabase);
        connect(executeButton, &QPushButton::clicked, this, &MySQLQueryGUI::executeQuery);
    }

    ~MySQLQueryGUI() {
        if (connection) {
            connection->close();
            delete connection;
        }
    }

private slots:
    void connectToDatabase() {
        try {
            // Get connection parameters
            std::string host = hostLineEdit->text().toStdString();
            std::string port = portLineEdit->text().toStdString();
            std::string user = userLineEdit->text().toStdString();
            std::string password = passwordLineEdit->text().toStdString();
            std::string dbName = dbNameLineEdit->text().toStdString();

            // Create connection
            driver = get_driver_instance();

            std::string connString = "tcp://" + host + ":" + port;
            connection = driver->connect(connString, user, password);

            if (!dbName.empty()) {
                connection->setSchema(dbName);
            }

            // Update UI state
            isConnected = true;
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(true);
            executeButton->setEnabled(true);

            QMessageBox::information(this, "Connection Success", "Successfully connected to the database.");
        } catch (sql::SQLException &e) {
            QMessageBox::critical(this, "Connection Error",
                                  QString("Error connecting to MySQL database: %1\nError code: %2").arg(e.what()).arg(e.getErrorCode()));
        }
    }

    void disconnectFromDatabase() {
        if (connection) {
            connection->close();
            delete connection;
            connection = nullptr;
        }

        isConnected = false;
        connectButton->setEnabled(true);
        disconnectButton->setEnabled(false);
        executeButton->setEnabled(false);

        QMessageBox::information(this, "Disconnected", "Database connection closed.");
    }

    void executeQuery() {
        if (!isConnected || !connection) {
            QMessageBox::warning(this, "Not Connected", "Please connect to a database first.");
            return;
        }

        // Clear previous results
        model->clear();

        std::string query = queryTextEdit->toPlainText().trimmed().toStdString();
        if (query.empty()) {
            QMessageBox::warning(this, "Empty Query", "Please enter a SQL query.");
            return;
        }

        try {
            sql::Statement *stmt = connection->createStatement();

            // Check if it's a SELECT query
            bool isSelect = query.substr(0, 6) == "SELECT" ||
                           query.substr(0, 6) == "select" ||
                           query.substr(0, 4) == "SHOW" ||
                           query.substr(0, 4) == "show";

            if (isSelect) {
                // Execute SELECT query and display results
                sql::ResultSet *res = stmt->executeQuery(query);

                // Get column metadata
                sql::ResultSetMetaData *meta = res->getMetaData();
                int columnCount = meta->getColumnCount();

                // Set up column headers
                QStringList headers;
                for (int i = 1; i <= columnCount; i++) {
                    headers << QString::fromStdString(meta->getColumnName(i));
                }
                model->setHorizontalHeaderLabels(headers);

                // Populate data
                int row = 0;
                while (res->next()) {
                    for (int col = 1; col <= columnCount; col++) {
                        QString value = QString::fromStdString(res->getString(col));
                        QStandardItem *item = new QStandardItem(value);
                        model->setItem(row, col-1, item);
                    }
                    row++;
                }

                delete res;
                QMessageBox::information(this, "Query Success",
                                        QString("Query executed successfully. %1 rows returned.").arg(row));
            } else {
                // Execute non-SELECT query and show affected rows
                int affectedRows = stmt->executeUpdate(query);
                QMessageBox::information(this, "Query Success",
                                        QString("Query executed successfully. %1 rows affected.").arg(affectedRows));
            }

            delete stmt;
        } catch (sql::SQLException &e) {
            QMessageBox::critical(this, "Query Error",
                                  QString("Error executing query: %1\nError code: %2").arg(e.what()).arg(e.getErrorCode()));
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MySQLQueryGUI window;
    window.show();
    return app.exec();
}
