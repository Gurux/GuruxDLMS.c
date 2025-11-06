/**
 * @file hal_errors.h
 * @brief Определения кодов ошибок для HAL.
 */

#ifndef HAL_ERRORS_H
#define HAL_ERRORS_H

/**
 * @brief Определяет стандартные коды возврата для функций HAL.
 */
#define HAL_SUCCESS 0         /**< Операция успешно завершена. */
#define HAL_ERROR_GENERIC -1  /**< Общая ошибка. */
#define HAL_ERROR_NOMEM -2    /**< Недостаточно памяти. */
#define HAL_ERROR_INVALID_PARAM -3 /**< Неверный параметр. */
#define HAL_ERROR_IO -4       /**< Ошибка ввода-вывода. */
#define HAL_ERROR_TIMEOUT -5  /**< Таймаут операции. */

#endif // HAL_ERRORS_H
