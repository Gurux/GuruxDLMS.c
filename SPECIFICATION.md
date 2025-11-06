# Техническая спецификация API

## 1. Введение

Настоящий документ описывает низкоуровневый C API для встраиваемой системы на базе GD32H7. API предназначен для организации обмена данными с устройством через TCP/IP и последовательный порт (UART).

Основная цель API — предоставить простой, модульный и портируемый интерфейс для взаимодействия с аппаратными ресурсами и реализации протокола обмена данными верхнего уровня.

## 2. Архитектура

API спроектирован в виде двух четко разделенных уровней:

1.  **Аппаратный уровень (Hardware Abstraction Layer - HAL)**: Этот уровень предоставляет прямой, но унифицированный доступ к аппаратным ресурсам системы. Он включает в себя модули для работы с транспортными каналами (TCP, Serial), таймерами и энергонезависимой памятью (NVM).

2.  **Уровень обмена данными (Exchange Protocol)**: Этот уровень реализует **stateless**-логику протокола для структурированного обмена данными. Он представлен единственной функцией `exchange_handle_request`, которая принимает буфер с запросом и заполняет буфер для ответа. Этот уровень полностью отделен от транспорта.

Приложение выступает в роли связующего звена: оно получает данные от HAL, передает их в `exchange_handle_request` и отправляет полученный ответ обратно через HAL.

## 3. Аппаратный уровень (HAL)

HAL состоит из нескольких модулей, каждый из которых представлен соответствующим заголовочным файлом.

### 3.1. Коды ошибок (`hal_errors.h`)

Все функции HAL возвращают целочисленные коды состояния.

- `HAL_SUCCESS` (0): Операция успешно завершена.
- `HAL_ERROR_GENERIC` (-1): Общая или неизвестная ошибка.
- `HAL_ERROR_INVALID_PARAM` (-3): В функцию передан неверный параметр.
- `HAL_ERROR_IO` (-4): Ошибка ввода-вывода.

### 3.2. Транспортный уровень (`hal_transport.h`)

Модуль отвечает за создание серверов, принимающих входящие подключения. API является асинхронным и основан на callback-функциях.
- `transport_listen()`: Создает сервер (TCP или Serial).
- `transport_channel_register_callbacks()`: Регистрирует обработчики для событий канала (получение данных, отключение).
- `transport_channel_write()`: Асинхронно отправляет данные в канал.
- `transport_channel_disconnect()`: Закрывает канал.

### 3.3. Таймеры (`hal_timers.h`)

Модуль предоставляет возможность создавать программные таймеры (`timer_create`, `timer_start`, `timer_stop`, `timer_destroy`).

### 3.4. Энергонезависимая память (`hal_nvm.h`)

Модуль обеспечивает базовый интерфейс для работы с NVM (`nvm_init`, `nvm_write`, `nvm_read`).

## 4. Уровень обмена данными (`exchange.h`)

Этот модуль реализует протокол обмена и является полностью stateless.

### 4.1. Основная функция

- `int exchange_handle_request(...)`: Главная функция, которая обрабатывает один запрос.

Она принимает:
- `context`: Структуру с callback-функциями, реализованными приложением.
- `request_buf`: Указатель на буфер с входящим запросом.
- `response_buf`: Указатель на буфер, куда будет записан ответ.

Функция парсит заголовок запроса, проверяет его корректность и вызывает соответствующий callback (`on_data_request` или `on_command`) для фактического получения данных или выполнения действия. Затем она формирует пакет ответа (либо с данными, либо с кодом ошибки) в `response_buf`.

## 5. Формат бинарных пакетов

Все числовые значения передаются в порядке Little Endian.

### 5.1. Пакет Запроса (Клиент -> Устройство)

Общая структура:
```
[ Header (3 байта) | Payload (переменная длина) ]
```

**Header:**
- `Command Type` (1 байт): Тип команды.
  - `0x01`: Запрос данных (Read Dataset).
  - `0x02`: Выполнение команды (Execute Command).
