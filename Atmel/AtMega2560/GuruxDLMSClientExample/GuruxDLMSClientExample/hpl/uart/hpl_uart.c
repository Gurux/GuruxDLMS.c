/**
 * \file
 *
 * \brief SAM Serial Communication Interface
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
#include <compiler.h>
#include <hpl_uart_config.h>
#include <hpl_usart_async.h>

#include <hpl_usart_sync.h>
#include <peripheral_clk_config.h>
#include <utils.h>
#include <utils_assert.h>

#ifndef CONF_UART_0_ENABLE
#define CONF_UART_0_ENABLE 0
#endif
#ifndef CONF_UART_1_ENABLE
#define CONF_UART_1_ENABLE 0
#endif
#ifndef CONF_UART_2_ENABLE
#define CONF_UART_2_ENABLE 0
#endif
#ifndef CONF_UART_3_ENABLE
#define CONF_UART_3_ENABLE 0
#endif
#ifndef CONF_UART_4_ENABLE
#define CONF_UART_4_ENABLE 0
#endif

/** Amount of UART. */
#define UART_AMOUNT                                                                                                    \
	(CONF_UART_0_ENABLE + CONF_UART_1_ENABLE + CONF_UART_2_ENABLE + CONF_UART_3_ENABLE + CONF_UART_4_ENABLE)

/**
 * \brief Macro is used to fill uart configuration structure based on
 * its number
 *
 * \param[in] n The number of structures
 */

#define UART_CONFIGURATION(n)                                                                                          \
	{                                                                                                                  \
		n,                                                                                                             \
		    (CONF_UART_##n##_USCLKS << UART_MR_BRSRCCK_Pos) | UART_MR_PAR(CONF_UART_##n##_PARITY)                      \
		        | UART_MR_CHMODE(CONF_UART_##n##_CHMODE) | (CONF_UART_##n##_FILTER << UART_MR_FILTER_Pos),             \
		    UART_BRGR_CD(CONF_UART_##n##_BAUD_CD)                                                                      \
	}
/**
 * \brief UART configuration type
 */
struct uart_configuration {
	uint8_t             number;
	hri_uart_mr_reg_t   mr;
	hri_uart_brgr_reg_t brgr;
};

#if UART_AMOUNT < 1
/** Dummy array to pass compiling. */
static struct uart_configuration _uarts[1] = {{0}};
#else
/**
 * \brief Array of UART configurations
 */
static struct uart_configuration _uarts[] = {
#if CONF_UART_0_ENABLE == 1
    UART_CONFIGURATION(0),
#endif
#if CONF_UART_1_ENABLE == 1
    UART_CONFIGURATION(1),
#endif
#if CONF_UART_2_ENABLE == 1
    UART_CONFIGURATION(2),
#endif
#if CONF_UART_3_ENABLE == 1
    UART_CONFIGURATION(3),
#endif
#if CONF_UART_4_ENABLE == 1
    UART_CONFIGURATION(4),
#endif
};
#endif

static uint8_t _uart_get_irq_num(const void *const hw);
static uint8_t _get_uart_index(const void *const hw);
static uint8_t _uart_get_hardware_index(const void *const hw);

static int32_t     _uart_init(void *const hw);
static inline void _uart_enable(void *const hw);
static inline void _uart_disable(void *const hw);
static inline void _uart_deinit(void *const hw);
static uint16_t    _uart_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                             const enum usart_baud_rate_mode mode, const uint8_t fraction);
static void        _uart_set_baud_rate(void *const hw, const uint32_t baud_rate);
static void        _uart_set_data_order(void *const hw, const enum usart_data_order order);
static void        _uart_set_mode(void *const hw, const enum usart_mode mode);
static void        _uart_set_parity(void *const hw, const enum usart_parity parity);
static void        _uart_set_stop_bits(void *const hw, const enum usart_stop_bits stop_bits);
static void        _uart_set_character_size(void *const hw, const enum usart_character_size size);

/**
 * \brief Retrieve IRQ number for the given hardware instance
 * \param[in] p The pointer to interrupt parameter
 */
static uint8_t _uart_get_irq_num(const void *const hw)
{
	ASSERT(hw);
	uint8_t i;
	i = _uart_get_hardware_index(hw);
	switch (i) {
	case 0:
		return UART0_IRQn;

	case 1:
		return UART1_IRQn;
	case 2:
		return UART2_IRQn;

	case 3:
		return UART3_IRQn;

	case 4:
		return UART4_IRQn;
	default:
		ASSERT(false);
		return ERR_INVALID_DATA;
	}
}

/**
 * \brief Init irq param with the given twi hardware instance
 */
static void _uart_init_irq_param(const void *const hw, struct _usart_async_device *dev)
{
}

/**
 * \brief Initialize synchronous UART
 */
int32_t _usart_sync_init(struct _usart_sync_device *const device, void *const hw)
{
	ASSERT(device);
	ASSERT(hw);

	device->hw = hw;

	return _uart_init(hw);
}

/**
 * \brief Initialize asynchronous UART
 */
int32_t _uart_usart_async_init(struct _usart_async_device *const device, void *const hw)
{
	int32_t init_status;

	ASSERT(device);

	init_status = _uart_init(hw);
	if (init_status) {
		return init_status;
	}
	device->hw = hw;

	_uart_init_irq_param(hw, device);
	NVIC_DisableIRQ((IRQn_Type)_uart_get_irq_num(hw));
	NVIC_ClearPendingIRQ((IRQn_Type)_uart_get_irq_num(hw));
	NVIC_EnableIRQ((IRQn_Type)_uart_get_irq_num(hw));

	return ERR_NONE;
}

/**
 * \brief De-initialize UART
 */
void _usart_sync_deinit(struct _usart_sync_device *const device)
{
	ASSERT(device);
	_uart_deinit(device->hw);
}

/**
 * \brief De-initialize UART
 */
void _uart_usart_async_deinit(struct _usart_async_device *const device)
{
	ASSERT(device);
	NVIC_DisableIRQ((IRQn_Type)_uart_get_irq_num(device->hw));
	_uart_deinit(device->hw);
}

/**
 * \brief Calculate baud rate register value
 */
uint16_t _usart_sync_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                         const enum usart_baud_rate_mode mode, const uint8_t fraction)
{
	return _uart_calculate_baud_rate(baud, clock_rate, samples, mode, fraction);
}

/**
 * \brief Calculate baud rate register value
 */
uint16_t _uart_usart_async_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                               const enum usart_baud_rate_mode mode, const uint8_t fraction)
{
	return _uart_calculate_baud_rate(baud, clock_rate, samples, mode, fraction);
}

