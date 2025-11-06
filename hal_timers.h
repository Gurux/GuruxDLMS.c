/**
 * @file hal_timers.h
 * @brief Аппаратный уровень (HAL) для системных таймеров.
 *
 * Этот модуль предоставляет интерфейс для создания и управления таймерами,
 * которые вызывают заданную callback-функцию по истечении заданного интервала.
 */

#ifndef HAL_TIMERS_H
#define HAL_TIMERS_H

#include <stdint.h>
#include <stddef.h>

/**
 * @typedef timer_t
 * @brief Непрозрачный указатель на объект таймера.
 */
typedef struct timer_s* timer_t;

/**
 * @brief Callback-функция, вызываемая по истечении времени таймера.
 * @param timer Сработавший таймер.
 * @param user_data Пользовательские данные, связанные с таймером.
 */
typedef void (*timer_callback_t)(timer_t timer, void* user_data);

/**
 * @brief Создает новый таймер.
 *
 * Таймер не запускается автоматически после создания. Для запуска необходимо
 * вызвать функцию `timer_start`.
 *
 * @param callback Функция, которая будет вызвана по истечении времени.
 * @param user_data Указатель на пользовательские данные, который будет передан в callback.
 * @return Указатель на созданный таймер или NULL в случае ошибки.
 */
timer_t timer_create(timer_callback_t callback, void* user_data);

/**
 * @brief Запускает или перезапускает таймер.
 *
 * @param timer Таймер для запуска.
 * @param timeout_ms Интервал в миллисекундах, через который таймер должен сработать.
 * @param periodic Если true, таймер будет автоматически перезапускаться после каждого срабатывания.
 *                 Если false, таймер сработает один раз.
 * @return HAL_SUCCESS в случае успеха.
 */
int timer_start(timer_t timer, uint32_t timeout_ms, uint8_t periodic);

/**
 * @brief Останавливает таймер.
 *
 * @param timer Таймер для остановки.
 * @return HAL_SUCCESS в случае успеха.
 */
int timer_stop(timer_t timer);

/**
 * @brief Уничтожает таймер и освобождает связанные с ним ресурсы.
 *
 * @param timer Таймер для уничтожения.
 * @return HAL_SUCCESS в случае успеха.
 */
int timer_destroy(timer_t timer);

#endif // HAL_TIMERS_H
