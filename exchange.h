/**
 * @file exchange.h
 * @brief Протокол обмена данными (Dataset/Datapoint).
 */

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Регистрирует callback-функцию для обработки запроса на чтение данных.
 * @param dataset Номер датасета.
 * @param datapoint Номер датапоинта.
 * @param handler Указатель на функцию-обработчик.
 */
// void exchange_register_read_handler(uint16_t dataset, uint16_t datapoint, read_handler_t handler);


/**
 * @brief Обрабатывает входящий пакет с запросом/командой.
 *
 * Эта функция является точкой входа для модуля обмена. Она парсит запрос,
 * выполняет соответствующее действие (чтение/запись/команду) и формирует
 * пакет с ответом.
 *
 * @param request_data Указатель на буфер с данными запроса.
 * @param request_len Длина данных запроса.
 * @param response_buffer Указатель на буфер для формирования ответа.
 * @param response_len Указатель на переменную, куда будет записана длина ответа.
 * @param client_context Непрозрачный указатель на контекст клиента для поддержки
 *                       состояния многоблочных передач.
 * @return HAL_SUCCESS в случае успеха.
 */
int exchange_handle_request(
    uint8_t* request_data,
    size_t request_len,
    uint8_t* response_buffer,
    size_t* response_len,
    void* client_context);

#endif // EXCHANGE_H
