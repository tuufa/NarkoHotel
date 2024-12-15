#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <Windows.h>
#include <algorithm>
#include <sstream>
using namespace std;

// Перечисление доступных услуг
// Это перечисление (enum class) определяет типы услуг, которые можно предложить клиентам.
// Каждая услуга ассоциируется с уникальным значением.
enum class ServiceType
{
    Breakfast = 1,        // Завтрак
    Lunch = 2,            // Обед
    Dinner = 3,           // Ужин
    FullMeal = 4,         // Полное питание (завтрак, обед, ужин со скидкой)
    Sauna = 5,            // Сауна
    Pool = 6,             // Бассейн
    BathAccessories = 7,  // Дополнительные ванные принадлежности
    Laundry = 8           // Услуги прачечной
};

// Функция для преобразования типа услуги в строковое представление
// Это необходимо для удобного отображения услуг на экране.
string serviceToString(ServiceType service)
{
    switch (service)
    {
    case ServiceType::Breakfast: return "Завтрак";
    case ServiceType::Lunch: return "Обед";
    case ServiceType::Dinner: return "Ужин";
    case ServiceType::FullMeal: return "Полное питание (с скидкой 15%)";
    case ServiceType::Sauna: return "Сауна";
    case ServiceType::Pool: return "Бассейн";
    case ServiceType::BathAccessories: return "Дополнительные ванные принадлежности";
    case ServiceType::Laundry: return "Услуги прачечной";
    default: return "Неизвестная услуга";
    }
}


// Класс клиента
// Определяет данные и методы, относящиеся к клиентам отеля.
class Client
{
private:
    string name;        // Имя клиента
    int bonusPoints;    // Бонусные баллы клиента

public:
    // Конструкторы
    Client() : name(""), bonusPoints(0) {}
    Client(const string& clientName) : name(clientName), bonusPoints(0) {}

    // Метод для получения имени клиента (с проверкой на пустоту)
    string getName() const
    {
        return name.empty() ? "Неизвестный клиент" : name;
    }

    // Метод для начисления бонусных баллов
    // За каждые 20 рублей добавляется 1 бонусный балл.
    void addBonusPoints(double amount)
    {
        bonusPoints += static_cast<int>(amount / 20);
    }

    // Метод для получения текущего количества бонусных баллов
    int getBonusPoints() const
    {
        return bonusPoints;
    }

    // Метод для расчета доступной скидки (в процентах)
    // Каждые 5000 баллов дают 5% скидки. Максимум - 75%.
    double getDiscount() const
    {
        int discount = (bonusPoints / 5000) * 5;
        return discount > 75 ? 75 : discount;
    }

};



// Класс для управления номерами в отеле
// Содержит информацию о всех номерах и их статусе (свободен/занят).
class RoomManager
{
private:
    unordered_map<string, pair<string, double>> rooms;  // Карта номеров: номер -> (тип, цена за ночь)
    unordered_map<string, bool> occupiedRooms;         // Карта занятости: номер -> занят/свободен

public:
    // Конструктор инициализирует комнаты и помечает их как свободные
    RoomManager(unordered_map<string, pair<string, double>> roomDetails)
    {
        rooms = roomDetails;
        for (const auto& room : rooms)
        {
            occupiedRooms[room.first] = false;
        }
    }

    // Метод для отображения всех свободных номеров
    void displayAvailableRooms() const
    {
        cout << "Свободные номера:\n";
        for (const auto& room : rooms)
        {
            if (!occupiedRooms.at(room.first))
            {
                cout << "Номер: " << room.first << " Тип: " << room.second.first << " Цена за ночь: " << room.second.second << " руб\n";
            }
        }
    }

    // Проверяет, доступен ли номер
    bool isAvailable(const string& roomNum) const
    {
        return rooms.find(roomNum) != rooms.end() && !occupiedRooms.at(roomNum);
    }

    // Помечает номер как занятый
    void setOccupied(const string& roomNum)
    {
        occupiedRooms[roomNum] = true;
    }

    // Помечает номер как свободный
    void setVacant(const string& roomNum)
    {
        occupiedRooms[roomNum] = false;
    }

    // Получает базовую цену номера
    double getAdjustedPrice(const string& roomNum) const
    {
        return rooms.at(roomNum).second;
    }

    // Рассчитывает процент занятости отеля
    double getOccupancyRate() const
    {
        int totalRooms = rooms.size();
        int occupied = count_if(occupiedRooms.begin(), occupiedRooms.end(), [](const pair<string, bool>& p)
            {
                return p.second;
            });
        return (static_cast<double>(occupied) / totalRooms) * 100;
    }

