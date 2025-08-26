#pragma once
#include <string>
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>
#include <msclr/marshal.h>
#include <vector>
#include <list>
#include <thread>
#include <tuple>

namespace client {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Net;
    using namespace System::Net::Sockets;
    using namespace System::Text;
    using namespace System::Collections::Generic;
    using namespace System::Threading;
    using namespace System::Threading::Tasks;

    public ref class MyMenu : public System::Windows::Forms::Form
    {
    public:
        MyMenu(void)
        {
            InitializeComponent();
            chatBuffers = gcnew Dictionary<String^, ChatBuffer^>();
            onlineUsersList = gcnew List<String^>();
        }
        MyMenu(String^ senderName, TcpClient^ client, NetworkStream^ stream) : MyMenu()
        {
            username = senderName;
            tcpClientMenu = client;
            streamMenu = stream;

            // Отобразить информацию о пользователе
            labelUsername->Text = "Login: " + senderName;

            GetUserInfo(username);
            CreateGeneralChatPanel();
            RequestOnlineUsers();
        }

    protected:
        ~MyMenu()
        {
            SendLogoutMessage();
            if (components)
            {
                delete components;
            }
        }

        virtual void OnFormClosing(FormClosingEventArgs^ e) override
        {
            SendLogoutMessage();
            if (components)
            {
                delete components;
            }
            exit(0);
        }

    private:

        System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code

        System::Windows::Forms::Label^ labelFullName;
        System::Windows::Forms::Label^ labelUsername;
        System::Windows::Forms::TextBox^ messageTextBox;
        System::Windows::Forms::Button^ sendButton;
        System::Windows::Forms::Label^ label1;
        System::Windows::Forms::Panel^ panel1;
        System::Windows::Forms::FlowLayoutPanel^ onlineUsersPanel;

        void InitializeComponent(void) {
            this->labelFullName = (gcnew System::Windows::Forms::Label());
            this->labelUsername = (gcnew System::Windows::Forms::Label());
            this->onlineUsersPanel = (gcnew System::Windows::Forms::FlowLayoutPanel());
            this->messageTextBox = (gcnew System::Windows::Forms::TextBox());
            this->sendButton = (gcnew System::Windows::Forms::Button());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->panel1 = (gcnew System::Windows::Forms::Panel());
            this->SuspendLayout();
            // 
            // labelFullName
            // 
            this->labelFullName->AutoSize = true;
            this->labelFullName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12));
            this->labelFullName->Location = System::Drawing::Point(10, 9);
            this->labelFullName->Name = L"labelFullName";
            this->labelFullName->Size = System::Drawing::Size(100, 25);
            this->labelFullName->TabIndex = 0;
            this->labelFullName->Text = L"Full Name";
            // 
            // labelUsername
            // 
            this->labelUsername->AutoSize = true;
            this->labelUsername->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
            this->labelUsername->Location = System::Drawing::Point(12, 34);
            this->labelUsername->Name = L"labelUsername";
            this->labelUsername->Size = System::Drawing::Size(51, 17);
            this->labelUsername->TabIndex = 1;
            this->labelUsername->Text = L"Login: ";
            // 
            // onlineUsersPanel
            // 
            this->onlineUsersPanel->AutoScroll = true;
            this->onlineUsersPanel->Location = System::Drawing::Point(12, 54);
            this->onlineUsersPanel->Name = L"onlineUsersPanel";
            this->onlineUsersPanel->Size = System::Drawing::Size(200, 362);
            this->onlineUsersPanel->TabIndex = 2;
            // 
            // messageTextBox
            // 
            this->messageTextBox->Location = System::Drawing::Point(218, 394);
            this->messageTextBox->Name = L"messageTextBox";
            this->messageTextBox->Size = System::Drawing::Size(433, 22);
            this->messageTextBox->TabIndex = 4;
            // 
            // sendButton
            // 
            this->sendButton->Location = System::Drawing::Point(657, 393);
            this->sendButton->Name = L"sendButton";
            this->sendButton->Size = System::Drawing::Size(75, 23);
            this->sendButton->TabIndex = 6;
            this->sendButton->Text = L"Send";
            this->sendButton->UseVisualStyleBackColor = true;
            this->sendButton->Click += gcnew System::EventHandler(this, &MyMenu::sendButton_Click);
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12));
            this->label1->Location = System::Drawing::Point(225, 6);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(106, 25);
            this->label1->TabIndex = 7;
            this->label1->Text = L"NameChat";
            // 
            // panel1
            // 
            this->panel1->Location = System::Drawing::Point(222, 43);
            this->panel1->Name = L"messageContainer";
            this->panel1->Size = System::Drawing::Size(511, 348);
            this->panel1->TabIndex = 8;
            this->panel1->BorderStyle = BorderStyle::FixedSingle;
            this->panel1->AutoScroll = true;
            this->panel1->BackColor = Color::LightGray;
            // 
            // MyMenu
            // 
            this->ClientSize = System::Drawing::Size(740, 423);
            this->Controls->Add(this->panel1);
            this->Controls->Add(this->label1);
            this->Controls->Add(this->sendButton);
            this->Controls->Add(this->messageTextBox);
            this->Controls->Add(this->onlineUsersPanel);
            this->Controls->Add(this->labelUsername);
            this->Controls->Add(this->labelFullName);
            this->Name = L"MyMenu";
            this->Text = L"MyMenu";
            this->ResumeLayout(false);
            this->PerformLayout();

        }