/**
 * \brief Enable UART sync module
 */
void _usart_sync_enable(struct _usart_sync_device *const device)
{
	ASSERT(device);
	_uart_enable(device->hw);
}

/**
 * \brief Enable UART async module
 */
void _uart_usart_async_enable(struct _usart_async_device *const device)
{
	ASSERT(device);
	_uart_enable(device->hw);
}

/**
 * \brief Disable UART sync module
 */
void _usart_sync_disable(struct _usart_sync_device *const device)
{
	ASSERT(device);
	_uart_disable(device->hw);
}

/**
 * \brief Disable UART async module
 */
void _uart_usart_async_disable(struct _usart_async_device *const device)
{
	ASSERT(device);
	_uart_disable(device->hw);
}

/**
 * \brief Set baud rate
 */
void _usart_sync_set_baud_rate(struct _usart_sync_device *const device, const uint32_t baud_rate)
{
	ASSERT(device);
	_uart_set_baud_rate(device->hw, baud_rate);
}

/**
 * \brief Set baud rate
 */
void _uart_usart_async_set_baud_rate(struct _usart_async_device *const device, const uint32_t baud_rate)
{
	ASSERT(device);
	_uart_set_baud_rate(device->hw, baud_rate);
}

/**
 * \brief Set data order
 */
void _usart_sync_set_data_order(struct _usart_sync_device *const device, const enum usart_data_order order)
{
	ASSERT(device);
	_uart_set_data_order(device->hw, order);
}

/**
 * \brief Set data order
 */
void _uart_usart_async_set_data_order(struct _usart_async_device *const device, const enum usart_data_order order)
{
	ASSERT(device);
	_uart_set_data_order(device->hw, order);
}

/**
 * \brief Set mode
 */
void _usart_sync_set_mode(struct _usart_sync_device *const device, const enum usart_mode mode)
{
	ASSERT(device);
	_uart_set_mode(device->hw, mode);
}

/**
 * \brief Set mode
 */
void _uart_usart_async_set_mode(struct _usart_async_device *const device, const enum usart_mode mode)
{
	ASSERT(device);
	_uart_set_mode(device->hw, mode);
}

