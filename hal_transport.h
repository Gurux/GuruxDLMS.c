/**
 * @file hal_transport.h
 * @brief Аппаратный уровень (HAL) для транспортных каналов (TCP и Serial).
 *
 * Этот модуль предоставляет унифицированный интерфейс для создания серверов,
 * принимающих подключения по TCP и UART.
 */

#ifndef HAL_TRANSPORT_H
#define HAL_TRANSPORT_H

#include <stdint.h>
#include <stddef.h>

/**
 * @enum transport_type_t
 * @brief Определяет тип транспортного канала.
 */
typedef enum {
    TRANSPORT_TYPE_TCP,     /**< TCP/IP канал. */
    TRANSPORT_TYPE_SERIAL   /**< Последовательный (UART) канал. */
} transport_type_t;

/**
 * @enum transport_state_t
 * @brief Определяет состояние транспортного канала.
 */
typedef enum {
    TRANSPORT_STATE_DISCONNECTED, /**< Канал отключен. */
    TRANSPORT_STATE_CONNECTED     /**< Канал подключен. */
} transport_state_t;

/**
 * @typedef transport_channel_t
 * @brief Непрозрачный указатель на объект канала (установленное соединение).
 */
typedef struct transport_channel_s* transport_channel_t;

/**
 * @typedef transport_listener_t
 * @brief Непрозрачный указатель на объект листенера (сервера).
 */
typedef struct transport_listener_s* transport_listener_t;

/**
 * @brief Callback, вызываемый при получении данных по каналу.
 * @param channel Канал, по которому пришли данные.
 * @param data Указатель на буфер с данными.
 * @param len Размер данных в байтах.
 * @param user_data Пользовательские данные, связанные с каналом.
 */
typedef void (*transport_on_data_received_t)(transport_channel_t channel, uint8_t* data, size_t len, void* user_data);

/**
 * @brief Callback, вызываемый при изменении состояния канала (например, отключении).
 * @param channel Канал, состояние которого изменилось.
 * @param state Новое состояние канала.
 * @param user_data Пользовательские данные, связанные с каналом.
 */
typedef void (*transport_on_state_changed_t)(transport_channel_t channel, transport_state_t state, void* user_data);

/**
 * @brief Callback, вызываемый листенером при новом входящем подключении.
 * @param listener Лисенер, который принял подключение.
 * @param new_channel Новый канал, созданный для этого клиента.
 * @param user_data Пользовательские данные, связанные с листенером.
 */
typedef void (*transport_on_new_connection_t)(transport_listener_t listener, transport_channel_t new_channel, void* user_data);


/** @struct transport_tcp_listen_config_t
 *  @brief Конфигурация для TCP листенера.
 */
typedef struct {
    uint16_t port; /**< TCP порт для прослушивания. */
} transport_tcp_listen_config_t;

// ... (Doxygen для Serial config)

/**
 * @brief Создает и запускает лисенер (сервер).
 * @param type Тип транспорта.
 * @param config Указатель на структуру конфигурации.
 * @param on_connection Callback для новых подключений.
 * @param user_data Пользовательские данные для листенера.
 * @return Указатель на созданный лисенер или NULL в случае ошибки.
 */
transport_listener_t transport_listen(transport_type_t type, void* config, transport_on_new_connection_t on_connection, void* user_data);

/**
 * @brief Регистрирует коллбэки для событий на конкретном канале.
 * @param channel Канал.
 * @param on_data Callback на получение данных.
 * @param on_state Callback на изменение состояния.
 * @param user_data Пользовательские данные, которые будут передаваться в коллбэки канала.
 * @return HAL_SUCCESS в случае успеха.
 */
int transport_channel_register_callbacks(transport_channel_t channel, transport_on_data_received_t on_data, transport_on_state_changed_t on_state, void* user_data);

/**
 * @brief Асинхронно отправляет данные через канал.
 * @param channel Канал для отправки.
 * @param data Указатель на буфер с данными.
 * @param len Размер данных.
 * @return HAL_SUCCESS, если отправка успешно начата.
 */
int transport_channel_write(transport_channel_t channel, const uint8_t* data, size_t len);


#endif // HAL_TRANSPORT_H
