/**
 * @file hal_timers.h
 * @brief Аппаратный уровень (HAL) для аппаратных таймеров.
 */

#ifndef HAL_TIMERS_H
#define HAL_TIMERS_H

#include <stdint.h>

/**
 * @enum timer_type_t
 * @brief Определяет тип таймера.
 */
typedef enum {
    TIMER_TYPE_ONE_SHOT, /**< Таймер сработает один раз и остановится. */
    TIMER_TYPE_PERIODIC  /**< Таймер будет перезапускаться автоматически. */
} timer_type_t;

/**
 * @typedef timer_t
 * @brief Непрозрачный указатель на объект таймера.
 */
typedef struct timer_s* timer_t;

/**
 * @brief Callback, вызываемый по истечении времени таймера.
 * @param timer Таймер, который вызвал callback.
 * @param user_data Пользовательские данные, связанные с таймером.
 */
typedef void (*timer_callback_t)(timer_t timer, void* user_data);

/**
 * @brief Инициализирует модуль таймеров.
 * @return HAL_SUCCESS в случае успеха.
 */
int timers_init();

/**
 * @brief Создает новый таймер.
 * @param type Тип таймера (однократный или периодический).
 * @param callback Функция, которая будет вызвана по истечении интервала.
 * @param user_data Указатель на пользовательские данные для передачи в callback.
 * @return Указатель на созданный таймер или NULL в случае ошибки.
 */
timer_t timer_create(timer_type_t type, timer_callback_t callback, void* user_data);

/**
 * @brief Запускает таймер.
 * @param timer Указатель на таймер.
 * @param interval_ms Интервал в миллисекундах.
 * @return HAL_SUCCESS в случае успеха.
 */
int timer_start(timer_t timer, uint32_t interval_ms);

/**
 * @brief Останавливает таймер.
 * @param timer Указатель на таймер.
 * @return HAL_SUCCESS в случае успеха.
 */
int timer_stop(timer_t timer);

/**
 * @brief Удаляет таймер и освобождает связанные с ним ресурсы.
 * @param timer Указатель на таймер.
 * @return HAL_SUCCESS в случае успеха.
 */
int timer_delete(timer_t timer);

#endif // HAL_TIMERS_H
