#include <iostream>
#include <vector>
#include <functional>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <thread>
#include <memory>
#include <mutex>
#include <bcrypt.h>
#include <SQLAPI.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "odbc32.lib")

using namespace std;

class ChatServer {
public:
    ChatServer() {

        try {
            con.Connect(_TSA("kurstest"), _TSA("kseroff"), _TSA("1111"), SA_PostgreSQL_Client);
        }
        catch (const SAException& ex) {
            cerr << ex.ErrText().GetMultiByteChars() << endl;
        }

        if (!con.isAlive()) {
            cerr << "Не удалось утсановить связь с базой данных" << endl;
            return;
        }

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Не удалось инициализировать winsock" << endl;
            return;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            cerr << "Не удалось создать сокет" << endl;
            WSACleanup();
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(777);  // Порт для прослушивания

        if (::bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == (SOCKET_ERROR)) {
            cerr << "Не удалось привязать сокет" << endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            cerr << "Прослушать не удалось" << endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        cout << "Сервер запущен (port: 777)" << endl;
    }

    ~ChatServer() {
        closesocket(serverSocket);
        con.Disconnect();
        WSACleanup();
    }

    void Run() {
        while (true) {
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                cerr << "Принять не удалось" << endl;
                return;
            }
            thread clientThread(&ChatServer::HandleClient, this, clientSocket);
            clientThread.detach();
        }
    }



private:
    WSADATA wsaData;
    SOCKET serverSocket;
    SAConnection con;

    mutex chatMutex;

    struct ClientInfo {
        SOCKET socket;
        string firstName;
        string lastName;
        string username;
    };

    vector<ClientInfo> clients;

    struct Message {
        string sender;
        string recipient;
        string text;
    };

    void HandleClient(SOCKET clientSocket) {
        while (true) {

            try {
                // Получаем информацию о пользователе от клиента
                string userInfo = ReceiveMessage(clientSocket);
                if (userInfo.empty()) {
                    cout << "error" << endl;
                }

                // Разбиваем информацию о пользователе на действие и параметры
                istringstream iss(userInfo);
                string action, username, count1, count2, count3;

                getline(iss, action, '\n');
                getline(iss, username, '\n');
                getline(iss, count1, '\n');
                getline(iss, count2, '\n');
                getline(iss, count3, '\n');

                // Выполняем соответствующее действие
                if (action == "register") {
                    SendMessage("", clientSocket, RegisterUser(username, count1, count2, count3) ? "true" : "false");
                    // login, password, name, surmane;
                }
                else if (action == "login") {
                    SendMessage("", clientSocket, AuthenticateUser(username, count1) ? "true" : "false");
                    // login, password;
                }
                else if (action == "Connect") {
                    AddClientToOnlineList(clientSocket, username);
                    // login;
                }
                else if (action == "GetInfo") {
                    SendMessage("", clientSocket, GetUserInfo(username));
                    // login;
                }
                else if (action == "Disconnect") {
                    // Обработка выхода пользователя из приложения
                    RemoveClientFromOnlineList(clientSocket);
                    closesocket(clientSocket);
                    cout << "User " << username << " logged out." << endl;
                    break;
                }
                else {
                    cout << "unknown action" << endl;
                    break;
                }

            }
            catch (const exception& ex) {
                cerr << "Error handling client: " << ex.what() << endl;
                closesocket(clientSocket);
            }

        }
    }

    // Отправка сообщения
    void SendMessage(const string& action, SOCKET clientSocket, const string& message) {
        string sendmessage;
        if (action != "") {
            sendmessage = (action + "|");
        }
        sendmessage += message;
        send(clientSocket, sendmessage.c_str(), sendmessage.length(), 0);
    }

    bool RegisterUser(const string& username, const string& password, const string& name, const string& surname) {
        try {
            SACommand cmd(&con);
            cmd.setCommandText(_TSA("SELECT password FROM users WHERE username = :1"));
            cmd << username.c_str();
            cmd.Execute();
            if (cmd.FetchNext()) {
                cout << "Registration for user " << username << " canceled." << endl;
                return false;
            }

            cout << "Подтвердить регистрацию " << username << " (1/0)" << endl;
            bool yn; cin >> yn;
            if (yn) {
                // Генерация случайной соли
                string salt = GenerateSalt();

                // Хэширование пароля с солью
                string hashedPassword = HashPasswordWithSalt(password, salt);

                // Выполнение запроса на регистрацию
                cmd.setCommandText(_TSA("INSERT INTO users (username, password, salt, name, surmane) VALUES (:1, :2, :3, :4, :5)"));
                cmd << username.c_str() << hashedPassword.c_str() << salt.c_str() << name.c_str() << surname.c_str();
                cmd.Execute();

                cout << "User " << username << " registered successfully!" << endl;
                return true;
            }
            else
                cout << "Registration for user " << username << " canceled." << endl;
            return false;
        }
        catch (const SAException& ex) {
            cerr << ex.ErrText().GetMultiByteChars() << endl;
            return false;
        }
    }

