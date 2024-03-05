
/**
 * @file opensbi_interface.h
 * @brief opensbi 接口头文件
 * @author Zone.N (Zone.Niuzh@hotmail.com)
 * @version 1.0
 * @date 2024-03-05
 * @copyright MIT LICENSE
 * https://github.com/MRNIU/opensbi_interface
 * @par change log:
 * <table>
 * <tr><th>Date<th>Author<th>Description
 * <tr><td>2024-03-05<td>Zone.N<td>创建文件
 * </table>
 */

#ifndef OPENSBI_INTERFACE_SRC_INCLUDE_OPENSBI_INTERFACE_H
#define OPENSBI_INTERFACE_SRC_INCLUDE_OPENSBI_INTERFACE_H

#include <stdint.h>

// SBI return error codes
#define SBI_SUCCESS 0
#define SBI_ERR_FAILED -1
#define SBI_ERR_NOT_SUPPORTED -2
#define SBI_ERR_INVALID_PARAM -3
#define SBI_ERR_DENIED -4
#define SBI_ERR_INVALID_ADDRESS -5
#define SBI_ERR_ALREADY_AVAILABLE -6
#define SBI_ERR_ALREADY_STARTED -7
#define SBI_ERR_ALREADY_STOPPED -8
#define SBI_ERR_NO_SHMEM -9

// SBI functions must return a pair of values in a0 and a1, with a0 returning an
// error code.
struct sbiret {
  long error;
  long value;
};

// ----------------------------------------------------------------------------
// Base Extension (EID #0x10)
// The base extension is designed to be as small as possible. As such, it only
// contains functionality for probing which SBI extensions are available and for
// querying the version of the SBI. All functions in the base extension must be
// supported by all SBI implementations, so there are no error returns defined.

/**
 * @brief Get SBI specification version (FID #0)
 * @return struct sbiret.error  Always 0
 * @return struct sbiret.value  The minor number of the SBI specification is
 * encoded in the low 24 bits, with the major number encoded in the next 7 bits.
 * Bit 31 must be 0 and is reserved for future expansion.
 */
struct sbiret sbi_get_spec_version(void);

const char *SBI_IMPL_ID_NAMES[] = {
    "Berkeley Boot Loader (BBL)",
    "OpenSBI",
    "Xvisor",
    "KVM",
    "RustSBI",
    "Diosix",
    "Coffer",
    "Xen Project",
    "PolarFire Hart Software Services",
};

/**
 * @brief Get SBI implementation ID (FID #1)
 * @return sbiret.error  Always 0
 * @return sbiret.value  Returns the current SBI implementation ID, which is
 * different for every SBI implementation.
 */
struct sbiret sbi_get_impl_id(void);

/**
 * @brief Get SBI implementation version (FID #2)
 * @return sbiret.error  Always 0
 * @return sbiret.value  Returns the current SBI implementation version.
 */
struct sbiret sbi_get_impl_version(void);

/**
 * @brief Probe SBI extension (FID #3)
 * @return sbiret.error  Always 0
 * @return sbiret.value  Returns 0 if the given SBI extension ID (EID) is not
 * available, or 1 if it is available unless defined as any other non-zero value
 * by the implementation.
 */
struct sbiret sbi_probe_extension(long extension_id);

/**
 * @brief Get machine vendor ID (FID #4)
 * @return sbiret.error  Always 0
 * @return sbiret.value  Return a value that is legal for the `mvendorid` CSR
 * and 0 is always a legal value for this CSR.
 */
struct sbiret sbi_get_mvendorid(void);

/**
 * @brief Get machine architecture ID (FID #5)
 * @return sbiret.error  Always 0
 * @return sbiret.value  Return a value that is legal for the `marchid` CSR and
 * 0 is always a legal value for this CSR.
 */
struct sbiret sbi_get_marchid(void);

/**
 * @brief Get machine implementation ID (FID #6)
 * @return sbiret.error  Always 0
 * @return sbiret.value  Return a value that is legal for the `mimpid` CSR and 0
 * is always a legal value for this CSR.
 */
