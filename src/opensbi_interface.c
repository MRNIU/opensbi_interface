
/**
 * @file opensbi_interface.c
 * @brief opensbi 接口实现
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

#include "include/opensbi_interface.h"

static struct sbiret ecall(unsigned long _arg0, unsigned long _arg1,
                           unsigned long _arg2, unsigned long _arg3,
                           unsigned long _arg4, unsigned long _arg5,
                           unsigned long _fid, unsigned long _eid) {
  struct sbiret ret;
  register uintptr_t a0 __asm__("a0") = (uintptr_t)(_arg0);
  register uintptr_t a1 __asm__("a1") = (uintptr_t)(_arg1);
  register uintptr_t a2 __asm__("a2") = (uintptr_t)(_arg2);
  register uintptr_t a3 __asm__("a3") = (uintptr_t)(_arg3);
  register uintptr_t a4 __asm__("a4") = (uintptr_t)(_arg4);
  register uintptr_t a5 __asm__("a5") = (uintptr_t)(_arg5);
  register uintptr_t a6 __asm__("a6") = (uintptr_t)(_fid);
  register uintptr_t a7 __asm__("a7") = (uintptr_t)(_eid);

  __asm__("ecall"
          : "+r"(a0), "+r"(a1)
          : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
          : "memory");
  ret.error = a0;
  ret.value = a1;
  return ret;
}
