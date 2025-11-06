/**
 * @file hal_nvm.h
 * @brief Аппаратный уровень (HAL) для энергонезависимой памяти (NVM).
 */

#ifndef HAL_NVM_H
#define HAL_NVM_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Инициализирует модуль NVM.
 * @return HAL_SUCCESS в случае успеха.
 */
int nvm_init();

/**
 * @brief Читает блок данных из NVM.
 * @param address Адрес начала чтения.
 * @param buffer Указатель на буфер для сохранения прочитанных данных.
 * @param size Количество байт для чтения.
 * @return HAL_SUCCESS в случае успеха.
 */
int nvm_read(uint32_t address, uint8_t* buffer, size_t size);

/**
 * @brief Записывает блок данных в NVM.
 * @param address Адрес начала записи.
 * @param buffer Указатель на буфер с данными для записи.
 * @param size Количество байт для записи.
 * @return HAL_SUCCESS в случае успеха.
 */
int nvm_write(uint32_t address, const uint8_t* buffer, size_t size);

/**
 * @brief Стирает сектор NVM.
 * @param address Адрес начала сектора для стирания.
 * @param size Размер стираемого сектора.
 * @return HAL_SUCCESS в случае успеха.
 */
int nvm_erase(uint32_t address, size_t size);

#endif // HAL_NVM_H
