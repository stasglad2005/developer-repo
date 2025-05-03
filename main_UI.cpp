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
		cout << "3. Войти как безопасник\n";
		cout << "---------------------------------\n";
		cout << "Ввод-->\n";
		char menu = _getch();
		switch (menu) {
		case '0': SaveDataBaseBank(); return;
		case '1': loginClient(clientAccount, atm); break;
		case '2': loginAdmin(clientAccount, atm); break;
		case '3': LoginSecuriry(clientAccount, atm); break;
		default: system("cls");
		}
	}
}