/**
 * \brief Set parity
 */
void _usart_sync_set_parity(struct _usart_sync_device *const device, const enum usart_parity parity)
{
	ASSERT(device);
	_uart_set_parity(device->hw, parity);
}

/**
 * \brief Set parity
 */
void _uart_usart_async_set_parity(struct _usart_async_device *const device, const enum usart_parity parity)
{
	ASSERT(device);
	_uart_set_parity(device->hw, parity);
}

/**
 * \brief Set stop bits mode
 */
void _usart_sync_set_stop_bits(struct _usart_sync_device *const device, const enum usart_stop_bits stop_bits)
{
	ASSERT(device);
	_uart_set_stop_bits(device->hw, stop_bits);
}

/**
 * \brief Set stop bits mode
 */
void _uart_usart_async_set_stop_bits(struct _usart_async_device *const device, const enum usart_stop_bits stop_bits)
{
	ASSERT(device);
	_uart_set_stop_bits(device->hw, stop_bits);
}

/**
 * \brief Set character size
 */
void _usart_sync_set_character_size(struct _usart_sync_device *const device, const enum usart_character_size size)
{
	ASSERT(device);
	_uart_set_character_size(device->hw, size);
}

/**
 * \brief Set character size
 */
void _uart_usart_async_set_character_size(struct _usart_async_device *const device,
                                          const enum usart_character_size   size)
{
	ASSERT(device);
	_uart_set_character_size(device->hw, size);
}

/**
 * \brief Retrieve uart status
 */
uint32_t _usart_sync_get_status(const struct _usart_sync_device *const device)
{
	ASSERT(device);
	return (uint32_t)hri_uart_read_SR_reg(device->hw);
}

/**
 * \brief Retrieve uart status
 */
uint32_t _uart_usart_async_get_status(const struct _usart_async_device *const device)
{
	ASSERT(device);
	return (uint32_t)hri_uart_read_SR_reg(device->hw);
}

/**
 * \brief Write a byte to the given UART instance
 */
void _usart_sync_write_byte(struct _usart_sync_device *const device, uint8_t data)
{
	ASSERT(device);
	hri_uart_write_THR_reg(device->hw, (hri_uart_thr_reg_t)data);
}

/**
 * \brief Write a byte to the given UART instance
 */
void _uart_usart_async_write_byte(struct _usart_async_device *const device, uint8_t data)
{
	ASSERT(device);
	hri_uart_write_THR_reg(device->hw, (hri_uart_thr_reg_t)data);
}

/**
 * \brief Read a byte from the given UART instance
 */
uint8_t _usart_sync_read_byte(const struct _usart_sync_device *const device)
{
	ASSERT(device);
	return (uint8_t)(hri_uart_read_RHR_RXCHR_bf(device->hw));
}

/**
 * \brief Check if UART is ready to send next byte
 */
bool _usart_sync_is_ready_to_send(const struct _usart_sync_device *const device)
{
	ASSERT(device);
	return hri_uart_get_SR_TXRDY_bit(device->hw);
}

/**
 * \brief Check if USART transmission complete
 */
bool _usart_sync_is_transmit_done(const struct _usart_sync_device *const device)
{
	ASSERT(device);
	return hri_uart_get_SR_TXEMPTY_bit(device->hw);
}

/**
 * \brief Check if UART is ready to send next byte
 */
bool _uart_usart_async_is_byte_sent(const struct _usart_async_device *const device)
{
	ASSERT(device);
	return hri_uart_get_SR_TXRDY_bit(device->hw);
}

/**
 * \brief Check if there is data received by UART
 */
bool _usart_sync_is_byte_received(const struct _usart_sync_device *const device)
{
	ASSERT(device);
	return hri_uart_get_SR_RXRDY_bit(device->hw);
}

/**
 * \brief Set the state of flow control pins
 */
void _usart_sync_set_flow_control_state(struct _usart_sync_device *const     device,
                                        const union usart_flow_control_state state)
{
	ASSERT(device);
	(void)device;
	(void)state;
}

/**
 * \brief Set the state of flow control pins
 */
void _uart_usart_async_set_flow_control_state(struct _usart_async_device *const    device,
                                              const union usart_flow_control_state state)
{
	ASSERT(device);
	(void)device;
	(void)state;
}

