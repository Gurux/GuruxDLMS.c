/**
 * @file exchange.h
 * @brief Протокол обмена данными верхнего уровня.
 *
 * Этот модуль предоставляет единую stateless-функцию для обработки
 * входящих запросов и формирования ответов. Он полностью
 * отделен от транспортного уровня. Приложение отвечает за получение
 * буфера с запросом (например, из TCP сокета) и предоставление
 * этого буфера данной функции.
 */

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <stdint.h>
#include <stddef.h>

/**
 * @struct datapoint_t
 * @brief Описывает одну точку данных внутри датасета.
 */
typedef struct {
    uint16_t id;      /**< Уникальный идентификатор точки данных. */
    const char* name; /**< Имя точки данных. */
} datapoint_t;

/**
 * @struct dataset_t
 * @brief Описывает набор точек данных (датасет).
 */
typedef struct {
    uint16_t id;                /**< Уникальный идентификатор датасета. */
    const char* name;           /**< Имя датасета. */
    const datapoint_t* points;  /**< Массив точек данных. */
    size_t num_points;          /**< Количество точек в массиве. */
} dataset_t;

/**
 * @brief Callback-функция, реализуемая приложением для предоставления данных.
 *
 * Вызывается, когда парсер получает корректный запрос на данные.
 * Реализация этой функции должна записать запрошенные данные в `response_buf`.
 *
 * @param dataset_id ID запрошенного датасета.
 * @param response_buf Буфер для записи ответа.
 * @param response_buf_size Максимальный размер буфера ответа.
 * @param[out] actual_response_len Длина фактически записанных данных.
 * @param user_data Пользовательские данные.
 * @return 0 в случае успеха, иначе код ошибки.
 */
typedef int (*exchange_on_data_request_t)(uint16_t dataset_id, uint8_t* response_buf, size_t response_buf_size, size_t* actual_response_len, void* user_data);

/**
 * @brief Callback-функция, реализуемая приложением для выполнения команды.
 *
 * @param command_id ID команды для выполнения.
 * @param params Параметры команды.
 * @param params_len Длина параметров.
 * @param user_data Пользовательские данные.
 * @return 0 в случае успеха, иначе код ошибки.
 */
typedef int (*exchange_on_command_t)(uint16_t command_id, const uint8_t* params, size_t params_len, void* user_data);

/**
 * @struct exchange_context_t
 * @brief Контекст, содержащий callback-функции, необходимые для обработки запросов.
 */
typedef struct {
    exchange_on_data_request_t on_data_request; /**< Callback для запроса данных. */
    exchange_on_command_t on_command;           /**< Callback для выполнения команды. */
    void* user_data;                            /**< Пользовательские данные. */
} exchange_context_t;

/**
 * @brief Главная функция обработки запроса.
 *
 * Эта функция парсит входящий `request_buf`, определяет тип запроса (например,
 * чтение датасета) и вызывает соответствующий callback из `context`.
 * Результат выполнения callback'а (данные или код ошибки) форматируется
 * и записывается в `response_buf`.
 *
 * @param context Контекст с callback-функциями приложения.
 * @param request_buf Буфер с данными входящего запроса.
 * @param request_len Длина входящего запроса.
 * @param response_buf Буфер для записи ответа.
 * @param response_buf_size Максимальный размер буфера ответа.
 * @param[out] actual_response_len Длина фактически записанного ответа.
 * @return 0 в случае успеха, иначе код ошибки.
 */
int exchange_handle_request(const exchange_context_t* context,
                              const uint8_t* request_buf,
                              size_t request_len,
                              uint8_t* response_buf,
                              size_t response_buf_size,
                              size_t* actual_response_len);

#endif // EXCHANGE_H