struct sbiret sbi_get_mimpid(void);

#if 0
// ----------------------------------------------------------------------------
// Legacy Extensions (EIDs #0x00 - #0x0F)

/**
 * @brief Set Timer (EID #0x00)
 * Programs the clock for next event after `stime_value` time. This function
 * also clears the pending timer interrupt bit.
 * @return This SBI call returns 0 upon success or an implementation specific
 * negative error code.
 * @note If the supervisor wishes to clear the timer interrupt without
 * scheduling the next timer event, it can either request a timer interrupt
 * infinitely far into the future (i.e., (uint64_t)-1), or it can instead mask
 * the timer interrupt by clearing `sie.STIE` CSR bit.
 */
long sbi_set_timer(uint64_t stime_value);

/**
 * @brief Console Putchar (EID #0x01)
 * Write data present in `ch` to debug console.
 * @return This SBI call returns 0 upon success or an implementation specific
 * negative error code.
 * @note Unlike `sbi_console_getchar()`, this SBI call *will block* if there
 * remain any pending characters to be transmitted or if the receiving terminal
 * is not yet ready to receive the byte. However, if the console doesn’t exist
 * at all, then the character is thrown away.
 */
long sbi_console_putchar(int ch);

/**
 * @brief Console Getchar (EID #0x02)
 * Read a byte from debug console.
 * @return The SBI call returns the byte on success, or -1 for failure.
 */
long sbi_console_getchar(void);

/**
 * @brief Clear IPI (EID #0x03)
 * Clears the pending IPIs if any. The IPI is cleared only in the hart for which
 * this SBI call is invoked. `sbi_clear_ipi()` is deprecated because S-mode code
 * can clear `sip.SSIP` CSR bit directly.
 * @return This SBI call returns 0 if no IPI had been pending, or an
 * implementation specific positive value if an IPI had been pending.
 */
long sbi_clear_ipi(void);

/**
 * @brief Send IPI (EID #0x04)
 * Send an inter-processor interrupt to all the harts defined in hart_mask.
 * Interprocessor interrupts manifest at the receiving harts as Supervisor
 * Software Interrupts.
 * @return This SBI call returns 0 upon success or an implementation specific
 * negative error code.
 * @note hart_mask is a virtual address that points to a bit-vector of harts.
 * The bit vector is represented as a sequence of unsigned longs whose length
 * equals the number of harts in the system divided by the number of bits in an
 * unsigned long, rounded up to the next integer.
 */
long sbi_send_ipi(const unsigned long *hart_mask);

/**
 * @brief Remote FENCE.I (EID #0x05)
 * Instructs remote harts to execute `FENCE.I` instruction. The `hart_mask` is
 * same as described in `sbi_send_ipi()`.
 * @return This SBI call returns 0 upon success or an implementation specific
 * negative error code.
 */
long sbi_remote_fence_i(const unsigned long *hart_mask);

/**
 * @brief Remote SFENCE.VMA (EID #0x06)
 * Instructs the remote harts to execute one or more `SFENCE.VMA` instructions,
 * covering the range of virtual addresses between `start` and `start + size`.
 * @return This SBI call returns 0 upon success or an implementation specific
 * negative error code.
 * @note The remote fence operation applies to the entire address space if
 * either: start and size are both 0, or size is equal to 2^XLEN-1.
 */
long sbi_remote_sfence_vma(const unsigned long *hart_mask, unsigned long start,
                           unsigned long size);

/**
 * @brief Remote SFENCE.VMA with ASID (EID #0x07)
 * Instruct the remote harts to execute one or more `SFENCE.VMA` instructions,
 * covering the range of virtual addresses between `start` and `start + size`.
 * This covers only the given `ASID`.
 * @return This SBI call returns 0 upon success or an implementation specific
 * negative error code.
 * @note The remote fence operation applies to the entire address space if
 * either: start and size are both 0, or size is equal to 2^XLEN-1.
 */
