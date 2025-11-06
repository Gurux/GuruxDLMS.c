/**
 * @file exchange.h
 * @brief Протокол обмена данными (Dataset/Datapoint), отделенный от транспорта.
 */

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Обрабатывает входящий пакет с запросом/командой и формирует ответ.
 *
 * Эта функция является точкой входа для модуля обмена. Она парсит запрос,
 * выполняет соответствующее действие (чтение/запись/команду), обрабатывает
 * логику многоблочной передачи и формирует пакет с ответом.
 *
 * @param request_data Указатель на буфер с данными запроса, полученными от транспорта.
 * @param request_len Длина данных запроса.
 * @param response_buffer Указатель на буфер, предоставленный приложением для формирования ответа.
 * @param response_len Указатель на переменную, куда будет записана фактическая длина ответа.
 * @param client_context Непрозрачный указатель на контекст клиента. Модуль обмена
 *                       будет использовать этот контекст для хранения состояния
 *                       многоблочных передач для каждого клиента.
 * @return HAL_SUCCESS в случае успеха, или код ошибки.
 */
int exchange_handle_request(
    uint8_t* request_data,
    size_t request_len,
    uint8_t* response_buffer,
    size_t* response_len,
    void* client_context);

#endif // EXCHANGE_H
