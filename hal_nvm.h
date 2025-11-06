/**
 * @file hal_nvm.h
 * @brief Аппаратный уровень (HAL) для энергонезависимой памяти (NVM).
 *
 * Этот модуль предоставляет интерфейс для чтения и записи данных
 * в энергонезависимую память.
 */

#ifndef HAL_NVM_H
#define HAL_NVM_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Выполняет инициализацию подсистемы NVM.
 *
 * @return HAL_SUCCESS в случае успеха.
 */
int nvm_init(void);

/**
 * @brief Записывает блок данных в NVM.
 *
 * @param address Адрес для начала записи.
 * @param data Указатель на буфер с данными для записи.
 * @param len Размер данных для записи в байтах.
 * @return HAL_SUCCESS в случае успеха.
 */
int nvm_write(uint32_t address, const uint8_t* data, size_t len);

/**
 * @brief Читает блок данных из NVM.
 *
 * @param address Адрес для начала чтения.
 * @param data Указатель на буфер, в который будут помещены прочитанные данные.
 * @param len Размер данных для чтения в байтах.
 * @return HAL_SUCCESS в случае успеха.
 */
int nvm_read(uint32_t address, uint8_t* data, size_t len);

#endif // HAL_NVM_H