- `Payload Length` (2 байта, uint16_t): Длина поля `Payload`.

**Payload для `0x01` (Read Dataset):**
- `Dataset ID` (2 байта, uint16_t): Идентификатор запрашиваемого датасета.

**Payload для `0x02` (Execute Command):**
- `Command ID` (2 байта, uint16_t): Идентификатор команды.
- `Parameters` (оставшаяся часть Payload): Бинарные параметры команды.

### 5.2. Пакет Ответа (Устройство -> Клиент)

Общая структура:
```
[ Header (3 байта) | Payload (переменная длина) ]
```

**Header:**
- `Status Code` (1 байт): Статус выполнения запроса.
  - `0x00`: Успех (OK).
  - `0x01`: Ошибка: неверный формат запроса.
  - `0x02`: Ошибка: неизвестный датасет или команда.
  - `0x03`: Ошибка: внутренняя ошибка выполнения.
- `Payload Length` (2 байта, uint16_t): Длина поля `Payload`.

**Payload при успехе (`Status Code = 0x00`):**
- Содержит запрошенные данные (для Read Dataset) или результат выполнения команды (для Execute Command). Формат этих данных определяется приложением.

**Payload при ошибке (`Status Code != 0x00`):**
- `Payload` пуст (`Payload Length = 0`).

## 6. Пример использования (псевдокод)

```c
#include "hal_transport.h"
#include "exchange.h"

#define RESPONSE_BUFFER_SIZE 1024

// --- Реализация callback'ов для exchange_context_t ---

// Callback для предоставления данных по запросу
int on_data_request_impl(uint16_t dataset_id, uint8_t* response_buf, size_t response_buf_size, size_t* actual_response_len, void* user_data) {
    if (dataset_id == 100) { // Пример: ID 100 - это серийный номер
        const char* serial = "SN12345";
        size_t len = strlen(serial);
        if (len > response_buf_size) {
            return -1; // Буфер слишком мал
        }
        memcpy(response_buf, serial, len);
        *actual_response_len = len;
        return 0; // Успех
    }
    return -2; // Неизвестный датасет
}

// --- Логика приложения ---

// Контекст для обработки запросов
exchange_context_t exchange_ctx = {
    .on_data_request = on_data_request_impl,
    .on_command = NULL, // Команды не обрабатываем в этом примере
    .user_data = NULL
};

// Вызывается при получении данных из транспорта
void on_data_received(transport_channel_t channel, uint8_t* data, size_t len, void* user_data) {
    uint8_t response_buf[RESPONSE_BUFFER_SIZE];
    size_t actual_response_len = 0;

    // Обрабатываем запрос и получаем ответ в response_buf
    int result = exchange_handle_request(&exchange_ctx,
                                         data, len,
                                         response_buf, RESPONSE_BUFFER_SIZE,
                                         &actual_response_len);

    if (result == 0 && actual_response_len > 0) {
        // Отправляем успешный ответ обратно клиенту
        transport_channel_write(channel, response_buf, actual_response_len);
    }
    // Если result != 0, exchange_handle_request уже сформировал пакет с ошибкой.
    // Мы все равно его отправляем.
    else if (result != 0) {
         transport_channel_write(channel, response_buf, actual_response_len);
    }
}

// Вызывается при новом подключении
void on_new_connection(transport_listener_t listener, transport_channel_t new_channel, void* user_data) {
    // Регистрируем обработчик для входящих данных.
    // user_data здесь не используется, но может быть полезен для хранения состояния сессии.
    transport_channel_register_callbacks(new_channel, on_data_received, NULL, NULL);
}

void main() {
    transport_tcp_listen_config_t tcp_config = { .port = 1234 };
    transport_listen(TRANSPORT_TYPE_TCP, &tcp_config, on_new_connection, NULL);

    while(1) {
        // ... цикл обработки событий ...
    }
}
```