/**
 * \brief Retrieve the state of flow control pins
 */
union usart_flow_control_state _usart_sync_get_flow_control_state(const struct _usart_sync_device *const device)
{
	ASSERT(device);
	(void)device;
	union usart_flow_control_state state;

	state.value           = 0;
	state.bit.unavailable = 1;
	return state;
}

/**
 * \brief Retrieve the state of flow control pins
 */
union usart_flow_control_state _uart_usart_async_get_flow_control_state(const struct _usart_async_device *const device)
{
	ASSERT(device);
	(void)device;
	union usart_flow_control_state state;

	state.value           = 0;
	state.bit.unavailable = 1;
	return state;
}

/**
 * \brief Enable data register empty interrupt
 */
void _uart_usart_async_enable_byte_sent_irq(struct _usart_async_device *const device)
{
	ASSERT(device);
	hri_uart_set_IMR_TXRDY_bit(device->hw);
}

/**
 * \brief Enable transmission complete interrupt
 */
void _uart_usart_async_enable_tx_done_irq(struct _usart_async_device *const device)
{
	ASSERT(device);
	hri_uart_set_IMR_TXEMPTY_bit(device->hw);
}

/**
 * \brief Retrieve ordinal number of the given uart hardware instance
 * \param[in] p The pointer to interrupt parameter
 */
static uint8_t _uart_get_hardware_index(const void *const hw)
{
	ASSERT(hw);
#ifdef _UNIT_TEST_
	return ((uint32_t)hw - (uint32_t)UART0) / sizeof(Uart);
#endif
	if (UART0 == hw) {
		return 0;
	} else if (UART1 == hw) {
		return 1;
	} else if (UART2 == hw) {
		return 2;
	} else if (UART3 == hw) {
		return 3;
	} else if (UART4 == hw) {
		return 4;
	} else {
		return ERR_INVALID_DATA;
	}
}

/**
 * \brief Retrieve ordinal number of the given UART hardware instance
 */
uint8_t _usart_sync_get_hardware_index(const struct _usart_sync_device *const device)
{
	ASSERT(device);
	return _uart_get_hardware_index(device->hw);
}

/**
 * \brief Retrieve ordinal number of the given UART hardware instance
 */
uint8_t _uart_usart_async_get_hardware_index(const struct _usart_async_device *const device)
{
	ASSERT(device);
	return _uart_get_hardware_index(device->hw);
}

/**
 * \brief Enable/disable UART interrupt
 */
void _uart_usart_async_set_irq_state(struct _usart_async_device *const     device,
                                     const enum _usart_async_callback_type type, const bool state)
{
	ASSERT(device);
	if (state) {
		if (USART_ASYNC_BYTE_SENT == type || USART_ASYNC_TX_DONE == type) {
			hri_uart_set_IMR_TXRDY_bit(device->hw);
			hri_uart_set_IMR_TXEMPTY_bit(device->hw);
		} else if (USART_ASYNC_RX_DONE == type) {
			hri_uart_set_IMR_RXRDY_bit(device->hw);
		} else if (USART_ASYNC_ERROR == type) {
			UART_CRITICAL_SECTION_ENTER();
			((Uart *)(device->hw))->UART_IER |= (UART_SR_OVRE | UART_SR_FRAME | UART_SR_PARE);
			UART_CRITICAL_SECTION_LEAVE();
		}
	} else {
		if (USART_ASYNC_BYTE_SENT == type || USART_ASYNC_TX_DONE == type) {
			hri_uart_clear_IMR_TXRDY_bit(device->hw);
			hri_uart_clear_IMR_TXEMPTY_bit(device->hw);
		} else if (USART_ASYNC_RX_DONE == type) {
			hri_uart_clear_IMR_RXRDY_bit(device->hw);
		} else if (USART_ASYNC_ERROR == type) {
			UART_CRITICAL_SECTION_ENTER();
			((Usart *)(device->hw))->US_IDR |= (UART_SR_OVRE | UART_SR_FRAME | UART_SR_PARE);
			UART_CRITICAL_SECTION_LEAVE();
		}
	}
}

/**
 * \brief Retrieve usart sync helper functions
 */
void *_uart_get_usart_sync(void)
{
	return (void *)NULL;
}

/**
 * \brief Retrieve usart async helper functions
 */
void *_uart_get_usart_async(void)
{
	return (void *)NULL;
}