long sbi_remote_sfence_vma_asid(const unsigned long *hart_mask,
                                unsigned long start, unsigned long size,
                                unsigned long asid);

/**
 * @brief System Shutdown (EID #0x08)
 * Puts all the harts to shutdown state from supervisor point of view.
 * @return This SBI call doesn’t return irrespective whether it succeeds or
 * fails.
 */
void sbi_shutdown(void);
#endif

// ----------------------------------------------------------------------------
// Timer Extension (EID #0x54494D45 "TIME")
// Set Timer (FID #0)
struct sbiret sbi_set_timer(uint64_t stime_value);

// ----------------------------------------------------------------------------
// IPI Extension (EID #0x735049 "sPI: s-mode IPI")
// Send IPI (FID #0)
struct sbiret sbi_send_ipi(unsigned long hart_mask,
                           unsigned long hart_mask_base);

// ----------------------------------------------------------------------------
// RFENCE Extension (EID #0x52464E43 "RFNC")

// Remote FENCE.I (FID #0)
struct sbiret sbi_remote_fence_i(unsigned long hart_mask,
                                 unsigned long hart_mask_base);

// Remote SFENCE.VMA (FID #1)
struct sbiret sbi_remote_sfence_vma(unsigned long hart_mask,
                                    unsigned long hart_mask_base,
                                    unsigned long start_addr,
                                    unsigned long size);

// Remote SFENCE.VMA with ASID (FID #2)
struct sbiret sbi_remote_sfence_vma_asid(unsigned long hart_mask,
                                         unsigned long hart_mask_base,
                                         unsigned long start_addr,
                                         unsigned long size,
                                         unsigned long asid);

// Remote HFENCE.GVMA with VMID (FID #3)
struct sbiret sbi_remote_hfence_gvma_vmid(unsigned long hart_mask,
                                          unsigned long hart_mask_base,
                                          unsigned long start_addr,
                                          unsigned long size,
                                          unsigned long vmid);
// Remote HFENCE.GVMA (FID #4)
struct sbiret sbi_remote_hfence_gvma(unsigned long hart_mask,
                                     unsigned long hart_mask_base,
                                     unsigned long start_addr,
                                     unsigned long size);

// Remote HFENCE.VVMA with ASID (FID #5)
struct sbiret sbi_remote_hfence_vvma_asid(unsigned long hart_mask,
                                          unsigned long hart_mask_base,
                                          unsigned long start_addr,
                                          unsigned long size,
                                          unsigned long asid);

// Remote HFENCE.VVMA (FID #6)
struct sbiret sbi_remote_hfence_vvma(unsigned long hart_mask,
                                     unsigned long hart_mask_base,
                                     unsigned long start_addr,
                                     unsigned long size);

// ----------------------------------------------------------------------------
// Hart State Management Extension (EID #0x48534D "HSM")

// Hart start (FID #0)
struct sbiret sbi_hart_start(unsigned long hartid, unsigned long start_addr,
                             unsigned long opaque);

// Hart stop (FID #1)
struct sbiret sbi_hart_stop(void);

// Hart get status (FID #2)
struct sbiret sbi_hart_get_status(unsigned long hartid);

// Hart suspend (FID #3)
struct sbiret sbi_hart_suspend(uint32_t suspend_type, unsigned long resume_addr,
                               unsigned long opaque);

// ----------------------------------------------------------------------------
// System Reset Extension (EID #0x53525354 "SRST")

// System reset (FID #0)
struct sbiret sbi_system_reset(uint32_t reset_type, uint32_t reset_reason);

// ----------------------------------------------------------------------------
// Performance Monitoring Unit Extension (EID #0x504D55 "PMU")

// Get number of counters (FID #0)
struct sbiret sbi_pmu_num_counters();

// Get details of a counter (FID #1)
struct sbiret sbi_pmu_counter_get_info(unsigned long counter_idx);

