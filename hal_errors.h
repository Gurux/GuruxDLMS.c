/**
 * @file hal_errors.h
 * @brief Общие коды ошибок для HAL.
 */

#ifndef HAL_ERRORS_H
#define HAL_ERRORS_H

#define HAL_SUCCESS 0               /**< Операция выполнена успешно. */
#define HAL_ERROR_GENERIC -1        /**< Общая (неспецифическая) ошибка. */
#define HAL_ERROR_INVALID_PARAM -2  /**< Передан неверный параметр. */
#define HAL_ERROR_NO_MEMORY -3      /**< Ошибка выделения памяти. */
#define HAL_ERROR_TIMEOUT -4        /**< Таймаут операции. */
#define HAL_ERROR_NOT_SUPPORTED -5  /**< Операция не поддерживается. */
#define HAL_ERROR_BUSY -6           /**< Ресурс занят. */

#endif // HAL_ERRORS_H