    // Возвращает все данные о комнатах
    unordered_map<string, pair<string, double>> getRooms() const
    {
        return rooms;
    }
};


// Класс для бронирования, который управляет информацией о заказах клиентов
class Booking {
protected:
    string roomNumber;        // Номер комнаты
    string roomType;          // Тип комнаты (например, стандарт, люкс и т.д.)
    int nights;               // Количество ночей, на которые забронирован номер
    double basePrice;         // Базовая стоимость номера за ночь
    double serviceCost;       // Стоимость дополнительных услуг
    double discount;          // Скидка, примененная к бронированию
    double occupancyRate;     // Уровень загруженности отеля (например, процент от заполняемости)
    Client* client;           // Указатель на клиента, который сделал бронирование

public:
    // Конструктор класса. Инициализирует бронирование с основными данными.
    Booking(string roomNum, string roomT, int n, double price, double occupancy, Client* c = nullptr)
        : roomNumber(roomNum), roomType(roomT), nights(n), basePrice(price), serviceCost(0), discount(0), occupancyRate(occupancy), client(c) {
        if (client) {
            client->addBonusPoints(price * nights);  // Если клиент указан, начисляем бонусные баллы за стоимость номера
        }
    }

    // Деструктор класса (необходим для управления памятью, если необходимо)
    virtual ~Booking() {}

    // Метод для получения номера комнаты
    string getRoomNumber() const {
        return roomNumber;
    }

    // Метод для добавления услуги в бронирование
    void addService(ServiceType service)
    {
        double servicePrice = 0.0;  // Переменная для хранения стоимости услуги
        // В зависимости от типа услуги, устанавливаем ее цену
        switch (service) {
        case ServiceType::Breakfast:
            servicePrice = 300.0;  // Цена завтрака
            break;
        case ServiceType::Lunch:
            servicePrice = 500.0;  // Цена обеда
            break;
        case ServiceType::Dinner:
            servicePrice = 400.0;  // Цена ужина
            break;
        case ServiceType::FullMeal:
            servicePrice = 300.0 + 500.0 + 400.0;  // Стоимость полного питания (завтрак + обед + ужин)
            servicePrice -= servicePrice * 0.15;   // Применяем скидку 15% на полное питание
            break;
        case ServiceType::Sauna:
            servicePrice = 650.0;  // Цена сауны
            break;
        case ServiceType::Pool:
            servicePrice = 700.0;  // Цена бассейна
            break;
        case ServiceType::BathAccessories:
            servicePrice = 340.0;  // Цена аксессуаров для ванной
            break;
        case ServiceType::Laundry:
            servicePrice = 1200.0; // Цена стирки
            break;
        default:
            cout << "Неизвестная услуга!" << endl;  // Если услуга не найдена, выводим сообщение об ошибке
            return;
        }

        serviceCost += servicePrice;  // Добавляем стоимость услуги к общей стоимости
    }

    // Метод для применения скидки
    void applyDiscount(double percent)
    {
        discount = percent;  // Применяем скидку к бронированию
    }

    // Перерасчет общей стоимости бронирования с учетом загруженности отеля и бонусных баллов клиента
    virtual double calculateTotal() const {
        // Динамическая стоимость, которая зависит от уровня загруженности отеля
        double dynamicPrice = basePrice * (1 + (static_cast<int>(occupancyRate) / 10) * 0.05);
        // Общая стоимость бронирования с учетом ночей и дополнительных услуг, с применением скидки
        double total = (dynamicPrice * nights + serviceCost) * (1 - discount / 100);

        // Если у клиента есть бонусные баллы, применяем скидку по бонусам
        if (client) {
            total -= total * (client->getDiscount() / 100);  // Применяем скидку клиента по бонусам
        }

        return total;  // Возвращаем общую стоимость
    }

    // Метод для вывода информации о бронировании
    virtual void printBooking() const
    {
        cout << "Номер: " << roomNumber << " (" << roomType << ")\n";  // Вывод номера и типа комнаты
        cout << "Количество ночей: " << nights << "\n";  // Вывод количества ночей
        cout << "Дополнительные услуги: " << fixed << setprecision(2) << serviceCost << " руб\n";  // Вывод стоимости услуг
        cout << "Общая стоимость: " << fixed << setprecision(2) << calculateTotal() << " руб\n";  // Вывод общей стоимости
        if (client) {  // Если клиент существует, выводим информацию о клиенте
            cout << "Имя клиента: " << client->getName() << "\n";
            cout << "Бонусные баллы: " << client->getBonusPoints() << "\n";
        }
    }
};