// Find and configure a matching counter (FID #2)
struct sbiret sbi_pmu_counter_config_matching(unsigned long counter_idx_base,
                                              unsigned long counter_idx_mask,
                                              unsigned long config_flags,
                                              unsigned long event_idx,
                                              uint64_t event_data);

// Start a set of counters (FID #3)
struct sbiret sbi_pmu_counter_start(unsigned long counter_idx_base,
                                    unsigned long counter_idx_mask,
                                    unsigned long start_flags,
                                    uint64_t initial_value);

// Stop a set of counters (FID #4)
struct sbiret sbi_pmu_counter_stop(unsigned long counter_idx_base,
                                   unsigned long counter_idx_mask,
                                   unsigned long stop_flags);

// Read a firmware counter (FID #5)
struct sbiret sbi_pmu_counter_fw_read(unsigned long counter_idx);

// Read a firmware counter high bits (FID #6)
struct sbiret sbi_pmu_counter_fw_read_hi(unsigned long counter_idx);

// Set PMU snapshot shared memory (FID #7)
struct sbiret sbi_pmu_snapshot_set_shmem(unsigned long shmem_phys_lo,
                                         unsigned long shmem_phys_hi,
                                         unsigned long flags);

// ----------------------------------------------------------------------------
// Debug Console Extension (EID #0x4442434E "DBCN")

// Console Write (FID #0)
struct sbiret sbi_debug_console_write(unsigned long num_bytes,
                                      unsigned long base_addr_lo,
                                      unsigned long base_addr_hi);

// Console Read (FID #1)
struct sbiret sbi_debug_console_read(unsigned long num_bytes,
                                     unsigned long base_addr_lo,
                                     unsigned long base_addr_hi);

// Console Write Byte (FID #2)
struct sbiret sbi_debug_console_write_byte(uint8_t byte);

// ----------------------------------------------------------------------------
// System Suspend Extension (EID #0x53555350 "SUSP")

// System Suspend (FID #0)
struct sbiret sbi_system_suspend(uint32_t sleep_type, unsigned long resume_addr,
                                 unsigned long opaque);

// ----------------------------------------------------------------------------
// CPPC Extension (EID #0x43505043 "CPPC")

// Probe CPPC register (FID #0)
struct sbiret sbi_cppc_probe(uint32_t cppc_reg_id);

// Read CPPC register (FID #1)
struct sbiret sbi_cppc_read(uint32_t cppc_reg_id);

// Read CPPC register high bits (FID #2)
struct sbiret sbi_cppc_read_hi(uint32_t cppc_reg_id);

// Write to CPPC register (FID #3)
struct sbiret sbi_cppc_write(uint32_t cppc_reg_id, uint64_t val);

// ----------------------------------------------------------------------------
// Nested Acceleration Extension (EID #0x4E41434C "NACL")

// Probe nested acceleration feature (FID #0)
struct sbiret sbi_nacl_probe_feature(uint32_t feature_id);

// Set nested acceleration shared memory (FID #1)
struct sbiret sbi_nacl_set_shmem(unsigned long shmem_phys_lo,
                                 unsigned long shmem_phys_hi,
                                 unsigned long flags);

// Synchronize shared memory CSRs (FID #2)
struct sbiret sbi_nacl_sync_csr(unsigned long csr_num);

// Synchronize shared memory HFENCEs (FID #3)
struct sbiret sbi_nacl_sync_hfence(unsigned long entry_index);

// Synchronize shared memory and emulate SRET (FID #4)
struct sbiret sbi_nacl_sync_sret(void);

// ----------------------------------------------------------------------------
// Steal-time Accounting Extension (EID #0x535441 "STA")

// Set Steal-time Shared Memory Address (FID #0)
struct sbiret sbi_steal_time_set_shmem(unsigned long shmem_phys_lo,
                                       unsigned long shmem_phys_hi,
                                       unsigned long flags);

// ----------------------------------------------------------------------------

#endif