#pragma endregion

        ref class UserInfo {
        public:
            String^ FirstName;
            String^ LastName;
            String^ Username;

            UserInfo(String^ firstName, String^ lastName, String^ username) {
                FirstName = firstName;
                LastName = lastName;
                Username = username;
            }
        };

        ref class ChatBuffer {
        public:
            String^ ChatName;
            List<Tuple<String^, String^>^>^ Messages; // Список кортежей (отправитель, сообщение)

            ChatBuffer(String^ chatName) {
                ChatName = chatName;
                Messages = gcnew List<Tuple<String^, String^>^>();
            }

            void AddMessage(String^ sender, String^ message) {
                Messages->Add(gcnew Tuple<String^, String^>(sender, message));
            }
        };

        TcpClient^ tcpClientMenu;
        NetworkStream^ streamMenu;
        String^ username;

        List<String^>^ onlineUsersList;
        array<String^>^ action;

        String^ activeChatUser;
        Dictionary<String^, ChatBuffer^>^ chatBuffers;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        void RequestOnlineUsers()
        {
            try {
                String^ request = "Connect";
                String^ credentialsMessage = String::Format("{0}\n{1}", request, username);

                array<Byte>^ data = Encoding::UTF8->GetBytes(credentialsMessage);
                streamMenu->Write(data, 0, data->Length);

                streamMenu->Flush();
                Task::Run(gcnew Action(this, &MyMenu::ListenForOnlineUsers));
            }
            catch (Exception^ e) {
                Console::WriteLine("Error requesting user info: " + e->Message);
            }
        }

        void SendLogoutMessage() {
            try {
                // Отправляем сообщение на сервер о выходе из онлайн
                String^ logoutMessage = String::Format("{0}\n{1}", "Disconnect", username);
                array<Byte>^ data = Encoding::UTF8->GetBytes(logoutMessage);
                streamMenu->Write(data, 0, data->Length);
            }
            catch (Exception^ e) {
                Console::WriteLine("Error sending logout message: " + e->Message);
            }
        }

        void CreateGeneralChatPanel() {
            Panel^ userPanel = gcnew Panel();
            userPanel->BorderStyle = BorderStyle::FixedSingle;
            userPanel->Width = onlineUsersPanel->Width - 14;
            userPanel->Height = 50;

            // Создаем и добавляем Label для имени и фамилии
            Label^ nameLabel = gcnew Label();
            nameLabel->Text = "General Chat";
            nameLabel->Font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10);
            nameLabel->Location = System::Drawing::Point(5, 5);
            nameLabel->AutoSize = true;

            // Создаем и добавляем Label для логина
            Label^ usernameLabel = gcnew Label();
            usernameLabel->Text = "Login: " + "|Chat|";
            usernameLabel->Font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8);
            usernameLabel->Location = System::Drawing::Point(5, 30);
            usernameLabel->AutoSize = true;
            usernameLabel->Visible = false;

            // Добавляем Label на панель
            userPanel->Controls->Add(nameLabel);
            userPanel->Controls->Add(usernameLabel);

            // Добавляем обработчик события для клика по панели
            userPanel->Click += gcnew EventHandler(this, &MyMenu::OpenChatButtonClick);

            // Добавляем панель на панель с онлайн пользователями
            onlineUsersPanel->Controls->Add(userPanel);

            if (!ChatBufferExists("|Chat|"))
                CreateBufferForChat("|Chat|");

            OpenChatButtonClick(userPanel, EventArgs::Empty);
        }

        //////////////////////////////////////////////////////////////////////////////////////////////

        void ListenForOnlineUsers()
        {
            try
            {
                array<Byte>^ responseBuffer = gcnew array<Byte>(1024);
                while (true)
                {
                    IAsyncResult^ result = streamMenu->BeginRead(responseBuffer, 0, responseBuffer->Length, nullptr, nullptr);
                    int bytesRead = streamMenu->EndRead(result);

                    if (bytesRead > 0)
                    {
                        String^ response = Encoding::UTF8->GetString(responseBuffer, 0, bytesRead);

                        action = response->Split(gcnew array<Char>{'|'}, StringSplitOptions::RemoveEmptyEntries);

                        // Check the received action
                        if (action[0] == "OnlineUsers")
                        {
                            String^ users;
                            for (int i = 1; i < action->Length; i++)
                            {
                                users += (action[i] + "|");
                            }
                            this->Invoke(gcnew Action<String^>(this, &MyMenu::UpdateOnlineUsers), users);
                        }
                        else if (action[0] == "SendMessag")
                        {
                            String^ recipient = action[1];
                            String^ message = action[2];

                            String^ bufer = (username + recipient);
                            if (!ChatBufferExists(bufer)) {
                                CreateBufferForChat(bufer);
                            }
                            AppendMessageToBuffer(bufer, recipient, message);

                            if (activeChatUser == recipient) {
                                this->Invoke(gcnew Action<String^, String^, bool>(this, &MyMenu::AppendMessage), recipient, message, false);
                            }
                            else {
                                this->Invoke(gcnew Action<String^>(this, &MyMenu::AddRedDotToUserPanel), recipient);
                            }
                        }
                        else if (action[0] == "SendMessageGeneralChat") {
                            String^ recipient = action[1];
                            String^ message = action[2];

                            AppendMessageToBuffer("|Chat|", recipient, message);

                            if (activeChatUser == "|Chat|") {
                                this->Invoke(gcnew Action<String^, String^, bool>(this, &MyMenu::AppendMessage), recipient, message, false);
                            }
                            else {
                                this->Invoke(gcnew Action<String^>(this, &MyMenu::AddRedDotToUserPanel), "|Chat|");
                            }
                        }
                    }
                    else
                    {
                        System::Threading::Thread::Sleep(100);
                    }
                }
            }
            catch (Exception^ e)
            {
                Console::WriteLine("Error receiving online users: " + e->Message);
            }
        }

        void AddRedDotToUserPanel(String^ username)
        {
            Panel^ userPanel = FindUserPanelByUsername(username);

            if (userPanel != nullptr)
            {
                // Проверяем, существует ли уже точка
                for each (Control ^ control in userPanel->Controls)
                {
                    if (control->Name == "redDot")
                        return; // Точка уже добавлена
                }

                // Создаем новую панель для точки
                Panel^ redDotPanel = gcnew Panel();
                redDotPanel->Name = "redDot";
                redDotPanel->BackColor = Color::Red;
                redDotPanel->Width = 8;
                redDotPanel->Height = 8;
                redDotPanel->Location = System::Drawing::Point(userPanel->Width - redDotPanel->Width - 10, 22); // Отступ 10px от верхнего края и 8px справа

                // Добавляем панель точки на панель пользователя
                userPanel->Controls->Add(redDotPanel);
            }
        }

        void RemoveRedDotFromUserPanel(Panel^ userPanel)
        {
            for each (Control ^ control in userPanel->Controls)
            {
                if (control->Name == "redDot")
                {
                    userPanel->Controls->Remove(control);
                    return;
                }
            }
        }

        void UpdateOnlineUsers(String^ response) {
            array<String^>^ newOnlineUsers = response->Split(gcnew array<Char>{'|'}, StringSplitOptions::RemoveEmptyEntries);

            // Удаляем панели пользователей, которых уже нет в новом списке
            for (int i = onlineUsersList->Count - 1; i >= 0; i--) {
                String^ existingUser = onlineUsersList[i];

                // Исключаем логин общего чата
                if (existingUser != "|Chat|") {
                    if (Array::IndexOf(newOnlineUsers, existingUser) == -1) {
                        onlineUsersList->RemoveAt(i);
                        RemoveUserPanel(existingUser);
                    }
                }
            }

            // Добавляем новые панели для пользователей, которые еще не отображены
            for each (String ^ newUser in newOnlineUsers) {
                if (newUser != username) {
                    if (FindUserPanelByUsername(newUser) == nullptr) {
                        CreatePanelForUser(newUser);
                        onlineUsersList->Add(newUser);
                    }
                }
            }
        }

        void RemoveUserPanel(String^ username) {
            for each (Control ^ control in onlineUsersPanel->Controls) {
                Panel^ userPanel = dynamic_cast<Panel^>(control);
                if (userPanel != nullptr) {
                    Label^ usernameLabel = dynamic_cast<Label^>(userPanel->Controls[1]);
                    if (usernameLabel != nullptr) {
                        String^ panelUsername = usernameLabel->Text->Substring(7);
                        if (panelUsername == username) {
                            onlineUsersPanel->Controls->Remove(userPanel);
                            return;
                        }
                    }
                }
            }
        }

        void CreatePanelForUser(String^ user) {
            // Создаем панель
            Panel^ userPanel = gcnew Panel();
            userPanel->BorderStyle = BorderStyle::FixedSingle;
            userPanel->Width = onlineUsersPanel->Width - 14;
            userPanel->Height = 50;

            // Создаем и добавляем Label для имени и фамилии
            Label^ nameLabel = gcnew Label();
            nameLabel->Text = GetUserInfo(user);
            nameLabel->Font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10);
            nameLabel->Location = System::Drawing::Point(5, 5);
            nameLabel->AutoSize = true;

            // Создаем и добавляем Label для логина
            Label^ usernameLabel = gcnew Label();
            usernameLabel->Text = "Login: " + user;
            usernameLabel->Font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8);
            usernameLabel->Location = System::Drawing::Point(5, 30);
            usernameLabel->AutoSize = true;

            // Добавляем Label на панель
            userPanel->Controls->Add(nameLabel);
            userPanel->Controls->Add(usernameLabel);

            // Добавляем обработчик события для клика по панели
            userPanel->Click += gcnew EventHandler(this, &MyMenu::OpenChatButtonClick);

            // Добавляем панель на панель с онлайн пользователями
            onlineUsersPanel->Controls->Add(userPanel);
        }

        String^ GetUserInfo(String^ user) {
            // Отправляем запрос на сервер для получения информации о пользователе
            try {
                String^ request = "GetInfo";
                String^ credentialsMessage = String::Format("{0}\n{1}", request, user);

                array<Byte>^ data = Encoding::UTF8->GetBytes(credentialsMessage);
                streamMenu->Write(data, 0, data->Length);

                array<Byte>^ responseBuffer = gcnew array<Byte>(1024);
                int bytesRead = streamMenu->Read(responseBuffer, 0, responseBuffer->Length);

                if (bytesRead > 0) {
                    String^ response = Encoding::UTF8->GetString(responseBuffer, 0, bytesRead);
                    if (user == username)
                    {
                        labelFullName->Text = response; 
                        return "";
                    }
                    else 
                    { return response; }
                }
            }
            catch (Exception^ e) {
                Console::WriteLine("Error requesting user info: " + e->Message);
            }

            return "";
        }

        Panel^ FindUserPanelByUsername(String^ username) {
            for each (Control ^ control in onlineUsersPanel->Controls) {
                Panel^ userPanel = dynamic_cast<Panel^>(control);
                if (userPanel != nullptr) {
                    Label^ usernameLabel = dynamic_cast<Label^>(userPanel->Controls[1]);
                    if (usernameLabel != nullptr) {
                        String^ panelUsername = usernameLabel->Text->Substring(7);
                        if (panelUsername == username) {
                            return userPanel;
                        }
                    }
                }
            }
            return nullptr;
        }

        void OpenChatButtonClick(Object^ sender, EventArgs^ e) {
            Panel^ clickedPanel = dynamic_cast<Panel^>(sender);

            if (clickedPanel != nullptr) {
                String^ clickedUsername = clickedPanel->Controls[1]->Text->Substring(7); // Убираем "Login: " из имени пользователя

                // Сбрасываем задний фон для всех панелей
                for each (Control ^ control in onlineUsersPanel->Controls) {
                    Panel^ userPanel = dynamic_cast<Panel^>(control);
                    if (userPanel != nullptr) {
                        userPanel->BackColor = SystemColors::Control;

                        // Удаляем красную точку, если она есть
                        RemoveRedDotFromUserPanel(userPanel);
                    }
                }

                panel1->Controls->Clear();

                if (clickedUsername == "|Chat|") {
                    String^ buffer = "|Chat|";
                    ChatBuffer^ targetBuffer = chatBuffers[buffer];

                    // Перебираем сообщения в буфере
                    for each (Tuple<String^, String^> ^ message in targetBuffer->Messages)
                    {
                        String^ sender = message->Item1; // Получаем отправителя
                        String^ messageText = message->Item2; // Получаем текст сообщения

                        AppendMessage(sender, messageText, (sender == username));
                    }
                }
                else {
                    if ((chatBuffers->Count == 0) || !ChatBufferExists((username + clickedUsername))) {
                        CreateBufferForChat(username + clickedUsername);
                    }
                    else {
                        String^ buffer = (username + clickedUsername);

                        ChatBuffer^ targetBuffer = chatBuffers[buffer];

                        // Перебираем сообщения в буфере
                        for each (Tuple<String^, String^> ^ message in targetBuffer->Messages)
                        {
                            String^ sender = message->Item1; // Получаем отправителя
                            String^ messageText = message->Item2; // Получаем текст сообщения

                            AppendMessage(sender, messageText, (sender == username));
                        }

                    }
                }

                // Устанавливаем задний фон светло-зеленым для активной панели
                clickedPanel->BackColor = Color::LightGreen;

                if (clickedUsername == "|Chat|") {
                    activeChatUser = "|Chat|";
                    label1->Text = "Chat: " + "general chat";
                }
                else {
                    activeChatUser = clickedUsername;
                    label1->Text = "Chat: " + clickedUsername;
                }
            }
        }

        void sendButton_Click(Object^ e1, EventArgs^ e) {
            String^ message = messageTextBox->Text;

            if (message == "") {
                return;
            }

            // Проверка длины сообщения
            if (message->Length > 255) {
                // Разделение сообщения на части по 255 символов
                int chunkSize = 255;
                for (int i = 0; i < message->Length; i += chunkSize) {
                    String^ chunk = message->Substring(i, Math::Min(chunkSize, message->Length - i));

                    // Отправка сообщения на сервер с указанием получателя
                    String^ fullMessage = String::Format("{0}\n{1}\n{2}\n{3}", "SendMessage", username, activeChatUser, chunk);
                    array<Byte>^ data = Encoding::UTF8->GetBytes(fullMessage);
                    streamMenu->Write(data, 0, data->Length);

                    String^ buffer;
                    activeChatUser == "|Chat|" ? buffer = "|Chat|" : buffer = (username + activeChatUser);

                    // Добавление части сообщения в буфер
                    AppendMessageToBuffer(buffer, username, chunk);

                    // Отображение отправленной части сообщения в диалоговом окне клиента
                    AppendMessage(username, chunk, true);

                    // Пауза между отправкой частей сообщения (0.1 секунда)
                    System::Threading::Thread::Sleep(100);
                }
            }
            else {
                // Отправка сообщения на сервер с указанием получателя
                String^ fullMessage = String::Format("{0}\n{1}\n{2}\n{3}", "SendMessage", username, activeChatUser, message);
                array<Byte>^ data = Encoding::UTF8->GetBytes(fullMessage);
                streamMenu->Write(data, 0, data->Length);

                String^ buffer;
                activeChatUser == "|Chat|" ? buffer = "|Chat|" : buffer = (username + activeChatUser);

                // Добавление сообщения в буфер
                AppendMessageToBuffer(buffer, username, message);

                // Отображение отправленного сообщения в диалоговом окне клиента
                AppendMessage(username, message, true);
            }

            // Очистка текстового поля после отправки
            messageTextBox->Text = "";
        }

        // Метод для добавления сообщения в RichTextBox
        void AppendMessage(String^ sender, String^ message, bool isSent)
        {
            // Создаем новую панель для сообщения
            Panel^ messagePanel = gcnew Panel();
            messagePanel->BackColor = isSent ? Color::LightGreen : Color::LightBlue;
            //messagePanel->BorderStyle = BorderStyle::FixedSingle;
            messagePanel->Width = panel1->Width - 12;
            messagePanel->Left = 2;
            messagePanel->Top = panel1->Controls->Count > 0 ?
                panel1->Controls[panel1->Controls->Count - 1]->Bottom + 5 : 2;

            // Создаем и добавляем Label для имени отправителя
            Label^ senderLabel = gcnew Label();
            sender == username ? senderLabel->Text = L"You" : senderLabel->Text = sender;
            senderLabel->Font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, FontStyle::Bold);
            senderLabel->ForeColor = isSent ? Color::Green : Color::DarkBlue;
            senderLabel->Location = System::Drawing::Point(5, 2);
            senderLabel->AutoSize = true;

            // Создаем и добавляем Label для текста сообщения
            Label^ messageLabel = gcnew Label();
            messageLabel->Text = message;
            messageLabel->Font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9);
            messageLabel->Location = System::Drawing::Point(10, senderLabel->Bottom + 2);
            messageLabel->AutoSize = true;

            int maxWidth = panel1->Width - 25;

            // Измеряем размер текста

            System::Drawing::Size temp1Size = TextRenderer::MeasureText(message, messageLabel->Font);
            System::Drawing::Size temp2Size = TextRenderer::MeasureText(senderLabel->Text, senderLabel->Font);

            System::Drawing::Size textSize;
            temp1Size.Width > temp2Size.Width ? 
                textSize = TextRenderer::MeasureText(message, messageLabel->Font) :
                textSize = TextRenderer::MeasureText(senderLabel->Text, senderLabel->Font);

            // Проверяем, нужно ли переносить текст на следующую строку
            if (textSize.Width > maxWidth)
            {
                messageLabel->Width = maxWidth - 20; // Устанавливаем ширину текста
                messageLabel->MaximumSize = System::Drawing::Size(maxWidth - 20, 0); // Ограничиваем ширину
                messageLabel->AutoSize = false; // Отключаем автоматическую подгонку размера
                messageLabel->Height = messageLabel->PreferredHeight + 8; // Устанавливаем высоту
            }

            // Устанавливаем ширину блока в зависимости от текста, но не более maxWidth
            messagePanel->Width = Math::Min(textSize.Width + 20, maxWidth);

            // Устанавливаем высоту блока в зависимости от текста, минимум 20px
            messagePanel->Height = Math::Max(messageLabel->Bottom + 4, 20);

            // Создаем графический путь для закругления углов
            System::Drawing::Drawing2D::GraphicsPath^ roundedPath = gcnew System::Drawing::Drawing2D::GraphicsPath();
            int cornerRadius = 10; // Радиус закругления

            roundedPath->AddArc(0, 0, cornerRadius * 2, cornerRadius * 2, 180, 90);
            roundedPath->AddArc(messagePanel->Width - cornerRadius * 2, 0, cornerRadius * 2, cornerRadius * 2, 270, 90);
            roundedPath->AddArc(messagePanel->Width - cornerRadius * 2, messagePanel->Height - cornerRadius * 2, cornerRadius * 2, cornerRadius * 2, 0, 90);
            roundedPath->AddLine(0, messagePanel->Height, 0, cornerRadius);
            roundedPath->CloseFigure();

            // Устанавливаем область обрезки для закругления углов
            messagePanel->Region = gcnew System::Drawing::Region(roundedPath);

            messagePanel->Controls->Add(senderLabel);
            messagePanel->Controls->Add(messageLabel);

            // Устанавливаем белый цвет текста
            senderLabel->ForeColor = Color::White;
            messageLabel->ForeColor = Color::White;

            // Добавляем панель на RichTextBox
            panel1->Controls->Add(messagePanel);

            panel1->AutoScrollPosition = System::Drawing::Point(0, panel1->Height);

            // Очищаем текстовое поле после отправки
            messageTextBox->Text = "";
        }

        void CreateBufferForChat(String^ chatName)
        {
            ChatBuffer^ buffer = gcnew ChatBuffer(chatName);
            chatBuffers->Add(chatName, buffer);
        }

        void AppendMessageToBuffer(String^ chatName, String^ sender, String^ message)
        {
            if (chatBuffers->ContainsKey(chatName))
            {
                Tuple<String^, String^>^ newMessage = gcnew Tuple<String^, String^>(sender, message);
                chatBuffers[chatName]->Messages->Add(newMessage);
            }
        }

        bool ChatBufferExists(String^ chatName)
        {
            return chatBuffers->ContainsKey(chatName);
        }

    };
}