/**
 * \brief Retrieve usart dma helper functions
 */
void *_uart_get_usart_dma(void)
{
	return (void *)NULL;
}

/**
 * \internal Uart interrupt handler
 *
 * \param[in] p The pointer to interrupt parameter
 */
static void _uart_interrupt_handler(struct _usart_async_device *device)
{
	ASSERT(device);
	void *hw = device->hw;

	if (hri_uart_get_SR_TXRDY_bit(hw) && hri_uart_get_IMR_TXRDY_bit(hw)) {
		hri_uart_clear_IMR_TXRDY_bit(hw);
		device->usart_cb.tx_byte_sent(device);
	} else if (hri_uart_get_SR_TXEMPTY_bit(hw) && hri_uart_get_IMR_TXEMPTY_bit(hw)) {
		hri_uart_clear_IMR_TXEMPTY_bit(hw);
		device->usart_cb.tx_done_cb(device);
	} else if (hri_uart_get_SR_RXRDY_bit(hw) && hri_uart_get_IMR_RXRDY_bit(hw)) {
		if (hri_uart_read_SR_reg(hw) & (UART_SR_OVRE | UART_SR_FRAME | UART_SR_PARE)) {
			hri_uart_read_RHR_reg(hw);
			hri_uart_write_CR_reg(hw, UART_CR_RSTSTA);
			return;
		}
		device->usart_cb.rx_done_cb(device, (uint8_t)hri_uart_read_RHR_RXCHR_bf(hw));
	} else if (hri_uart_read_SR_reg(hw) & (UART_SR_OVRE | UART_SR_FRAME | UART_SR_PARE)) {
		hri_uart_write_CR_reg(hw, UART_CR_RSTSTA);
		device->usart_cb.error_cb(device);
	}
}

/**
 * \internal Retrieve ordinal number of the given uart hardware instance
 *
 * \param[in] hw The pointer to hardware instance

 * \return The ordinal number of the given uart hardware instance
 */
static uint8_t _get_uart_index(const void *const hw)
{
	ASSERT(hw);
	uint8_t uart_offset = _uart_get_hardware_index(hw);
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_uarts); i++) {
		if (_uarts[i].number == uart_offset) {
			return i;
		}
	}

	ASSERT(false);
	return 0;
}

/**
 * \internal Initialize UART
 *
 * \param[in] hw The pointer to hardware instance
 *
 * \return The status of initialization
 */
static int32_t _uart_init(void *const hw)
{
	ASSERT(hw);
	uint8_t i = _get_uart_index(hw);
	/* Disable the Write Protect. */
	hri_uart_write_WPMR_reg(hw, UART_WPMR_WPKEY_PASSWD);

	/* Disable and reset TX and RX. */
	hri_uart_write_CR_reg(hw, UART_CR_RSTRX | UART_CR_RXDIS | UART_CR_RSTTX | UART_CR_TXDIS);
	/* Reset status bits. */
	hri_uart_write_CR_reg(hw, UART_CR_RSTSTA);

	hri_uart_write_MR_reg(hw, _uarts[i].mr);
	hri_uart_write_BRGR_reg(hw, _uarts[i].brgr);

	return ERR_NONE;
}

/**
 * \internal De-initialize UART
 *
 * \param[in] hw The pointer to hardware instance
 */
static inline void _uart_deinit(void *const hw)
{
	ASSERT(hw);
	hri_uart_write_CR_reg(hw, UART_CR_RSTRX);
	hri_uart_write_CR_reg(hw, UART_CR_RSTTX);
	hri_uart_write_CR_reg(hw, UART_CR_RSTSTA);
}

/**
 * \internal Enable UART
 *
 * \param[in] hw The pointer to hardware instance
 */
static inline void _uart_enable(void *const hw)
{
	ASSERT(hw);
	hri_uart_write_CR_reg(hw, UART_CR_RXEN | UART_CR_TXEN);
}

/**
 * \internal Disable UART
 *
 * \param[in] hw The pointer to hardware instance
 */
static inline void _uart_disable(void *const hw)
{
	ASSERT(hw);
	hri_uart_write_CR_reg(hw, UART_CR_RXDIS | UART_CR_TXDIS);
}

/**
 * \internal Calculate baud rate register value
 *
 * \param[in] baud Required baud rate
 * \param[in] clock_rate UART clock frequency
 * \param[in] samples The number of samples
 * \param[in] mode UART mode
 * \param[in] fraction A fraction value
 *
 * \return Calculated baud rate register value
 */