    bool AuthenticateUser(const string& username, const string& password) {
        try {

            for (const auto& client : clients) {
                if (client.username == username) {
                    cout << "user " << username << " already online" << endl;
                    return false;
                }
            }

            SACommand cmd(&con);
            cmd.setCommandText(_TSA("SELECT salt FROM users WHERE username = :1"));
            cmd << username.c_str();
            cmd.Execute();

            string salt;
            if (cmd.FetchNext()) {
                SAField& saltField = cmd.Field(_TSA("salt"));
                salt = saltField.asString().GetMultiByteChars();
            }

            if (!salt.empty()) {
                string hashedPassword = HashPasswordWithSalt(password, salt);

                cmd.setCommandText(_TSA("SELECT password FROM users WHERE username = :1 AND password = :2"));
                cmd << username.c_str() << hashedPassword.c_str();
                cmd.Execute();

                bool authenticationResult = cmd.FetchNext();

                if (authenticationResult) {
                    cout << "User " << username << " authenticated successfully!" << endl;
                }
                else {
                    cout << "Authentication failed for user " << username << "." << endl;
                }

                return authenticationResult;
            }

            cout << "User " << username << " not found." << endl;
            return false;
        }
        catch (const SAException& ex) {
            cerr << ex.ErrText().GetMultiByteChars() << endl;
            return false;
        }
    }

    string GenerateSalt() {
        // Генерация случайной соли с использованием hash
        hash<string> hasher;
        string salt = to_string(hasher(to_string(rand())));

        return salt;
    }

    string HashPasswordWithSalt(const string& password, const string& salt) {
        // Хэширование пароля с солью
        string combined = password + salt;
        hash<string> hasher;
        string hashedPassword = to_string(hasher(combined));

        return hashedPassword;
    }

    void AddClientToOnlineList(SOCKET clientSocket, const string& username) {
        lock_guard<mutex> lock(mutex);

        ClientInfo newClient;
        newClient.socket = clientSocket;
        newClient.username = username;
        cout << "User " << username << " online!" << endl;

        clients.push_back(newClient);

        // Отправить сообщение всем клиентам об обновлении списка онлайн
        BroadcastOnlineUsers();
    }

    void RemoveClientFromOnlineList(SOCKET clientSocket) {
        lock_guard<mutex> lock(mutex);

        auto it = find_if(clients.begin(), clients.end(),
            [clientSocket](const ClientInfo& c) { return c.socket == clientSocket; });

        if (it != clients.end()) {
            clients.erase(it);

            // Отправить сообщение всем клиентам об обновлении списка онлайн
            BroadcastOnlineUsers();
        }
    }


    void BroadcastOnlineUsers() {
        stringstream onlineUsers;
        for (const auto& client : clients) {
            onlineUsers << client.username << "|";
        }
        string onlresult = onlineUsers.str();

        for (const auto& client : clients) {
            SendMessage("OnlineUsers", client.socket, onlresult);
        }
    }

    string GetUserInfo(const string& username) {
        try {
            // Выполнение запроса на получение имени и фамилии
            SACommand cmd(&con);
            cmd.setCommandText(_TSA("SELECT name, surmane FROM users WHERE username = :1"));
            cmd << username.c_str();
            cmd.Execute();

            // Получение данных из результата запроса
            string userInfo;
            if (cmd.FetchNext()) {
                SAField& nameField = cmd.Field(_TSA("name"));
                SAField& surnameField = cmd.Field(_TSA("surmane"));
                string name = nameField.asString().GetMultiByteChars();
                string surname = surnameField.asString().GetMultiByteChars();
                userInfo = name + " " + surname;
            }

            return userInfo;
        }
        catch (const SAException& ex) {
            cerr << "Error retrieving user info: " << ex.ErrText().GetMultiByteChars() << endl;
            return "";
        }
    }

    // Получить сообщениe
    string ReceiveMessage(SOCKET clientSocket) {
        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            return string(buffer);
        }
        return "";
    }

};

int main() {
    srand(time(NULL));
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    ChatServer server;
    server.Run();

    return 0;
}
