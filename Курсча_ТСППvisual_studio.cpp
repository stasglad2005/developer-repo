#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <random>
#include <ctime> // Для инициализации генератора случайных чисел
#include <algorithm> 
#include <conio.h>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <fstream>
#include <thread>

using namespace std;

string getDateTime();
string randomName[][1] = {
	{"Ivan Ivanov"},
	{"Petr Petrov"},
	{"Sergey Sigorov"},
	{"Mariya Smirnova"},
	{"Elene Krapivina"}
};
int randomNum[5][2] = {
	{ 127, 0},
	{ 451, 500},
	{ 304, 3500},
	{ 717, 1000},
	{ 689, 5000},
};
int sizeName = sizeof(randomName) / sizeof(randomName[0]);
int sizeNum = sizeof(randomNum) / sizeof(randomNum[0]);
// Перечисления для состояний и типов
enum class ATMState { WORKING, NO_WORKING, NEEDS_MAINTENANCE };
class BankAccount;
class ATM;
class People {
	int ID;
	string fullName;
	string accountNumber;
	string pin; // PIN код (храним в классе Client для упрощения примера)
public:
	People(){}//add
	People(int id_, string name_, string number_, string p) {
		ID = id_, fullName = name_;
		accountNumber = number_,pin = p;
	}
	virtual ~People(){}
};
class Client : public People{
	float balance;
	int count_lock;
public:
	Client() {}
	Client(float cash, int id_, string name_, string number_, string p) : People(id_, name_, number_, p) {
		balance = cash, count_lock = 0;
	}
	void setCountLock(int x) {
		count_lock = x;
		system("cls");
		cout << "Состояние счета изменено.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
	}
	bool checkPin(string p);//проверка пин-кода
	double getBalance() { return balance; } // баланс
	void deposit(float amount) { balance += amount; } // внесение денег
	void withdraw(float amount) { balance -= amount; } // снятие денег
	bool verifyData(string account) { return (accountNumber == account); }
	bool checkCount_lock() {
		if (count_lock >= 3) return false;
		return true;
	}
	void printForNotifierInFile(fstream& f) {
		f << setw(10) << clientID << "|" << setw(25) << fullName << "|" << setw(11) << accountNumber << "|";
		if (count_lock >= 3) f << setw(30) << "Заблокирован|\n";
		else f << setw(25) << "Открыт|\n";
	}
	void printForNotifier() {
		cout << setw(10) << clientID << "|" << setw(25) << fullName << "|" << setw(11) << accountNumber << "|";
		if (count_lock >= 3) cout << setw(30) << "Заблокирован|\n";
		else cout << setw(25) << "Открыт|\n";
	}
	void printClient() { cout << "Вы вошли как " << fullName << ".  Номер счета: " << accountNumber << endl; }
	void printClientDetalis();
};
// Класс для администратора
class Admin {
	int adminID;
	string fullName;
	string login;
	string password;
public:
	Admin() {}
	Admin(int ID, string Name, string log, string pin) {
		adminID = ID, fullName = Name;
		login = log, password = pin;
	}
	void printAdmin() {
		cout << "Вы вошли как " << fullName << ".  Логин: " << login << endl;
	}
	bool verifyAdmin(string log, string pin) { return (log == login && pin == password); }
	void unlock_lock(BankAccount& clientAccount);
};
class Notifier {
	int ID_error;
	string date_error;
	string type_error;
	string sms_error;
	Client client;
public:
	Notifier(string type, string sms, Client& ob) {
		date_error = getDateTime();
		type_error = type, sms_error = sms;
		client = ob;
	}
	string type() { return type_error; }
	void notifier_id(int q) { ID_error = q; }
	void printNotifierInFile(fstream& f) {
		f << "|" << setw(9) << ID_error << "|" << setw(19) << date_error << "|" << setw(20) << type_error << "|" << setw(50) << sms_error << "|";
		client.printForNotifierInFile(f);
		f << "+---------+-------------------+--------------------+--------------------------------------------------+----------+-------------------------+-----------+-----------------+\n";
	}
	void printNotifier() {
		cout << "|" << setw(9) << ID_error << "|" << setw(19) << date_error << "|" << setw(20) << type_error << "|" << setw(50) << sms_error << "|";
		client.printForNotifier();
		cout << "+---------+-------------------+--------------------+--------------------------------------------------+----------+-------------------------+-----------+-----------------+\n";
	}
};
class BankAccount {
	vector<Client> clients;
	vector<Admin> admins;
	int id;
public:
	BankAccount(int k) { id = k; }
	void addClient(Client& client) { clients.push_back(client); }//добавление клиентов в вектор
	void addAdmin(Admin& admin) { admins.push_back(admin); }//добавление админов в вектор
	bool checkClient(string account, Client& clientTemp, string enteredPIN, Client& client, ATM& atm);//проверка существования клиента
	bool checkAdmin(Admin& adminTemp, string account, string enteredPIN);//проверка существования админа
	Client* countClient(int a) {
		for (auto& client : clients) {
			if (client.checkCount() == a) {
				return &client;
			}
		}
		return nullptr;
	}
	void printAllAccount(int a) { // просмотр всех клиентов для админа
		system("cls");
		int c = 1;
		cout << "+---+----------+-------------------------+-----------+----------+-----------------+\n";
		cout << "| № |ID клиента|       ФИО клиента       |номер счета|  баланс  | Состояние счета |\n";
		cout << "+---+----------+-------------------------+-----------+----------+-----------------+\n";
		for (auto& client : clients) {
			client.count = c;
			client.printClientDetalis();
			c++;
		}
		if (a == 0) {
			cout << "Нажмите любую клавишу для продолжения...";
			_getch();
			system("cls");
		}
	}
};