static uint16_t _uart_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                          const enum usart_baud_rate_mode mode, const uint8_t fraction)
{
	if (USART_BAUDRATE_ASYNCH_ARITHMETIC == mode) {
		return clock_rate / baud / samples;
	}

	if (USART_BAUDRATE_ASYNCH_FRACTIONAL == mode) {
		return clock_rate / baud / samples + US_BRGR_FP(fraction);
	}

	if (USART_BAUDRATE_SYNCH == mode) {
		return clock_rate / baud;
	}

	return 0;
}

/**
 * \internal Set baud rate
 *
 * \param[in] device The pointer to UART device instance
 * \param[in] baud_rate A baud rate to set
 */
static void _uart_set_baud_rate(void *const hw, const uint32_t baud_rate)
{
	ASSERT(hw);
	uint32_t uart_freq = 0, cd = 0;
	uint8_t  i = _get_uart_index(hw);
	switch (i) {
	case 0:
#ifdef CONF_UART0_FREQUENCY
		uart_freq = CONF_UART0_FREQUENCY;
#endif
		break;

	case 1:
#ifdef CONF_UART1_FREQUENCY
		uart_freq = CONF_UART1_FREQUENCY;
#endif
		break;

	case 2:
#ifdef CONF_UART2_FREQUENCY
		uart_freq = CONF_UART2_FREQUENCY;
#endif
		break;

	case 3:
#ifdef CONF_UART3_FREQUENCY
		uart_freq = CONF_UART3_FREQUENCY;
#endif
		break;

	case 4:
#ifdef CONF_UART4_FREQUENCY
		uart_freq = CONF_UART4_FREQUENCY;
#endif
		break;

	default:
		ASSERT(false);
	}

	cd = uart_freq / baud_rate / 16;

	hri_uart_write_BRGR_CD_bf(hw, (hri_uart_brgr_reg_t)cd);
}

/**
 * \internal Set data order
 *
 * \param[in] device The pointer to UART device instance
 * \param[in] order A data order to set
 */
static void _uart_set_data_order(void *const hw, const enum usart_data_order order)
{
	ASSERT(hw);
	(void)hw;
	(void)order;
}

/**
 * \internal Set mode
 *
 * \param[in] device The pointer to UART device instance
 * \param[in] mode A mode to set
 */
static void _uart_set_mode(void *const hw, const enum usart_mode mode)
{
	ASSERT(hw);
	(void)hw;
	(void)mode;
}

/**
 * \internal Set parity
 *
 * \param[in] device The pointer to UART device instance
 * \param[in] parity A parity to set
 */
static void _uart_set_parity(void *const hw, const enum usart_parity parity)
{
	ASSERT(hw);
	hri_uart_write_WPMR_reg(hw, US_WPMR_WPKEY_PASSWD);
	switch (parity) {
	case USART_PARITY_EVEN:
		hri_uart_write_MR_PAR_bf(hw, (hri_uart_mr_reg_t)0x0u);
		break;

	case USART_PARITY_ODD:
		hri_uart_write_MR_PAR_bf(hw, (hri_uart_mr_reg_t)0x1u);
		break;

	case USART_PARITY_SPACE:
		hri_uart_write_MR_PAR_bf(hw, (hri_uart_mr_reg_t)0x2u);
		break;

	case USART_PARITY_MARK:
		hri_uart_write_MR_PAR_bf(hw, (hri_uart_mr_reg_t)0x3u);
		break;

	case USART_PARITY_NONE:
		hri_uart_write_MR_PAR_bf(hw, (hri_uart_mr_reg_t)0x4u);
		break;

	default:
		ASSERT(false);
	}
}

/**
 * \internal Set stop bits mode
 *
 * \param[in] device The pointer to UART device instance
 * \param[in] stop_bits A stop bits mode to set
 */
static void _uart_set_stop_bits(void *const hw, const enum usart_stop_bits stop_bits)
{
	ASSERT(hw);
	(void)hw;
	(void)stop_bits;
}

/**
 * \internal Set character size
 *
 * \param[in] device The pointer to UART device instance
 * \param[in] size A character size to set
 */
static void _uart_set_character_size(void *const hw, const enum usart_character_size size)
{
	ASSERT(hw);
	(void)hw;
	(void)size;
}
