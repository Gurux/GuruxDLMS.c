/**
 * \file
 *
 * \brief SAM FPU
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
 */

#ifdef _SAME70_FPU_COMPONENT_
#ifndef _HRI_FPU_E70_H_INCLUDED_
#define _HRI_FPU_E70_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_FPU_CRITICAL_SECTIONS)
#define FPU_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define FPU_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define FPU_CRITICAL_SECTION_ENTER()
#define FPU_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_fpu_fpcar_reg_t;
typedef uint32_t hri_fpu_fpccr_reg_t;
typedef uint32_t hri_fpu_fpdscr_reg_t;

static inline void hri_fpu_set_FPCCR_LSPACT_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_LSPACT;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_LSPACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_LSPACT) >> 0;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_LSPACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_LSPACT;
	tmp |= value << 0;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_LSPACT_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_LSPACT;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_LSPACT_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_LSPACT;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_USER_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_USER;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_USER_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_USER) >> 1;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_USER_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_USER;
	tmp |= value << 1;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_USER_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_USER;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_USER_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_USER;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_THREAD_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_THREAD;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_THREAD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_THREAD) >> 3;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_THREAD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_THREAD;
	tmp |= value << 3;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_THREAD_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_THREAD;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_THREAD_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_THREAD;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_HFRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_HFRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_HFRDY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_HFRDY) >> 4;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_HFRDY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_HFRDY;
	tmp |= value << 4;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_HFRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_HFRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_HFRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_HFRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_MMRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_MMRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_MMRDY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_MMRDY) >> 5;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_MMRDY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_MMRDY;
	tmp |= value << 5;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_MMRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_MMRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_MMRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_MMRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_BFRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_BFRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_BFRDY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_BFRDY) >> 6;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_BFRDY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_BFRDY;
	tmp |= value << 6;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_BFRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_BFRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_BFRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_BFRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_MONRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_MONRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_MONRDY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_MONRDY) >> 8;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_MONRDY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_MONRDY;
	tmp |= value << 8;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_MONRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_MONRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_MONRDY_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_MONRDY;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_LSPEN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_LSPEN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_LSPEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_LSPEN) >> 30;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_LSPEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_LSPEN;
	tmp |= value << 30;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_LSPEN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_LSPEN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_LSPEN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_LSPEN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_ASPEN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= FPU_FPCCR_ASPEN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPCCR_ASPEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp = (tmp & FPU_FPCCR_ASPEN) >> 31;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPCCR_ASPEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= ~FPU_FPCCR_ASPEN;
	tmp |= value << 31;
	((Fpu *)hw)->FPCCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_ASPEN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~FPU_FPCCR_ASPEN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_ASPEN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= FPU_FPCCR_ASPEN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPCCR_reg(const void *const hw, hri_fpu_fpccr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR |= mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpccr_reg_t hri_fpu_get_FPCCR_reg(const void *const hw, hri_fpu_fpccr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCCR;
	tmp &= mask;
	return tmp;
}

static inline void hri_fpu_write_FPCCR_reg(const void *const hw, hri_fpu_fpccr_reg_t data)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR = data;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCCR_reg(const void *const hw, hri_fpu_fpccr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR &= ~mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCCR_reg(const void *const hw, hri_fpu_fpccr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCCR ^= mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpccr_reg_t hri_fpu_read_FPCCR_reg(const void *const hw)
{
	return ((Fpu *)hw)->FPCCR;
}

static inline void hri_fpu_set_FPCAR_ADDRESS_bf(const void *const hw, hri_fpu_fpcar_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCAR |= FPU_FPCAR_ADDRESS(mask);
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpcar_reg_t hri_fpu_get_FPCAR_ADDRESS_bf(const void *const hw, hri_fpu_fpcar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCAR;
	tmp = (tmp & FPU_FPCAR_ADDRESS(mask)) >> 3;
	return tmp;
}

static inline void hri_fpu_write_FPCAR_ADDRESS_bf(const void *const hw, hri_fpu_fpcar_reg_t data)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPCAR;
	tmp &= ~FPU_FPCAR_ADDRESS_Msk;
	tmp |= FPU_FPCAR_ADDRESS(data);
	((Fpu *)hw)->FPCAR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCAR_ADDRESS_bf(const void *const hw, hri_fpu_fpcar_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCAR &= ~FPU_FPCAR_ADDRESS(mask);
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCAR_ADDRESS_bf(const void *const hw, hri_fpu_fpcar_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCAR ^= FPU_FPCAR_ADDRESS(mask);
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpcar_reg_t hri_fpu_read_FPCAR_ADDRESS_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCAR;
	tmp = (tmp & FPU_FPCAR_ADDRESS_Msk) >> 3;
	return tmp;
}

static inline void hri_fpu_set_FPCAR_reg(const void *const hw, hri_fpu_fpcar_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCAR |= mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpcar_reg_t hri_fpu_get_FPCAR_reg(const void *const hw, hri_fpu_fpcar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPCAR;
	tmp &= mask;
	return tmp;
}

static inline void hri_fpu_write_FPCAR_reg(const void *const hw, hri_fpu_fpcar_reg_t data)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCAR = data;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPCAR_reg(const void *const hw, hri_fpu_fpcar_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCAR &= ~mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPCAR_reg(const void *const hw, hri_fpu_fpcar_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPCAR ^= mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpcar_reg_t hri_fpu_read_FPCAR_reg(const void *const hw)
{
	return ((Fpu *)hw)->FPCAR;
}

static inline void hri_fpu_set_FPDSCR_FZ_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR |= FPU_FPDSCR_FZ;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPDSCR_FZ_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp = (tmp & FPU_FPDSCR_FZ) >> 24;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPDSCR_FZ_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp &= ~FPU_FPDSCR_FZ;
	tmp |= value << 24;
	((Fpu *)hw)->FPDSCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPDSCR_FZ_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR &= ~FPU_FPDSCR_FZ;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPDSCR_FZ_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR ^= FPU_FPDSCR_FZ;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPDSCR_DN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR |= FPU_FPDSCR_DN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPDSCR_DN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp = (tmp & FPU_FPDSCR_DN) >> 25;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPDSCR_DN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp &= ~FPU_FPDSCR_DN;
	tmp |= value << 25;
	((Fpu *)hw)->FPDSCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPDSCR_DN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR &= ~FPU_FPDSCR_DN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPDSCR_DN_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR ^= FPU_FPDSCR_DN;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPDSCR_AHP_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR |= FPU_FPDSCR_AHP;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_fpu_get_FPDSCR_AHP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp = (tmp & FPU_FPDSCR_AHP) >> 26;
	return (bool)tmp;
}

static inline void hri_fpu_write_FPDSCR_AHP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp &= ~FPU_FPDSCR_AHP;
	tmp |= value << 26;
	((Fpu *)hw)->FPDSCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPDSCR_AHP_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR &= ~FPU_FPDSCR_AHP;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPDSCR_AHP_bit(const void *const hw)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR ^= FPU_FPDSCR_AHP;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_set_FPDSCR_RMode_bf(const void *const hw, hri_fpu_fpdscr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR |= FPU_FPDSCR_RMode(mask);
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpdscr_reg_t hri_fpu_get_FPDSCR_RMode_bf(const void *const hw, hri_fpu_fpdscr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp = (tmp & FPU_FPDSCR_RMode(mask)) >> 22;
	return tmp;
}

static inline void hri_fpu_write_FPDSCR_RMode_bf(const void *const hw, hri_fpu_fpdscr_reg_t data)
{
	uint32_t tmp;
	FPU_CRITICAL_SECTION_ENTER();
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp &= ~FPU_FPDSCR_RMode_Msk;
	tmp |= FPU_FPDSCR_RMode(data);
	((Fpu *)hw)->FPDSCR = tmp;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPDSCR_RMode_bf(const void *const hw, hri_fpu_fpdscr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR &= ~FPU_FPDSCR_RMode(mask);
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPDSCR_RMode_bf(const void *const hw, hri_fpu_fpdscr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR ^= FPU_FPDSCR_RMode(mask);
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpdscr_reg_t hri_fpu_read_FPDSCR_RMode_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp = (tmp & FPU_FPDSCR_RMode_Msk) >> 22;
	return tmp;
}

static inline void hri_fpu_set_FPDSCR_reg(const void *const hw, hri_fpu_fpdscr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR |= mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpdscr_reg_t hri_fpu_get_FPDSCR_reg(const void *const hw, hri_fpu_fpdscr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Fpu *)hw)->FPDSCR;
	tmp &= mask;
	return tmp;
}

static inline void hri_fpu_write_FPDSCR_reg(const void *const hw, hri_fpu_fpdscr_reg_t data)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR = data;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_clear_FPDSCR_reg(const void *const hw, hri_fpu_fpdscr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR &= ~mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_fpu_toggle_FPDSCR_reg(const void *const hw, hri_fpu_fpdscr_reg_t mask)
{
	FPU_CRITICAL_SECTION_ENTER();
	((Fpu *)hw)->FPDSCR ^= mask;
	FPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_fpu_fpdscr_reg_t hri_fpu_read_FPDSCR_reg(const void *const hw)
{
	return ((Fpu *)hw)->FPDSCR;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_FPU_E70_H_INCLUDED */
#endif /* _SAME70_FPU_COMPONENT_ */