// Хранилище клиентов, где ключом является имя клиента, а значением - объект клиента
unordered_map<string, Client> clients;

int main()
{
    SetConsoleCP(1251);  // Устанавливаем кодировку консоли для ввода (кириллица)
    SetConsoleOutputCP(1251);  // Устанавливаем кодировку консоли для вывода (кириллица)

    // Инициализация подробностей номеров (номер, тип, цена)
    unordered_map<string, pair<string, double>> roomDetails =
    {
        {"101", {"Single Room", 1000}},
        {"102", {"Double Room", 1500}},
        {"201", {"Suite", 3000}},
        {"202", {"Suite", 3200}},
        {"301", {"Single Room", 1100}},
        {"302", {"Double Room", 1600}},
        {"303", {"Suite", 3500}},
        {"401", {"Single Room", 1200}},
        {"402", {"Double Room", 1700}},
        {"403", {"Suite", 3800}},
        {"501", {"Single Room", 1300}},
        {"502", {"Double Room", 1800}},
        {"503", {"Suite", 4000}}
    };

    // Создание объекта менеджера номеров, передавая информацию о номерах
    RoomManager roomManager(roomDetails);

    while (true)  // Основной цикл программы
    {
        // Вывод текущей загруженности отеля
        cout << "Текущая загруженность отеля: " << fixed << setprecision(2) << roomManager.getOccupancyRate() << "%\n";
        cout << "Меню:\n1. Посмотреть свободные номера\n2. Индивидуальное бронирование\n3. Групповое бронирование\n4. Выселение постояльца\n0. Выход\n";
        cout << "Ваш выбор: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            // Просмотр свободных номеров
            roomManager.displayAvailableRooms();
            break;
        }

        case 2:
        {
            // Индивидуальное бронирование
            roomManager.displayAvailableRooms();  // Показываем свободные номера

            cout << "Введите номер комнаты: ";
            string roomNum;
            cin >> roomNum;

            // Проверяем, свободен ли выбранный номер
            if (!roomManager.isAvailable(roomNum))
            {
                cout << "Выбранный номер уже занят.\n";
                break;
            }

            cout << "Введите количество ночей: ";
            int nights;
            cin >> nights;

            // Ввод имени клиента
            cout << "Введите имя клиента (или оставьте пустым для анонимного клиента): ";
            string clientName;
            cin.ignore();  // Очищаем буфер ввода
            getline(cin, clientName);

            Client* client = nullptr;
            if (!clientName.empty())
            {
                // Если клиент не анонимный, проверяем, существует ли он в базе
                if (clients.find(clientName) == clients.end())
                {
                    clients[clientName] = Client(clientName);  // Если не существует, создаем нового клиента
                }
                client = &clients[clientName];
            }

            // Создаем бронирование
            Booking* booking = new Booking(roomNum, roomManager.getAdjustedPrice(roomNum) > 1500 ? "Suite" : "Single Room", nights, roomManager.getAdjustedPrice(roomNum), roomManager.getOccupancyRate(), client);

            // Выбор дополнительных услуг
            cout << "Выберите дополнительные услуги (введите 0, если не хотите выбрать услуги):\n";
            cout << "1. Завтрак - 300 руб\n2. Обед - 500 руб\n3. Ужин - 400 руб\n4. Полное питание - 15% скидка\n";
            cout << "5. Сауна - 650 руб\n6. Бассейн - 700 руб\n7. Ванные принадлежности - 340 руб\n8. Прачечная - 1200 руб\n";
            cout << "Введите номер услуги (несколько услуг можно выбрать через пробел, 0 для завершения): ";
            int serviceChoice;
            string line;
            cin.ignore();  // Очищаем буфер ввода
            getline(cin, line);
            stringstream ss(line);

            // Обработка выбранных услуг
            while (ss >> serviceChoice)
            {
                if (serviceChoice == 0) break;  // Завершаем выбор услуг, если введен 0
                ServiceType service = static_cast<ServiceType>(serviceChoice);
                booking->addService(service);  // Добавляем выбранную услугу к бронированию
            }

            // Отметка номера как занятый
            roomManager.setOccupied(roomNum);
            booking->printBooking();  // Вывод информации о бронировании

            break;
        }
        case 3:
        {
            // Групповое бронирование
            roomManager.displayAvailableRooms();  // Показываем свободные номера

            cout << "Введите количество номеров для бронирования: ";
            int numberOfRooms;
            cin >> numberOfRooms;

            vector<Booking*> groupBookings;  // Вектор для хранения бронирований группы

            for (int i = 0; i < numberOfRooms; ++i)
            {
                cout << "Введите номер комнаты для бронирования " << (i + 1) << ": ";
                string roomNum;
                cin >> roomNum;

                // Проверка на доступность номера
                if (!roomManager.isAvailable(roomNum))
                {
                    cout << "Номер " << roomNum << " уже занят.\n";
                    --i;  // Перепроходим выбор для текущего номера
                    continue;
                }

                cout << "Введите количество ночей: ";
                int nights;
                cin >> nights;

                cout << "Введите имя клиента (или оставьте пустым для анонимного клиента): ";
                string clientName;
                cin.ignore();
                getline(cin, clientName);

                Client* client = nullptr;
                if (!clientName.empty())
                {
                    // Если клиент не анонимный, проверяем в базе
                    if (clients.find(clientName) == clients.end())
                    {
                        clients[clientName] = Client(clientName);  // Добавляем нового клиента в базу
                    }
                    client = &clients[clientName];
                }

                // Создаем бронирование для каждого номера
                Booking* booking = new Booking(roomNum, roomManager.getAdjustedPrice(roomNum) > 1500 ? "Suite" : "Single Room", nights, roomManager.getAdjustedPrice(roomNum), roomManager.getOccupancyRate(), client);

                // Выбор дополнительных услуг
                cout << "Выберите дополнительные услуги для номера " << roomNum << " (введите 0, если не хотите выбрать услуги):\n";
                cout << "1. Завтрак - 300 руб\n2. Обед - 500 руб\n3. Ужин - 400 руб\n4. Полное питание - 15% скидка\n";
                cout << "5. Сауна - 650 руб\n6. Бассейн - 700 руб\n7. Ванные принадлежности - 340 руб\n8. Прачечная - 1200 руб\n";
                cout << "Введите номер услуги (несколько услуг можно выбрать через пробел, 0 для завершения): ";
                int serviceChoice;
                string line;
                cin.ignore();  // Очищаем буфер ввода
                getline(cin, line);
                stringstream ss(line);

                // Обработка выбранных услуг
                while (ss >> serviceChoice)
                {
                    if (serviceChoice == 0) break;  // Завершаем выбор услуг, если введен 0
                    ServiceType service = static_cast<ServiceType>(serviceChoice);
                    booking->addService(service);  // Добавляем услугу к бронированию
                }

                groupBookings.push_back(booking);  // Добавляем бронирование в группу
            }

            // Подсчитываем общую стоимость для группы
            double totalCost = 0.0;
            for (auto& booking : groupBookings)
            {
                totalCost += booking->calculateTotal();  // Суммируем стоимости всех бронирований
            }

            cout << "Общая стоимость для группы бронирований: " << fixed << setprecision(2) << totalCost << " руб\n";

            // Выводим и освобождаем ресурсы для каждого бронирования
            for (auto& booking : groupBookings)
            {
                booking->printBooking();  // Печатаем информацию о каждом бронировании
                roomManager.setOccupied(booking->getRoomNumber());  // Отмечаем номер как занятый
                delete booking;  // Освобождаем память, выделенную под бронирование
            }

            break;
        }
        case 4:
        {
            // Выселение постояльца
            cout << "Занятые номера:\n";
            bool foundOccupied = false;
            // Выводим только занятые номера
            for (const auto& room : roomManager.getRooms())
            {
                if (!roomManager.isAvailable(room.first))
                {  // Если номер занят
                    cout << "Номер: " << room.first
                        << " Тип: " << room.second.first
                        << " Цена: " << room.second.second
                        << " руб\n";
                    foundOccupied = true;
                }
            }

            if (!foundOccupied)
            {
                cout << "Нет занятых номеров для выселения.\n";
                break;
            }

            cout << "Введите номер комнаты, которую хотите освободить: ";
            string roomNum;
            cin >> roomNum;

            // Проверяем, свободен ли номер
            if (roomManager.isAvailable(roomNum))
            {
                cout << "Этот номер уже свободен.\n";
                break;
            }

            // Освобождаем номер
            roomManager.setVacant(roomNum);  // Устанавливаем статус комнаты как свободной
            cout << "Номер " << roomNum << " успешно освобожден.\n";
            break;
        }

        case 0:
            return 0;  // Завершаем программу

        default:
            cout << "Неверный выбор. Попробуйте снова.\n";  // Обработка неверного выбора
            break;
        }
    }

    return 0;  // Завершаем программу
}
