#pragma once
#include "MyMenu.h"
#include <string>
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

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
	using namespace System::IO;

	public ref class Login : public System::Windows::Forms::Form
	{
	public:
		Login(void)
		{
			InitializeComponent();
		}

	protected:
		~Login()
		{
			if (components)
			{
				delete components;
			}
		}

	private:

		TcpClient^ tcpClient;
		NetworkStream^ stream;

		String^ action;
		String^ username;
		String^ password;
		String^ name;
		MyMenu^ myMenu;

		bool registrationMode = false;

		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		System::Windows::Forms::TextBox^ MyLoginTextBox;
		System::Windows::Forms::TextBox^ passwordTextBox;
		System::Windows::Forms::Button^ MyLoginButton;

		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::Label^ label2;
		System::Windows::Forms::Label^ label3;

		System::Windows::Forms::TextBox^ firstNameTextBox;

		System::Windows::Forms::Button^ backButton;
		System::Windows::Forms::Button^ registerConfirmButton;
		System::Windows::Forms::Label^ label6;
		System::Windows::Forms::Label^ label5;

		void InitializeComponent(void) {
			this->MyLoginTextBox = (gcnew System::Windows::Forms::TextBox());
			this->passwordTextBox = (gcnew System::Windows::Forms::TextBox());
			this->MyLoginButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->firstNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->backButton = (gcnew System::Windows::Forms::Button());
			this->registerConfirmButton = (gcnew System::Windows::Forms::Button());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// MyLoginTextBox
			// 
			this->MyLoginTextBox->Location = System::Drawing::Point(81, 30);
			this->MyLoginTextBox->Name = L"MyLoginTextBox";
			this->MyLoginTextBox->Size = System::Drawing::Size(173, 22);
			this->MyLoginTextBox->TabIndex = 0;
			// 
			// passwordTextBox
			// 
			this->passwordTextBox->Location = System::Drawing::Point(81, 58);
			this->passwordTextBox->Name = L"passwordTextBox";
			this->passwordTextBox->PasswordChar = '*';
			this->passwordTextBox->Size = System::Drawing::Size(173, 22);
			this->passwordTextBox->TabIndex = 1;
			// 
			// MyLoginButton
			// 
			this->MyLoginButton->Location = System::Drawing::Point(131, 86);
			this->MyLoginButton->Name = L"MyLoginButton";
			this->MyLoginButton->Size = System::Drawing::Size(82, 35);
			this->MyLoginButton->TabIndex = 2;
			this->MyLoginButton->Text = L"MyLogin";
			this->MyLoginButton->UseVisualStyleBackColor = true;
			this->MyLoginButton->Click += gcnew System::EventHandler(this, &Login::MyLoginButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(1, 35);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(43, 17);
			this->label1->TabIndex = 4;
			this->label1->Text = L"MyLogin";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(-1, 63);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(69, 17);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Password";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(-1, 89);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(45, 17);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Name";
			this->label3->Visible = false;
			// 
			// firstNameTextBox
			// 
			this->firstNameTextBox->Location = System::Drawing::Point(81, 86);
			this->firstNameTextBox->Name = L"firstNameTextBox";
			this->firstNameTextBox->Size = System::Drawing::Size(173, 22);
			this->firstNameTextBox->TabIndex = 8;
			this->firstNameTextBox->Visible = false;
			// 
			// backButton
			// 
			this->backButton->Location = System::Drawing::Point(1, 175);
			this->backButton->Name = L"backButton";
			this->backButton->Size = System::Drawing::Size(48, 27);
			this->backButton->TabIndex = 10;
			this->backButton->Text = L"Back";
			this->backButton->UseVisualStyleBackColor = true;
			this->backButton->Visible = false;
			this->backButton->Click += gcnew System::EventHandler(this, &Login::backButton_Click);
			// 
			// registerConfirmButton
			// 
			this->registerConfirmButton->Location = System::Drawing::Point(81, 115);
			this->registerConfirmButton->Name = L"registerConfirmButton";
			this->registerConfirmButton->Size = System::Drawing::Size(173, 35);
			this->registerConfirmButton->TabIndex = 11;
			this->registerConfirmButton->Text = L"Register";
			this->registerConfirmButton->UseVisualStyleBackColor = true;
			this->registerConfirmButton->Visible = false;
			this->registerConfirmButton->Click += gcnew System::EventHandler(this, &Login::registerButton_Click);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(118, 9);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(0, 17);
			this->label5->TabIndex = 12;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(118, 124);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(144, 17);
			this->label6->TabIndex = 13;
			this->label6->Text = L"Зарегестрироваться";
			this->label6->Click += gcnew System::EventHandler(this, &Login::registerButtonLabel_Click);
			// 
			// MyLogin
			// 
			this->ClientSize = System::Drawing::Size(278, 145);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->registerConfirmButton);
			this->Controls->Add(this->backButton);
			this->Controls->Add(this->firstNameTextBox);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->MyLoginButton);
			this->Controls->Add(this->passwordTextBox);
			this->Controls->Add(this->MyLoginTextBox);
			this->Name = L"MyLogin";
			this->Text = L"Authorization";
			this->ResumeLayout(false);
			this->PerformLayout();

		}

		void registerButtonLabel_Click(System::Object^ sender, System::EventArgs^ e) {
			ClientSize = System::Drawing::Size(298, 203);
			MyLoginButton->Visible = false;
			label6->Visible = false;
			backButton->Visible = true;
			registerConfirmButton->Visible = true;
			firstNameTextBox->Visible = true;
			label3->Visible = true;
			backButton->Visible = true;
		}

		void backButton_Click(System::Object^ sender, System::EventArgs^ e) {
			ClientSize = System::Drawing::Size(298, 147);
			MyLoginButton->Visible = true;
			label6->Visible = true;
			backButton->Visible = false;
			registerConfirmButton->Visible = false;
			firstNameTextBox->Visible = false;
			label3->Visible = false;
			backButton->Visible = false;
		}

		void MyLoginButton_Click(System::Object^ sender, System::EventArgs^ e) {
			action = "LOGIN";
			username = MyLoginTextBox->Text;
			password = passwordTextBox->Text;

			SendCredentialsToServer();
		}

		void registerButton_Click(System::Object^ sender, System::EventArgs^ e) {
			action = "REGISTER";

			username = MyLoginTextBox->Text;
			for (int i = 0; i < username->Length; i++) {
				if (username[i] == '|' || username[i] == '=') {
					MessageBox::Show(L"Логин не можер содержать специальные символы\n" + e, L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
					//label5->Text = L"Логин не можер содержать специальные символы";
					return;
				}
			}

			password = passwordTextBox->Text;
			if (password == "") {
				MessageBox::Show(L"Заполните поле регистрации\n" + e, L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				//label5->Text = L"Заполните поле регистрации";
				return;
			}

			name = firstNameTextBox->Text;
			if (password == "") {
				MessageBox::Show(L"Заполните поле Имя\n" + e, L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				//label5->Text = L"Заполните поле Имя";
				return;
			}

			SendCredentialsToServer();
		}

		void sendMessage(String^ message) {
			try {
				array<Byte>^ data = Encoding::UTF8->GetBytes(message);
				stream->Write(data, 0, data->Length);
				stream->Flush();

				// Получение ответа от сервера
				array<Byte>^ responseBuffer = gcnew array<Byte>(1024);
				int bytesRead = stream->Read(responseBuffer, 0, responseBuffer->Length);
				if (bytesRead > 0) {
					String^ response = Encoding::UTF8->GetString(responseBuffer, 0, bytesRead);
					if (response->ToLower() == "true") {
						this->Hide();
						myMenu = gcnew MyMenu(username, tcpClient, stream);
						myMenu->ShowDialog();
					}
					else {
						label5->Text = L"Ошибка при авторизации";
					}
				}
			}
			catch (Exception^ ex) {
				Console::WriteLine("Ошибка при отправке сообщения: " + ex->Message);
			}
		}

		// Внесем изменения в метод отправки данных на сервер
		void SendCredentialsToServer() {
			try {
				// Создаем подключение к серверу
				tcpClient = gcnew TcpClient("192.168.152.112", 777);  // IP-адрес и порт сервера
				stream = tcpClient->GetStream();

				// Проверяем состояние подключения
				if (tcpClient->Connected) {
					// Отправляем данные на сервер
					String^ credentialsMessage;
					action == "LOGIN" ?
						credentialsMessage = String::Format("{0}\n{1}\n{2}", action, username, password) :
						credentialsMessage = String::Format("{0}\n{1}\n{2}\n{3}", action, username, password, name);

					sendMessage(credentialsMessage);
				}
				else {
					MessageBox::Show(L"сбой при подключении к сервер\n", L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
					//label5->Text = L"сбой при подключении к серверу";
				}
			}
			catch (Exception^ ex) {
				MessageBox::Show(L"Ошибка: " + ex->Message, L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				// label5->Text = L"Ошибка: " + ex->Message;
			}
			finally {
				if (stream != nullptr) {
					stream->Close();
				}
				if (tcpClient != nullptr) {
					tcpClient->Close();
				}
			}
		}

#pragma endregion
	};
}