// Класс для банкомата
class ATM {
	vector<Notifier> notifiers;
	int atmID;
	string location;
	float cashBalance;
	ATMState state;
	//Administrator admin;
public:
	ATM(string loc, float initialCash, ATMState newState) {
		atmID = 777, location = loc, state = newState;
		cashBalance = initialCash;
		//admin = admin1;
	}
	void printBalance(Client& clientTemp) {
		cout << "Ваш баланс: " << clientTemp.getBalance() << endl;
		cout << "Нажмите любую клавишу для продолжения...";
		_getch();
		system("cls");
	}
	void addNotifier(Notifier& notifier) { notifiers.push_back(notifier); }//добавление ошибок в вектор
	void menuATMClient(Client& clientTemp);//меню клиента
	string menuATMAdmin(Admin& admin, BankAccount& clientAccount);//меню админа
	float checkDeposit(Client& client); // пополнение счета
	// Проверка состояния банкомата
	int checkATMState() const {
		switch (state) {
		case ATMState::WORKING:            return 0;
		case ATMState::NO_WORKING:         return 1;
		case ATMState::NEEDS_MAINTENANCE: return -1;
		}
		return 999;
	}
	string sendError();//отправка отчетов
	bool withdrawCash(Client& client);// Снятие наличных
	void menuATMState();// меню изменения состояния ATM
	void setATMState(ATMState newState) { // Изменение состояния банкомата 
		state = newState;
		cout << "Состояние банкомата изменено.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
	}
	void printATMDetails(int y) { // Показать информацию о банкомате
		if (y == 1) cout << "Местоположение: " << location << endl;
		else {
			cout << "------------------------------------------------------\n";
			cout << "Детали банкомата:\n";
			cout << "ID банкомата: " << atmID << endl;
			cout << "Местоположение: " << location << endl;
			cout << "Состояние банкомата: ";
			if (checkATMState() == 0) cout << "Работает\n";
			else if (checkATMState() == 1) cout << "Не работает\n";
			else cout << "Требует обслуживания\n";
			cout << "Баланс наличных: " << cashBalance << endl;
			cout << "------------------------------------------------------\n";
			cout << "Нажмите любую клавишу для продолжения...";
			_getch();
			system("cls");
		}
	}
	int allNotifier() {
		if (notifiers.empty()) {
			cout << "Ошибок не зафиксированно.\n";
			this_thread::sleep_for(chrono::seconds(2));
			system("cls");
			return -1;
		}
		int q = 101;
		cout << "+---------+-------------------+--------------------+--------------------------------------------------+----------+-------------------------+-----------+-----------------+\n";
		cout << "|ID ошибки|    Дата ошибки    |     Тип ошибки     |                 Сообщение ошибки                 |ID клиента|       ФИО клиента       |номер счета| Состояние счета |\n";
		cout << "+---------+-------------------+--------------------+--------------------------------------------------+----------+-------------------------+-----------+-----------------+\n";
		for (auto& notifier : notifiers) {
			notifier.notifier_id(q);
			notifier.printNotifier();
			q++;
		}
		cout << "Esc - не создавать отчёт\n";
		cout << "Нажмите любую клавишу для продолжения...";
		int v = _getch();
		if (v == 27) {
			system("cls");
			return 1;
		}
		system("cls");
		return 0;

	}
	void allNotifierInFile(fstream& f) {
		f << "+---------+-------------------+--------------------+--------------------------------------------------+----------+-------------------------+-----------+-----------------+\n";
		f << "|ID ошибки|    Дата ошибки    |     Тип ошибки     |                 Сообщение ошибки                 |ID клиента|       ФИО клиента       |номер счета| Состояние счета |\n";
		f << "+---------+-------------------+--------------------+--------------------------------------------------+----------+-------------------------+-----------+-----------------+\n";
		for (auto& notifier : notifiers) {
			notifier.printNotifierInFile(f);
		}
	}
};
class Bank {
	string nameFile;
public:
	void loginClient(BankAccount& clientAccount, ATM& atm);
	void startSystem();
	void loginAdmin(BankAccount& clientAccount, ATM& atm);
	void getFile(string nameF) { nameFile = nameF; }
	void openFile() {
		system("cls");
		fstream f(nameFile);
		if (f.is_open()) {
			string line;
			while (getline(f, line)) {
				cout << line << endl;
			}
			f.close();
		}
		else {
			cout << "Ошибка открытия файла. Такого файла не существует!\n";
			f.close();
			this_thread::sleep_for(chrono::seconds(2));
			system("cls");
		}
	}
};
// Функция для очистки потока ввода (используется после неудачного ввода)
void clearInput() {
	cin.clear();
	cin.ignore();
}
string getDateTime() {
	auto now = chrono::system_clock::now();
	time_t currentTime = chrono::system_clock::to_time_t(now);
	struct tm timeinfo;
	errno_t err = localtime_s(&timeinfo, &currentTime);
	if (err != 0) return "Error getting time";
	char buffer[80];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
	return string(buffer);
}
int main() {
	setlocale(LC_ALL, "Russian");
	Bank bank;
	bank.startSystem();
	return 0;
}
void Bank::startSystem() {
	BankAccount clientAccount(213);
	Client ob1(100, "Andrey Grishko", "1111", 3200, "1111"), ob2(700, "Masha Svechova", "1212", 500, "1212"), ob3(199, "Maksim Drozdov", "3333", 4850, "3333");
	clientAccount.addClient(ob1), clientAccount.addClient(ob2), clientAccount.addClient(ob3);
	//создаем админов
	Admin admin1(999, "Stas Gladkov", "123", "123"), admin2(777, "Test Testov", "test", "test123");
	clientAccount.addAdmin(admin1), clientAccount.addAdmin(admin2);
	//Создаем банкомат
	ATM atm("пр-кт Героев-Сталинграда, д. 27", 15000.0, ATMState::WORKING);

	while (true) {
		cout << "\n   Выберите действие:\n";
		cout << "---------------------------------\n";
		cout << "0. Выход из программы\n";
		cout << "1. Войти как клиент\n";
		cout << "2. Войти как администратор\n";
		cout << "3. Просмотр отчета\n";
		cout << "---------------------------------\n";
		cout << "Ввод-->\n";
		char menu = _getch();
		switch (menu) {
		case '0': return;
		case '1': loginClient(clientAccount, atm); break;
		case '2': loginAdmin(clientAccount, atm); break;
		case '3': openFile(); break;
		default: system("cls");
		}
	}
}
void Bank::loginAdmin(BankAccount& clientAccount, ATM& atm) {
	system("cls");
	Admin adminTemp;
	string account, enteredPIN;
	cout << "Введите логин: "; cin >> account;
	cout << "Введите пароль: "; cin >> enteredPIN;
	if (clientAccount.checkAdmin(adminTemp, account, enteredPIN)) {
		getFile(atm.menuATMAdmin(adminTemp, clientAccount));
	}
}
void Bank::loginClient(BankAccount& clientAccount, ATM& atm) {
	system("cls");
	if (atm.checkATMState() == 1) {
		cout << "Банкомат временно не работает, приносим свои извенения!\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return;
	}
	else if (atm.checkATMState() == -1) {
		cout << "Банкомат находится на обслуживании, приносим свои извенения!\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return;
	}
	Client clientTemp;
	string account, enteredPIN;
	cout << "Введите номер счета: "; cin >> account;
	cout << "Введите PIN-код: "; cin >> enteredPIN;
	mt19937 generator(time(0));
	uniform_int_distribution<int> distribution(0, sizeName - 1);
	int randomIndex = distribution(generator);
	int randomNumIndex = randomIndex;
	float randomBalance = static_cast<float>(randomNum[randomNumIndex][1]);
	Client client(randomNum[randomNumIndex][0], randomName[randomIndex][0], account, randomBalance, enteredPIN);
	if (clientAccount.checkClient(account, clientTemp, enteredPIN, client, atm)) {
		atm.menuATMClient(clientTemp);
		clientAccount.addClient(clientTemp);
	}
}
void Client::printClientDetalis() {
	cout << "|" << setw(3) << count << "|" << setw(10) << clientID << "|" << setw(25) << fullName << "|" << setw(11) << accountNumber << "|" << setw(10) << balance << "|";
	if (count_lock >= 3) cout << setw(30) << "Заблокирован|\n";
	else cout << setw(25) << "Открыт|\n";
	cout << "+---+----------+-------------------------+-----------+----------+-----------------+\n";
}
bool BankAccount::checkAdmin(Admin& adminTemp, string account, string enteredPIN) {
	for (auto& admin : admins) {
		if (admin.verifyAdmin(account, enteredPIN)) {
			cout << "Авторизация успешна!\n";
			adminTemp = admin;
			this_thread::sleep_for(chrono::seconds(2));
			system("cls");
			return true;
		}
	}
	cout << "Неверный логин или пароль!\n";
	this_thread::sleep_for(chrono::seconds(2));
	system("cls");
	return false;
}
bool BankAccount::checkClient(string account, Client& clientTemp, string enteredPIN, Client& client, ATM& atm) {
	auto it = find_if(clients.begin(), clients.end(), [&](Client& client) { return client.verifyData(account); });
	if (it != clients.end()) {
		clientTemp = *it;
		if (!it->checkCount_lock()) {
			cout << "Ваша карта заблокирована\n";
			this_thread::sleep_for(chrono::seconds(2));
			system("cls");
			Notifier notifier("authorization error", "Attempt to access a blocked account", clientTemp);
			atm.addNotifier(notifier);
			return false;
		}
		if (it->checkPin(enteredPIN)) {
			clients.erase(it);
			return true;
		}
		else {
			Notifier notifier("authorization error", "Unauthorized access attempt", clientTemp);
			atm.addNotifier(notifier);
		}
	}
	else {
		while (true) {
			cout << "Ваша карта не активированна! Для активации нажмите Enter. Для выхода Esc...\n";
			int k = _getch();
			system("cls");
			if (k == 27) return false;
			else if (k == 13) {
				addClient(client);
				cout << "Ваша карта активированна\n";
				this_thread::sleep_for(chrono::seconds(2));
				system("cls");
				return false;
			}
			else cout << "Неверный ввод!\n";
		}
	}
	return false;
}
string ATM::menuATMAdmin(Admin& adminTemp, BankAccount& clientAccount) {
	string nameFile;
	system("cls");
	while (true) {
		adminTemp.printAdmin();
		cout << "\n   Выберите действие:\n";
		cout << "---------------------------------------------\n";
		cout << "0. Выйти из учетной записи\n";
		cout << "1. Информация о клиентах банка\n";
		cout << "2. Информация о банкомате\n";
		cout << "3. Разблокировать/заблокировать клиента\n";
		cout << "4. Изменить состояние банкомата\n";
		cout << "5. Отправить отчет об ошибках\n";
		cout << "---------------------------------------------\n";
		cout << "Ввод--> ";
		char menu = _getch();
		switch (menu) {
		case '0': system("cls"); return nameFile;
		case '1': system("cls"); clientAccount.printAllAccount(0); break;
		case '2': system("cls"); printATMDetails(0); break;
		case '3': system("cls"); adminTemp.unlock_lock(clientAccount); break;
		case '4': system("cls"); menuATMState(); break;
		case '5': system("cls"); nameFile = sendError(); break;
		default: system("cls");
		}
	}
	return nameFile;
}
void ATM::menuATMState() {
	while (true) {
		cout << "\n   Выберите состояние:\n";
		cout << "-------------------------------\n";
		cout << "0. Выйти\n";
		cout << "1. Работает\n";
		cout << "2. Не работает\n";
		cout << "3. Требует обслуживания\n";
		cout << "-------------------------------\n";
		cout << "Ввод-->\n ";
		char menu1 = _getch();
		switch (menu1) {
		case '0': system("cls"); return;
		case '1': system("cls"); setATMState(ATMState::WORKING); return;
		case '2': system("cls"); setATMState(ATMState::NO_WORKING); return;
		case '3': system("cls"); setATMState(ATMState::NEEDS_MAINTENANCE); return;
		default: system("cls");
		}
	}
}
string ATM::sendError() {
	if (allNotifier() == 1) {
		cout << "Отчет не сформирован.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return "";
	}
	if (notifiers.empty()) {
		system("cls");
		return "";
	}
	string name = "error_log.txt";
	fstream f(name, ios::out);
	if (!f) {
		f.close();
		cout << "Ошибка открытия файла!\n";
		return "";
	}
	allNotifierInFile(f);
	f.close();
	cout << "Отчет сформирован и отправлен!\n";
	this_thread::sleep_for(chrono::seconds(2));
	system("cls");
	return name;
}
void Admin::unlock_lock(BankAccount& clientAccount) {
	clientAccount.printAllAccount(1);
	cout << "Для выхода - 0\n";
	cout << "Выберите номер клиента:\n";
	int num;
	cin >> num;
	if (cin.fail()) {
		clearInput();
		cout << "Ошибка: Некорректный ввод.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return;
	}
	else if (num == 0) {
		clearInput();
		system("cls");
		return;
	}
	else if (num < 0) {
		clearInput();
		cout << "Ошибка: Некорректный ввод.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return;
	}
	system("cls");
	Client* ob = clientAccount.countClient(num);
	if (ob != nullptr) {
		while (true) {
			cout << "+---+----------+-------------------------+-----------+----------+-----------------+\n";
			cout << "| № |ID клиента|       ФИО клиента       |номер счета|  баланс  | Состояние счета |\n";
			cout << "+---+----------+-------------------------+-----------+----------+-----------------+\n";
			ob->printClientDetalis();
			cout << "\n   Выберите действие:\n";
			cout << "----------------------------\n";
			cout << "0. Выйти\n";
			cout << "1. Разблокировать клиента\n";
			cout << "2. Заблокировать клиента\n";
			cout << "----------------------------\n";
			cout << "Ввод-->\n";
			char menu = _getch();
			switch (menu) {
			case '0': system("cls"); return;
			case '1': ob->setCountLock(0); return;
			case '2': ob->setCountLock(3); return;
			default: system("cls");
			}
		}
	}
	else {
		cout << "Клиент не найден!\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
	}
}
void ATM::menuATMClient(Client& clientTemp) {
	system("cls");
	while (true) {
		printATMDetails(1);
		clientTemp.printClient();
		cout << "\n   Выберите действие:\n";
		cout << "---------------------------------\n";
		cout << "0. Выйти из учетной записи\n";
		cout << "1. Проверить баланс\n";
		cout << "2. Пополнить счет\n";
		cout << "3. Снять наличные\n";
		cout << "---------------------------------\n";
		cout << "Ввод-->\n";
		char menu = _getch();
		switch (menu) {
		case '0': system("cls"); return;
		case '1': system("cls"); printBalance(clientTemp); break;
		case '2': system("cls"); clientTemp.deposit(checkDeposit(clientTemp)); break;
		case '3': system("cls"); withdrawCash(clientTemp); break;
		default: system("cls");
		}
	}
}
bool ATM::withdrawCash(Client& client) {
	float amount;
	cout << "Введите сумму для снятия\n";
	cin >> amount;
	if (cin.fail()) {
		cout << "Ошибка: Некорректный ввод.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return false;
	}
	else if (amount > cashBalance) {
		cout << "Недостаточно средств в банкомате. Обратитесь в отделение банка.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		Notifier notifier("ATM error", "Insufficient funds at the ATM", client);
		addNotifier(notifier);
		return false;
	}
	else if (amount < 0) {
		cout << "Ошибка: Некорректная сумма для снятия.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return false;
	}
	else if (amount > client.getBalance()) {
		cout << "Недостаточно средств на счете.\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return false;
	}
	client.withdraw(amount);
	cashBalance -= amount;
	cout << cashBalance << "Наличные успешно выданы.\n";
	this_thread::sleep_for(chrono::seconds(2));
	system("cls");
	return true;
}
bool Client::checkPin(string p) {
	if (pin == p) return true;
	else {
		count_lock++;
		cout << "Неверный пин-код!" << endl;
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return false;
	}
}
float ATM::checkDeposit(Client& client) {
	if (cashBalance >= 50000) {
		cout << "Банкомат переполнен. Вы не можете внести наличные\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		Notifier notifier("ATM error", "The ATM is full, you need to extract money from it", client);
		addNotifier(notifier);
		return 0;
	}
	cout << "Введите сумму для внесения - ";
	float k = 0;
	cin >> k;
	if (k < 0) {
		cout << "Некорректный ввод!\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return 0;
	}
	else if (cin.fail()) {
		cout << "Некорректный ввод!";
		clearInput();
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return 0;
	}
	else {
		cashBalance += k;
		cout << "Счет успешно пополнен\n";
		this_thread::sleep_for(chrono::seconds(2));
		system("cls");
		return k;
	}
}
